#pragma once
#include "glm/fwd.hpp"
#include <glm/vec2.hpp> // glm::vec2
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
		bool anchorY;
		bool anchorX;
		GUIPivot pivot;
		GUIAdjustMod adjustMod;
	};

	struct GUIComposer final {

	};
};