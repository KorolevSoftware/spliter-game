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
    main_window.initialize("Splitter", 600, 600);

    Engine::Graphics main_graphics;
    main_graphics.initialize();

    Engine::GUINode node1;
    node1.position = glm::vec2(300,300);
    node1.size = glm::vec2(300, 300);
    node1.adjustMod = Engine::GUIAdjustMod::Fit;
    node1.anchorX = false;
    node1.anchorY = false;
    node1.pivot = Engine::GUIPivot::Centre;
    node1.color = glm::vec4(1, 0, 0, 1);
    node1.parent = nullptr;

    Engine::GUINode node2;
    node2.position = glm::vec2(100, 0);
    node2.size = glm::vec2(100, 100);
    node2.adjustMod = Engine::GUIAdjustMod::Fit;
    node2.anchorX = false;
    node2.anchorY = false;
    node2.pivot = Engine::GUIPivot::Centre;
    node2.color = glm::vec4(0, 0, 1, 1);
    node2.parent = &node1;

    while (true) { // engine loop
        osEvents(main_window);
    
        main_graphics.beginDraw(main_window.getWidth(), main_window.getHeight());
        main_graphics.drawGui(node1);
        main_graphics.drawGui(node2);

        main_graphics.endDraw();

        main_window.present();
    }
    return 0;
}