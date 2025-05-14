#include "gui_billboard.h"

namespace Engine {
	BillboardData::BillboardData(vec2 textureCoord1, vec2 textureCoord2) : textureCoord1(textureCoord1), textureCoord2(textureCoord2) {}

	bool BillboardData::input(const vec2& clickPosition, const GUIBase& base)  {
		vec2 dim = base.size / 2.0f;

		if (-dim.x > clickPosition.x || dim.x < clickPosition.x)
			return false;

		if (-dim.y > clickPosition.y || dim.y < clickPosition.y)
			return false;

		return true;
	}

	int BillboardData::generator(GUIVertex* vertexBuffer, const GUIBase& base, const std::vector<Atlas>& atlasBuffer) {
		vec2 dim = base.size / 2.0f;

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

		vec2 textCoord3 = vec2(this->textureCoord1.x, this->textureCoord2.y);
		vec2 textCoord4 = vec2(this->textureCoord2.x, this->textureCoord1.y);

		for(auto i = 0; i < 6; i++) {
			vertexBuffer[i].color = base.color;
		}

		vertexBuffer[0].position = position1;
		vertexBuffer[0].texCoords = this->textureCoord1;

		vertexBuffer[1].position = position2;
		vertexBuffer[1].texCoords = textCoord3;

		vertexBuffer[2].position = position3;
		vertexBuffer[2].texCoords = textCoord4;

		vertexBuffer[3].position = position4;
		vertexBuffer[3].texCoords = textCoord4;

		vertexBuffer[4].position = position5;
		vertexBuffer[4].texCoords = textCoord3;

		vertexBuffer[5].position = position6;
		vertexBuffer[5].texCoords = this->textureCoord2;
		return 6;
	}


	GUINodeID make_billboard(GUIComposer& composer, uint32_t atlasID, uint32_t patchID) {
		const Atlas& atlas = composer.atlasBuffer[atlasID];
		const Patch& patch = atlas.images.find(patchID)->second;

		GUINodeID node = composer.makeNode();

		vec2 textureCoord1 = vec2(patch.origin) / vec2(atlas.width, atlas.height);
		vec2 textureCoord2 = vec2(patch.size) / vec2(atlas.width, atlas.height);
		IGUIPrimitive* primitive = new BillboardData(textureCoord1, textureCoord2);
		composer.setPrimitive(primitive, node);
		return node;
	}
};