#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	Vulkan_Buffer aSortableDepthBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_sortable_depth_buffers(const VulkanQueueCollection &rQueues) {
		uint8_t u8FrameInFlightCreateIndex = 0;
		while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			PRINT_DEBUG("Creating processing buffer for depth at frame-in-flight index ", u8FrameInFlightCreateIndex);
			if (aSortableDepthBuffers[u8FrameInFlightCreateIndex].create(
					0, 
					1000 * sizeof(DepthShaderData), 
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
					rQueues.u8QueueCount, 
					rQueues.queueFamilyIndices.get(), 
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
				u8FrameInFlightCreateIndex++;
			else {
				RE_FATAL_ERROR("Failed to create processing buffer for depth in Vulkan at frame-in-flight index ", u8FrameInFlightCreateIndex);
				break;
			}
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++) {
			PRINT_DEBUG("Destroying sortable depth buffer at frame-in-flight index ", u8FrameInFlightDestroyIndex, " for failure setting them up");
			aSortableDepthBuffers[u8FrameInFlightDestroyIndex].destroy();
		}
		return false;
	}

	void destroy_sortable_depth_buffers() {
		for (Vulkan_Buffer &rSortableDepthBuffer : aSortableDepthBuffers)
			rSortableDepthBuffer.destroy();
	}

}
