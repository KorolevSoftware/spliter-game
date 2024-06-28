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
    node1.color = glm::vec4(1, 1, 1, 1);

    Engine::GUINode node2;
    node2.position = glm::vec2(100, 0);
    node2.size = glm::vec2(100, 100);
    node2.adjustMod = Engine::GUIAdjustMod::Fit;
    node2.anchorX = false;
    node2.anchorY = false;
    node2.pivot = Engine::GUIPivot::Centre;
    node2.color = glm::vec4(0, 0, 1, 1);

    node1.childs.push_back(node2);

    Engine::GUIComposer composer(100);

    std::vector<uint8_t> tex_image;
    uint8_t depth;
    uint32_t width, height;
    main_window.loadImage("cat.png", tex_image, depth, width, height);
    main_graphics.setImage(tex_image, width, height, depth);
    while (true) { // engine loop
        osEvents(main_window);
    
        main_graphics.beginDraw(main_window.getWidth(), main_window.getHeight());

        // Draw gui
        composer.compose(node1, glm::vec2(600, 600), glm::vec2((float)main_window.getWidth(), (float)main_window.getHeight()), glm::vec2(0));
        main_graphics.drawGui(composer.getBufferData(), composer.getRenderBufSizeof(), composer.getVertexCount());
        

        main_graphics.endDraw();
        main_window.present();
        composer.clearVertexBuffer();
    }
    return 0;
}