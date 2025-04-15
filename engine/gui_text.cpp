#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec2
#include <glm/vec4.hpp> // glm::vec2
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

	int make_char(GUIVertex* vertexBuffer, const glm::vec4& color, const glm::vec3& position, const glm::vec2& size, const glm::vec2& textureCoord1, const glm::vec2& textureCoord2) {
		glm::vec2 dim = size;

		//2-----------------------3
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//|                       |
		//1-----------------------4

		glm::vec3 position1(0, 0, 0.0f); // 1
		glm::vec3 position2(0, dim.y, 0.0f); // 2
		glm::vec3 position3(dim.x, 0, 0.0f); // 4

		glm::vec3 position4(dim.x, 0, 0.0f); // 4
		glm::vec3 position5(0, dim.y, 0.0f); // 2
		glm::vec3 position6(dim.x, dim.y, 0.0f); // 3

		vertexBuffer[0] = GUIVertex(position1 + position, color, textureCoord1);
		vertexBuffer[1] = GUIVertex(position2 + position, color, glm::vec2(textureCoord1.x, textureCoord2.y));
		vertexBuffer[2] = GUIVertex(position3 + position, color, glm::vec2(textureCoord2.x, textureCoord1.y));

		vertexBuffer[3] = GUIVertex(position3 + position, color, glm::vec2(textureCoord2.x, textureCoord1.y));
		vertexBuffer[4] = GUIVertex(position5 + position, color, glm::vec2(textureCoord1.x, textureCoord2.y));
		vertexBuffer[5] = GUIVertex(position6 + position, color, textureCoord2);
		return 6;
	}

	bool text_input(const glm::vec2& clickPosition, const GUIBase* base, void* userData) {
		return false;
	}

	int generator_text(GUIVertex* vertexBuffer, const GUIBase* base, void* userData) {
		TextData* textData = reinterpret_cast<TextData*>(userData);

		int text_offset = 0;
		int vertexAddCount = 0;
		float halfHeight = 0;

		if (!textData->text.empty()) {
			const char first = textData->text[0];
			halfHeight = textData->font->glyphs[first].h / 2;
		}

		for (auto& ch : textData->text) {
			Rectangle r_ch = textData->font->glyphs[ch];
			text_offset -= r_ch.w;
		}

		text_offset /= 2;
		for (auto& ch : textData->text) {
			Rectangle r_ch = textData->font->glyphs[ch];

			glm::vec4 normalize = glm::vec4(r_ch.x, r_ch.y, r_ch.w, r_ch.h) / 512.0f;
			glm::vec2 textureCoord1 = glm::vec2(normalize.x, normalize.y);
			glm::vec2 textureCoord2 = textureCoord1 + glm::vec2(normalize.z, normalize.w);

			glm::vec3 position = glm::vec3(text_offset, halfHeight, 0);
			glm::vec2 size = glm::vec2(r_ch.w, -r_ch.h);
			vertexAddCount += make_char(&vertexBuffer[vertexAddCount], glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), position, size, textureCoord1, textureCoord2);
			text_offset += r_ch.w;
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