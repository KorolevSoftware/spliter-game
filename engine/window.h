#pragma once
#include <spdlog/spdlog.h>

struct SDL_Window;

namespace Engine {


	enum class WindowStatus {
		Success,
		Failure
	};

	struct WindowEvent {
		bool pressed;
		bool released;
		uint32_t posX;
		uint32_t posY;
	};

	struct Window final {

		uint32_t width, height;
		bool getEvent(WindowEvent& event);

		WindowStatus initialize(uint32_t width, uint32_t height);
		WindowStatus finalize();
		void present();

	private:
		SDL_Window* window;
		void* GLContext;
	};
};