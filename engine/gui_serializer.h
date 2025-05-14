#pragma once
#include "gui2.h"
#include <vector>

namespace Engine {
	struct GuiSerialize {
		GUIComposer deserialize(const std::byte* buffer, size_t dataSizeOf);
		std::byte* serialize(const GUIComposer& composer, size_t& dataSizeOf);

	private:
		std::vector<std::byte> data;
	};
};