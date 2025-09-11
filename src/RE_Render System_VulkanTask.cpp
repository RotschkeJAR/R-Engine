#include "RE_Render System_Internal.hpp"

namespace RE {

	static uint8_t find_next_best_logical_queue(const uint32_t u32QueueTypeCount, const VkQueueFlagBits *const vk_paeRequiredQueueTypes, const uint8_t u8LateNextLogicalQueue) {
		std::vector<uint8_t> firstUsableQueues;
		firstUsableQueues.reserve(u8LogicalQueueCount);
		for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++)
			firstUsableQueues.push_back(u8LogicalQueueIndex);
		std::vector<uint8_t> removableQueues;
		removableQueues.reserve(firstUsableQueues.size());
		VkQueueFlags vk_eQueueTypesInUse = 0;
		for (uint32_t u32QueueTypeIndex = 0; u32QueueTypeIndex < u32QueueTypeCount; u32QueueTypeIndex++) {
			for (const uint8_t u8LogicalQueueIndex : firstUsableQueues)
				if ((vk_paeQueueTypes[u8LogicalQueueIndex] & vk_paeRequiredQueueTypes[u32QueueTypeIndex]) == 0)
					removableQueues.push_back(u8LogicalQueueIndex);
				else
					vk_eQueueTypesInUse |= vk_paeRequiredQueueTypes[u32QueueTypeIndex];
			if (!removableQueues.empty()) {
				if (removableQueues.size() < firstUsableQueues.size()) {
					for (const uint8_t u8LogicalQueueToRemove : removableQueues)
						firstUsableQueues.erase(std::find(firstUsableQueues.begin(), firstUsableQueues.end(), u8LogicalQueueToRemove));
					removableQueues.clear();
					if (firstUsableQueues.size() == 1)
						break;
				} else
					break;
			}
		}
		if (u8LateNextLogicalQueue < u8LogicalQueueCount && std::find(firstUsableQueues.begin(), firstUsableQueues.end(), u8LateNextLogicalQueue) != firstUsableQueues.end())
			return u8LateNextLogicalQueue;
		else if (firstUsableQueues.size() == 1)
			return firstUsableQueues[0];
		else {
			uint8_t u8BestQueue;
			uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
			for (const uint8_t u8LogicalQueueIndex : firstUsableQueues) {
				const uint8_t u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paeQueueTypes[u8LogicalQueueIndex] & (~vk_eQueueTypesInUse));
				if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
					u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
					u8BestQueue = u8LogicalQueueIndex;
				}
			}
			return u8BestQueue;
		}
	}

	VulkanTask::VulkanTask() {}

	VulkanTask::VulkanTask(const uint32_t u32FunctionsCount, const VkQueueFlagBits *const vk_paeQueueTypePerFunctionRequiredInOrder, const bool bTransient) {
		PUSH_TO_CALLSTACKTRACE(init(u32FunctionsCount, vk_paeQueueTypePerFunctionRequiredInOrder, bTransient));
	}

	VulkanTask::VulkanTask(const uint32_t u32FunctionsCount, const uint8_t *pau8LogicalQueueIndexPerFunctionRequiredInOrder, const bool bTransient) {
		PUSH_TO_CALLSTACKTRACE(init(u32FunctionsCount, pau8LogicalQueueIndexPerFunctionRequiredInOrder, bTransient));
	}

	VulkanTask::VulkanTask(const VulkanTask &rCopy) {
		PUSH_TO_CALLSTACKTRACE(init(rCopy));
	}

	VulkanTask::~VulkanTask() {
		if (is_valid())
			PUSH_TO_CALLSTACKTRACE(destroy());
	}
	
	void VulkanTask::init(const uint32_t u32FunctionsCount, const VkQueueFlagBits *const vk_paeQueueTypePerFunctionRequiredInOrder, const bool bTransient) {
		paFunctions = std::make_unique<std::function<void (VkCommandBuffer, uint8_t, uint8_t, uint8_t)>[]>(u32FunctionsCount);
		this->u32FunctionsCount = u32FunctionsCount;
		VkQueueFlags vk_eAllRequiredQueueTypes = 0;
		for (uint32_t u32FunctionIndex = 0; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++)
			vk_eAllRequiredQueueTypes |= vk_paeQueueTypePerFunctionRequiredInOrder[u32FunctionIndex];
		std::vector<uint8_t> indicesOfPerfectQueues;
		indicesOfPerfectQueues.reserve(u8LogicalQueueCount);
		for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++)
			if ((vk_paeQueueTypes[u8LogicalQueueIndex] & vk_eAllRequiredQueueTypes) == vk_eAllRequiredQueueTypes)
				indicesOfPerfectQueues.push_back(u8LogicalQueueIndex);
		if (!indicesOfPerfectQueues.empty()) [[likely]] {
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
			commandBuffers = std::make_unique<VkCommandBuffer[]>(u32CommandBufferCount);
			PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer[0], VK_COMMAND_BUFFER_LEVEL_PRIMARY, u32CommandBufferCount, &commandBuffers[0]));
			commandBufferIndicesPerFunction = std::make_shared<uint32_t[]>(u32CommandBufferCount);
			std::fill(commandBufferIndicesPerFunction.get(), commandBufferIndicesPerFunction.get() + u32FunctionsCount, 0);
		} else [[unlikely]] {
			std::vector<uint8_t> queuesToUseInOrder;
			queuesToUseInOrder.reserve(u32FunctionsCount);
			for (uint32_t u32FunctionIndex = 0; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++)
				if (u32FunctionIndex && (queuesToUseInOrder[u32FunctionIndex - 1] & vk_paeQueueTypePerFunctionRequiredInOrder[u32FunctionIndex]))
					queuesToUseInOrder.push_back(queuesToUseInOrder[u32FunctionIndex - 1]);
				else
					queuesToUseInOrder.push_back(find_next_best_logical_queue(u32FunctionsCount - u32FunctionIndex, &vk_paeQueueTypePerFunctionRequiredInOrder[u32FunctionIndex], u8LogicalQueueCount));

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
			commandBuffers = std::make_unique<VkCommandBuffer[]>(u32CommandBufferCount);
			const uint8_t u8CommandPoolIndexOffset = bTransient ? COMMAND_POOL_OFFSET_TRANSIENT : COMMAND_POOL_OFFSET_NORMAL;
			for (uint32_t u32CommandBufferIndex = 0; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++) {
				commandPoolPerCommandBuffer[u32CommandBufferIndex] = vk_pahCommandPools[queueIndicesPerCommandBuffer[u32CommandBufferIndex] * COMMAND_POOLS_PER_QUEUE + u8CommandPoolIndexOffset];
				PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer[u32CommandBufferIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &commandBuffers[u32CommandBufferIndex]));
			}
			if (u32CommandBufferCount > 1) {
			const VkSemaphoreTypeCreateInfo vk_timelineSemaphoreCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
				.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE
			};
				const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = &vk_timelineSemaphoreCreateInfo
				};
				vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hInternalSemaphore);
			}
		}
	}

	void VulkanTask::init(const uint32_t u32FunctionsCount, const uint8_t *const pau8LogicalQueueIndexPerFunctionRequiredInOrder, const bool bTransient) {
		this->u32FunctionsCount = u32FunctionsCount;
		paFunctions = std::make_unique<std::function<void (VkCommandBuffer, uint8_t, uint8_t, uint8_t)>[]>(u32FunctionsCount);
		commandBufferIndicesPerFunction = std::make_shared<uint32_t[]>(u32FunctionsCount);
		commandBufferIndicesPerFunction[0] = 0;
		u32CommandBufferCount = 1;
		uint8_t u32CurrentLogicalQueueIndex = pau8LogicalQueueIndexPerFunctionRequiredInOrder[0];
		for (uint32_t u32FunctionIndex = 1; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++) {
			if (u32CurrentLogicalQueueIndex != pau8LogicalQueueIndexPerFunctionRequiredInOrder[u32FunctionIndex]) {
				u32CurrentLogicalQueueIndex = pau8LogicalQueueIndexPerFunctionRequiredInOrder[u32FunctionIndex];
				u32CommandBufferCount++;
			}
			commandBufferIndicesPerFunction[u32FunctionIndex] = u32CommandBufferCount - 1;
		}
		queueIndicesPerCommandBuffer = std::make_shared<uint8_t[]>(u32CommandBufferCount);
		commandPoolPerCommandBuffer = std::make_shared<VkCommandPool[]>(u32CommandBufferCount);
		uint32_t u32CommandBufferIndex = 1;
		u32CurrentLogicalQueueIndex = pau8LogicalQueueIndexPerFunctionRequiredInOrder[0];
		queueIndicesPerCommandBuffer[0] = u32CurrentLogicalQueueIndex;
		commandPoolPerCommandBuffer[0] = vk_pahCommandPools[u32CurrentLogicalQueueIndex * COMMAND_POOLS_PER_QUEUE + (bTransient ? COMMAND_POOL_OFFSET_TRANSIENT : COMMAND_POOL_OFFSET_NORMAL)];
		for (uint32_t u32FunctionIndex = 1; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++)
			if (u32CurrentLogicalQueueIndex != pau8LogicalQueueIndexPerFunctionRequiredInOrder[u32FunctionIndex]) {
				u32CurrentLogicalQueueIndex = pau8LogicalQueueIndexPerFunctionRequiredInOrder[u32FunctionIndex];
				queueIndicesPerCommandBuffer[u32CommandBufferIndex] = u32CurrentLogicalQueueIndex;
				commandPoolPerCommandBuffer[u32CommandBufferIndex] = vk_pahCommandPools[u32CurrentLogicalQueueIndex * COMMAND_POOLS_PER_QUEUE + (bTransient ? COMMAND_POOL_OFFSET_TRANSIENT : COMMAND_POOL_OFFSET_NORMAL)];
				u32CommandBufferIndex++;
			}
		commandBuffers = std::make_unique<VkCommandBuffer[]>(u32CommandBufferCount);
		u32CommandBufferIndex = 0;
		while (u32CommandBufferIndex < u32CommandBufferCount) {
			PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer[u32CommandBufferIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &commandBuffers[u32CommandBufferIndex]));
			u32CommandBufferIndex++;
		}
		if (u32CommandBufferCount > 1) {
			const VkSemaphoreTypeCreateInfo vk_timelineSemaphoreCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
				.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE
			};
			const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = &vk_timelineSemaphoreCreateInfo
			};
			vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hInternalSemaphore);
		}
	}

	void VulkanTask::init(const VulkanTask_Queues &rQueues, const bool bTransient) {
		const uint8_t *const pu8EndOfLogicalQueuesToUse = rQueues.pau8LogicalQueueIndices + u32FunctionsCount;
		u32FunctionsCount = rQueues.u32FunctionsCount;
		paFunctions = std::make_unique<std::function<void (VkCommandBuffer, uint8_t, uint8_t, uint8_t)>[]>(u32FunctionsCount);
		commandBufferIndicesPerFunction = std::make_shared<uint32_t[]>(u32FunctionsCount);
		commandBufferIndicesPerFunction[0] = 0;
		uint8_t u8CurrentLogicalQueueIndex;
		if (rQueues.pau8LogicalQueueIndices[0] < u8LogicalQueueCount)
			u8CurrentLogicalQueueIndex = rQueues.pau8LogicalQueueIndices[0];
		else {
			const uint32_t u32IndexToLateNextLogicalQueue = std::find_if(rQueues.pau8LogicalQueueIndices, pu8EndOfLogicalQueuesToUse, [&](const uint8_t &ru8LogicalQueueIndex) -> bool { return ru8LogicalQueueIndex < u8LogicalQueueCount; }) - rQueues.pau8LogicalQueueIndices;
			PUSH_TO_CALLSTACKTRACE(u8CurrentLogicalQueueIndex = find_next_best_logical_queue(u32IndexToLateNextLogicalQueue, rQueues.vk_paeQueueTypes, !u32IndexToLateNextLogicalQueue ? u8LogicalQueueCount : rQueues.pau8LogicalQueueIndices[u32IndexToLateNextLogicalQueue]));
		}
		std::vector<uint8_t> logicalQueuesInOrder;
		logicalQueuesInOrder.reserve(u32FunctionsCount);
		logicalQueuesInOrder.push_back(u8CurrentLogicalQueueIndex);
		for (uint32_t u32FunctionIndex = 1; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++) {
			if (rQueues.pau8LogicalQueueIndices[u32FunctionIndex] < u8LogicalQueueCount && rQueues.pau8LogicalQueueIndices[u32FunctionIndex] != u8CurrentLogicalQueueIndex) {
				u8CurrentLogicalQueueIndex = rQueues.pau8LogicalQueueIndices[u32FunctionIndex];
				logicalQueuesInOrder.push_back(u8CurrentLogicalQueueIndex);
			} else if ((rQueues.vk_paeQueueTypes[u32FunctionIndex] & vk_paeQueueTypes[u8CurrentLogicalQueueIndex]) == 0) {
				const uint32_t u32IndexToLateNextLogicalQueue = pu8EndOfLogicalQueuesToUse - std::find_if(rQueues.pau8LogicalQueueIndices + u32FunctionIndex, pu8EndOfLogicalQueuesToUse, [&](const uint8_t u8LogicalQueueIndex) -> bool { return u8LogicalQueueIndex < u8LogicalQueueCount; });
				PUSH_TO_CALLSTACKTRACE(u8CurrentLogicalQueueIndex = find_next_best_logical_queue(u32IndexToLateNextLogicalQueue - u32FunctionIndex, &rQueues.vk_paeQueueTypes[u32FunctionIndex], !u32IndexToLateNextLogicalQueue ? u8LogicalQueueCount : rQueues.pau8LogicalQueueIndices[u32IndexToLateNextLogicalQueue]));
				logicalQueuesInOrder.push_back(u8CurrentLogicalQueueIndex);
			}
			commandBufferIndicesPerFunction[u32FunctionIndex] = static_cast<uint32_t>(logicalQueuesInOrder.size() - 1);
		}
		u32CommandBufferCount = static_cast<uint32_t>(logicalQueuesInOrder.size());
		queueIndicesPerCommandBuffer = std::make_shared<uint8_t[]>(u32CommandBufferCount);
		commandPoolPerCommandBuffer = std::make_shared<VkCommandPool[]>(u32CommandBufferCount);
		commandBuffers = std::make_unique<VkCommandBuffer[]>(u32CommandBufferCount);
		for (uint32_t u32CommandBufferIndex = 0; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++) {
			queueIndicesPerCommandBuffer[u32CommandBufferIndex] = logicalQueuesInOrder[u32CommandBufferIndex];
			commandPoolPerCommandBuffer[u32CommandBufferIndex] = vk_pahCommandPools[logicalQueuesInOrder[u32CommandBufferIndex] * COMMAND_POOLS_PER_QUEUE + (bTransient ? COMMAND_POOL_OFFSET_TRANSIENT : COMMAND_POOL_OFFSET_NORMAL)];
			PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer[u32CommandBufferIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &commandBuffers[u32CommandBufferIndex]));
		}
		if (u32CommandBufferCount > 1) {
			const VkSemaphoreTypeCreateInfo vk_timelineSemaphoreCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
				.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE
			};
			const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = &vk_timelineSemaphoreCreateInfo
			};
			vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hInternalSemaphore);
		}
	}

	void VulkanTask::init(const VulkanTask &rCopy) {
		u32FunctionsCount = rCopy.u32FunctionsCount;
		u32CommandBufferCount = rCopy.u32CommandBufferCount;
		queueIndicesPerCommandBuffer = rCopy.queueIndicesPerCommandBuffer;
		commandPoolPerCommandBuffer = rCopy.commandPoolPerCommandBuffer;
		commandBufferIndicesPerFunction = rCopy.commandBufferIndicesPerFunction;
		commandBuffers = std::make_unique<VkCommandBuffer[]>(u32CommandBufferCount);
		paFunctions = std::make_unique<std::function<void (VkCommandBuffer, uint8_t, uint8_t, uint8_t)>[]>(rCopy.u32FunctionsCount);
		for (uint32_t u32CommandBufferIndex = 0; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++)
			PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer[u32CommandBufferIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &commandBuffers[u32CommandBufferIndex]));
		if (u32CommandBufferCount > 1) {
			const VkSemaphoreTypeCreateInfo vk_timelineSemaphoreCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
				.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE
			};
			const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = &vk_timelineSemaphoreCreateInfo
			};
			vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hInternalSemaphore);
		}
	}

	void VulkanTask::destroy() {
		for (uint32_t u32CommandBufferIndex = 0; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++)
			vkFreeCommandBuffers(vk_hDevice, commandPoolPerCommandBuffer[u32CommandBufferIndex], u32CommandBufferCount, &commandBuffers[u32CommandBufferIndex]);
		if (u32CommandBufferCount > 1)
			vkDestroySemaphore(vk_hDevice, vk_hInternalSemaphore, nullptr);
		queueIndicesPerCommandBuffer.reset();
		commandPoolPerCommandBuffer.reset();
		commandBuffers.reset();
		commandBufferIndicesPerFunction.reset();
		paFunctions.reset();
	}

	void VulkanTask::record(const VkCommandBufferUsageFlags vk_eUsageFlags) {
		uint32_t u32CommandBufferIndex = 0;
		begin_recording_vulkan_command_buffer(commandBuffers[u32CommandBufferIndex], vk_eUsageFlags, nullptr);
		for (uint32_t u32FunctionIndex = 0; u32FunctionIndex < u32FunctionsCount; u32FunctionIndex++) {
			if (u32CommandBufferIndex != commandBufferIndicesPerFunction[u32FunctionIndex]) {
				if (u32CommandBufferIndex > 0)
					vkEndCommandBuffer(commandBuffers[u32CommandBufferIndex - 1]);
				u32CommandBufferIndex = commandBufferIndicesPerFunction[u32FunctionIndex];
				begin_recording_vulkan_command_buffer(commandBuffers[u32CommandBufferIndex], vk_eUsageFlags, nullptr);
			}
			const uint8_t u8PreviousLogicalQueue = u32FunctionIndex > 0 ? get_logical_queue_index_for_function(u32FunctionIndex - 1) : RE_VK_LOGICAL_QUEUE_IGNORED;
			uint8_t u8NextLogicalQueue = (u32FunctionsCount > 1 && u32FunctionIndex < (u32FunctionsCount - 1)) ?  get_logical_queue_index_for_function(u32FunctionIndex + 1) : RE_VK_LOGICAL_QUEUE_IGNORED;
			PUSH_TO_CALLSTACKTRACE_DETAILED(paFunctions[u32FunctionIndex](commandBuffers[u32CommandBufferIndex], u8PreviousLogicalQueue, get_logical_queue_index_for_function(u32FunctionIndex), u8NextLogicalQueue), append_to_string("Function index ", u32FunctionIndex).c_str());
		}
		vkEndCommandBuffer(commandBuffers[commandBufferIndicesPerFunction[u32FunctionsCount - 1]]);
	}

	void VulkanTask::submit(const uint32_t u32SemaphoresToWaitForCount, const VkSemaphoreSubmitInfo *const vk_paSemaphoresToWaitFor, const uint32_t u32SemaphoresToSignal, const VkSemaphoreSubmitInfo *const vk_paSemaphoresToSignal, const VkFence vk_hFenceToSignal) {
		VkCommandBufferSubmitInfo vk_commandBufferSubmitInfo;
		vk_commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		vk_commandBufferSubmitInfo.pNext = nullptr;
		vk_commandBufferSubmitInfo.commandBuffer = commandBuffers[0];
		vk_commandBufferSubmitInfo.deviceMask = 0;
		VkSubmitInfo2 vk_submissionInfo;
		vk_submissionInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		vk_submissionInfo.pNext = nullptr;
		vk_submissionInfo.flags = 0;
		vk_submissionInfo.waitSemaphoreInfoCount = u32SemaphoresToWaitForCount;
		vk_submissionInfo.pWaitSemaphoreInfos = vk_paSemaphoresToWaitFor;
		vk_submissionInfo.commandBufferInfoCount = 1;
		vk_submissionInfo.pCommandBufferInfos = &vk_commandBufferSubmitInfo;
		if (u32CommandBufferCount == 1) {
			vk_submissionInfo.signalSemaphoreInfoCount = u32SemaphoresToSignal;
			vk_submissionInfo.pSignalSemaphoreInfos = vk_paSemaphoresToSignal;
			vkQueueSubmit2(vk_pahQueues[queueIndicesPerCommandBuffer[0]], 1, &vk_submissionInfo, vk_hFenceToSignal);
		} else {
			VkSemaphoreSubmitInfo vk_a2InternalSemaphoreSubmissionInfo[2] = {
				{
					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
					.semaphore = vk_hInternalSemaphore,
					.value = 1,
					.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
					.deviceIndex = 0
				}, {
					.sType = vk_a2InternalSemaphoreSubmissionInfo[0].sType,
					.semaphore = vk_a2InternalSemaphoreSubmissionInfo[0].semaphore,
					.value = 2,
					.stageMask = vk_a2InternalSemaphoreSubmissionInfo[0].stageMask,
					.deviceIndex = vk_a2InternalSemaphoreSubmissionInfo[0].deviceIndex
				}
			};
			vk_submissionInfo.signalSemaphoreInfoCount = 1;
			vk_submissionInfo.pSignalSemaphoreInfos = &vk_a2InternalSemaphoreSubmissionInfo[0];
			vkQueueSubmit2(vk_pahQueues[queueIndicesPerCommandBuffer[0]], 1, &vk_submissionInfo, VK_NULL_HANDLE);
			vk_submissionInfo.waitSemaphoreInfoCount = 1;
			vk_submissionInfo.pWaitSemaphoreInfos = &vk_a2InternalSemaphoreSubmissionInfo[0];
			vk_submissionInfo.pSignalSemaphoreInfos = &vk_a2InternalSemaphoreSubmissionInfo[1];
			for (uint32_t u32CommandBufferIndex = 1; u32CommandBufferIndex < u32CommandBufferCount - 1; u32CommandBufferIndex++) {
				vk_a2InternalSemaphoreSubmissionInfo[u32CommandBufferIndex % 2].value = u32CommandBufferIndex + 1;
				vk_commandBufferSubmitInfo.commandBuffer = commandBuffers[u32CommandBufferIndex];
				vkQueueSubmit2(vk_pahQueues[queueIndicesPerCommandBuffer[u32CommandBufferIndex]], 1, &vk_submissionInfo, VK_NULL_HANDLE);
				std::swap(vk_submissionInfo.pWaitSemaphoreInfos, vk_submissionInfo.pSignalSemaphoreInfos);
			}
			vk_submissionInfo.signalSemaphoreInfoCount = u32SemaphoresToSignal;
			vk_submissionInfo.pSignalSemaphoreInfos = vk_paSemaphoresToSignal;
			vkQueueSubmit2(vk_pahQueues[queueIndicesPerCommandBuffer[u32CommandBufferCount - 1]], 1, &vk_submissionInfo, vk_hFenceToSignal);
		}
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
