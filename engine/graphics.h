#pragma once
#include "glm/fwd.hpp"


namespace Engine {

	struct Graphics final {
		void initialize();
		void drawBox(glm::vec3 size, glm::vec4 color);
		void drawRect2D(glm::vec2 size, glm::vec2 position);
	};
};