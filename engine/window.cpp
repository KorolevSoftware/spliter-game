#include "window.h"
#include <SDL.h>
//#include <SDL_opengles2.h>
#include <iostream>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <SDL_image.h>

namespace Engine {
    WindowStatus Window::initialize(std::string_view name, uint32_t width, uint32_t height) {
        this->width = width;
        this->height = height;
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        IMG_Init(IMG_INIT_PNG);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        //Create a window
        window = SDL_CreateWindow(name.data(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width, height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

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

    void Window::loadImage(std::filesystem::path path, std::vector<uint8_t>& pixels, uint8_t& depth, uint32_t& width, uint32_t& height) {
        SDL_Surface* img = IMG_Load(path.string().c_str());
        pixels.reserve(img->h * img->pitch);
        std::copy(
            reinterpret_cast<uint8_t*>(img->pixels),
            reinterpret_cast<uint8_t*>(img->pixels) + img->h * img->pitch,
            std::back_inserter(pixels)
        );
        depth = img->pitch / img->w;
        width = img->w;
        height = img->h;
        SDL_FreeSurface(img);

    }

    void Window::present() {        
        SDL_GL_SwapWindow(window);
    }
    
    bool Window::getEvent(WindowEvent& wEvent) {
        SDL_Event ev;

        if (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_WINDOWEVENT) {
                if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                    width = ev.window.data1;
                    height = ev.window.data2;
                    spdlog::info("Screen resize to: {0}x{1}", width, height);
                }
                
            }
            wEvent.released = ev.type == SDL_MOUSEBUTTONUP;
            wEvent.pressed = ev.type == SDL_MOUSEBUTTONDOWN;
            wEvent.posX = (uint32_t)ev.button.x;
            wEvent.posY = (uint32_t)ev.button.y;
            return true;
        }
        return false;
    }

    uint32_t Window::getWidth() {
        return width;
    }

    uint32_t Window::getHeight() {
        return height;
    }
}