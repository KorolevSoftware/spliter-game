#include "window.h"
#include <SDL.h>
//#include <SDL_opengles2.h>
#include <iostream>
#include <glad/glad.h>

namespace Engine {
    WindowStatus Window::initialize(std::string_view name, uint32_t width, uint32_t height) {
        this->width = width;
        this->height = height;

        //Create a window
        window = SDL_CreateWindow(name.data(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width, height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

        if (window == nullptr) {
            spdlog::critical("SDL_CreateWindow error create window");
            return WindowStatus::Failure;
        }

        spdlog::info("Created SDL Window: Success");
        GLContext = SDL_GL_CreateContext(window);

        if (GLContext == nullptr) {
            spdlog::critical("SDL_GL_CreateContext error create opengl surface {0}", SDL_GetError());
            return WindowStatus::Failure;
        }

        spdlog::info("Created SDL GL context: Success");

        gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress);

        return WindowStatus::Success;
    }

    WindowStatus Window::finalize() {
        if (window == nullptr) {
            return WindowStatus::Failure;
        }
        spdlog::info("Destroy SDL Window: Success");
        SDL_DestroyWindow(window); // Free up window
        SDL_Quit();  // Shutdown SDL
        return WindowStatus::Success;
    }

    void Window::present() {

        SDL_GL_MakeCurrent(window, GLContext);
        glClearColor(51 / 255.0, 51 / 255.0, 51 / 255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        SDL_GL_SwapWindow(window);
    }
    
    bool Window::getEvent(WindowEvent& wEvent) {
        SDL_Event ev;

        if (SDL_PollEvent(&ev)) {
            wEvent.released = ev.type == SDL_MOUSEBUTTONUP;
            wEvent.pressed = ev.type == SDL_MOUSEBUTTONDOWN;
            wEvent.posX = (uint32_t)ev.button.x;
            wEvent.posY = (uint32_t)ev.button.y;
            return true;
        }
        return false;
    }
}