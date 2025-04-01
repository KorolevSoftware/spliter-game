#include <glm/vec2.hpp> // glm::vec2
#include "gui_billboard.h"

namespace Engine2 {

	struct BilboardData {
		glm::vec2 textureCoord1;
		glm::vec2 textureCoord2;
		BilboardData(glm::vec2 textureCoord1, glm::vec2 textureCoord2) : textureCoord1(textureCoord1), textureCoord2(textureCoord2) {}
	};

	bool bilboard_input(int x, int y, const GUIBase* base, void* userData) {
		glm::vec2 dim = base->size / 2.0f;

		if (-dim.x > x || dim.x < x)
			return false;

		if (-dim.y > y || dim.y < y)
			return false;

		int gg = 0;
		return true;
	}

	int generator(GUIVertex* vertexBuffer, const GUIBase* base, void* userData) {
		BilboardData* bilboard = reinterpret_cast<BilboardData*>(userData);
		glm::vec2 dim = base->size / 2.0f;

			//2-----------------------3
			//|                       |
			//|                       |
			//|                       |
			//|                       |
			//|                       |
			//|                       |
			//|                       |
			//1-----------------------4

		glm::vec3 position1(-dim.x, -dim.y, 0.0f); // 1
		glm::vec3 position2(-dim.x, dim.y, 0.0f); // 2
		glm::vec3 position3(dim.x, -dim.y, 0.0f); // 4

		glm::vec3 position4(dim.x, -dim.y, 0.0f); // 4
		glm::vec3 position5(-dim.x, dim.y, 0.0f); // 2
		glm::vec3 position6(dim.x, dim.y, 0.0f); // 3

		vertexBuffer[0] = GUIVertex(position1, base->color, bilboard->textureCoord1);
		vertexBuffer[1] = GUIVertex(position2, base->color, glm::vec2(bilboard->textureCoord1.x, bilboard->textureCoord2.y));
		vertexBuffer[2] = GUIVertex(position3, base->color, glm::vec2(bilboard->textureCoord2.x, bilboard->textureCoord1.y));
											  
		vertexBuffer[3] = GUIVertex(position3, base->color, glm::vec2(bilboard->textureCoord2.x, bilboard->textureCoord1.y));
		vertexBuffer[4] = GUIVertex(position5, base->color, glm::vec2(bilboard->textureCoord1.x, bilboard->textureCoord2.y));
		vertexBuffer[5] = GUIVertex(position6, base->color, bilboard->textureCoord2);
		return 6;
	}

	GUINode make_billboard(const glm::vec2& textureCoord1, const glm::vec2& textureCoord2) {
		Engine2::GUINode node;
		node.base.position = glm::vec2(124.0, 834.0);
		node.base.size = glm::vec2(400.0, 200.0);
		node.adjustMod = Engine2::GUIAdjustMod::Fit;
		node.anchorX = false;
		node.anchorY = false;
		node.pivot = Engine2::GUIPivot::Centre;
		node.base.color = glm::vec4(1, 0, 0, 1);
		node.hash = 1111;
		for (size_t i = 0; i < 10; i++) {
			node.children[i] = nullptr;
		}
		node.generator.generator = generator;
		node.generator.input = bilboard_input;
		node.generator.userData = new BilboardData(textureCoord1, textureCoord2);
		return node;
	}
}; 