#pragma once
#include "glm/fwd.hpp"
#include "gui.h"

namespace Engine {
	struct Graphics final {
		enum class BufferType {
			Static,
			Dynamic,
			Stream,
		};
		using GPUBuffer = uint32_t;

		void initialize();
		void beginDraw(uint32_t width, uint32_t height);
		void endDraw();
		void setImage(const std::vector<uint8_t>& pixels, uint32_t width, uint32_t height, uint8_t depth);
		void drawGui(const uint8_t* data, uint32_t sizeofdata, uint32_t vertexCount);
	};
};