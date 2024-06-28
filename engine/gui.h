#pragma once
#include "glm/fwd.hpp"
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec2
#include <glm/vec4.hpp> // glm::vec2
#include <list>
#include <vector>

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

	enum class GUIAdjustMod {
		Fit,
		Zoom,
		Stretch
	};

	struct GUINode {
		glm::vec2 size;
		glm::vec2 position;
		glm::vec4 color;
		glm::vec2 textureCoord;
		bool anchorY;
		bool anchorX;
		GUIPivot pivot;
		GUIAdjustMod adjustMod;
		std::list<GUINode> childs;
	};

	struct GUIVertex {
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoords;

		GUIVertex(glm::vec3 position, glm::vec4 color, glm::vec2 texCoords);
	};

	struct GUIDrawCommand {
		uint32_t start;
		uint32_t end;
		uint32_t textureID;
	};

	struct GUIComposer final {
		GUIComposer(uint32_t poolSize);
		bool compose(const GUINode& node, const glm::vec2& localResolution, const glm::vec2& actualResolution, const glm::vec2& parentOffset);
		void clearVertexBuffer();
		const uint8_t* getBufferData() const;
		uint32_t getVertexCount() const;
		uint32_t getRenderBufSizeof();

	private:
		std::vector<GUIVertex> vertexBuffer;
		std::vector<GUIDrawCommand> commandBuffer;
	};
};