#include "engine/window.h"
#include "engine/graphics.h"

bool osEvents(Engine::Window& window) {
    Engine::WindowEvent wEvent;

    while (window.getEvent(wEvent)) { // event loop

    }
    return true;
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

    Engine::GUINode help;
    help.position = glm::vec2(69, 1100);
    help.size = glm::vec2(55, 55);
    help.adjustMod = Engine::GUIAdjustMod::Fit;
    help.anchorX = true;
    help.anchorY = false;
    help.pivot = Engine::GUIPivot::Centre;
    help.color = glm::vec4(0, 0, 1, 1);

    Engine::GUINode score;
    score.position = glm::vec2(528, 1100);
    score.size = glm::vec2(80, 55);
    score.adjustMod = Engine::GUIAdjustMod::Fit;
    score.anchorX = true;
    score.anchorY = false;
    score.pivot = Engine::GUIPivot::Centre;
    score.color = glm::vec4(0, 0, 1, 1);

    Engine::GUINode menu;
    menu.position = glm::vec2(300, 140);
    menu.size = glm::vec2(80, 55);
    menu.adjustMod = Engine::GUIAdjustMod::Fit;
    menu.anchorX = false;
    menu.anchorY = false;
    menu.pivot = Engine::GUIPivot::Centre;
    menu.color = glm::vec4(0, 0, 1, 1);

    Engine::GUINode skins;
    skins.position = glm::vec2(-200, 0);
    skins.size = glm::vec2(80, 80);
    skins.adjustMod = Engine::GUIAdjustMod::Fit;
    skins.anchorX = false;
    skins.anchorY = false;
    skins.pivot = Engine::GUIPivot::Centre;
    skins.color = glm::vec4(0, 0, 1, 1);

    Engine::GUINode noads;
    noads.position = glm::vec2(-65, 0);
    noads.size = glm::vec2(80, 80);
    noads.adjustMod = Engine::GUIAdjustMod::Fit;
    noads.anchorX = false;
    noads.anchorY = false;
    noads.pivot = Engine::GUIPivot::Centre;
    noads.color = glm::vec4(0, 0, 1, 1);

    Engine::GUINode ratings;
    ratings.position = glm::vec2(70, 0);
    ratings.size = glm::vec2(80, 80);
    ratings.adjustMod = Engine::GUIAdjustMod::Fit;
    ratings.anchorX = false;
    ratings.anchorY = false;
    ratings.pivot = Engine::GUIPivot::Centre;
    ratings.color = glm::vec4(0, 0, 1, 1);

    Engine::GUINode settings;
    settings.position = glm::vec2(205, 0);
    settings.size = glm::vec2(80, 80);
    settings.adjustMod = Engine::GUIAdjustMod::Fit;
    settings.anchorX = false;
    settings.anchorY = false;
    settings.pivot = Engine::GUIPivot::Centre;
    settings.color = glm::vec4(0, 0, 1, 1);

    menu.childs.push_back(skins);
    menu.childs.push_back(noads);
    menu.childs.push_back(ratings);
    menu.childs.push_back(settings);

    Engine::GUINode testStrech;
    testStrech.position = glm::vec2(533, 603);
    testStrech.size = glm::vec2(200, 100);
    testStrech.adjustMod = Engine::GUIAdjustMod::Stretch;
    testStrech.anchorX = false;
    testStrech.anchorY = false;
    testStrech.pivot = Engine::GUIPivot::Centre;
    testStrech.color = glm::vec4(1, 1, 1, 1);


    Engine::GUIComposer composer(500);

    std::vector<uint8_t> tex_image;
    uint8_t depth;
    uint32_t width, height;
    main_window.loadImage("cat.png", tex_image, depth, width, height);
    main_graphics.setImage(tex_image, width, height, depth);
    while (true) { // engine loop
        osEvents(main_window);
    
        main_graphics.beginDraw(main_window.getWidth(), main_window.getHeight());

        // Draw gui
        composer.compose(help, glm::vec2(600, 1200), glm::vec2((float)main_window.getWidth(), (float)main_window.getHeight()), glm::vec2(0));
        composer.compose(score, glm::vec2(600, 1200), glm::vec2((float)main_window.getWidth(), (float)main_window.getHeight()), glm::vec2(0));
        composer.compose(menu, glm::vec2(600, 1200), glm::vec2((float)main_window.getWidth(), (float)main_window.getHeight()), glm::vec2(0));
        composer.compose(startButton, glm::vec2(600, 1200), glm::vec2((float)main_window.getWidth(), (float)main_window.getHeight()), glm::vec2(0));
        composer.compose(testStrech, glm::vec2(600, 1200), glm::vec2((float)main_window.getWidth(), (float)main_window.getHeight()), glm::vec2(0));

        main_graphics.drawGui(composer.getBufferData(), composer.getRenderBufSizeof(), composer.getVertexCount());
        

        main_graphics.endDraw();
        main_window.present();
        composer.clearVertexBuffer();
    }
    return 0;
}