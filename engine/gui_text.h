#pragma once
#include "gui2.h"
#include <string>

namespace Engine {
	struct TextData : IGUIPrimitive {
		std::string text;
		uint32_t atlasID;
		TextData(const std::string& text, uint32_t atlasID);
		GUIDrawCommand generator(GUIVertex* vertex, const GUIBase& base, const std::vector<Atlas>& atlasBuffer) override;
		bool input(const vec2& clickPosition, const GUIBase& base) override;
	};

	GUINodeID make_text(GUIComposer& composer, const std::string& text, uint32_t atlasID);
};