#include "RE_Renderer_RenderTask.hpp"
#include "RE_GPU.hpp"

namespace RE {
	
	VulkanTask aRenderTasks[RE_VK_FRAMES_IN_FLIGHT];
	VkFence vk_ahRenderFences[RE_VK_FRAMES_IN_FLIGHT];
#ifdef RENDERER_INCLUDE_EMPTY_PRESENT
	VkCommandPool vk_hCommandPoolEmptyPresent;
	VkCommandBuffer vk_hCommandBufferEmptyPresent;
	VkFence vk_hEmptyPresentFence;
#endif

	bool create_render_tasks() {
		PRINT_DEBUG("Initializing first render task");
		constexpr VkQueueFlagBits vk_aeQueuesUsedForRendering[] = {
			VK_QUEUE_TRANSFER_BIT,
			VK_QUEUE_COMPUTE_BIT,
			VK_QUEUE_GRAPHICS_BIT,
			VK_QUEUE_TRANSFER_BIT
		};
		constexpr bool bIndividualResets = true,
			bTransient = false;
		if (aRenderTasks[0].init(
				sizeof(vk_aeQueuesUsedForRendering) / sizeof(vk_aeQueuesUsedForRendering[0]),
				vk_aeQueuesUsedForRendering,
				bIndividualResets,
				true,
				bTransient)) {
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
				if (syncObjCreateIndex == RE_VK_FRAMES_IN_FLIGHT) {
				#ifdef RENDERER_INCLUDE_EMPTY_PRESENT
					PRINT_DEBUG("Creating Vulkan command pool for empty presentations");
					if (create_vulkan_command_pool(
							0,
							queueFamilyIndices[aRenderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_IMAGE_BLIT)],
							&vk_hCommandPoolEmptyPresent)) {
						PRINT_DEBUG("Allocating Vulkan command buffer from pool ", vk_hCommandPoolEmptyPresent, " for empty presentations");
						if (alloc_vulkan_command_buffers(
								vk_hCommandPoolEmptyPresent,
								VK_COMMAND_BUFFER_LEVEL_PRIMARY,
								1,
								&vk_hCommandBufferEmptyPresent)) {
							PRINT_DEBUG("Creating Vulkan fence to synchronize empty presentations");
							if (create_vulkan_fence(VK_FENCE_CREATE_SIGNALED_BIT, &vk_hEmptyPresentFence)) {
								return true;
							}
						}
						PRINT_DEBUG("Destroying Vulkan command pool due to failure setting up resources for empty presentations");
						vkDestroyCommandPool(vk_hDevice, vk_hCommandPoolEmptyPresent, nullptr);
					}
				#else
					return true;
				#endif
				} else
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
	#ifdef RENDERER_INCLUDE_EMPTY_PRESENT
		PRINT_DEBUG("Destroying Vulkan fence used for empty presentations");
		vkDestroyFence(vk_hDevice, vk_hEmptyPresentFence, nullptr);
		PRINT_DEBUG("Destroying Vulkan command pool used for empty presentations");
		vkDestroyCommandPool(vk_hDevice, vk_hCommandPoolEmptyPresent, nullptr);
	#endif
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

#ifdef RENDERER_INCLUDE_EMPTY_PRESENT
	bool prepare_render_tasks_for_dummy_presentation() {
		PRINT_DEBUG("Preparing render tasks for dummy presentation");
		if (wait_for_rendering_finished()) {
			bool bSuccess = true;
			for (uint uFrameInFlightIndex = 0; uFrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; uFrameInFlightIndex++)
				bSuccess = bSuccess
						&& aRenderTasks[uFrameInFlightIndex].record(
							RENDER_TASK_SUBINDEX_BUFFER_TRANSFER,
							0,
							nullptr)
						&& aRenderTasks[uFrameInFlightIndex].record(
							RENDER_TASK_SUBINDEX_PROCESSING,
							0,
							nullptr)
						&& aRenderTasks[uFrameInFlightIndex].record(
							RENDER_TASK_SUBINDEX_RENDERING,
							0,
							nullptr);
			return bSuccess;
		}
		return false;
	}
#endif

}
