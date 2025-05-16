#pragma once
#include "gui2.h"

namespace Engine {
	struct BillboardData : IGUIPrimitive {
		vec2 textureCoord1;
		vec2 textureCoord2;
		uint32_t atlasID;
		BillboardData(vec2 textureCoord1, vec2 textureCoord2, uint32_t atlasID);
		GUIDrawCommand generator(GUIVertex* vertexBuffer, const GUIBase& base, const std::vector<Atlas>& atlasBuffer) override;
		bool input(const vec2& clickPosition, const GUIBase& base) override;
	};

	GUINodeID make_billboard(GUIComposer& composer, uint32_t atlasID, uint32_t patchID);
};