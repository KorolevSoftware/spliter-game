#pragma once
#include <list>
#include "gui_math.h"
#include <vector>
#include <array>
#include <unordered_map>

namespace Engine {
	float radians(float angle);
	using vec2 = vec<2, float>;
	using vec3 = vec<3, float>;
	using vec4 = vec<4, float>;
	using mat4 = mat<4, float>;

	using i16vec2 = vec<2, uint16_t>;

	enum class GUIPivot {
		Centre,
		North,
		NorthEast,
		East,
		SouthEast,
		South,
		SouthWest,
		West,
		NorthWest,
	};

	enum class GUIAdjustMode {
		Fit,
		Zoom,
		Stretch
	};

	struct Patch {
		i16vec2 origin, size;
	};

	struct Atlas {
		std::unordered_map<uint32_t, Patch> images;
		uint32_t textureID;
		uint32_t width, height;
	};

	struct GUIVertex {
		vec3 position;
		vec4 color;
		vec2 texCoords;
		GUIVertex();
		GUIVertex(vec3 position, vec4 color, vec2 texCoords);
	};

	struct GUIBase {
		vec2 size;
		vec4 color;
	};

	struct GUIDrawCommand {
		uint32_t start;
		uint32_t count;
		uint32_t textureID;
		GUIDrawCommand(uint32_t count, uint32_t textureID);
		static GUIDrawCommand None();
	};

	struct IGUIPrimitive {
		virtual GUIDrawCommand generator(GUIVertex* vertexBuffer, const GUIBase& base, const std::vector<Atlas>& atlasBuffer) = 0;
		virtual bool input(const vec2& clickPosition, const GUIBase& base) = 0;
	};

	using GUINodeID = uint32_t;

	struct GUINode {
		uint32_t hash;
		GUIPivot pivot;
		GUIAdjustMode adjustMode;

		bool anchorY;
		bool anchorX;
		bool visable = true;

		vec3 position;
		vec3 scale;
		float angleZ;
		mat4 localTransform;
		vec2 invAdjustScale;

		GUIBase base;
		IGUIPrimitive* primitive;

		GUINodeID parent;
		std::list<GUINodeID> children;

		GUINode();
	};

	

	struct GUIComposer final {
		GUIComposer(uint32_t vertexPoolSize, uint32_t nodePoolSize);
		void compose(GUINodeID node, const vec2& localResolution, const vec2& actualResolution, const vec2& parentOffset);
		void composeScreen(GUINodeID node, const vec2& aspectRation);
		const uint8_t* getBufferData() const;
		uint32_t getVertexCount() const;
		uint32_t getRenderBufSizeof() const;
		bool pickNode(GUINodeID node, const vec2& pos) const;

		GUINodeID makeNode();
		GUINodeID findNode(uint32_t hash) const;
		bool isValide(GUINodeID node) const;

		void setAdjustMode(GUINodeID node, GUIAdjustMode mode);
		void setPrimitive(IGUIPrimitive* primitive, GUINodeID node);
		void setScale(GUINodeID node, vec3 scale);
		void setPosition(GUINodeID node, vec3 position);
		void setRotationZ(GUINodeID node, float angle);
		void setSize(GUINodeID node, vec2 size);
		void setColor(GUINodeID node, vec4 color);
		void setChildren(GUINodeID parent, GUINodeID node);
		void setAnchorY(GUINodeID node, bool isEnable);
		void setAnchorX(GUINodeID node, bool isEnable);
		void setPivot(GUINodeID node, GUIPivot pivot);
		void setHesh(GUINodeID node, uint32_t hash);

		uint32_t makeAtlas(std::unordered_map<uint32_t, Patch> images, uint32_t textureID, uint32_t width, uint32_t height);

		GUINodeID getHesh(GUINodeID node) const;
		GUIPivot getPivot(GUINodeID node) const;
		bool getAnchorY(GUINodeID node) const;
		bool getAnchorX(GUINodeID node) const;
		vec4 setColor(GUINodeID node) const;
		vec2 getSize(GUINodeID node) const;
		float getRotationZ(GUINodeID node) const;
		vec3 getPosition(GUINodeID node) const;
		vec3 getScale(GUINodeID node) const;
		GUIAdjustMode getAdjustMode(GUINodeID node) const;
		uint32_t getChildrenCount(GUINodeID node) const;
		GUINodeID* getChildrens(GUINodeID node, uint32_t* size) const;

		uint32_t vertexArrayOffset;
		GUIVertex* vertexBuffer;
		std::vector<GUINode> nodePoolBuffer;
		std::vector<GUIDrawCommand> commandBuffer;
		std::vector<Atlas> atlasBuffer;
	};
};