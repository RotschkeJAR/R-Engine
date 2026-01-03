#include "RE_Renderer_Internal.hpp"

namespace RE {

	Vulkan_Buffer aSortableDepthBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_storage_buffers() {
		std::vector<uint32_t> queuesInStorageBuffer;
		queuesInStorageBuffer.reserve(2);
		queuesInStorageBuffer.push_back(renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_PROCESSING));
		uint8_t u8FrameInFlightCreateIndex = 0;
		while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			if (!aSortableDepthBuffers.create(0, , VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 1, nullptr, ))
				break;
			u8FrameInFlightCreateIndex++;
		}
	}

	void destroy_storage_buffers() {

	}

}
