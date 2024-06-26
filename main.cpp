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
    node1.size = glm::vec2(100, 100);
    node1.adjustMod = Engine::GUIAdjustMod::Stretch;

    Engine::GUINode node2;
    node2.position = glm::vec2(600, 0);
    node2.size = glm::vec2(100, 100);

    Engine::GUINode node3;
    node3.position = glm::vec2(0, 600);
    node3.size = glm::vec2(100, 100);

    Engine::GUINode node4;
    node4.position = glm::vec2(600, 600);
    node4.size = glm::vec2(100, 100);



    while (true) { // engine loop
        osEvents(main_window);
    
        main_graphics.beginDraw(main_window.getWidth(), main_window.getHeight());
        main_graphics.drawGui(node1);

        main_graphics.endDraw();

        main_window.present();
    }
    return 0;
}