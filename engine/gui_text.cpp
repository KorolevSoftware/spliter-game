#include "gui_text.h"
#include <numeric>

namespace Engine {
	TextData::TextData(const std::string& text, uint32_t atlasID) {
		this->text = text;
		this->atlasID = atlasID;
	}

	int make_char(GUIVertex* vertexBuffer, const vec4& color, const vec3& position, const vec2& dim, const vec2& textureCoord1, const vec2& textureCoord2) {
		//2-----------------------3
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//1-----------------------4

		vec3 position1(0, 0, 0.0f); // 1
		vec3 position2(0, dim.y, 0.0f); // 2
		vec3 position3(dim.x, 0, 0.0f); // 4

		vec3 position4(dim.x, 0, 0.0f); // 4
		vec3 position5(0, dim.y, 0.0f); // 2
		vec3 position6(dim.x, dim.y, 0.0f); // 3

		vertexBuffer[0] = GUIVertex(position1 + position, color, textureCoord1);
		vertexBuffer[1] = GUIVertex(position2 + position, color, vec2(textureCoord1.x, textureCoord2.y));
		vertexBuffer[2] = GUIVertex(position3 + position, color, vec2(textureCoord2.x, textureCoord1.y));

		vertexBuffer[3] = GUIVertex(position3 + position, color, vec2(textureCoord2.x, textureCoord1.y));
		vertexBuffer[4] = GUIVertex(position5 + position, color, vec2(textureCoord1.x, textureCoord2.y));
		vertexBuffer[5] = GUIVertex(position6 + position, color, textureCoord2);
		return 6;
	}

	bool TextData::input(const vec2& clickPosition, const GUIBase& base) {
		return false;
	}

	int TextData::generator(GUIVertex * vertexBuffer, const GUIBase& base, const std::vector<Atlas>& atlasBuffer) {
		if (this->text.empty()) {
			return 0;
		}

		const Atlas& atlasFont = atlasBuffer[atlasID];

		std::vector<Patch> textPatch;
		textPatch.reserve(this->text.length());
		for (const auto& ch : this->text) { // Offset by X axis
			if (ch == 0) {
				continue;
			}
			auto iter = atlasFont.images.find(ch);
			textPatch.push_back(iter->second);
		}

		int textOffestX = std::accumulate(
			textPatch.begin(),
			textPatch.end(),
			0,
			[](int sum, const Patch& patch) {return sum - (patch.size.x - patch.origin.x); });

		auto patch = textPatch.front();
		float textOffsetY = (patch.origin.y - patch.size.y) / 2;

		textOffestX /= 2;
		int vertexAddCount = 0;
		for (const auto& r_ch : textPatch) {
			vec4 normalize = vec4(r_ch.origin, r_ch.size) / float(atlasFont.width);
			vec2 textureCoord1 = vec2(normalize.x, normalize.y);
			vec2 textureCoord2 = vec2(normalize.z, normalize.p);

			vec3 position = vec3(textOffestX,-textOffsetY, 0);
			vec2 size = vec2(
				r_ch.size.x - r_ch.origin.x,
				r_ch.origin.y - r_ch.size.y
			);

			vertexAddCount += make_char(&vertexBuffer[vertexAddCount], vec4(0.0f, 0.0f, 0.0f, 0.0f), position, size, textureCoord1, textureCoord2);
			textOffestX += size.x;
		}
		return vertexAddCount;
	}

	GUINodeID make_text(GUIComposer& composer, const std::string& text, uint32_t atlasID) {
		GUINodeID node = composer.makeNode();
		IGUIPrimitive* primitive = new TextData(text, atlasID);
		composer.setPrimitive(primitive, node);
		return node;
	}
};