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
    main_window.initialize("Splitter", 400, 600);

    Engine::Graphics main_graphics;
    main_graphics.initialize();

    while (true) { // engine loop
        osEvents(main_window);
        main_window.present();
    }
    return 0;
}