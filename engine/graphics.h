#pragma once
#include "glm/fwd.hpp"
#include "gui.h"

namespace Engine {
	struct Box {
		glm::vec3 position;
		glm::vec3 size;
	};

	struct Graphics final {
		enum class BufferType {
			Static,
			Dynamic,
			Stream,
		};
		using GPUBuffer = uint32_t;

		void initialize(void* GPUContext);
		void beginDraw(uint32_t width, uint32_t height);
		void endDraw();
		void drawBoxes(const std::vector<Box>& boxes);
		void setImage(const std::vector<uint8_t>& pixels, uint32_t width, uint32_t height, uint8_t depth);
		void drawGui(const uint8_t* data, uint32_t sizeofdata, uint32_t vertexCount);
		void setZoom(float zoom);
		void setCameraOffsetY(float offset);
		float aspectRation;
		float zoom;
		glm::vec3 origin;
	};
};