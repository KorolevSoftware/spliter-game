#pragma once
#include "gui2.h"
#include <glm/vec2.hpp> // glm::vec2

namespace Engine2 {
	GUINode make_billboard(const glm::vec2& textureCoord1, const glm::vec2& textureCoord2);
};