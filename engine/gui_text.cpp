#include <glm/vec2.hpp> // glm::vec2
#include "gui_text.h"
#include "string"
namespace Engine2 {

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

	int generator_text(GUIVertex* vertexBuffer, const GUIBase* base, void* userData) {
		TextData* textData = reinterpret_cast<TextData*>(userData);
		glm::vec2 text_pos(0, 0);
		int text_offset = 0;
		int vertexAddCount = 0;
		for (auto& ch : textData->text) {
			Rectangle r_ch = textData->font->glyphs[ch];
			text_offset -= r_ch.w;
		}
		text_offset /= 2;
		for (auto& ch : textData->text) {
			Rectangle r_ch = textData->font->glyphs[ch];

			glm::vec2 textureCoord1 = glm::vec2(r_ch.x / 512.0f, r_ch.y / 512.0f);
			glm::vec2 textureCoord2 = glm::vec2(textureCoord1.x + r_ch.w / 512.0f, textureCoord1.y + r_ch.h / 512.0f);
			std::swap(textureCoord1, textureCoord2);
			textureCoord1.y = 1.0f - textureCoord1.y;
			textureCoord2.y = 1.0f - textureCoord2.y;
			std::swap(textureCoord1.x, textureCoord2.x);

			glm::vec3 position = glm::vec3(text_offset + text_pos.x, text_pos.y, 0);
			glm::vec2 size = glm::vec2(r_ch.w, r_ch.h);
			vertexAddCount += make_char(&vertexBuffer[vertexAddCount], glm::vec4(0.0f, 0.0f,0.0f, 1.0f), position, size, textureCoord1, textureCoord2);
			text_offset += r_ch.w;
		}
		return vertexAddCount;
	}

	GUINode make_text(const std::string& text, Font* font) {
		Engine2::GUINode node;
		node.base.position = glm::vec2(0, 0);
		node.base.size = glm::vec2(200.0, 100.0);
		node.adjustMod = Engine2::GUIAdjustMod::Fit;
		node.anchorX = false;
		node.anchorY = false;
		node.pivot = Engine2::GUIPivot::Centre;
		node.base.color = glm::vec4(1, 0, 0, 1);
		node.hash = 1111;
		for (size_t i = 0; i < 10; i++) {
			node.children[i] = nullptr;
		}
		node.generator.generator = generator_text;
		node.generator.userData = new TextData(text, font);
		return node;
	}
};