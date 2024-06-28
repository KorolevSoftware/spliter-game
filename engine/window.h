#pragma once
#include <spdlog/spdlog.h>
#include <string_view>
#include <filesystem>
#include <vector>

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
		uint32_t getWidth();
		uint32_t getHeight();
		WindowStatus initialize(std::string_view name, uint32_t width, uint32_t height);
		WindowStatus finalize();
		
		void loadImage(std::filesystem::path path, std::vector<uint8_t> &pixels, uint8_t& depth, uint32_t& width, uint32_t& height);

		void present();

	private:
		SDL_Window* window;
		void* GLContext;
	};
};