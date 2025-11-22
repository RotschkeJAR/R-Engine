#include "RE_GPU_Internal.hpp"

namespace RE {

	static uint8_t find_least_featured_queue(const std::vector<uint8_t> &rLogicalQueueIndices, const VkQueueFlagBits vk_eRequiredQueueType) {
		if (rLogicalQueueIndices.size() == 1)
			return rLogicalQueueIndices[0];
		int32_t i32LowestCountForSideFeatures = std::numeric_limits<int32_t>::max();
		uint8_t u8BestQueue;
		for (const uint8_t u8LogicalQueueIndex : rLogicalQueueIndices) {
			const int32_t i32SideFeatureCount = std::popcount<VkQueueFlags>(vk_paeQueueTypes[u8LogicalQueueIndex] & (~vk_eRequiredQueueType));
			if (i32LowestCountForSideFeatures > i32SideFeatureCount) {
				i32LowestCountForSideFeatures = i32SideFeatureCount;
				u8BestQueue = u8LogicalQueueIndex;
			}
		}
		return u8BestQueue;
	}

	VulkanTask::VulkanTask() {}

	VulkanTask::VulkanTask(const uint32_t u32FunctionsCount, const VkQueueFlagBits *const vk_paeQueueTypePerFunctionRequiredInOrder, const bool bIndividualResets, const bool bTransient) {
		init(u32FunctionsCount, vk_paeQueueTypePerFunctionRequiredInOrder, bIndividualResets, bTransient);
	}

	VulkanTask::VulkanTask(const uint32_t u32FunctionsCount, const uint8_t *pau8LogicalQueueIndexPerFunctionRequiredInOrder, const bool bIndividualResets, const bool bTransient) {
		init(u32FunctionsCount, pau8LogicalQueueIndexPerFunctionRequiredInOrder, bIndividualResets, bTransient);
	}

	VulkanTask::VulkanTask(const VulkanTask_Queues &rQueues, const bool bIndividualResets, const bool bTransient) {
		init(rQueues, bIndividualResets, bTransient);
	}

	VulkanTask::VulkanTask(const VulkanTask &rCopy, const bool bIndividualResets, const bool bTransient) {
		init(rCopy, bIndividualResets, bTransient);
	}

	VulkanTask::~VulkanTask() {
		if (is_valid())
			destroy();
	}
	
	bool VulkanTask::init(const uint32_t u32FunctionsCount, const VkQueueFlagBits *const vk_paeQueueTypePerFunctionRequiredInOrder, const bool bIndividualResets, const bool bTransient) {
		std::vector<uint8_t> logicalQueuesPerFunction;
		logicalQueuesPerFunction.reserve(u32FunctionsCount);
		{
			std::vector<uint8_t> queuesToUseInOrder, usableLogicalQueueIndices, removableQueues;
			queuesToUseInOrder.reserve(u32FunctionsCount);
			usableLogicalQueueIndices.reserve(u8LogicalQueueCount);
			removableQueues.reserve(u8LogicalQueueCount);
			for (uint32_t u32FunctionIndex = 0; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++) {
				if (usableLogicalQueueIndices.empty())
					for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++)
						if ((vk_paeQueueTypePerFunctionRequiredInOrder[u32FunctionsCount] & vk_paeQueueTypes[u8LogicalQueueIndex]) != 0)
							usableLogicalQueueIndices.push_back(u8LogicalQueueIndex);
				else {
					for (const uint8_t u8LogicalQueueIndex : usableLogicalQueueIndices)
						if ((vk_paeQueueTypes[u8LogicalQueueIndex] & vk_paeQueueTypePerFunctionRequiredInOrder[u32FunctionIndex]) == 0)
							removableQueues.push_back(u8LogicalQueueIndex);
					if (removableQueues.empty()) {
						if (removableQueues.size() == usableLogicalQueueIndices.size()) {
							queuesToUseInOrder.push_back(find_least_featured_queue(usableLogicalQueueIndices, vk_paeQueueTypePerFunctionRequiredInOrder[u32FunctionIndex]));
							usableLogicalQueueIndices.clear();
						} else
							for (const uint8_t u8RemovableQueueIndex : removableQueues)
								usableLogicalQueueIndices.erase(std::find(usableLogicalQueueIndices.begin(), usableLogicalQueueIndices.end(), u8RemovableQueueIndex));
						removableQueues.clear();
					}
				}
			}
			if (!usableLogicalQueueIndices.empty())
				queuesToUseInOrder.push_back(find_least_featured_queue(usableLogicalQueueIndices, vk_paeQueueTypePerFunctionRequiredInOrder[u32FunctionsCount - 1]));
			uint32_t u32QueueToUseIndex = 1;
			uint8_t u8CurrentLogicalQueueIndex = queuesToUseInOrder[0];
			logicalQueuesPerFunction.push_back(queuesToUseInOrder[0]);
			for (uint32_t u32FunctionIndex = 1; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++) {
				if ((vk_paeQueueTypes[u8CurrentLogicalQueueIndex] & vk_paeQueueTypePerFunctionRequiredInOrder[u32FunctionIndex]) == 0) {
					u8CurrentLogicalQueueIndex = queuesToUseInOrder[u32QueueToUseIndex];
					u32QueueToUseIndex++;
				}
				logicalQueuesPerFunction.push_back(u8CurrentLogicalQueueIndex);
			}
		}
		return init(u32FunctionsCount, logicalQueuesPerFunction.data(), bIndividualResets, bTransient);
	}

	bool VulkanTask::init(const uint32_t u32FunctionsCount, const uint8_t *const pau8LogicalQueueIndexPerFunctionRequiredInOrder, const bool bIndividualResets, const bool bTransient) {
		PRINT_DEBUG_CLASS("Initializing Vulkan task based on logical queue indices");
		this->u32FunctionsCount = u32FunctionsCount;
		this->bTransient = bTransient;
		queueIndicesPerCommandPool = std::make_shared<uint8_t[]>(uniqueLogicalQueueIndices.size());
		commandPools = std::make_unique<VkCommandPool[]>(uniqueLogicalQueueIndices.size());
		commandPoolIndexPerCommandBuffer = std::make_shared<uint8_t[]>(u32FunctionsCount);
		commandBuffers = std::make_unique<VkCommandBuffer[]>(u32CommandBufferCount);
		std::vector<uint8_t> uniqueLogicalQueueIndices;
		uniqueLogicalQueueIndices.reserve(u8LogicalQueueCount);
		const VkCommandPoolCreateFlags vk_eCommandPoolCreateFlags = (bIndividualResets ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0) | (bTransient ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0);
		uint32_t u32FunctionIndex = 0;
		uint8_t u8CommandPoolCreateIndex = 0;
		while (u32FunctionIndex < u32FunctionsCount) {
			const uint8_t u8LogicalQueueIndex = pau8LogicalQueueIndexPerFunctionRequiredInOrder[u32FunctionIndex];
			const auto logicalQueueIndexIter = std::find(uniqueLogicalQueueIndices.begin(), uniqueLogicalQueueIndices.end(), u8LogicalQueueIndex);
			if (logicalQueueIndexIter == uniqueLogicalQueueIndices.end()) {
				const VkCommandPoolCreateInfo vk_cmdPoolCreateInfo = {
					.sType = VK_STRUCTURE_COMMAND_POOL_CREATE_INFO,
					.flags = vk_eCommandPoolCreateFlags,
					.queueFamilyIndex = queueFamilyIndices[u8LogicalQueueIndex]
				};
				if (vkCreateCommandPool(vk_hDevice, &vk_cmdPoolCreateInfo, nullptr, &commandPools[u8CommandPoolCreateIndex]) != VK_SUCCESS) {
					RE_FATAL_ERROR("Failed creating Vulkan command pool connected to queue family ", vk_cmdPoolCreateInfo.queueFamilyIndex);
					break;
				}
				queueIndicesPerCommandPool[u8CommandPoolCreateIndex] = u8LogicalQueueIndex;
				uniqueLogicalQueueIndices.push_back(u8LogicalQueueIndex);
				commandPoolIndexPerCommandBuffer[u32FunctionIndex] = u8CommandPoolCreateIndex;
				u8CommandPoolCreateIndex++;
			} else
				commandPoolIndexPerCommandBuffer[u32FunctionIndex] = logicalQueueIndexIter - uniqueLogicalQueueIndices.begin();
			if (!alloc_vulkan_command_buffers(commandPools[commandPoolIndexPerCommandBuffer[u32FunctionIndex]], VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &commandBuffers[u32FunctionIndex])) {
				RE_FATAL_ERROR("Failed to allocate command buffer for function at index ", u32FunctionIndex);
				break;
			}
			u32FunctionIndex++;
		}
		if (u32FunctionIndex == u32FunctionsCount) {
			u8CommandPoolCount = u8CommandPoolCreateIndex;
			if (u8CommandPoolCount > 1) {
				PRINT_DEBUG_CLASS("Creating Vulkan timeline semaphore");
				const VkSemaphoreTypeCreateInfo vk_timelineSemaphoreInfo = {
					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
					.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE
				};
				const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
					.pNext = &vk_timelineSemaphoreInfo
				};
				if (vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hInternalSemaphore) == VK_SUCCESS)
					return true;
				else
					RE_ERROR("Failed to create internal Vulkan timeline semaphore to synchronize command buffers within a task");
			} else
				return true;
		} else
			for (uint8_t u8CommandPoolDestroyIndex = 0; u8CommandPoolDestroyIndex < u8CommandPoolCreateIndex; u8CommandPoolDestroyIndex++)
				vkDestroyCommandPool(vk_hDevice, commandPools[], nullptr);
		queueIndicesPerCommandPool.reset();
		commandPools.reset();
		commandPoolIndexPerCommandBuffer.reset();
		commandBuffers.reset();
		return false;
	}

	bool VulkanTask::init(const VulkanTask_Queues &rQueues, const bool bIndividualResets, const bool bTransient) {
		std::vector<uint8_t> logicalQueuesPerFunction;
		logicalQueuesPerFunction.reserve(rQueues.u32FunctionsCount);
		{
			std::vector<uint8_t> usableLogicalQueueIndices, removableQueues;
			usableLogicalQueueIndices.reserve(u8LogicalQueueCount);
			removableQueues.reserve(u8LogicalQueueCount);
			for (uint32_t u32FunctionIndex = 0; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++)
				if (rQueues.pau8LogicalQueueIndices[u32FunctionIndex] < u8LogicalQueueCount)
					logicalQueuesPerFunction.push_back(rQueues.pau8LogicalQueueIndices[u32FunctionIndex]);
				else if (u32FunctionIndex && rQueues.pau32StrictSeparationIds[u32FunctionIndex] == rQueues.pau32StrictSeparationIds[u32FunctionIndex - 1] && !logicalQueuesPerFunction.empty() && (vk_paeQueueTypes[logicalQueuesPerFunction.back()] & rQueues.vk_paeQueueTypes[u32FunctionIndex]))
					logicalQueuesPerFunction.push_back(logicalQueuesPerFunction.back());
				else {
					for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++)
						if ((vk_paeQueueTypes[u8LogicalQueueIndex] & rQueues.vk_paeQueueTypes[u32FunctionIndex]) != 0)
							usableLogicalQueueIndices.push_back(u8LogicalQueueIndex);
					for (uint32_t u32FunctionForwardIndex = u32FunctionIndex + 1; u32FunctionForwardIndex < u32FunctionIndex; u32FunctionForwardIndex++) {
						if (rQueues.pau32StrictSeparationIds[u32FunctionIndex] != rQueues.pau32StrictSeparationIds[u32FunctionForwardIndex])
							break;
						if (rQueues.pau8LogicalQueueIndices[u32FunctionForwardIndex] < u8LogicalQueueCount) {
							if (std::find(usableLogicalQueueIndices.begin(), usableLogicalQueueIndices.end(), rQueues.pau8LogicalQueueIndices[u32FunctionForwardIndex]) != usableLogicalQueueIndices.end())
								logicalQueuesPerFunction.push_back(rQueues.pau8LogicalQueueIndices[u32FunctionForwardIndex]);
							break;
						}
						for (const uint8_t u8LogicalQueueIndex : usableLogicalQueueIndices)
							if ((vk_paeQueueTypes[u8LogicalQueueIndex] & rQueues.vk_paeQueueTypes[u32FunctionForwardIndex]) == 0)
								removableQueues.push_back(u8LogicalQueueIndex);
						if (usableLogicalQueueIndices.size() <= removableQueues.size())
							break;
						for (const uint8_t u8RemovableQueueIndex : removableQueues)
							usableLogicalQueueIndices.erase(std::find(usableLogicalQueueIndices.begin(), usableLogicalQueueIndices.end(), u8RemovableQueueIndex));
						removableQueues.clear();
					}
					logicalQueuesPerFunction.push_back(find_least_featured_queue(usableLogicalQueueIndices, rQueues.vk_paeQueueTypes[u32FunctionIndex]));
					usableLogicalQueueIndices.clear();
					removableQueues.clear();
				}
		}
		return init(logicalQueuesPerFunction.size(), logicalQueuesPerFunction.data(), bIndividualResets, bTransient);
	}

	bool VulkanTask::init(const VulkanTask &rCopy, const bool bIndividualResets, const bool bTransient) {
		PRINT_DEBUG_CLASS("Copying initialized data to Vulkan task ", this, " from ", &rCopy);
		u32FunctionsCount = rCopy.u32FunctionsCount;
		u8CommandPoolCount = rCopy.u8CommandPoolCount;
		bTransient = rCopy.bTransient;
		queueIndicesPerCommandPool = rCopy.queueIndicesPerCommandPool;
		commandPools = std::make_unique<VkCommandPool[]>(u8CommandPoolCount);
		commandPoolIndexPerCommandBuffer = rCopy.commandPoolIndexPerCommandBuffer;
		commandBuffers = std::make_unique<VkCommandBuffer[]>(u32FunctionsCount);
		PRINT_DEBUG_CLASS("Creating ", u32FunctionsCount, " Vulkan command pools");
		const VkCommandPoolCreateFlags vk_eCommandPoolCreateFlags = (bIndividualResets ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0) | (bTransient ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0);
		uint8_t u8CommandPoolCreateIndex = 0;
		while (u8CommandPoolCreateIndex < u8CommandPoolCount) {
			PRINT_DEBUG_CLASS("Creating Vulkan command pool at index ", u8CommandPoolCreateIndex);
			const VkCommandPoolCreateInfo vk_cmdPoolCreateInfo = {
				.sType = VK_STRUCTURE_COMMAND_POOL_CREATE_INFO,
				.flags = vk_eCommandPoolCreateFlags,
				.queueFamilyIndex = queueIndicesPerCommandPool[u8CommandPoolCreateIndex]
			};
			if (vkCreateCommandPool(vk_hDevice, &vk_cmdPoolCreateInfo, nullptr, &commandPools[u8CommandPoolCreateIndex]) != VK_SUCCESS) {
				RE_FATAL_ERROR("Failed allocating Vulkan command pool at index ", u8CommandPoolCreateIndex);
				break;
			}
			u8CommandPoolCreateIndex++;
		}
		if (u8CommandPoolCreateIndex == u8CommandPoolCount) {
			PRINT_DEBUG_CLASS("Allocating ", u32FunctionsCount, " Vulkan command buffers");
			uint32_t u32CommandBufferCreateIndex = 0;
			while (u32CommandBufferCreateIndex < u32FunctionsCount) {
				PRINT_DEBUG_CLASS("Allocating Vulkan command buffer at index ", u32CommandBufferCreateIndex);
				if (!alloc_vulkan_command_buffers(commandPoolPerCommandBuffer[u32CommandBufferCreateIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &commandBuffers[u32CommandBufferCreateIndex])) {
					RE_FATAL_ERROR("Failed allocating Vulkan command buffer at index ", u32CommandBufferCreateIndex);
					break;
				}
				u32CommandBufferCreateIndex++;
			}
			if (u32CommandBufferCreateIndex == u32CommandBufferCount) {
				if (u8CommandPoolCount > 1) {
					PRINT_DEBUG_CLASS("Creating Vulkan timeline semaphore");
					const VkSemaphoreTypeCreateInfo vk_timelineSemaphoreCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
						.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE
					};
					const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
						.pNext = &vk_timelineSemaphoreCreateInfo
					};
					if (vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hInternalSemaphore) == VK_SUCCESS)
						return true;
					else
						RE_ERROR("Failed to create internal Vulkan timeline semaphore to synchronize command buffers within a task");
				} else
					return true;
			}
		}
		for (uint8_t u8CommandPoolDestroyIndex = 0; u8CommandPoolDestroyIndex < u8CommandPoolCreateIndex; u8CommandPoolDestroyIndex++) {
			PRINT_DEBUG_CLASS("Destroying Vulkan command pool ", commandPools[u8CommandPoolDestroyIndex], " due to failure initializing the Vulkan task");
			vkDestroyCommandPool(vk_hDevice, commandPools[u8CommandPoolDestroyIndex], nullptr);
		}
		queueIndicesPerCommandPool.reset();
		commandPools.reset();
		commandPoolIndexPerCommandBuffer.reset();
		commandBuffers.reset();
		return false;
	}

	void VulkanTask::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan task");
		for (uint8_t u8CommandPoolDestroyIndex = 0; u8CommandPoolDestroyIndex < u8CommandPoolCount; u8CommandPoolDestroyIndex++) {
			PRINT_DEBUG_CLASS("Destroying Vulkan command pool ", commandPools[u8CommandPoolDestroyIndex]);
			vkDestroyCommandPool(vk_hDevice, commandPools[u8CommandPoolDestroyIndex], nullptr);
		}
		if (u8CommandPoolCount > 1) {
			PRINT_DEBUG_CLASS("Destroying Vulkan timeline semaphore ", vk_hInternalSemaphore);
			vkDestroySemaphore(vk_hDevice, vk_hInternalSemaphore, nullptr);
		}
		queueIndicesPerCommandPool.reset();
		commandPools.reset();
		commandPoolIndexPerCommandBuffer.reset();
		commandBuffers.reset();
	}

	void VulkanTask::record(const uint32_t u32FunctionIndex, const VkCommandBufferUsageFlags vk_eUsageFlags, std::function<void (VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue)> pRecorderFunction) {
		if (begin_recording_vulkan_command_buffer(commandBuffers[u32FunctionIndex], vk_eUsageFlags, nullptr)) {
			pRecorderFunction(
				commandBuffers[u32FunctionIndex], 
				u32FunctionIndex > 0 ? queueIndicesPerCommandPool[commandPoolIndexPerCommandBuffer[u32FunctionIndex - 1]] : RE_VK_LOGICAL_QUEUE_IGNORED, 
				queueIndicesPerCommandPool[commandPoolIndexPerCommandBuffer[u32FunctionIndex]], 
				u32FunctionIndex < (u32FunctionsCount - 1) ? queueIndicesPerCommandPool[commandPoolIndexPerCommandBuffer[u32FunctionIndex + 1]] : RE_VK_LOGICAL_QUEUE_IGNORED);
			if (vkEndCommandBuffer(commandBuffers[u32FunctionIndex]) != VK_SUCCESS)
				RE_FATAL_ERROR("Failed to finish recording Vulkan command buffer ", commandBuffers[u32FunctionIndex], " at index ", u32FunctionIndex);
		} else
			RE_FATAL_ERROR("Failed beginning to record Vulkan command buffer ", commandBuffers[u32FunctionIndex], " at index ", u32FunctionIndex);
	}

	bool VulkanTask::submit(const uint32_t u32SemaphoresToWaitForCount, const VkSemaphoreSubmitInfo *const vk_paSemaphoresToWaitFor, const VkPipelineStageFlags2 *const vk_paeInternSemaphoreWaits, const uint32_t u32SemaphoresToSignal, const VkSemaphoreSubmitInfo *const vk_paSemaphoresToSignal, const VkFence vk_hFenceToSignal) {
		PRINT_DEBUG_CLASS("Submitting command buffers of Vulkan task ", this);
		std::vector<VkCommandBufferSubmitInfo> commandBufferSubmissionInfos;
		commandBufferSubmissionInfos.reserve(u32FunctionsCount / u8CommandPoolCount);
		VkSubmitInfo2 vk_submissionInfo;
		vk_submissionInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		vk_submissionInfo.pNext = nullptr;
		vk_submissionInfo.flags = 0;
		vk_submissionInfo.waitSemaphoreInfoCount = u32SemaphoresToWaitForCount;
		vk_submissionInfo.pWaitSemaphoreInfos = vk_paSemaphoresToWaitFor;
		vk_submissionInfo.signalSemaphoreInfoCount = 1;
		uint64_t u64TimelineSemaphoreValue;
		vkGetSemaphoreCounterValue(vk_hDevice, vk_hInternalSemaphore, &u64TimelineSemaphoreValue);
		constexpr uint8_t u8SemaphoreInfoCount = 2;
		VkSemaphoreSubmitInfo vk_a2InternalSemaphoreSubmissionInfo[u8SemaphoreInfoCount];
		vk_a2InternalSemaphoreSubmissionInfo[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		vk_a2InternalSemaphoreSubmissionInfo[0].pNext = nullptr;
		vk_a2InternalSemaphoreSubmissionInfo[0].semaphore = vk_hInternalSemaphore;
		vk_a2InternalSemaphoreSubmissionInfo[0].value = u64TimelineSemaphoreValue + 1;
		vk_a2InternalSemaphoreSubmissionInfo[0].deviceIndex = 0;
		vk_a2InternalSemaphoreSubmissionInfo[1].sType = vk_a2InternalSemaphoreSubmissionInfo[0].sType;
		vk_a2InternalSemaphoreSubmissionInfo[1].pNext = vk_a2InternalSemaphoreSubmissionInfo[0].pNext;
		vk_a2InternalSemaphoreSubmissionInfo[1].semaphore = vk_a2InternalSemaphoreSubmissionInfo[0].semaphore;
		vk_a2InternalSemaphoreSubmissionInfo[1].deviceIndex = vk_a2InternalSemaphoreSubmissionInfo[0].deviceIndex;
		
		VkPipelineStageFlags2 vk_eSemaphoreWaitFlags = 0;
		uint8_t u8CurrentCommandPoolIndex = commandPoolIndexPerCommandBuffer[0];
		uint8_t u8CurrentSemaphoreInfoIndex = 0;
		for (uint32_t u32FunctionIndex = 0; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++) {
			if (u8CurrentCommandPoolIndex != commandPoolIndexPerCommandBuffer[u32FunctionIndex]) {
				const uint8_t u8LogicalQueueIndex = queueIndicesPerCommandPool[u8CurrentCommandPoolIndex];
				const VkQueue vk_hLogicalQueue = vk_pahQueues[u8LogicalQueueIndex];
				vk_a2InternalSemaphoreSubmissionInfo[u8CurrentSemaphoreInfoIndex].stageMask = vk_eSemaphoreWaitFlags;
				vk_submissionInfo.waitSemaphoreInfoCount = 1;
				vk_submissionInfo.pSignalSemaphoreInfos = &vk_a2InternalSemaphoreSubmissionInfo[u8CurrentSemaphoreInfoIndex];
				if (vkQueueSubmit2(vk_hLogicalQueue, 1, &vk_submissionInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
					RE_FATAL_ERROR("Failed to submit a subtask to the logical queue ", vk_hLogicalQueue, " at index ", u8LogicalQueueIndex);
					return false;
				}
				vk_a2InternalSemaphoreSubmissionInfo[(u8CurrentSemaphoreInfoIndex + 1) % u8SemaphoreInfoCount].value = vk_a2InternalSemaphoreSubmissionInfo[u8CurrentSemaphoreInfoIndex].value + 1;
				vk_submissionInfo.pWaitSemaphoreInfos = &vk_a2InternalSemaphoreSubmissionInfo[u8CurrentSemaphoreInfoIndex];
				commandBufferSubmissionInfos.clear();
				u8CurrentCommandPoolIndex = commandPoolIndexPerCommandBuffer[u32FunctionIndex];
				u8CurrentSemaphoreInfoIndex = (u8CurrentSemaphoreInfoIndex + 1) % u8SemaphoreInfoCount;
				vk_eSemaphoreWaitFlags = 0;
			}
			commandBufferSubmissionInfos.emplace_back();
			commandBufferSubmissionInfos.back().sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			commandBufferSubmissionInfos.back().pNext = nullptr;
			commandBufferSubmissionInfos.back().commandBuffer = commandBuffers[u32FunctionIndex];
			commandBufferSubmissionInfos.back().deviceMask = 0;
			if (u32FunctionIndex > 0)
				vk_eSemaphoreWaitFlags |= vk_paeInternSemaphoreWaits[u32FunctionIndex - 1];
		}

		PRINT_DEBUG_CLASS("Submitting last subtask");
		vk_submissionInfo.signalSemaphoreInfoCount = u32SemaphoresToSignal;
		vk_submissionInfo.pSignalSemaphoreInfos = vk_paSemaphoresToSignal;
		const uint8_t u8LogicalQueueIndex = queueIndicesPerCommandPool[commandPoolIndexPerCommandBuffer[u32FunctionsCount - 1]];
		const VkQueue vk_hLogicalQueue = vk_pahQueues[u8LogicalQueueIndex];
		if (vkQueueSubmit2(vk_hLogicalQueue, 1, &vk_submissionInfo, vk_hFenceToSignal) == VK_SUCCESS) {
			RE_FATAL_ERROR("Failed to submit last subtask to the logical queue ", vk_hLogicalQueue, " at index ", u8LogicalQueueIndex);
			return false;
		}
		return true;
	}

	void VulkanTask::reset_all(const VkCommandPoolResetFlags vk_eResetFlags) const {
		for (uint8_t u8CommandPoolIndex = 0; u8CommandPoolIndex < u8CommandPoolCount; u8CommandPoolIndex++)
			vkResetCommandPool(vk_hDevice, commandPools[u8CommandPoolIndex], vk_eResetFlags);
	}

	uint32_t VulkanTask::get_function_count() const {
		return u32FunctionsCount;
	}

	uint8_t VulkanTask::get_logical_queue_index_for_function(const uint32_t u32FunctionIndex) const {
		return queueIndicesPerCommandBuffer[commandBufferIndicesPerFunction[u32FunctionIndex]];
	}

	bool VulkanTask::is_valid() const {
		return static_cast<bool>(commandBuffers);
	}

}
