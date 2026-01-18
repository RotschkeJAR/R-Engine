#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	Vulkan_Buffer aSortableDepthBuffers[RE_VK_FRAMES_IN_FLIGHT], aGameObjectBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_processing_buffers() {
		PRINT_DEBUG("Querying queues the processing buffers will be used in");
		constexpr uint32_t au32RelevantFunctionIndices[] = {
			RENDER_TASK_SUBINDEX_PROCESSING, 
			RENDER_TASK_SUBINDEX_RENDERING
		};
		const VulkanQueueCollection queueCollection = renderTasks[0].queues_of_functions(au32RelevantFunctionIndices, sizeof(au32RelevantFunctionIndices) / sizeof(au32RelevantFunctionIndices[0]));
		uint8_t u8FrameInFlightCreateIndex = 0;
		while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			PRINT_DEBUG("Creating processing buffer for depth at frame-in-flight index ", u8FrameInFlightCreateIndex);
			if (aSortableDepthBuffers[u8FrameInFlightCreateIndex].create(
					0, 
					gameObjectCounterBuffer * (sizeof(float) + sizeof(uint32_t)), 
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
					queueCollection.u8QueueCount, 
					queueCollection.queueFamilyIndices.get(), 
					RE_VK_GPU_RAM, 
					0)) {
				PRINT_DEBUG("Creating processing buffer for game objects at frame-in-flight index ", u8FrameInFlightCreateIndex);
				if (aGameObjectBuffers[u8FrameInFlightCreateIndex].create(
						0, 
						gameObjectCounterBuffer * (sizeof(float[16]) + sizeof(float[4]) + sizeof(uint32_t)), 
						VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
						queueCollection.u8QueueCount, 
						queueCollection.queueFamilyIndices.get(), 
						RE_VK_GPU_RAM, 
						0)) {
					u8FrameInFlightCreateIndex++;
					continue;
				} else
					RE_FATAL_ERROR("Failed to create processing buffer for game objects in Vulkan at frame-in-flight index ", u8FrameInFlightCreateIndex);
				PRINT_DEBUG("Destroying processing buffer for depth at frame-in-flight index ", u8FrameInFlightCreateIndex, " for failure creating processing buffer for game objects");
				aSortableDepthBuffers[u8FrameInFlightCreateIndex].destroy();
			} else
				RE_FATAL_ERROR("Failed to create processing buffer for depth in Vulkan at frame-in-flight index ", u8FrameInFlightCreateIndex);
			break;
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++) {
			PRINT_DEBUG("Destroying processing buffers at frame-in-flight index ", u8FrameInFlightDestroyIndex, " for failure setting them up");
			aSortableDepthBuffers[u8FrameInFlightDestroyIndex].destroy();
			aGameObjectBuffers[u8FrameInFlightDestroyIndex].destroy();
		}
		return false;
	}

	void destroy_processing_buffers() {
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightDestroyIndex++) {
			PRINT_DEBUG("Destroying processing buffers at frame-in-flight index ", u8FrameInFlightDestroyIndex);
			aSortableDepthBuffers[u8FrameInFlightDestroyIndex].destroy();
			aGameObjectBuffers[u8FrameInFlightDestroyIndex].destroy();
		}
	}

}
