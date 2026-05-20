#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	VulkanTask aRenderTasks[RE_VK_FRAMES_IN_FLIGHT];
	VkFence vk_ahRenderFences[RE_VK_FRAMES_IN_FLIGHT];

	bool create_render_tasks() {
		PRINT_DEBUG("Initializing first render task");
		constexpr VkQueueFlagBits vk_aeQueuesUsedForRendering[] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_TRANSFER_BIT};
		constexpr bool bIndividualResets = true, bTransient = false;
		if (aRenderTasks[0].init(sizeof(vk_aeQueuesUsedForRendering) / sizeof(vk_aeQueuesUsedForRendering[0]), vk_aeQueuesUsedForRendering, bIndividualResets, true, bTransient)) {
			size_t renderTaskCreateIndex = 1;
			while (renderTaskCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				PRINT_DEBUG("Initializing render task at index ", renderTaskCreateIndex, " by using the first render task's precomputed data");
				if (aRenderTasks[renderTaskCreateIndex].init(aRenderTasks[0], bIndividualResets, bTransient)) {
					renderTaskCreateIndex++;
					continue;
				}
				break;
			}
			if (renderTaskCreateIndex == RE_VK_FRAMES_IN_FLIGHT) {
				size_t syncObjCreateIndex = 0;
				while (syncObjCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
					aRenderTasks[syncObjCreateIndex].record(RENDER_TASK_SUBINDEX_BUFFER_TRANSFER, 0, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {});
					PRINT_DEBUG("Creating Vulkan fence at index ", syncObjCreateIndex, " for render tasks");
					if (create_vulkan_fence(VK_FENCE_CREATE_SIGNALED_BIT, &vk_ahRenderFences[syncObjCreateIndex])) {
						syncObjCreateIndex++;
						continue;
					}
					break;
				}
				if (syncObjCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
					return true;
				else
					RE_FATAL_ERROR("Failed to create Vulkan fence at index ", syncObjCreateIndex, " to wait on rendering to finish");
				for (size_t syncObjDestroyIndex = 0; syncObjDestroyIndex < syncObjCreateIndex; syncObjDestroyIndex++) {
					PRINT_DEBUG("Destroying Vulkan fence ", vk_ahRenderFences[syncObjDestroyIndex], " at index ", syncObjDestroyIndex, " due to failure initializing all synchronization objects");
					vkDestroyFence(vk_hDevice, vk_ahRenderFences[syncObjDestroyIndex], nullptr);
				}
			} else
				RE_FATAL_ERROR("Failed to initialize render task at index ", renderTaskCreateIndex);
			for (size_t renderTaskDestroyIndex = 0; renderTaskDestroyIndex < renderTaskCreateIndex; renderTaskDestroyIndex++) {
				PRINT_DEBUG("Destroying render task at index ", renderTaskDestroyIndex, " due to failure creating synchronization objects for render tasks");
				aRenderTasks[renderTaskDestroyIndex].destroy();
			}
		} else
			RE_FATAL_ERROR("Failed to initialize render task at index 0");
		return false;
	}

	void destroy_render_tasks() {
		for (size_t syncObjDestroyIndex = 0; syncObjDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; syncObjDestroyIndex++) {
			PRINT_DEBUG("Destroying Vulkan fence ", vk_ahRenderFences[syncObjDestroyIndex], " at index ", syncObjDestroyIndex);
			vkDestroyFence(vk_hDevice, vk_ahRenderFences[syncObjDestroyIndex], nullptr);
		}
		for (size_t renderTaskDestroyIndex = 0; renderTaskDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; renderTaskDestroyIndex++) {
			PRINT_DEBUG("Destroying render task at index ", renderTaskDestroyIndex);
			aRenderTasks[renderTaskDestroyIndex].destroy();
		}
	}

	bool wait_for_rendering_finished() {
		PRINT_DEBUG("Waiting for all render process finished");
		return vkWaitForFences(vk_hDevice, RE_VK_FRAMES_IN_FLIGHT, vk_ahRenderFences, VK_TRUE, std::numeric_limits<uint64_t>::max()) == VK_SUCCESS;
	}

}
