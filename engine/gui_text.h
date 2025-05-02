#pragma once
#include "gui2.h"
#include <string>

namespace Engine {
	struct Font {
		Patch glyphs[255];
	};
	GUINodeID make_text(GUIComposer& composer, const std::string& text, Font* font);
};