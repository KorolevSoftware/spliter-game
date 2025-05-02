#pragma once
#include "gui2.h"
#include <string>

namespace Engine {
	struct Font {
		std::array<Patch, 255> glyphs;
	};

	GUINodeID make_text(GUIComposer& composer, const std::string& text, Font* font);
};