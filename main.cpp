#include "engine/window.h"
#include "engine/graphics.h"
#include <SDL_ttf.h>
#include <SDL_surface.h>
#include "engine/gui2.h"
#include "engine/gui_billboard.h"
#include "engine/gui_text.h"
#include <glm/trigonometric.hpp>

void createBox();

Engine::vec2 mouse_pos;
bool pick = false;
bool osEvents(Engine::Window& window) {
	return true;
}

Engine::vec2 localResolution = Engine::vec2(600.0f, 1200.0f);

float y_offset = 10;
float y_offset_off = 0;
static std::vector<Engine::Box> boxes;

bool moveByX = true;


bool clipBox(const Engine::Box& first, Engine::Box& second, glm::vec3& newSize) {
	glm::vec3 clipOrigins = first.position - second.position;
	clipOrigins.x = abs(clipOrigins.x);
	clipOrigins.y = abs(clipOrigins.y);
	clipOrigins.z = abs(clipOrigins.z);
	glm::vec3 depth = clipOrigins - first.size - second.size;

	if (depth.x > 0.0f || depth.z > 0.0f)
		return false;

	second.position.x = (first.position.x + second.position.x) * 0.5f;
	second.position.z = (first.position.z + second.position.z) * 0.5f;
	second.size.x = abs(depth.x / 2.0f);
	second.size.z = abs(depth.z / 2.0f);
	return true;
}

bool gameOver = false;
glm::vec3 nextSize;
void createBox() {
	size_t boxesCount = boxes.size();
	if (boxesCount > 1) {

		bool canByMake = clipBox(boxes[boxesCount - 2], boxes[boxesCount - 1], boxes[boxesCount - 1].size);

		if (!canByMake) {
			gameOver = true;
			return;
		}
		moveByX = !moveByX;
	}

	Engine::Box nextBox;
	if (moveByX) {
		nextBox.position = glm::vec3(-3.0f, 0, 0);
		if (boxesCount > 0)
			nextBox.position.z = boxes[boxesCount - 1].position.z;
	} else {
		nextBox.position = glm::vec3(boxes[boxesCount - 1].position.x, 0, -3.0f);
		if (boxesCount > 0)
			nextBox.position.x = boxes[boxesCount - 1].position.x;
	}
	nextBox.position.y = y_offset;
	nextBox.size = boxes.back().size;
	y_offset += nextBox.size.y * 2;
	boxes.push_back(nextBox);
}


float lerp(float a, float b, float weight) {
	// Ключевой момент: сумма коэффициентов `weight` и `1 - weight` равна 1.
	return a * (1 - weight) + b * weight;
}

static Engine::Patch glyphs[255];


SDL_Surface* initFont(char* filename) {
	TTF_Init();
	SDL_Surface* surface, * text;
	Engine::Patch dest;
	int i;
	char c[2];

	int texture_size = 512;

	auto font = TTF_OpenFont(filename, 64);

	surface = SDL_CreateRGBSurface(0, texture_size, texture_size, 32, 0, 0, 0, 0xff);

	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGBA(surface->format, 0, 0, 0, 0));

	dest.origin = Engine::i16vec2(0, 0);
	SDL_Color white{ 255, 255, 255, 255 };

	for (i = ' '; i <= 'z'; i++) {
		c[0] = i;
		c[1] = 0;

		text = TTF_RenderUTF8_Blended(font, c, white);

		int charWidth;
		int charHeight;
		TTF_SizeText(font, c, &charWidth, &charHeight);
		dest.size = Engine::i16vec2(charWidth, charHeight);
		if (dest.origin.x + dest.size.x >= texture_size) {
			dest.origin.x = 0;

			dest.origin.y += dest.size.y + 1;

			if (dest.origin.y + dest.size.y >= texture_size) {
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Out of glyph space in %dx%d font atlas texture map.", texture_size, texture_size);
				exit(1);
			}
		}
		SDL_Rect rr;
 		rr.x = dest.origin.x;
		rr.y = dest.origin.y;
		rr.w = dest.size.x;
		rr.h = dest.size.y;
		SDL_BlitSurface(text, NULL, surface, &rr);

		Engine::Patch& g = glyphs[i];

		//start----------------------             		//----------------------------end
		//|							|			  		//|							  |
		//|							|             		//|							  |
		//|							|			  		//|							  |
		//|							|		=>	  		//|							  |
		//|							|			  		//|							  |
		//|							|			  		//|							  |
		//|							|			  		//|							  |
		//------------------------end			  		//start------------------------

		g.origin.x = rr.x;
		g.origin.y = rr.h + rr.y;
		g.size.x = rr.w + rr.x;
		g.size.y = rr.y;

		SDL_FreeSurface(text);
		dest.origin.x += rr.w;
	}

	return surface;
}

int main(int argc, char* argv[]) {
	Engine::Window main_window;
	main_window.initialize("Splitter", 600, 1200);

	Engine::Graphics main_graphics;
	main_graphics.initialize();

	Engine::GUIComposer composer(4000, 40);

	std::vector<uint8_t> tex_image;
	uint8_t depth;
	uint32_t width, height;

	SDL_Surface* img = initFont("arial.ttf");

	tex_image.reserve(img->h * img->pitch);
	std::copy(
		reinterpret_cast<uint8_t*>(img->pixels),
		reinterpret_cast<uint8_t*>(img->pixels) + img->h * img->pitch,
		std::back_inserter(tex_image)
	);
	depth = img->pitch / img->w;
	width = img->w;
	height = img->h;

	glm::vec3(glm::vec2(1, 2), 1);

	//main_window.loadImage("cat.png", tex_image, depth, width, height);
	main_graphics.setImage(tex_image, width, height, depth);

	Engine::Box mainBox;
	mainBox.position = glm::vec3(0.0f, -2.2f, 0.0f);
	mainBox.size = glm::vec3(0.8f, 0.1f, 0.8f);

	y_offset = mainBox.position.y + mainBox.size.y + 0.2f;

	float dir = -1.0f;
	nextSize = glm::vec3(0.8f, 0.2f, 0.8f);
	boxes.push_back(mainBox);
	createBox();
	main_graphics.setZoom(3);

	//Engine::GUINode text = composeText("Hello world");
	Engine::GUINodeID testQuad = Engine::make_billboard(composer, Engine::vec2(0.0f, 0.0f), Engine::vec2(1.0f, 1.0f));
	composer.setPosition(testQuad, Engine::vec3(124.0, 834.0, 0));
	composer.setSize(testQuad, Engine::vec2(400.0f, 200.0f));
	composer.setScale(testQuad, Engine::vec3(0.5f, 1.0f, 1.0f));
	composer.setRotationZ(testQuad, Engine::radians(45.0f));
	composer.setColor(testQuad, Engine::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	composer.setAdjustMode(testQuad, Engine::GUIAdjustMode::Stretch);

	Engine::GUINodeID box5 = Engine::make_billboard(composer, Engine::vec2(0.0f, 0.0f), Engine::vec2(1.0f, 1.0f));
	composer.setPosition(box5, Engine::vec3(139.0f, 0, 0));
	composer.setRotationZ(box5, Engine::radians(45.0f));
	composer.setSize(box5, Engine::vec2(100.0, 100.0));
	composer.setScale(box5, Engine::vec3(0.5, 1.0f, 1.0f));
	composer.setColor(box5, Engine::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	composer.setChildren(testQuad, box5);
	composer.setAnchorX(box5, true);
	composer.setAdjustMode(box5, Engine::GUIAdjustMode::Zoom);

	Engine::GUINodeID box6 = Engine::make_billboard(composer, Engine::vec2(0.0f, 0.0f), Engine::vec2(1.0f, 1.0f));
	composer.setPosition(box6, Engine::vec3(-50.0f, 0, 0));
	composer.setSize(box6, Engine::vec2(50.0f, 50.0));
	composer.setColor(box6, Engine::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	composer.setChildren(box5, box6);
	composer.setAdjustMode(box6, Engine::GUIAdjustMode::Fit);
	composer.setPivot(box6, Engine::GUIPivot::East);

	Engine::Font font;
	for (size_t i = 0; i < 255; i++) {
		font.glyphs[i] = glyphs[i];
	}

	Engine::GUINodeID box_text = Engine::make_text(composer, "Hello!", &font);
	composer.setChildren(box5, box_text);

	uint32_t size;
	auto arr = composer.getChildrens(box5, &size);
	float angle = 0;
	while (true) { // engine loop
		Engine::Box& select = boxes.back();

		if (select.position.x > 3.0f && moveByX) {
			dir = -1;
		}

		if (select.position.x < -3.0f && moveByX) {
			dir = 1;
		}

		if (select.position.z > 3.0f && !moveByX) {
			dir = -1;
		}

		if (select.position.z < -3.0f && !moveByX) {
			dir = 1;
		}
		angle += 1;
		//composer.setRotationZ(box5, Engine::radians(angle));
		if (moveByX) {
			select.position.x += dir * 0.016;
		} else {
			select.position.z += dir * 0.016;
		}

		if (gameOver) {
			main_graphics.setZoom(5);
		}
		y_offset_off = lerp(y_offset_off, y_offset, 0.016);

		Engine::vec2 screenResolution((float)main_window.getWidth(), (float)main_window.getHeight());
		Engine::vec2 vec2Zero(0.0f);

		composer.compose(testQuad, localResolution, screenResolution, vec2Zero);
		// INPUT
		Engine::WindowEvent wEvent;
		while (main_window.getEvent(wEvent)) { // event loop
			if (wEvent.pressed) {
				mouse_pos.x = wEvent.posX;
				mouse_pos.y = wEvent.posY;
				//spdlog::info("Click x: {} y:{}", wEvent.posX, wEvent.posY);
				//spdlog::info("createBox");
				if (composer.pickNode(box6, mouse_pos)) {
					createBox();
					spdlog::info("Hit");
				}
			}
		}
		main_graphics.beginDraw(main_window.getWidth(), main_window.getHeight());
		{
			// Draw gui
			//main_graphics.drawBoxes(boxes);
			main_graphics.setCameraOffsetY(y_offset_off);

			// TODO fix gui

			main_graphics.drawGui(composer.getBufferData(), composer.getRenderBufSizeof(), composer.getVertexCount());

			//mainBox.size = glm::vec3(2.0f);
		}
		main_graphics.endDraw();
		main_window.present();
	}
	return 0;
}