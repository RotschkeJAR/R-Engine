#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	std::array<VulkanTask, RE_VK_FRAMES_IN_FLIGHT> renderTasks;
	std::array<VkFence, RE_VK_FRAMES_IN_FLIGHT> renderFences;
	std::array<VkSemaphore, RE_VK_FRAMES_IN_FLIGHT> swapchainSemaphores;
	uint8_t u8CurrentFrameInFlightIndex = 0;

	bool create_render_tasks() {
		PRINT_DEBUG("Initializing first render task");
		constexpr VkQueueFlagBits vk_aeQueuesUsedForRendering[] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_TRANSFER_BIT};
		if (renderTasks[0].init(sizeof(vk_aeQueuesUsedForRendering) / sizeof(vk_aeQueuesUsedForRendering[0]), vk_aeQueuesUsedForRendering, false)) {
			size_t renderTaskCreateIndex = 1;
			while (renderTaskCreateIndex < renderTasks.size()) {
				PRINT_DEBUG("Initializing render task at index ", renderTaskCreateIndex, " by using the first render task's precomputed data");
				if (renderTasks[renderTaskCreateIndex].init(renderTasks[0])) {
					renderTaskCreateIndex++;
					continue;
				}
				break;
			}
			if (renderTaskCreateIndex == renderTasks.size()) {
				constexpr VkFenceCreateInfo vk_fenceCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
					.flags = VK_FENCE_CREATE_SIGNALED_BIT
				};
				constexpr VkSemaphoreCreateInfo vk_swapchainSemaphoreCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
				};
				size_t syncObjCreateIndex = 0;
				while (syncObjCreateIndex < renderFences.size()) {
					PRINT_DEBUG("Creating Vulkan fence at index ", syncObjCreateIndex, " for render tasks");
					if (vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, &renderFences[syncObjCreateIndex]) == VK_SUCCESS) {
						if (vkCreateSemaphore(vk_hDevice, &vk_swapchainSemaphoreCreateInfo, nullptr, &swapchainSemaphores[syncObjCreateIndex]) == VK_SUCCESS) {
							syncObjCreateIndex++;
							continue;
						}
						vkDestroyFence(vk_hDevice, renderFences[syncObjCreateIndex], nullptr);
					}
					break;
				}
				if (syncObjCreateIndex == static_cast<uint8_t>(renderFences.size()))
					return true;
				else
					RE_FATAL_ERROR("Failed to create Vulkan fence at index ", syncObjCreateIndex, " to wait on rendering to finish");
				for (size_t syncObjDestroyIndex = 0; syncObjDestroyIndex < syncObjCreateIndex; syncObjDestroyIndex++) {
					PRINT_DEBUG("Destroying Vulkan fence ", renderFences[syncObjDestroyIndex], " and semaphore ", swapchainSemaphores[syncObjDestroyIndex], " at index ", syncObjDestroyIndex, " due to failure initializing all synchronization objects");
					vkDestroySemaphore(vk_hDevice, swapchainSemaphores[syncObjDestroyIndex], nullptr);
					vkDestroyFence(vk_hDevice, renderFences[syncObjDestroyIndex], nullptr);
				}
			} else
				RE_FATAL_ERROR("Failed to initialize render task at index ", renderTaskCreateIndex);
			for (size_t renderTaskDestroyIndex = 0; renderTaskDestroyIndex < renderTaskCreateIndex; renderTaskDestroyIndex++) {
				PRINT_DEBUG("Destroying render task at index ", renderTaskDestroyIndex, " due to failure creating synchronization objects for render tasks");
				renderTasks[renderTaskDestroyIndex].destroy();
			}
		} else
			RE_FATAL_ERROR("Failed to initialize render task at index 0");
		return false;
	}

	void destroy_render_tasks() {
		for (size_t syncObjDestroyIndex = 0; syncObjDestroyIndex < renderFences.size(); syncObjDestroyIndex++) {
			PRINT_DEBUG("Destroying Vulkan fence ", renderFences[syncObjDestroyIndex], " and semaphore ", swapchainSemaphores[syncObjDestroyIndex], " at index ", syncObjDestroyIndex);
			vkDestroySemaphore(vk_hDevice, swapchainSemaphores[syncObjDestroyIndex], nullptr);
			vkDestroyFence(vk_hDevice, renderFences[syncObjDestroyIndex], nullptr);
		}
		for (size_t renderTaskDestroyIndex = 0; renderTaskDestroyIndex < renderTasks.size(); renderTaskDestroyIndex++) {
			PRINT_DEBUG("Destroying render task at index ", renderTaskDestroyIndex);
			renderTasks[renderTaskDestroyIndex].destroy();
		}
	}

	bool wait_for_rendering_finished() {
		PRINT_DEBUG("Waiting for all render process finished");
		return vkWaitForFences(vk_hDevice, renderFences.size(), renderFences.data(), VK_TRUE, std::numeric_limits<uint64_t>::max()) == VK_SUCCESS;
	}

	uint8_t get_render_buffer_transfer_queue_logical_index() {
		return renderTasks[0].get_logical_queue_index_for_function(0);
	}

	uint8_t get_render_graphics_queue_logical_index() {
		return renderTasks[0].get_logical_queue_index_for_function(1);
	}

}
