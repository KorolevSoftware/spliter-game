#include "engine/window.h"
#include "engine/graphics.h"
#include <SDL3_ttf/SDL_ttf.h>
#define SDL_MAIN_HANDLED true
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
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
    second.size.x = abs(depth.x/2.0f);
    second.size.z = abs(depth.z/2.0f);
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
    y_offset += nextBox.size.y*2;
    boxes.push_back(nextBox);
}


float lerp(float a, float b, float weight) {
    // Ключевой момент: сумма коэффициентов `weight` и `1 - weight` равна 1.
    return a * (1 - weight) + b * weight;
}

static SDL_Rect glyphs[255];


SDL_Surface* initFont(const char* filename) {
    TTF_Init();
    SDL_Surface* surface, * text;
    SDL_Rect dest {0};
    int i;
    char c[2];
    SDL_Rect* g;

    int texture_size = 512;

    auto font = TTF_OpenFont(filename, 64);
    
    surface = SDL_CreateSurface(texture_size, texture_size, SDL_PIXELFORMAT_BGRA32);

    SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGBA(surface, 0, 0, 0, 0));

    SDL_Color white{ 255, 255, 255, 255 };

    for (i = ' '; i <= 'z'; i++) {
        c[0] = i;
        c[1] = 0;

        text = TTF_RenderText_Blended(font, c, 1, white);

        TTF_GetStringSize(font, c,1, &dest.w, &dest.h);

        if (dest.x + dest.w >= texture_size) {
            dest.x = 0;

            dest.y += dest.h + 1;

            if (dest.y + dest.h >= texture_size) {
                SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Out of glyph space in %dx%d font atlas texture map.", texture_size, texture_size);
                exit(1);
            }
        }

        SDL_BlitSurface(text, NULL, surface, &dest);

        g = &glyphs[i];

        g->x = dest.x;
        g->y = dest.y;
        g->w = dest.w;
        g->h = dest.h;

        SDL_DestroySurface(text);

        dest.x += dest.w;
    }

    return surface;
}

Engine::GUINode composeText(std::string text) {
    Engine::GUINode root;
    root.position = glm::vec2(0, 250);
    root.size = glm::vec2(512, 512);
    root.adjustMod = Engine::GUIAdjustMod::Fit;
    root.anchorX = false;
    root.anchorY = false;
    root.pivot = Engine::GUIPivot::Centre;
    root.color = glm::vec4(1, 1, 1, 1);
    root.hash = 1111;
    root.visable = true;
    glm::vec2 text_pos(0, 0);
    int text_offset = 0;
    for (auto& ch : text) {
        SDL_Rect r_ch = glyphs[ch];

        Engine::GUINode n_ch;
        n_ch.textureCoord1 = glm::vec2(r_ch.x/512.0f, r_ch.y / 512.0f);
        n_ch.textureCoord2 = glm::vec2(n_ch.textureCoord1.x + r_ch.w / 512.0f, n_ch.textureCoord1.y + r_ch.h / 512.0f);
        std::swap(n_ch.textureCoord1, n_ch.textureCoord2);
        n_ch.textureCoord1.y = 1.0f - n_ch.textureCoord1.y;
        n_ch.textureCoord2.y = 1.0f - n_ch.textureCoord2.y;
        std::swap(n_ch.textureCoord1.x, n_ch.textureCoord2.x);

        n_ch.position = glm::vec2(text_offset + text_pos.x, text_pos.y);
        n_ch.size = glm::vec2(r_ch.w, r_ch.h);
        n_ch.adjustMod = Engine::GUIAdjustMod::Fit;
        n_ch.anchorX = false;
        n_ch.anchorY = false;
        n_ch.pivot = Engine::GUIPivot::West;
        n_ch.color = glm::vec4(1, 1, 1, 1);
        n_ch.hash = 0;
   
        root.childs.push_back(n_ch);
        text_offset += r_ch.w;
    }
    return root;
}


int main(int argc, char* argv[]) {
    Engine::Window main_window;
    main_window.initialize("Splitter", 600, 1200);

    Engine::Graphics main_graphics;
    main_graphics.initialize(main_window.GPUContext, main_window.getWidth(), main_window.getHeight());

    Engine::GUINode startButton;
    startButton.position = glm::vec2(300, 834);
    startButton.size = glm::vec2(400, 200);
    startButton.adjustMod = Engine::GUIAdjustMod::Fit;
    startButton.anchorX = false;
    startButton.anchorY = false;
    startButton.pivot = Engine::GUIPivot::Centre;
    startButton.color = glm::vec4(1, 1, 1, 1);
    startButton.hash = 1111;


    Engine::GUINode help;
    help.position = glm::vec2(69, 1100);
    help.size = glm::vec2(55, 55);
    help.adjustMod = Engine::GUIAdjustMod::Fit;
    help.anchorX = true;
    help.anchorY = false;
    help.pivot = Engine::GUIPivot::Centre;
    help.color = glm::vec4(0, 0, 1, 1);
    help.hash = 0;

    Engine::GUINode score;
    score.position = glm::vec2(528, 1100);
    score.size = glm::vec2(80, 55);
    score.adjustMod = Engine::GUIAdjustMod::Fit;
    score.anchorX = true;
    score.anchorY = false;
    score.pivot = Engine::GUIPivot::Centre;
    score.color = glm::vec4(0, 0, 1, 1);
    score.hash = 0;

    Engine::GUINode menu;
    menu.position = glm::vec2(300, 140);
    menu.size = glm::vec2(80, 55);
    menu.adjustMod = Engine::GUIAdjustMod::Fit;
    menu.anchorX = false;
    menu.anchorY = false;
    menu.pivot = Engine::GUIPivot::Centre;
    menu.color = glm::vec4(0, 0, 1, 1);
    menu.visable = false;
    menu.hash = 0;

    Engine::GUINode skins;
    skins.position = glm::vec2(-200, 0);
    skins.size = glm::vec2(80, 80);
    skins.adjustMod = Engine::GUIAdjustMod::Fit;
    skins.anchorX = false;
    skins.anchorY = false;
    skins.pivot = Engine::GUIPivot::Centre;
    skins.color = glm::vec4(0, 0, 1, 1);
    skins.hash = 0;

    Engine::GUINode noads;
    noads.position = glm::vec2(-65, 0);
    noads.size = glm::vec2(80, 80);
    noads.adjustMod = Engine::GUIAdjustMod::Fit;
    noads.anchorX = false;
    noads.anchorY = false;
    noads.pivot = Engine::GUIPivot::Centre;
    noads.color = glm::vec4(0, 0, 1, 1);
    noads.hash = 0;

    Engine::GUINode ratings;
    ratings.position = glm::vec2(70, 0);
    ratings.size = glm::vec2(80, 80);
    ratings.adjustMod = Engine::GUIAdjustMod::Fit;
    ratings.anchorX = false;
    ratings.anchorY = false;
    ratings.pivot = Engine::GUIPivot::Centre;
    ratings.color = glm::vec4(0, 0, 1, 1);
    ratings.hash = 0;

    Engine::GUINode settings;
    settings.position = glm::vec2(205, 0);
    settings.size = glm::vec2(80, 80);
    settings.adjustMod = Engine::GUIAdjustMod::Fit;
    settings.anchorX = false;
    settings.anchorY = false;
    settings.pivot = Engine::GUIPivot::Centre;
    settings.color = glm::vec4(0, 0, 1, 1);
    settings.hash = 0;

    menu.childs.push_back(skins);
    menu.childs.push_back(noads);
    menu.childs.push_back(ratings);
    menu.childs.push_back(settings);


    Engine::GUINode testStrech;
    testStrech.hash = 0;
    testStrech.position = glm::vec2(533, 603);
    testStrech.size = glm::vec2(200, 100);
    testStrech.adjustMod = Engine::GUIAdjustMod::Stretch;
    testStrech.anchorX = false;
    testStrech.anchorY = false;
    testStrech.pivot = Engine::GUIPivot::Centre;
    testStrech.color = glm::vec4(1, 1, 1, 1);
    testStrech.hash = 0;



    Engine::GUIComposer composer(4000);

    std::vector<uint8_t> tex_image;
    uint8_t depth;
    uint32_t width, height;

    SDL_Surface* img = initFont("arial.ttf");
    SDL_FlipSurface(img, SDL_FlipMode::SDL_FLIP_VERTICAL);
    tex_image.reserve(img->h* img->pitch);
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
    //mainBox.size = glm::vec3(0.8f, 0.4f, 0.8f);


    y_offset = mainBox.position.y + mainBox.size.y + 0.2f;

    float dir = -1.0f;
    nextSize = glm::vec3(0.8f, 0.2f, 0.8f);
    boxes.push_back(mainBox);
    createBox();
    main_graphics.setZoom(3);



  
    Engine::GUINode text = composeText("Hello world");

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

        if(moveByX) {
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
        composer.compose(text, localResolution, screenResolution, vec2Zero);
        /*composer.compose(help, localResolution, screenResolution, vec2Zero);
        composer.compose(score, localResolution, screenResolution, vec2Zero);
        composer.compose(menu, localResolution, screenResolution, vec2Zero);
        composer.compose(startButton, localResolution, screenResolution, vec2Zero);
        composer.compose(testStrech, localResolution, screenResolution, vec2Zero);*/

        // INPUT
        Engine::WindowEvent wEvent;
        while (main_window.getEvent(wEvent)) { // event loop
            if (wEvent.pressed) {
               
                mouse_pos.x = wEvent.posX;
                mouse_pos.y = wEvent.posY;
                spdlog::info("Click x: {} y:{}", wEvent.posX, wEvent.posY);
                spdlog::info("createBox");
                createBox();
                if (0 == composer.pickNode(mouse_pos)) {
                    
                }
            }
        }


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
//        sleep(16);
    }
    return 0;
}
