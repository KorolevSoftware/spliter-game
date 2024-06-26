#pragma once
#include "glm/fwd.hpp"
#include <glm/vec2.hpp> // glm::vec2
namespace Engine {
	enum class GUIAnchorY {
		None,
		Top,
		Bottomn
	};

	enum class GUIAnchorX {
		None,
		Left,
		Right
	};

	enum class GUIAdjustMod {
		Fit,
		Zoom,
		Stretch
	};

	struct GUINode {
		glm::vec2 size;
		glm::vec2 position;
		GUIAnchorY anchorY;
		GUIAnchorX anchorX;
		GUIAdjustMod adjustMod;
	};

	struct GUIComposer final {

	};
};