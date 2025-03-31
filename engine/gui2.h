#pragma once
#include "glm/fwd.hpp"
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec2
#include <glm/vec4.hpp> // glm::vec2
#include <list>
#include <vector>

namespace Engine2 {
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

	enum class GUIAdjustMod {
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
		glm::vec2 position;
		glm::vec4 color;
	};

	struct GUIPrimitiveGenerator {
		void* userData;
		int (*generator)(GUIVertex* vertex, const GUIBase* base, void* userData);
		void (*release)();
	};

	struct GUINode {
		uint32_t hash;
		GUIPivot pivot;
		GUIAdjustMod adjustMod;
		bool anchorY;
		bool anchorX;

		GUIBase base;
		GUIPrimitiveGenerator generator;
		GUINode* children[10];
		bool visable = true;
	};

	struct GUIDrawCommand {
		uint32_t start;
		uint32_t end;
		uint32_t textureID;
	};

	struct GUINodeScreen {
		uint32_t hash;
		glm::vec2 screen_p1;
		glm::vec2 screen_p2;
	};

	struct GUIComposer final {
		GUIComposer(uint32_t poolSize);
		bool compose(const GUINode& node, const glm::vec2& localResolution, const glm::vec2& actualResolution, const glm::vec2& parentOffset);
		bool composeScreen(const GUINode& node, const glm::vec2& aspectRation, const glm::vec2& parentOffset);
		void clearVertexBuffer();
		const uint8_t* getBufferData() const;
		uint32_t getVertexCount() const;
		uint32_t getRenderBufSizeof();
		uint32_t pickNode(glm::vec2 pos);
		uint32_t vertexArrayOffset;

	private:
		std::list<GUINodeScreen> nodesFromScreen;
		GUIVertex* vertexBuffer;
		std::vector<GUIDrawCommand> commandBuffer;
	};
};