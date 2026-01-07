#include "RE_Renderer_Internal.hpp"

namespace RE {

	bool create_storage_buffers() {
		constexpr uint32_t au32RelevantFunctionIndices[] = {RENDER_TASK_SUBINDEX_PROCESSING, RENDER_TASK_SUBINDEX_RENDERING};
		const VulkanQueueCollection queueCollection = renderTask[0].queues_of_functions(au32RelevantFunctionIndices, sizeof(au32RelevantFunctionIndices) / sizeof(au32RelevantFunctionIndices[0]));
		uint8_t u8FrameInFlightCreateIndex = 0;
		while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			if (!aSortableDepthBuffers.create(
					0, 
					, 
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
					queueCollection.u8QueueCount, 
					queueCollection.queueFamilyIndices, 
					RE_VK_GPU_RAM, 
					0))
				break;
			u8FrameInFlightCreateIndex++;
		}
	}

	void destroy_storage_buffers() {

	}

}
