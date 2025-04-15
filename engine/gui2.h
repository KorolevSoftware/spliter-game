#pragma once
#include "glm/fwd.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <list>

namespace Engine {
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
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoords;
		GUIVertex();
		GUIVertex(glm::vec3 position, glm::vec4 color, glm::vec2 texCoords);
	};

	struct GUIBase {
		glm::vec2 size;
		glm::vec4 color;
	};

	struct GUIPrimitive {
		void* userData;
		int (*generator)(GUIVertex* vertex, const GUIBase* base, void* userData);
		bool (*input)(const glm::vec2& clickPosition, const GUIBase* base, void* userData);
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

		glm::vec3 position;
		glm::vec3 scale;
		float angleZ;
		glm::mat4 localTransform;
		glm::mat4 invTransform;

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
		void compose(GUINodeID node, const glm::vec2& localResolution, const glm::vec2& actualResolution, const glm::vec2& parentOffset);
		void composeScreen(GUINodeID node, const glm::vec2& aspectRation);
		void clearVertexBuffer();
		const uint8_t* getBufferData() const;
		uint32_t getVertexCount() const;
		uint32_t getRenderBufSizeof() const;
		bool pickNode(GUINodeID node, const glm::vec2& pos);
		
		GUINodeID makeNode();
		void setAdjustMode(GUINodeID node, GUIAdjustMode mode);
		void setPrimitive(GUIPrimitive primitive, GUINodeID node);
		void setScale(GUINodeID node, glm::vec3 scale);
		void setPosition(GUINodeID node, glm::vec3 position);
		void setRotationZ(GUINodeID node, float angle);
		void setSize(GUINodeID node, glm::vec3 size);
		void setColor(GUINodeID node, glm::vec4 color);
		void setChildren(GUINodeID parent, GUINodeID node);
		void setAnchorY(GUINodeID node, bool isEnable);
		void setAnchorX(GUINodeID node, bool isEnable);
		void setPivot(GUINodeID node, GUIPivot pivot);
		void setHesh(GUINodeID node, uint32_t hash);

	private:
		uint32_t vertexArrayOffset;
		GUIVertex* vertexBuffer;
		std::vector<GUINode> nodePoolBuffer;
		std::vector<GUIDrawCommand> commandBuffer;
	};
};