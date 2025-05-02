#include "gui_text.h"
#include "string"
namespace Engine {

	struct TextData {
		std::string text;
		Font* font;
		TextData(const std::string& text, Font* font) {
			this->text = text;
			this->font = font;
		}
	};

	int make_char(GUIVertex* vertexBuffer, const vec4& color, const vec3& position, const vec2& dim, const vec2& textureCoord1, const vec2& textureCoord2) {
		//2-----------------------3
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//1-----------------------4

		vec3 position1(0, 0, 0.0f); // 1
		vec3 position2(0, dim.y, 0.0f); // 2
		vec3 position3(dim.x, 0, 0.0f); // 4

		vec3 position4(dim.x, 0, 0.0f); // 4
		vec3 position5(0, dim.y, 0.0f); // 2
		vec3 position6(dim.x, dim.y, 0.0f); // 3

		vertexBuffer[0] = GUIVertex(position1 + position, color, textureCoord1);
		vertexBuffer[1] = GUIVertex(position2 + position, color, vec2(textureCoord1.x, textureCoord2.y));
		vertexBuffer[2] = GUIVertex(position3 + position, color, vec2(textureCoord2.x, textureCoord1.y));

		vertexBuffer[3] = GUIVertex(position3 + position, color, vec2(textureCoord2.x, textureCoord1.y));
		vertexBuffer[4] = GUIVertex(position5 + position, color, vec2(textureCoord1.x, textureCoord2.y));
		vertexBuffer[5] = GUIVertex(position6 + position, color, textureCoord2);
		return 6;
	}

	bool text_input(const vec2& clickPosition, const GUIBase* base, void* userData) {
		return false;
	}

	int generator_text(GUIVertex* vertexBuffer, const GUIBase* base, void* userData) {
		TextData* textData = reinterpret_cast<TextData*>(userData);

		int textOffestX = 0;
		int vertexAddCount = 0;
		float textOffsetY = 0;

		if (textData->text.empty()) {
			return 0;

		}
		auto first = textData->text[0];
		auto patch = textData->font->glyphs[first];
		textOffsetY = (patch.origin.y - patch.size.y)/ 2;


		for (auto& ch : textData->text) { // Offset by X axis
			Patch r_ch = textData->font->glyphs[ch];
			textOffestX -= r_ch.size.x - r_ch.origin.x;
		}

		textOffestX /= 2;
		for (auto& ch : textData->text) {
			Patch r_ch = textData->font->glyphs[ch];

			vec4 normalize = vec4(r_ch.origin, r_ch.size) / 512.0f;
			vec2 textureCoord1 = vec2(normalize.x, normalize.y);
			vec2 textureCoord2 = vec2(normalize.z, normalize.p);

			vec3 position = vec3(textOffestX,-textOffsetY, 0);
			vec2 size = vec2(
				r_ch.size.x - r_ch.origin.x,
				r_ch.origin.y - r_ch.size.y
			);

			vertexAddCount += make_char(&vertexBuffer[vertexAddCount], vec4(0.0f, 0.0f, 0.0f, 0.0f), position, size, textureCoord1, textureCoord2);
			textOffestX += size.x;
		}
		return vertexAddCount;
	}

	GUINodeID make_text(GUIComposer& composer, const std::string& text, Font* font) {
		GUINodeID node = composer.makeNode();
		GUIPrimitive primitive;
		primitive.generator = generator_text;
		primitive.input = text_input;
		primitive.userData = new TextData(text, font);
		composer.setPrimitive(primitive, node);

		return node;
	}
};