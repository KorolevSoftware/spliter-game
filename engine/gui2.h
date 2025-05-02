#pragma once
#include <list>
#include "gui_math.h"
#include <vector>

namespace Engine {
	float radians(float angle);
	using vec2 = vec<2, float>;
	using vec3 = vec<3, float>;
	using vec4 = vec<4, float>;
	using mat4 = mat<4, float>;

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

	struct GUIPrimitive {
		void* userData;
		int (*generator)(GUIVertex* vertex, const GUIBase* base, void* userData);
		bool (*input)(const vec2& clickPosition, const GUIBase* base, void* userData);
		void (*release)();
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
		GUIPrimitive primitive;
		
		GUINodeID parent;
		std::list<GUINodeID> children;

		GUINode();
	};

	struct GUIDrawCommand {
		uint32_t start;
		uint32_t end;
		uint32_t textureID;
	};

	struct GUIComposer final {
		GUIComposer(uint32_t vertexPoolSize, uint32_t nodePoolSize);
		void compose(GUINodeID node, const vec2& localResolution, const vec2& actualResolution, const vec2& parentOffset);
		void composeScreen(GUINodeID node, const vec2& aspectRation);
		void clearVertexBuffer();
		const uint8_t* getBufferData() const;
		uint32_t getVertexCount() const;
		uint32_t getRenderBufSizeof() const;
		bool pickNode(GUINodeID node, const vec2& pos) const;
		
		GUINodeID makeNode();
		void setAdjustMode(GUINodeID node, GUIAdjustMode mode);
		void setPrimitive(GUIPrimitive primitive, GUINodeID node);
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
		GUINodeID findNode(uint32_t hash) const;
		bool isValide(GUINodeID node) const;

	private:
		uint32_t vertexArrayOffset;
		GUIVertex* vertexBuffer;
		std::vector<GUINode> nodePoolBuffer;
		std::vector<GUIDrawCommand> commandBuffer;
	};
};