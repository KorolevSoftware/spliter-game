#include "engine/window.h"
#include "engine/graphics.h"

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

int main(int argc, char* argv[]) {
    Engine::Window main_window;
    main_window.initialize("Splitter", 600, 1200);

    Engine::Graphics main_graphics;
    main_graphics.initialize();

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
    Engine::GUIComposer composer(500);

    std::vector<uint8_t> tex_image;
    uint8_t depth;
    uint32_t width, height;
    main_window.loadImage("cat.png", tex_image, depth, width, height);
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
        composer.compose(help, localResolution, screenResolution, vec2Zero);
        composer.compose(score, localResolution, screenResolution, vec2Zero);
        composer.compose(menu, localResolution, screenResolution, vec2Zero);
        composer.compose(startButton, localResolution, screenResolution, vec2Zero);
        composer.compose(testStrech, localResolution, screenResolution, vec2Zero);

        // INPUT
        Engine::WindowEvent wEvent;
        while (main_window.getEvent(wEvent)) { // event loop
            if (wEvent.pressed) {
               
                mouse_pos.x = wEvent.posX;
                mouse_pos.y = wEvent.posY;
                spdlog::info("Click x: {} y:{}", wEvent.posX, wEvent.posY);
                spdlog::info("createBox");
                if (0 == composer.pickNode(mouse_pos)) {
                    createBox();
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
    }
    return 0;
}