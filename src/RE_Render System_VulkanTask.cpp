#include "RE_Render System_Internal.hpp"

namespace RE {
	
	VulkanTask::VulkanTask(const uint32_t u32FunctionsCount, const VkQueueFlagBits *const vk_paeQueueTypePerFunctionRequiredInOrder, const bool bRequiresPresenting) : paFunctions(std::make_unique<std::function<void (VkCommandBuffer)>[]>(u32FunctionsCount)), 
	u32FunctionsCount(u32FunctionsCount) {
		VkQueueFlags vk_eAllRequiredQueueTypes = 0;
		for (uint32_t u32FunctionIndex = 0; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++)
			vk_eAllRequiredQueueTypes |= vk_paeQueueTypePerFunctionRequiredInOrder[u32FunctionIndex];
		std::vector<uint8_t> indicesOfPerfectQueues;
		indicesOfPerfectQueues.reserve(u8LogicalQueueCount);
		for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++)
			if ((vk_paeQueueTypes[u8LogicalQueueIndex] & vk_eAllRequiredQueueTypes) == vk_eAllRequiredQueueTypes)
				indicesOfPerfectQueues.push_back(u8LogicalQueueIndex);
		if (!indicesOfPerfectQueues.empty()) {
			uint8_t u8BestQueue;
			uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
			for (const uint8_t u8LogicalQueueIndex : indicesOfPerfectQueues) {
				const uint8_t u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paeQueueTypes[u8LogicalQueueIndex] ^ vk_eAllRequiredQueueTypes);
				if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
					u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
					u8BestQueue = u8LogicalQueueIndex;
				}
			}
			u32CommandBufferCount = 1;
			queueIndicesPerCommandBuffer = std::make_shared<uint8_t[]>(u32CommandBufferCount);
			queueIndicesPerCommandBuffer[0] = u8BestQueue;
			commandPoolPerCommandBuffer = std::make_shared<VkCommandPool[]>(u32CommandBufferCount);
			commandPoolPerCommandBuffer[0] = vk_pahCommandPools[u8BestQueue];
			vk_pahCommandBuffers = std::make_unique<VkCommandBuffer[]>(u32CommandBufferCount);
			PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer[0], VK_COMMAND_BUFFER_LEVEL_PRIMARY, u32CommandBufferCount, &vk_pahCommandBuffers[0]));
			commandBufferIndicesPerFunction = std::make_shared<uint32_t[]>(u8BestQueue);
			commandBufferIndicesPerFunction[0] = 0;
			if (bRequiresPresenting) {
				if ((vk_paeQueueTypes[u8BestQueue] & RE_VK_QUEUE_PRESENT_BIT) != 0)
					u8PresentQueueIndex = u8BestQueue;
				else {
					uint8_t u8BestPresentingQueue;
					uint8_t u8MinimalAmountOfSideFeaturesInPresentingQueue = std::numeric_limits<uint8_t>::max();
					for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++) {
						if ((vk_paeQueueTypes[u8LogicalQueueIndex] & RE_VK_QUEUE_PRESENT_BIT) == 0)
							continue;
						const uint8_t u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paeQueueTypes[u8LogicalQueueIndex] ^ RE_VK_QUEUE_PRESENT_BIT);
						if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInPresentingQueue) {
							u8MinimalAmountOfSideFeaturesInPresentingQueue = u8SideFeaturesCount;
							u8BestPresentingQueue = u8LogicalQueueIndex;
						}
					}
					u8PresentQueueIndex = u8BestPresentingQueue;
				}
			}
		} else {
			// TODO: There's no algorithm implemented to organize a Vulkan task to be executed across multiple queues yet
			RE_FATAL_ERROR("There's no algorithm implemented to organize a Vulkan task to be executed across multiple queues yet, which is essential at this point, but missing");
		}
	}

	VulkanTask::VulkanTask(const VulkanTask &rCopy) : queueIndicesPerCommandBuffer(rCopy.queueIndicesPerCommandBuffer), 
	commandPoolPerCommandBuffer(rCopy.commandPoolPerCommandBuffer), 
	vk_pahCommandBuffers(std::make_unique<VkCommandBuffer[]>(rCopy.u32CommandBufferCount)), 
	vk_pahInternalSemaphores(rCopy.u32CommandBufferCount > 0 ? std::make_unique<VkSemaphore[]>(rCopy.u32CommandBufferCount - 1) : nullptr), 
	commandBufferIndicesPerFunction(rCopy.commandBufferIndicesPerFunction), 
	paFunctions(std::make_unique<std::function<void (VkCommandBuffer)>[]>(rCopy.u32FunctionsCount)), 
	u32FunctionsCount(rCopy.u32FunctionsCount), 
	u32CommandBufferCount(rCopy.u32CommandBufferCount), 
	u8PresentQueueIndex(rCopy.u8PresentQueueIndex) {
		for (uint32_t u32CommandBufferIndex = 0; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++)
			PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer[u32CommandBufferIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, u32CommandBufferCount, &vk_pahCommandBuffers[u32CommandBufferIndex]));
		if (vk_pahInternalSemaphores && u32CommandBufferCount > 0) {
			const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
			};
			for (uint32_t u32InternalSemaphoreIndex = 0; u32InternalSemaphoreIndex < u32CommandBufferCount - 1; u32InternalSemaphoreIndex++)
				vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_pahInternalSemaphores[u32InternalSemaphoreIndex]);
		}
	}

	VulkanTask::~VulkanTask() {
		for (uint32_t u32CommandBufferIndex = 0; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++) {
			vkFreeCommandBuffers(vk_hDevice, commandPoolPerCommandBuffer[u32CommandBufferIndex], u32CommandBufferCount, &vk_pahCommandBuffers[u32CommandBufferIndex]);
			if (u32CommandBufferIndex > 0)
				vkDestroySemaphore(vk_hDevice, vk_pahInternalSemaphores[u32CommandBufferIndex - 1], nullptr);
		}
	}

	void VulkanTask::submit(const uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *const vk_pahSemaphoresToWaitFor, const VkPipelineStageFlags *const vk_pahStagesToWaitAt, const uint32_t u32SemaphoresToSignal, const VkSemaphore *const vk_pahSemaphoresToSignal, const VkFence vk_hFenceToSignal) {
		if (u32CommandBufferCount == 1)
			PUSH_TO_CALLSTACKTRACE(submit_to_vulkan_queue(vk_pahQueues[queueIndicesPerCommandBuffer[0]], u32SemaphoresToWaitForCount, vk_pahSemaphoresToWaitFor, vk_pahStagesToWaitAt, 1, vk_pahCommandBuffers.get(), u32SemaphoresToSignal, vk_pahSemaphoresToSignal, vk_hFenceToSignal));
		else {
			PUSH_TO_CALLSTACKTRACE(submit_to_vulkan_queue(vk_pahQueues[queueIndicesPerCommandBuffer[0]], u32SemaphoresToWaitForCount, vk_pahSemaphoresToWaitFor, vk_pahStagesToWaitAt, 1, &vk_pahCommandBuffers[0], 1, &vk_pahInternalSemaphores[0], VK_NULL_HANDLE));
			constexpr VkPipelineStageFlags vk_ePipelineStageToWaitAt = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			for (uint32_t u32CommandBufferIndex = 1; u32CommandBufferIndex < u32CommandBufferCount - 1; u32CommandBufferIndex++)
				PUSH_TO_CALLSTACKTRACE(submit_to_vulkan_queue(vk_pahQueues[queueIndicesPerCommandBuffer[u32CommandBufferIndex]], 1, &vk_pahInternalSemaphores[u32CommandBufferIndex - 1], &vk_ePipelineStageToWaitAt, 1, &vk_pahCommandBuffers[u32CommandBufferIndex], 1, &vk_pahInternalSemaphores[u32CommandBufferIndex], VK_NULL_HANDLE));
			PUSH_TO_CALLSTACKTRACE(submit_to_vulkan_queue(vk_pahQueues[queueIndicesPerCommandBuffer[u32CommandBufferCount - 1]], 1, &vk_pahInternalSemaphores[u32CommandBufferCount - 2], &vk_ePipelineStageToWaitAt, 1, &vk_pahCommandBuffers[u32CommandBufferCount - 1], u32SemaphoresToSignal, vk_pahSemaphoresToSignal, vk_hFenceToSignal));
		}
	}

	void VulkanTask::submit_and_present(const uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *const vk_pahSemaphoresToWaitFor, const VkPipelineStageFlags *const vk_pahStagesToWaitAt, const VkSemaphore *const vk_phSwapchainSemaphore, const VkFence vk_hFenceToSignal, const uint32_t *const pu32SwapchainImageIndex, VkResult *const vk_pePresentResult) {
		PUSH_TO_CALLSTACKTRACE(submit(u32SemaphoresToWaitForCount, vk_pahSemaphoresToWaitFor, vk_pahStagesToWaitAt, 1, vk_phSwapchainSemaphore, vk_hFenceToSignal));
		const VkPresentInfoKHR vk_presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = vk_phSwapchainSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &vk_hSwapchain,
			.pImageIndices = pu32SwapchainImageIndex,
			.pResults = vk_pePresentResult
		};
		vkQueuePresentKHR(vk_pahQueues[u8PresentQueueIndex], &vk_presentInfo);
	}

}
