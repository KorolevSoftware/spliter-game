#include "engine/window.h"
#include "engine/graphics.h"
#include <SDL_ttf.h>
#include <SDL_surface.h>
#include "engine/gui2.h"
#include "engine/gui_billboard.h"
#include "engine/gui_text.h"

void createBox();

glm::vec2 mouse_pos;
bool pick = false;
bool osEvents(Engine::Window& window) {

	return true;
}

glm::vec2 localResolution = glm::vec2(600, 1200);

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

static Engine2::Rectangle glyphs[255];


SDL_Surface* initFont(char* filename) {
	TTF_Init();
	SDL_Surface* surface, * text;
	Engine2::Rectangle dest;
	int i;
	char c[2];
	Engine2::Rectangle* g;

	int texture_size = 512;

	auto font = TTF_OpenFont(filename, 64);

	surface = SDL_CreateRGBSurface(0, texture_size, texture_size, 32, 0, 0, 0, 0xff);

	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGBA(surface->format, 0, 0, 0, 0));

	dest.x = dest.y = 0;
	SDL_Color white{ 255, 255, 255, 255 };

	for (i = ' '; i <= 'z'; i++) {
		c[0] = i;
		c[1] = 0;

		text = TTF_RenderUTF8_Blended(font, c, white);

		TTF_SizeText(font, c, &dest.w, &dest.h);

		if (dest.x + dest.w >= texture_size) {
			dest.x = 0;

			dest.y += dest.h + 1;

			if (dest.y + dest.h >= texture_size) {
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Out of glyph space in %dx%d font atlas texture map.", texture_size, texture_size);
				exit(1);
			}
		}
		SDL_Rect rr;
 		rr.x = dest.x;
		rr.y = dest.y;
		rr.w = dest.w;
		rr.h = dest.h;
		SDL_BlitSurface(text, NULL, surface, &rr);

		g = &glyphs[i];

		//start----------------------             		//----------------------------end
		//|							|			  		//|							  |
		//|							|             		//|							  |
		//|							|			  		//|							  |
		//|							|		=>	  		//|							  |
		//|							|			  		//|							  |
		//|							|			  		//|							  |
		//|							|			  		//|							  |
		//------------------------end			  		//start------------------------

		g->x = rr.x;
		g->y = rr.h+ rr.y;
		g->w = rr.w;
		g->h = -rr.h;

		SDL_FreeSurface(text);

		dest.x += rr.w;
	}

	return surface;
}

int main(int argc, char* argv[]) {
	Engine::Window main_window;
	main_window.initialize("Splitter", 600, 1200);

	Engine::Graphics main_graphics;
	main_graphics.initialize();

	Engine2::GUIComposer composer(4000);

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
	Engine2::GUINode testQuad = Engine2::make_billboard(glm::vec2(0, 0), glm::vec2(1, 1));
	Engine2::GUINode testQuad2 = Engine2::make_billboard(glm::vec2(0, 0), glm::vec2(1, 1));
	Engine2::GUINode testQuad3 = Engine2::make_billboard(glm::vec2(0, 0), glm::vec2(1, 1));
	testQuad.children[0] = &testQuad2;
	testQuad.base.angle = 0;
	testQuad.adjustMod = Engine2::GUIAdjustMod::Stretch;
	testQuad.hash = 1;
	testQuad.base.scale = glm::vec2(0.5, 1);

	testQuad2.base.size = glm::vec2(100);
	testQuad2.base.position = glm::vec2(139.0, 0);
	testQuad2.base.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	testQuad2.base.angle = 0;
	testQuad2.anchorX = true;
	testQuad2.adjustMod = Engine2::GUIAdjustMod::Zoom;
	testQuad2.pivot = Engine2::GUIPivot::Centre;
	testQuad2.children[0] = &testQuad3;
	testQuad2.hash = 2;
	testQuad2.base.scale = glm::vec2(0.5, 1);

	testQuad3.base.size = glm::vec2(50);
	testQuad3.base.angle = 0;
	testQuad3.base.position = glm::vec2(-50, 0);
	testQuad3.base.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	testQuad3.hash = 3;


	Engine2::Font font;
	for (size_t i = 0; i < 255; i++) {
		font.glyphs[i] = glyphs[i];
	}
	Engine2::GUINode text = Engine2::make_text("", &font);
	text.base.angle = 0;
	testQuad2.children[1] = &text;
	//text.base.scale = glm::vec2(1);


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
		//testQuad2.base.angle += 0.1;
		if (moveByX) {
			select.position.x += dir * 0.016;
		} else {
			select.position.z += dir * 0.016;
		}

		if (gameOver) {
			main_graphics.setZoom(5);
		}
		y_offset_off = lerp(y_offset_off, y_offset, 0.016);

		glm::vec2 screenResolution = glm::vec2((float)main_window.getWidth(), (float)main_window.getHeight());
		glm::vec2 vec2Zero(0);

		composer.compose(testQuad, localResolution, screenResolution, vec2Zero);
		// INPUT
		Engine::WindowEvent wEvent;
		while (main_window.getEvent(wEvent)) { // event loop
			if (wEvent.pressed) {

				mouse_pos.x = wEvent.posX;
				mouse_pos.y = wEvent.posY;
				//spdlog::info("Click x: {} y:{}", wEvent.posX, wEvent.posY);
				//spdlog::info("createBox");
				if (composer.pickNode(3, mouse_pos)) {
					createBox();
					spdlog::info("Hit");
				}
			}
		}
		//testQuad2.base.angle = testQuad2.base.angle + 0.1;
		main_graphics.beginDraw(main_window.getWidth(), main_window.getHeight());
		{
			// Draw gui
			main_graphics.drawBoxes(boxes);
			main_graphics.setCameraOffsetY(y_offset_off);

			// TODO fix gui

			main_graphics.drawGui(composer.getBufferData(), composer.getRenderBufSizeof(), composer.getVertexCount());

			//mainBox.size = glm::vec3(2.0f);
		}
		main_graphics.endDraw();
		main_window.present();
		composer.clearVertexBuffer();
	}
	return 0;
}