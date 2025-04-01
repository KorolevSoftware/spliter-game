#pragma once
#include "gui2.h"
#include <string>
#include <glm/vec2.hpp> // glm::vec2

namespace Engine2 {
	struct Rectangle {
		int x, y, w, h;
	};
	struct Font {
		Rectangle glyphs[255];
	};
	GUINode make_text(const std::string& text, Font* font);
};