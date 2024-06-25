#include "engine/window.h"

bool osEvents(Engine::Window& window) {
    Engine::WindowEvent wEvent;

    while (window.getEvent(wEvent)) { // event loop

    }
    return true;
}

int main(int argc, char* argv[]) {
    Engine::Window main_window;
    main_window.initialize(400, 600);



    while (true) { // engine loop
        osEvents(main_window);
    }
    return 0;
}