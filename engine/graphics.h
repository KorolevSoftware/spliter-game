#pragma once
#include "glm/fwd.hpp"
#include "gui.h"

namespace Engine {

	struct Graphics final {
		void initialize();
		void beginDraw(uint32_t width, uint32_t height);
		void endDraw();
		void setMatrixGUI();
		void drawBox(glm::vec3 size, glm::vec4 color);
		void drawRect2D(glm::vec2 size, glm::vec2 position);
		void drawGui(const GUINode& node);
	};
};