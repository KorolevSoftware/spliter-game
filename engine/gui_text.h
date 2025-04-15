#pragma once
#include "gui2.h"
#include <string>
#include <glm/vec2.hpp> // glm::vec2

namespace Engine {
	struct Rectangle {
		int x, y, w, h;
	};
	struct Font {
		Rectangle glyphs[255];
	};
	GUINodeID make_text(GUIComposer& composer, const std::string& text, Font* font);
};