#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec2
#include <glm/vec4.hpp> // glm::vec2
#include "gui_billboard.h"

namespace Engine {

	struct BilboardData {
		vec2 textureCoord1;
		vec2 textureCoord2;
		BilboardData(vec2 textureCoord1, vec2 textureCoord2) : textureCoord1(textureCoord1), textureCoord2(textureCoord2) {}
	};

	bool bilboard_input(const vec2& clickPosition, const GUIBase* base, void* userData) {
		vec2 dim = base->size / 2.0f;

		if (-dim.x > clickPosition.x || dim.x < clickPosition.x)
			return false;

		if (-dim.y > clickPosition.y || dim.y < clickPosition.y)
			return false;

		int gg = 0;
		return true;
	}

	int generator(GUIVertex* vertexBuffer, const GUIBase* base, void* userData) {
		BilboardData* bilboard = reinterpret_cast<BilboardData*>(userData);
		vec2 dim = base->size / 2.0f;

			//2-----------------------3
			//|                       |
			//|                       |
			//|                       |
			//|                       |
			//|                       |
			//|                       |
			//|                       |
			//1-----------------------4

		vec3 position1(-dim.x, -dim.y, 0.0f); // 1
		vec3 position2(-dim.x, dim.y, 0.0f); // 2
		vec3 position3(dim.x, -dim.y, 0.0f); // 4

		vec3 position4(dim.x, -dim.y, 0.0f); // 4
		vec3 position5(-dim.x, dim.y, 0.0f); // 2
		vec3 position6(dim.x, dim.y, 0.0f); // 3

		vec2 textCoord3 = vec2(bilboard->textureCoord1.x, bilboard->textureCoord2.y);
		vec2 textCoord4 = vec2(bilboard->textureCoord2.x, bilboard->textureCoord1.y);

		for(auto i = 0; i < 6; i++) {
			vertexBuffer[i].color = base->color;
		}
		vertexBuffer[0].position = position1;
		vertexBuffer[0].texCoords = bilboard->textureCoord1;

		vertexBuffer[1].position = position2;
		vertexBuffer[1].texCoords = textCoord3;

		vertexBuffer[2].position = position3;
		vertexBuffer[2].texCoords = textCoord4;

		vertexBuffer[3].position = position4;
		vertexBuffer[3].texCoords = textCoord4;

		vertexBuffer[4].position = position5;
		vertexBuffer[4].texCoords = textCoord3;

		vertexBuffer[5].position = position6;
		vertexBuffer[5].texCoords = bilboard->textureCoord2;
		return 6;
	}

	GUINodeID make_billboard(GUIComposer& composer, const vec2& textureCoord1, const vec2& textureCoord2) {
		GUINodeID node = composer.makeNode();
		GUIPrimitive primitive;
		primitive.generator = generator;
		primitive.input = bilboard_input;
		primitive.userData = new BilboardData(textureCoord1, textureCoord2);
		composer.setPrimitive(primitive, node);

		return node;
	}
}; 