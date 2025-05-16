#include <sstream>
#include <vector>
#include <cstring>

#include "gui_serializer.h"
#include "gui_billboard.h"
#include "gui_text.h"


namespace {
	using namespace Engine;

	template<typename T>
	void serialize(std::vector<std::byte>& data, const T& value) {
		const std::byte* valueBytes = reinterpret_cast<const std::byte*>(&value);
		const size_t valuelength = sizeof(value);
		for (size_t index = 0; index < valuelength; index++) {
			data.push_back(valueBytes[index]);
		}
	}

	template<typename T>
	void deserialize(const std::vector<std::byte>& data, size_t& offset, T& value) {
		const size_t valuelength = sizeof(value);
		std::memcpy(&value, data.data() + offset, valuelength);
		offset += valuelength;
	}

	void serialize(std::vector<std::byte>& data, const std::string& str) {
		size_t size = str.size();
		serialize(data, size);
		for (char ch: str) {
			data.push_back(static_cast<std::byte>(ch));
		}
	}

	void deserialize(const std::vector<std::byte>& data, size_t& offset, std::string& str) {
		size_t size;
		deserialize(data, offset, size);
		str.resize(size);
		for (size_t index = 0; index < size; index++) {
			str[index] = static_cast<char>(data[index + offset]);
		}
		offset += size;
	}

	template<typename T>
	void serialize(std::vector<std::byte>& data, const std::list<T>& list) {
		size_t size = list.size();
		serialize(data, size);
		for (const auto& item : list) {
			serialize(data, item);
		}
	}

	template<typename T>
	void serialize(std::vector<std::byte>& data, const std::unordered_map<uint32_t, T>& unMap) {
		size_t size = unMap.size();
		serialize(data, size);
		for (const auto& [key, item] : unMap) {
			serialize(data, item);
			serialize(data, key);
		}
	}

	template<typename T>
	void deserialize(const std::vector<std::byte>& data, size_t& offset, std::unordered_map<uint32_t, T>& unMap) {
		size_t size;
		deserialize(data, offset, size);
		unMap.clear();
		for (size_t i = 0; i < size; ++i) {
			T item;
			uint32_t key;
			deserialize(data, offset, item);
			deserialize(data, offset, key);
			unMap[key] = item;
		}
	}

	template<typename T>
	void deserialize(const std::vector<std::byte>& data, size_t& offset, std::list<T>& list) {
		size_t size;
		deserialize(data, offset, size);
		list.clear();
		for (size_t i = 0; i < size; ++i) {
			T item;
			deserialize(data, offset, item);
			list.push_back(item);
		}
	}

	void serialize(std::vector<std::byte>& data, const GUIBase& base) {
		serialize(data, base.size);
		serialize(data, base.color);
	}

	void deserialize(const std::vector<std::byte>& data, size_t& offset, GUIBase& base) {
		deserialize(data, offset, base.size);
		deserialize(data, offset, base.color);
	}

	void serialize(std::vector<std::byte>& data, const GUINode& node) {
		serialize(data, node.hash);
		serialize(data, node.pivot);
		serialize(data, node.adjustMode);
		serialize(data, node.anchorY);
		serialize(data, node.anchorX);
		serialize(data, node.visable);
		serialize(data, node.position);
		serialize(data, node.scale);
		serialize(data, node.angleZ);
		serialize(data, node.base);
		serialize(data, node.parent);
		serialize(data, node.children);

		if (auto textData = dynamic_cast<const TextData*>(node.primitive)) {
			uint32_t type = 1; // TextData
			serialize(data, type);
			serialize(data, textData->text);
			serialize(data, textData->atlasID);
		} else if (auto billboardData = dynamic_cast<const BillboardData*>(node.primitive)) {
			uint32_t type = 2; // BillboardData
			serialize(data, type);
			serialize(data, billboardData->textureCoord1);
			serialize(data, billboardData->textureCoord2);
			serialize(data, billboardData->atlasID);
		} else {
			uint32_t type = 0; // Unknown
			serialize(data, type);
		}
	}

	void serialize(std::vector<std::byte>& data, const Atlas& atlas) {
		serialize(data, atlas.textureID);
		serialize(data, atlas.height);
		serialize(data, atlas.width);
		serialize(data, atlas.images);
	}

	void deserialize(const std::vector<std::byte>& data, size_t& offset, Atlas& atlas) {
		deserialize(data, offset, atlas.textureID);
		deserialize(data, offset, atlas.height);
		deserialize(data, offset, atlas.width);
		deserialize(data, offset, atlas.images);
	}

	void deserialize(const std::vector<std::byte>& data, size_t& offset, GUINode& node) {
		deserialize(data, offset, node.hash);
		deserialize(data, offset, node.pivot);
		deserialize(data, offset, node.adjustMode);
		deserialize(data, offset, node.anchorY);
		deserialize(data, offset, node.anchorX);
		deserialize(data, offset, node.visable);
		deserialize(data, offset, node.position);
		deserialize(data, offset, node.scale);
		deserialize(data, offset, node.angleZ);
		deserialize(data, offset, node.base);
		deserialize(data, offset, node.parent);
		deserialize(data, offset, node.children);

		// Десериализация примитива
		uint32_t primitiveType;
		deserialize(data, offset, primitiveType);

		switch (primitiveType) {
		case 1:
		{ // TextData
			std::string text;
			uint32_t fontId;
			deserialize(data, offset, text);
			deserialize(data, offset, fontId);
			node.primitive = new TextData(text, fontId);
			break;
		}
		case 2:
		{ // BillboardData
			vec2 coord1, coord2;
			uint32_t atlasID;
			deserialize(data, offset, coord1);
			deserialize(data, offset, coord2);
			deserialize(data, offset, atlasID);
			node.primitive = new BillboardData(coord1, coord2, atlasID);
			break;
		}
		default:
			node.primitive = nullptr;
		}
	}
}

namespace Engine {

	GUIComposer GuiSerialize::deserialize(const std::byte* buffer, size_t dataSizeOf) {
		data.clear();
		data = std::vector<std::byte>(buffer, buffer + dataSizeOf);

		size_t offset = 0;
		GUIComposer composer(4000, 100);
		size_t nodeCount;
		::deserialize(data, offset, nodeCount);
		for (size_t index = 0; index < nodeCount; index++) {
			GUINode node;
			::deserialize(data, offset, node);
			composer.nodePoolBuffer.push_back(node);
		}

		size_t atlasCount;
		::deserialize(data, offset, atlasCount);
		for (size_t index = 0; index < atlasCount; index++) {
			Atlas atlas;
			::deserialize(data, offset, atlas);
			composer.atlasBuffer.push_back(atlas);
		}

		return composer;
	}

	std::byte* GuiSerialize::serialize(const GUIComposer& composer, size_t& dataSizeOf) {
		data.clear();
		::serialize(data, composer.nodePoolBuffer.size()); // node count
		for (const auto& node : composer.nodePoolBuffer) { // node
			::serialize(data, node);
		}

		::serialize(data, composer.atlasBuffer.size()); // atlas count
		for (const auto& atlas : composer.atlasBuffer) { // atlas
			::serialize(data, atlas);
		}

		dataSizeOf = data.size();
		return data.data();
	}
};