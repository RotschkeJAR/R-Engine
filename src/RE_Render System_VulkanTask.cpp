#include "RE_Render System_Internal.hpp"

namespace RE {

	VulkanTask::VulkanTask() {}

	VulkanTask::VulkanTask(const uint32_t u32FunctionsCount, const VkQueueFlagBits *const vk_paeQueueTypePerFunctionRequiredInOrder, const bool bRequiresPresenting, const bool bTransient) {
		PUSH_TO_CALLSTACKTRACE(init(u32FunctionsCount, vk_paeQueueTypePerFunctionRequiredInOrder, bRequiresPresenting, bTransient));
	}

	VulkanTask::VulkanTask(const VulkanTask &rCopy) {
		PUSH_TO_CALLSTACKTRACE(init(rCopy));
	}

	VulkanTask::~VulkanTask() {}
	
	void VulkanTask::init(const uint32_t u32FunctionsCount, const VkQueueFlagBits *const vk_paeQueueTypePerFunctionRequiredInOrder, const bool bRequiresPresenting, const bool bTransient) {
		paFunctions = std::make_unique<std::function<void (VkCommandBuffer, uint32_t, uint32_t, uint32_t)>[]>(u32FunctionsCount);
		this->u32FunctionsCount = u32FunctionsCount;
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
			commandPoolPerCommandBuffer[0] = vk_pahCommandPools[u8BestQueue * COMMAND_POOLS_PER_QUEUE + (bTransient ? COMMAND_POOL_OFFSET_TRANSIENT : COMMAND_POOL_OFFSET_NORMAL)];
			vk_pahCommandBuffers = std::make_unique<VkCommandBuffer[]>(u32CommandBufferCount);
			PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer[0], VK_COMMAND_BUFFER_LEVEL_PRIMARY, u32CommandBufferCount, &vk_pahCommandBuffers[0]));
			commandBufferIndicesPerFunction = std::make_shared<uint32_t[]>(u32CommandBufferCount);
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
			std::vector<uint8_t> queuesToUseInOrder;
			queuesToUseInOrder.reserve(u32FunctionsCount);
			for (uint32_t u32FunctionIndex = 0; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++)
				if (u32FunctionIndex && (queuesToUseInOrder[u32FunctionIndex - 1] & vk_paeQueueTypePerFunctionRequiredInOrder[u32FunctionIndex]))
					queuesToUseInOrder.push_back(queuesToUseInOrder[u32FunctionIndex - 1]);
				else {
					std::vector<uint32_t> potentialPerQueue;
					potentialPerQueue.resize(u8LogicalQueueCount);
					std::vector<bool> potentialStillPresent;
					potentialStillPresent.resize(potentialPerQueue.size());
					std::fill(potentialStillPresent.begin(), potentialStillPresent.end(), true);
					for (uint32_t u32NextFunctionIndex = u32FunctionIndex; u32NextFunctionIndex < u32FunctionsCount; u32NextFunctionIndex++) {
						for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++) {
							if (!potentialStillPresent[u8LogicalQueueIndex])
								continue;
							if ((vk_paeQueueTypes[u8LogicalQueueIndex] & vk_paeQueueTypePerFunctionRequiredInOrder[u32NextFunctionIndex]) != 0)
								potentialPerQueue[u8LogicalQueueIndex]++;
							else
								potentialStillPresent[u8LogicalQueueIndex] = false;
						}
						if (std::none_of(potentialStillPresent.begin(), potentialStillPresent.end(), [](const bool bPotential) -> bool { return bPotential; }))
							break;
					}
					uint8_t u8NextQueue;
					uint32_t u32HighestPotential = std::numeric_limits<uint32_t>::min();
					for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++)
						if (potentialPerQueue[u8LogicalQueueIndex] > u32HighestPotential) {
							u8NextQueue = u8LogicalQueueIndex;
							u32HighestPotential = potentialPerQueue[u8LogicalQueueIndex];
						}
					queuesToUseInOrder.push_back(u8NextQueue);
				}

			uint8_t u8CurrentLogicalQueueIndex = queuesToUseInOrder[0];
			u32CommandBufferCount = 1;
			for (uint32_t u32FunctionIndex = 1; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++)
				if (u8CurrentLogicalQueueIndex != queuesToUseInOrder[u32FunctionIndex]) {
					u8CurrentLogicalQueueIndex = queuesToUseInOrder[u32FunctionIndex];
					u32CommandBufferCount++;
				}
			commandBufferIndicesPerFunction = std::make_shared<uint32_t[]>(u32CommandBufferCount);
			commandBufferIndicesPerFunction[0] = 0;
			queueIndicesPerCommandBuffer = std::make_shared<uint8_t[]>(u32CommandBufferCount);
			queueIndicesPerCommandBuffer[0] = queuesToUseInOrder[0];
			u8CurrentLogicalQueueIndex = queueIndicesPerCommandBuffer[0];
			uint32_t u32CurrentCommandBufferIndex = 0;
			for (uint32_t u32FunctionIndex = 1; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++) {
				if (u8CurrentLogicalQueueIndex != queuesToUseInOrder[u32FunctionIndex]) {
					u32CurrentCommandBufferIndex++;
					queueIndicesPerCommandBuffer[u32CurrentCommandBufferIndex] = queuesToUseInOrder[u32FunctionIndex];
				}
				commandBufferIndicesPerFunction[u32FunctionIndex] = u32CurrentCommandBufferIndex;
			}
			commandPoolPerCommandBuffer = std::make_shared<VkCommandPool[]>(u32CommandBufferCount);
			vk_pahCommandBuffers = std::make_unique<VkCommandBuffer[]>(u32CommandBufferCount);
			const uint8_t u8CommandPoolIndexOffset = bTransient ? COMMAND_POOL_OFFSET_TRANSIENT : COMMAND_POOL_OFFSET_NORMAL;
			for (uint32_t u32CommandBufferIndex = 0; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++) {
				commandPoolPerCommandBuffer[u32CommandBufferIndex] = vk_pahCommandPools[queueIndicesPerCommandBuffer[u32CommandBufferIndex] * COMMAND_POOLS_PER_QUEUE + u8CommandPoolIndexOffset];
				PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer[u32CommandBufferIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &vk_pahCommandBuffers[u32CommandBufferIndex]));
			}
			const uint32_t u32SemaphoreCount = u32CommandBufferCount - 1;
			vk_pahInternalSemaphores = std::make_unique<VkSemaphore[]>(u32SemaphoreCount);
			const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
			};
			for (uint32_t u32SemaphoreIndex = 0; u32SemaphoreIndex < u32SemaphoreCount; u32SemaphoreIndex++)
				vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_pahInternalSemaphores[u32SemaphoreIndex]);
		}
	}

	void VulkanTask::init(const VulkanTask &rCopy) {
		queueIndicesPerCommandBuffer = rCopy.queueIndicesPerCommandBuffer;
		commandPoolPerCommandBuffer = rCopy.commandPoolPerCommandBuffer;
		vk_pahCommandBuffers = std::make_unique<VkCommandBuffer[]>(rCopy.u32CommandBufferCount);
		vk_pahInternalSemaphores = rCopy.u32CommandBufferCount > 0 ? std::make_unique<VkSemaphore[]>(rCopy.u32CommandBufferCount - 1) : nullptr;
		commandBufferIndicesPerFunction = rCopy.commandBufferIndicesPerFunction;
		paFunctions = std::make_unique<std::function<void (VkCommandBuffer, uint32_t, uint32_t, uint32_t)>[]>(rCopy.u32FunctionsCount);
		u32FunctionsCount = rCopy.u32FunctionsCount;
		u32CommandBufferCount = rCopy.u32CommandBufferCount;
		u8PresentQueueIndex = rCopy.u8PresentQueueIndex;
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

	void VulkanTask::destroy() {
		for (uint32_t u32CommandBufferIndex = 0; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++) {
			vkFreeCommandBuffers(vk_hDevice, commandPoolPerCommandBuffer[u32CommandBufferIndex], u32CommandBufferCount, &vk_pahCommandBuffers[u32CommandBufferIndex]);
			if (u32CommandBufferIndex > 0)
				vkDestroySemaphore(vk_hDevice, vk_pahInternalSemaphores[u32CommandBufferIndex - 1], nullptr);
		}
	}

	void VulkanTask::record(const VkCommandBufferUsageFlags vk_eUsageFlags) {
		uint32_t u32CommandBufferIndex = 0;
		for (uint32_t u32FunctionIndex = 0; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++) {
			if (u32CommandBufferIndex != commandBufferIndicesPerFunction[u32FunctionIndex]) {
				if (u32CommandBufferIndex > 0)
					vkEndCommandBuffer(vk_pahCommandBuffers[u32CommandBufferIndex - 1]);
				u32CommandBufferIndex = commandBufferIndicesPerFunction[u32FunctionIndex];
				begin_recording_vulkan_command_buffer(vk_pahCommandBuffers[u32CommandBufferIndex], vk_eUsageFlags, nullptr);
			}
			const uint32_t u32PreviousQueueFamily = u32FunctionIndex > 0 ? get_queue_family_index_for_function(u32FunctionIndex - 1) : VK_QUEUE_FAMILY_IGNORED;
			const uint32_t u32NextQueueFamily = u32FunctionIndex < (u32FunctionsCount - 1) ? get_queue_family_index_for_function(u32FunctionIndex + 1) : VK_QUEUE_FAMILY_IGNORED;
			PUSH_TO_CALLSTACKTRACE_DETAILED(paFunctions[u32FunctionIndex](vk_pahCommandBuffers[u32CommandBufferIndex], u32PreviousQueueFamily, get_queue_family_index_for_function(u32FunctionIndex), u32NextQueueFamily), append_to_string("Function index ", u32FunctionIndex).c_str());
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

	uint32_t VulkanTask::get_function_count() const {
		return u32FunctionsCount;
	}

	uint32_t VulkanTask::get_queue_family_index_for_function(const uint32_t u32FunctionIndex) const {
		return queueFamilyIndices[queueIndicesPerCommandBuffer[commandBufferIndicesPerFunction[u32FunctionIndex]]];
	}

}
