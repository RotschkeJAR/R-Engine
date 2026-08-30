#include "RE_GPU_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"

namespace RE {

	static unsigned find_least_featured_queue(const std::vector<unsigned> &rLogicalQueueIndices, const VkQueueFlags vk_eRequiredQueueTypes) {
		if (rLogicalQueueIndices.size() == 1)
			return rLogicalQueueIndices[0];
		unsigned uLowestCountForSideFeatures = UINT_MAX,
			uBestQueue = 0;
		for (const unsigned uLogicalQueueIndex : rLogicalQueueIndices) {
			const unsigned uSideFeatureCount = std::popcount<VkQueueFlags>(std_queueTypes[uLogicalQueueIndex] & (~vk_eRequiredQueueTypes));
			if (uLowestCountForSideFeatures > uSideFeatureCount) {
				uLowestCountForSideFeatures = uSideFeatureCount;
				uBestQueue = uLogicalQueueIndex;
			}
		}
		return uBestQueue;
	}

	VulkanTask::VulkanTask() {}

	VulkanTask::VulkanTask(const unsigned uFunctionsCount, const VkQueueFlagBits *const vk_paeQueueTypePerFunctionRequiredInOrder, const bool bIndividualResets, const bool bIncludePresentation, const bool bTransient) {
		init(uFunctionsCount, vk_paeQueueTypePerFunctionRequiredInOrder, bIndividualResets, bIncludePresentation, bTransient);
	}

	VulkanTask::VulkanTask(const unsigned uFunctionsCount, const unsigned *pauLogicalQueueIndexPerFunctionRequiredInOrder, const bool bIndividualResets, const bool bIncludePresentation, const bool bTransient) {
		init(uFunctionsCount, pauLogicalQueueIndexPerFunctionRequiredInOrder, bIndividualResets, bIncludePresentation, bTransient);
	}

	VulkanTask::VulkanTask(const VulkanTask_Queues &rQueues, const bool bIndividualResets, const bool bIncludePresentation, const bool bTransient) {
		init(rQueues, bIndividualResets, bIncludePresentation, bTransient);
	}

	VulkanTask::VulkanTask(const VulkanTask &rCopy, const bool bIndividualResets, const bool bTransient) {
		init(rCopy, bIndividualResets, bTransient);
	}

	VulkanTask::VulkanTask(VulkanTask &&rrTask) {
		std_queueIndexPerCommandPool = rrTask.std_queueIndexPerCommandPool;
		rrTask.std_queueIndexPerCommandPool.reset();
		std_commandPools = std::move(rrTask.std_commandPools);
		std_commandPoolIndexPerCommandBuffer = rrTask.std_commandPoolIndexPerCommandBuffer;
		rrTask.std_commandPoolIndexPerCommandBuffer.reset();
		std_commandBuffers = std::move(rrTask.std_commandBuffers);
		vk_hInternalSemaphore = rrTask.vk_hInternalSemaphore;
		uFunctionsCount = rrTask.uFunctionsCount;
		uCommandPoolCount = rrTask.uCommandPoolCount;
		uLogicalPresentQueueIndex = rrTask.uLogicalPresentQueueIndex;
		bTransient = rrTask.bTransient;
	}

	VulkanTask::~VulkanTask() {
		if (valid())
			destroy();
	}
	
	bool VulkanTask::init(const unsigned uFunctionsCount, const VkQueueFlagBits *const vk_paeQueueTypePerFunctionRequiredInOrder, const bool bIndividualResets, const bool bIncludePresentation, const bool bTransient) {
		PRINT_DEBUG_CLASS("Initializing Vulkan task based on queue types in ", vk_paeQueueTypePerFunctionRequiredInOrder, " containing ", uFunctionsCount, " functions");
		std::vector<unsigned> std_logicalQueuesPerFunction;
		std_logicalQueuesPerFunction.reserve(uFunctionsCount);
		{
			std::vector<unsigned> std_usableLogicalQueueIndices,
				std_removableQueues;
			std_usableLogicalQueueIndices.reserve(uLogicalQueueCount);
			std_removableQueues.reserve(uLogicalQueueCount);
			for (unsigned uFunctionIndex = 0; uFunctionIndex < uFunctionsCount; uFunctionIndex++) {
				const VkQueueFlagBits vk_eQueueType = vk_paeQueueTypePerFunctionRequiredInOrder[uFunctionIndex];
				PRINT_DEBUG_CLASS("Traversing function at index ", uFunctionIndex, " requiring queue type ", std::hex, vk_eQueueType);
				if (!std_logicalQueuesPerFunction.empty()) {
					PRINT_DEBUG_CLASS("Adding all logical queues supporting required queue type");
					if ((std_queueTypes[std_logicalQueuesPerFunction.back()] & vk_eQueueType) != 0) {
						std_logicalQueuesPerFunction.push_back(std_logicalQueuesPerFunction.back());
						continue;
					}
				}
				for (unsigned uLogicalQueueIndex = 0; uLogicalQueueIndex < uLogicalQueueCount; uLogicalQueueIndex++)
					if ((std_queueTypes[uLogicalQueueIndex] & vk_eQueueType) != 0)
						std_usableLogicalQueueIndices.push_back(uLogicalQueueIndex);
				VkQueueFlags vk_eForwardQueueTypes = vk_eQueueType;
				for (unsigned uFunctionForwardIndex = 0; uFunctionForwardIndex < uFunctionsCount; uFunctionForwardIndex++) {
					const VkQueueFlagBits vk_eForwardQueueType = vk_paeQueueTypePerFunctionRequiredInOrder[uFunctionForwardIndex];
					for (const unsigned uLogicalQueueIndex : std_usableLogicalQueueIndices)
						if ((std_queueTypes[uLogicalQueueIndex] & vk_eForwardQueueType) == 0)
							std_removableQueues.push_back(uLogicalQueueIndex);
					if (!std_removableQueues.empty()) {
						if (std_removableQueues.size() == std_usableLogicalQueueIndices.size()) {
							PRINT_DEBUG_CLASS("No more queue supports the required type. Finding and using least occupied queue");
							std_logicalQueuesPerFunction.push_back(find_least_featured_queue(std_usableLogicalQueueIndices, vk_eForwardQueueTypes));
							std_usableLogicalQueueIndices.clear();
							std_removableQueues.clear();
							break;
						} else
							for (const unsigned uRemovableQueueIndex : std_removableQueues) {
								const auto xUsableLogicalQueueIndicesIter = std::find(std_usableLogicalQueueIndices.begin(), std_usableLogicalQueueIndices.end(), uRemovableQueueIndex);
								if (xUsableLogicalQueueIndicesIter != std_usableLogicalQueueIndices.end()) {
									std_usableLogicalQueueIndices.erase(xUsableLogicalQueueIndicesIter);
									continue;
								}
							}
						std_removableQueues.clear();
					}
					vk_eForwardQueueTypes |= vk_eForwardQueueType;
				}
				if (!std_usableLogicalQueueIndices.empty()) {
					std_logicalQueuesPerFunction.push_back(find_least_featured_queue(std_usableLogicalQueueIndices, vk_eForwardQueueTypes));
					std_usableLogicalQueueIndices.clear();
				}
			}
		}
		return init(uFunctionsCount, std_logicalQueuesPerFunction.data(), bIndividualResets, bIncludePresentation, bTransient);
	}

	bool VulkanTask::init(const unsigned uFunctionsCount, const unsigned *const pauLogicalQueueIndexPerFunctionRequiredInOrder, const bool bIndividualResets, const bool bIncludePresentation, const bool bTransient) {
		PRINT_DEBUG_CLASS("Initializing Vulkan task based on logical queue indices in ", pauLogicalQueueIndexPerFunctionRequiredInOrder, " containing ", uFunctionsCount, " functions");
		this->uFunctionsCount = uFunctionsCount;
		this->bTransient = bTransient;
		PRINT_DEBUG_CLASS("Creating Vulkan command pools per logical queue");
		using QueueInfo = std::tuple<unsigned /*logical queue index*/, uint32_t /*command buffer count*/, VkCommandPool>;
	#define LOGICAL_QUEUE_INDEX_TUPLE   0
	#define CMD_BUFFER_COUNT_TUPLE      1
	#define CMD_POOL_TUPLE              2
		std::vector<QueueInfo> std_uniqueLogicalQueueInfos;
		std_uniqueLogicalQueueInfos.reserve(uLogicalQueueCount);
		VkCommandPoolCreateInfo vk_cmdPoolCreateInfo;
		vk_cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		vk_cmdPoolCreateInfo.pNext = nullptr;
		vk_cmdPoolCreateInfo.flags = (bIndividualResets ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0) | (bTransient ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0);
		unsigned uFunctionIndex = 0;
		while (uFunctionIndex < uFunctionsCount) {
			const unsigned uLogicalQueueIndex = pauLogicalQueueIndexPerFunctionRequiredInOrder[uFunctionIndex];
			PRINT_DEBUG_CLASS("Traversing logical queue index ", uLogicalQueueIndex, " for function at index ", uFunctionIndex);
			const auto xUniqueLogicalQueueInfoIter = std::find_if(std_uniqueLogicalQueueInfos.begin(), std_uniqueLogicalQueueInfos.end(), [&](const QueueInfo &rQueueInfo) -> bool {
				return std::get<LOGICAL_QUEUE_INDEX_TUPLE>(rQueueInfo) == uLogicalQueueIndex;
			});
			if (xUniqueLogicalQueueInfoIter == std_uniqueLogicalQueueInfos.end()) {
				PRINT_DEBUG_CLASS("Creating Vulkan command pool linked to queue family connected to logical queue index ", uLogicalQueueIndex);
				VkCommandPool vk_hCreatedCommandPool;
				vk_cmdPoolCreateInfo.queueFamilyIndex = std_queueFamilyIndices[uLogicalQueueIndex];
				if (vkCreateCommandPool(vk_hDevice, &vk_cmdPoolCreateInfo, nullptr, &vk_hCreatedCommandPool) != VK_SUCCESS) {
					RE_ERROR("Failed creating Vulkan command pool connected to queue family ", vk_cmdPoolCreateInfo.queueFamilyIndex);
					break;
				}
				std_uniqueLogicalQueueInfos.emplace_back(uLogicalQueueIndex, 1, vk_hCreatedCommandPool);
			} else {
				PRINT_DEBUG_CLASS("Incrementing command buffer count at logical queue index ", uLogicalQueueIndex);
				std::get<CMD_BUFFER_COUNT_TUPLE>(*xUniqueLogicalQueueInfoIter) += 1;
			}
			uFunctionIndex++;
		}
		if (uFunctionIndex == uFunctionsCount) {
			PRINT_DEBUG_CLASS("Allocating Vulkan command buffers per function");
			uCommandPoolCount = std_uniqueLogicalQueueInfos.size();
			std_queueIndexPerCommandPool = std::make_shared<unsigned[]>(uCommandPoolCount);
			std_commandPools = std::make_unique<VkCommandPool[]>(uCommandPoolCount);
			using CommandPoolInfo = std::tuple<unsigned /*command pool index*/, std::vector<VkCommandBuffer>>;
		#define CMD_POOL_INDEX_TUPLE      0
		#define CMD_BUFFER_VECTOR_TUPLE   1
			std::vector<CommandPoolInfo> std_commandBuffersPerPool;
			std_commandBuffersPerPool.resize(uLogicalQueueCount);
			unsigned uCommandPoolIndex = 0;
			while (uCommandPoolIndex < uCommandPoolCount) {
				const uint32_t u32CommandBuffersToAlloc = std::get<CMD_BUFFER_COUNT_TUPLE>(std_uniqueLogicalQueueInfos[uCommandPoolIndex]);
				const unsigned uLogicalQueueIndex = std::get<LOGICAL_QUEUE_INDEX_TUPLE>(std_uniqueLogicalQueueInfos[uCommandPoolIndex]);
				PRINT_DEBUG_CLASS("Allocating ", u32CommandBuffersToAlloc, " Vulkan command buffers in command pool linked to logical queue at index ", uLogicalQueueIndex);
				std::get<CMD_BUFFER_VECTOR_TUPLE>(std_commandBuffersPerPool[uLogicalQueueIndex]).resize(u32CommandBuffersToAlloc);
				std_queueIndexPerCommandPool[uCommandPoolIndex] = uLogicalQueueIndex;
				std_commandPools[uCommandPoolIndex] = std::get<CMD_POOL_TUPLE>(std_uniqueLogicalQueueInfos[uCommandPoolIndex]);
				std::get<CMD_POOL_INDEX_TUPLE>(std_commandBuffersPerPool[uLogicalQueueIndex]) = uCommandPoolIndex;
				if (!alloc_vulkan_command_buffers(std_commandPools[uCommandPoolIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, u32CommandBuffersToAlloc, std::get<1>(std_commandBuffersPerPool[uLogicalQueueIndex]).data())) {
					RE_ERROR("Failed to allocate ", u32CommandBuffersToAlloc, " in command pool ", std_commandPools[uCommandPoolIndex], " for Vulkan task");
					break;
				}
				uCommandPoolIndex++;
			}
			if (uCommandPoolIndex == uCommandPoolCount) {
				PRINT_DEBUG_CLASS("Sorting Vulkan command buffers to functions");
				std_commandPoolIndexPerCommandBuffer = std::make_shared<unsigned[]>(uFunctionsCount);
				std_commandBuffers = std::make_unique<VkCommandBuffer[]>(uFunctionsCount);
				for (uint32_t uFunctionIndex = 0; uFunctionIndex < uFunctionsCount; uFunctionIndex++) {
					PRINT_DEBUG_CLASS("Assigning Vulkan command buffer to function at index ", uFunctionIndex);
					const unsigned uLogicalQueueIndex = pauLogicalQueueIndexPerFunctionRequiredInOrder[uFunctionIndex];
					std_commandPoolIndexPerCommandBuffer[uFunctionIndex] = std::get<CMD_POOL_INDEX_TUPLE>(std_commandBuffersPerPool[uLogicalQueueIndex]);
					std_commandBuffers[uFunctionIndex] = std::get<CMD_BUFFER_VECTOR_TUPLE>(std_commandBuffersPerPool[uLogicalQueueIndex]).back();
					std::get<CMD_BUFFER_VECTOR_TUPLE>(std_commandBuffersPerPool[uLogicalQueueIndex]).pop_back();
				}
				if (bIncludePresentation) {
					const unsigned uLogicalQueueIndexOfLastFunction = std_queueIndexPerCommandPool[std_commandPoolIndexPerCommandBuffer[uFunctionsCount - 1]];
					if (std_presentationAvailablePerQueue[uLogicalQueueIndexOfLastFunction])
						uLogicalPresentQueueIndex = uLogicalQueueIndexOfLastFunction;
					else {
						unsigned uLeastSideFeaturesInQueue = UINT_MAX;
						for (unsigned uLogicalQueueIndex = 0; uLogicalQueueIndex < uLogicalQueueCount; uLogicalQueueIndex++) {
							if (!std_presentationAvailablePerQueue[uLogicalQueueIndex])
								continue;
							const unsigned uSideFeaturesCount = std::popcount<VkQueueFlags>(std_queueTypes[uLogicalQueueIndex]);
							if (uSideFeaturesCount < uLeastSideFeaturesInQueue) {
								uLeastSideFeaturesInQueue = uSideFeaturesCount;
								uLogicalPresentQueueIndex = uLogicalQueueIndex;
							}
						}
					}
				} else
					uLogicalPresentQueueIndex = RE_VK_LOGICAL_QUEUE_IGNORED;
				if (uCommandPoolCount > 1) {
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
				PRINT_DEBUG_CLASS("Freeing Vulkan command buffer arrays due to failure creating Vulkan timeline semaphore");
				std_commandPoolIndexPerCommandBuffer.reset();
				std_commandBuffers.reset();
			}
			PRINT_DEBUG_CLASS("Freeing Vulkan command pool arrays due to failure initializing the Vulkan task entirely");
			std_queueIndexPerCommandPool.reset();
			std_commandPools.reset();
		}
		PRINT_DEBUG_CLASS("Destroying Vulkan command pools due to failure initializing the Vulkan task entirely");
		for (const QueueInfo &rQueueInfo : std_uniqueLogicalQueueInfos)
			vkDestroyCommandPool(vk_hDevice, std::get<CMD_POOL_TUPLE>(rQueueInfo), nullptr);
		return false;
	#undef LOGICAL_QUEUE_INDEX_TUPLE
	#undef CMD_BUFFER_COUNT_TUPLE
	#undef CMD_POOL_TUPLE
	#undef CMD_POOL_INDEX_TUPLE
	#undef CMD_BUFFER_VECTOR_TUPLE
	}

	bool VulkanTask::init(const VulkanTask_Queues &rQueues, const bool bIndividualResets, const bool bIncludePresentation, const bool bTransient) {
		PRINT_DEBUG_CLASS("Initializing Vulkan task based on logical queue indices, types and separation IDs");
		std::vector<unsigned> std_logicalQueuesPerFunction;
		std_logicalQueuesPerFunction.reserve(rQueues.uFunctionsCount);
		{
			std::vector<unsigned> std_usableLogicalQueueIndices,
				std_removableQueues;
			std_usableLogicalQueueIndices.reserve(uLogicalQueueCount);
			std_removableQueues.reserve(uLogicalQueueCount);
			for (unsigned uFunctionIndex = 0; uFunctionIndex < rQueues.uFunctionsCount; uFunctionIndex++) {
				PRINT_DEBUG_CLASS("Iterating through details of function at ", uFunctionIndex);
				if (rQueues.pauLogicalQueueIndices[uFunctionIndex] < uLogicalQueueCount) {
					PRINT_DEBUG_CLASS("Adding logical queue index ", rQueues.pauLogicalQueueIndices[uFunctionIndex]);
					std_logicalQueuesPerFunction.push_back(rQueues.pauLogicalQueueIndices[uFunctionIndex]);
				} else if (uFunctionIndex && rQueues.pauStrictSeparationIds[uFunctionIndex] == rQueues.pauStrictSeparationIds[uFunctionIndex - 1] && !std_logicalQueuesPerFunction.empty() && (std_queueTypes[std_logicalQueuesPerFunction.back()] & rQueues.vk_paeQueueTypes[uFunctionIndex])) {
					PRINT_DEBUG_CLASS("Reusing logical queue ", std_logicalQueuesPerFunction.back());
					std_logicalQueuesPerFunction.push_back(std_logicalQueuesPerFunction.back());
				} else {
					PRINT_DEBUG_CLASS("Resolving queue type ", rQueues.vk_paeQueueTypes[uFunctionIndex], " into logical queue index");
					for (unsigned uLogicalQueueIndex = 0; uLogicalQueueIndex < uLogicalQueueCount; uLogicalQueueIndex++)
						if ((std_queueTypes[uLogicalQueueIndex] & rQueues.vk_paeQueueTypes[uFunctionIndex]) != 0)
							std_usableLogicalQueueIndices.push_back(uLogicalQueueIndex);
					for (unsigned uFunctionForwardIndex = uFunctionIndex + 1; uFunctionForwardIndex < uFunctionIndex; uFunctionForwardIndex++) {
						if (rQueues.pauStrictSeparationIds[uFunctionIndex] != rQueues.pauStrictSeparationIds[uFunctionForwardIndex])
							break;
						if (rQueues.pauLogicalQueueIndices[uFunctionForwardIndex] < uLogicalQueueCount) {
							if (std::find(std_usableLogicalQueueIndices.begin(), std_usableLogicalQueueIndices.end(), rQueues.pauLogicalQueueIndices[uFunctionForwardIndex]) != std_usableLogicalQueueIndices.end())
								std_logicalQueuesPerFunction.push_back(rQueues.pauLogicalQueueIndices[uFunctionForwardIndex]);
							goto BREAK_SEARCH_AND_CLEAR_USABLE_QUEUES;
						}
						for (const unsigned uLogicalQueueIndex : std_usableLogicalQueueIndices)
							if ((std_queueTypes[uLogicalQueueIndex] & rQueues.vk_paeQueueTypes[uFunctionForwardIndex]) == 0)
								std_removableQueues.push_back(uLogicalQueueIndex);
						if (std_usableLogicalQueueIndices.size() <= std_removableQueues.size())
							break;
						for (const unsigned uRemovableQueueIndex : std_removableQueues)
							std_usableLogicalQueueIndices.erase(std::find(std_usableLogicalQueueIndices.begin(), std_usableLogicalQueueIndices.end(), uRemovableQueueIndex));
						std_removableQueues.clear();
					}
					std_logicalQueuesPerFunction.push_back(find_least_featured_queue(std_usableLogicalQueueIndices, rQueues.vk_paeQueueTypes[uFunctionIndex]));
					BREAK_SEARCH_AND_CLEAR_USABLE_QUEUES:
						std_usableLogicalQueueIndices.clear();
					std_removableQueues.clear();
				}
				PRINT_DEBUG_CLASS("Next logical queue added ", std_logicalQueuesPerFunction.back());
			}
		}
		return init(std_logicalQueuesPerFunction.size(), std_logicalQueuesPerFunction.data(), bIndividualResets, bIncludePresentation, bTransient);
	}

	bool VulkanTask::init(const VulkanTask &rCopy, const bool bIndividualResets, const bool bTransient) {
		PRINT_DEBUG_CLASS("Copying initialized data from ", &rCopy);
		uFunctionsCount = rCopy.uFunctionsCount;
		uCommandPoolCount = rCopy.uCommandPoolCount;
		uLogicalPresentQueueIndex = rCopy.uLogicalPresentQueueIndex;
		this->bTransient = rCopy.bTransient;
		std_queueIndexPerCommandPool = rCopy.std_queueIndexPerCommandPool;
		std_commandPools = std::make_unique<VkCommandPool[]>(uCommandPoolCount);
		std_commandPoolIndexPerCommandBuffer = rCopy.std_commandPoolIndexPerCommandBuffer;
		std_commandBuffers = std::make_unique<VkCommandBuffer[]>(uFunctionsCount);
		PRINT_DEBUG_CLASS("Creating ", uCommandPoolCount, " Vulkan command pool/-s");
		const VkCommandPoolCreateFlags vk_eCommandPoolCreateFlags = (bIndividualResets ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0) | (bTransient ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0);
		unsigned uCommandPoolCreateIndex = 0;
		while (uCommandPoolCreateIndex < uCommandPoolCount) {
			PRINT_DEBUG_CLASS("Creating Vulkan command pool at index ", uCommandPoolCreateIndex);
			const VkCommandPoolCreateInfo vk_cmdPoolCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.flags = vk_eCommandPoolCreateFlags,
				.queueFamilyIndex = std_queueFamilyIndices[std_queueIndexPerCommandPool[uCommandPoolCreateIndex]]
			};
			if (vkCreateCommandPool(vk_hDevice, &vk_cmdPoolCreateInfo, nullptr, &std_commandPools[uCommandPoolCreateIndex]) != VK_SUCCESS) {
				RE_ERROR("Failed allocating Vulkan command pool at index ", uCommandPoolCreateIndex);
				break;
			}
			PRINT_DEBUG_CLASS("Created Vulkan command pool ", std_commandPools[uCommandPoolCreateIndex], " successfully at index ", uCommandPoolCreateIndex);
			uCommandPoolCreateIndex++;
		}
		if (uCommandPoolCreateIndex == uCommandPoolCount) {
			PRINT_DEBUG_CLASS("Allocating ", uFunctionsCount, " Vulkan command buffer/-s");
			unsigned uCommandBufferCreateIndex = 0;
			while (uCommandBufferCreateIndex < uFunctionsCount) {
				unsigned uCommandBufferAllocCount = 1;
				while ((uCommandBufferCreateIndex + uCommandBufferAllocCount) < uFunctionsCount
						&& std_commandPoolIndexPerCommandBuffer[uCommandBufferCreateIndex] == std_commandPoolIndexPerCommandBuffer[uCommandBufferCreateIndex + uCommandBufferAllocCount])
					uCommandBufferAllocCount++;
				PRINT_DEBUG_CLASS("Allocating ", uCommandBufferAllocCount, " Vulkan command buffer/-s at function index ", uCommandBufferCreateIndex, " (and onward)");
				if (!alloc_vulkan_command_buffers(
						std_commandPools[std_commandPoolIndexPerCommandBuffer[uCommandBufferCreateIndex]],
						VK_COMMAND_BUFFER_LEVEL_PRIMARY,
						uCommandBufferAllocCount,
						&std_commandBuffers[uCommandBufferCreateIndex])) {
					RE_ERROR("Failed allocating ", uCommandBufferAllocCount, " Vulkan command buffer at index ", uCommandBufferCreateIndex);
					break;
				}
				uCommandBufferCreateIndex += uCommandBufferAllocCount;
			}
			if (uCommandBufferCreateIndex == uFunctionsCount) {
				if (uCommandPoolCount > 1) {
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
		for (unsigned uCommandPoolDestroyIndex = 0; uCommandPoolDestroyIndex < uCommandPoolCreateIndex; uCommandPoolDestroyIndex++) {
			PRINT_DEBUG_CLASS("Destroying Vulkan command pool ", std_commandPools[uCommandPoolDestroyIndex], " due to failure initializing the Vulkan task");
			vkDestroyCommandPool(vk_hDevice, std_commandPools[uCommandPoolDestroyIndex], nullptr);
		}
		std_queueIndexPerCommandPool.reset();
		std_commandPools.reset();
		std_commandPoolIndexPerCommandBuffer.reset();
		std_commandBuffers.reset();
		return false;
	}

	void VulkanTask::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan task");
		for (unsigned uCommandPoolDestroyIndex = 0; uCommandPoolDestroyIndex < uCommandPoolCount; uCommandPoolDestroyIndex++) {
			PRINT_DEBUG_CLASS("Destroying Vulkan command pool ", std_commandPools[uCommandPoolDestroyIndex]);
			vkDestroyCommandPool(vk_hDevice, std_commandPools[uCommandPoolDestroyIndex], nullptr);
		}
		if (uCommandPoolCount > 1) {
			PRINT_DEBUG_CLASS("Destroying Vulkan timeline semaphore ", vk_hInternalSemaphore);
			vkDestroySemaphore(vk_hDevice, vk_hInternalSemaphore, nullptr);
		}
		std_queueIndexPerCommandPool.reset();
		std_commandPools.reset();
		std_commandPoolIndexPerCommandBuffer.reset();
		std_commandBuffers.reset();
	}

	bool VulkanTask::record(
			unsigned uFunctionIndex,
			VkCommandBufferUsageFlags vk_eUsageFlags,
			std::function<void (VkCommandBuffer vk_hCommandBuffer, unsigned uPreviousLogicalQueue, unsigned uCurrentLogicalQueue, unsigned uNextLogicalQueue)> std_recorderFunction) const {
		PRINT_DEBUG_CLASS("Beginning to record Vulkan command buffer of function at index ", uFunctionIndex);
		if (begin_recording_vulkan_command_buffer(std_commandBuffers[uFunctionIndex], vk_eUsageFlags, nullptr)) {
			if (static_cast<bool>(std_recorderFunction)) {
				PRINT_DEBUG_CLASS("Calling record function");
				std::invoke(
						std_recorderFunction,
						std_commandBuffers[uFunctionIndex], 
						uFunctionIndex > 0 ? std_queueIndexPerCommandPool[std_commandPoolIndexPerCommandBuffer[uFunctionIndex - 1]] : RE_VK_LOGICAL_QUEUE_IGNORED, 
						std_queueIndexPerCommandPool[std_commandPoolIndexPerCommandBuffer[uFunctionIndex]], 
						uFunctionIndex < (uFunctionsCount - 1) ? std_queueIndexPerCommandPool[std_commandPoolIndexPerCommandBuffer[uFunctionIndex + 1]] : uLogicalPresentQueueIndex);
			}
			PRINT_DEBUG_CLASS("Finishing to record Vulkan command buffer of function at index ", uFunctionIndex);
			if (vkEndCommandBuffer(std_commandBuffers[uFunctionIndex]) == VK_SUCCESS)
				return true;
			else
				RE_ERROR("Failed to finish recording Vulkan command buffer ", std_commandBuffers[uFunctionIndex], " at index ", uFunctionIndex);
		} else
			RE_ERROR("Failed beginning to record Vulkan command buffer ", std_commandBuffers[uFunctionIndex], " at index ", uFunctionIndex);
		return false;
	}

	bool VulkanTask::submit(
			uint32_t u32SemaphoresToWaitForCount,
			const VkSemaphoreSubmitInfo *vk_paSemaphoresToWaitFor,
			const VkPipelineStageFlags2 *vk_paeInternSemaphoreWaits,
			uint32_t u32SemaphoresToSignal,
			const VkSemaphoreSubmitInfo *vk_paSemaphoresToSignal,
			VkFence vk_hFenceToSignal) const {
#ifndef NDEBUG
		if (!vk_paeInternSemaphoreWaits && uFunctionsCount > 1)
			RE_ABORT("Intern semaphore wait pipeline stage is null");
#endif
		PRINT_DEBUG_CLASS("Submitting command buffers");
		std::vector<VkCommandBufferSubmitInfo> std_commandBufferSubmissionInfos;
		std_commandBufferSubmissionInfos.reserve(uFunctionsCount / uCommandPoolCount);
		VkSubmitInfo2 vk_submissionInfo;
		vk_submissionInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		vk_submissionInfo.pNext = nullptr;
		vk_submissionInfo.flags = 0;
		vk_submissionInfo.waitSemaphoreInfoCount = u32SemaphoresToWaitForCount;
		vk_submissionInfo.pWaitSemaphoreInfos = vk_paSemaphoresToWaitFor;
		vk_submissionInfo.signalSemaphoreInfoCount = 1;
		constexpr unsigned uSemaphoreInfoCount = 2;
		VkSemaphoreSubmitInfo vk_a2InternalSemaphoreSubmissionInfo[uSemaphoreInfoCount];
		uint64_t u64TimelineSemaphoreValue;
		if (uCommandPoolCount > 1) {
			PRINT_DEBUG_CLASS("Fetching currently held value of the internal timeline semaphore");
			vkGetSemaphoreCounterValue(vk_hDevice, vk_hInternalSemaphore, &u64TimelineSemaphoreValue);
			vk_a2InternalSemaphoreSubmissionInfo[0].value = u64TimelineSemaphoreValue + 1;
		}
		vk_a2InternalSemaphoreSubmissionInfo[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		vk_a2InternalSemaphoreSubmissionInfo[0].pNext = nullptr;
		vk_a2InternalSemaphoreSubmissionInfo[0].semaphore = vk_hInternalSemaphore;
		vk_a2InternalSemaphoreSubmissionInfo[0].deviceIndex = 0;
		vk_a2InternalSemaphoreSubmissionInfo[1].sType = vk_a2InternalSemaphoreSubmissionInfo[0].sType;
		vk_a2InternalSemaphoreSubmissionInfo[1].pNext = vk_a2InternalSemaphoreSubmissionInfo[0].pNext;
		vk_a2InternalSemaphoreSubmissionInfo[1].semaphore = vk_a2InternalSemaphoreSubmissionInfo[0].semaphore;
		vk_a2InternalSemaphoreSubmissionInfo[1].deviceIndex = vk_a2InternalSemaphoreSubmissionInfo[0].deviceIndex;
		
		VkPipelineStageFlags2 vk_eSemaphoreWaitFlags = 0;
		unsigned uCurrentCommandPoolIndex = std_commandPoolIndexPerCommandBuffer[0],
			uCurrentSemaphoreInfoIndex = 0;
		for (unsigned uFunctionIndex = 0; uFunctionIndex < uFunctionsCount; uFunctionIndex++) {
			if (uCurrentCommandPoolIndex != std_commandPoolIndexPerCommandBuffer[uFunctionIndex]) {
				const unsigned uLogicalQueueIndex = std_queueIndexPerCommandPool[uCurrentCommandPoolIndex];
				PRINT_DEBUG_CLASS("Submitting Vulkan command buffer/-s to logical queue at index ", uLogicalQueueIndex);
				const VkQueue vk_hLogicalQueue = std_queues[uLogicalQueueIndex];
				vk_a2InternalSemaphoreSubmissionInfo[uCurrentSemaphoreInfoIndex].stageMask = vk_eSemaphoreWaitFlags;
				vk_submissionInfo.commandBufferInfoCount = std_commandBufferSubmissionInfos.size();
				vk_submissionInfo.pCommandBufferInfos = std_commandBufferSubmissionInfos.data();
				vk_submissionInfo.pSignalSemaphoreInfos = &vk_a2InternalSemaphoreSubmissionInfo[uCurrentSemaphoreInfoIndex];
				const VkResult vk_eResultSubmit = vkQueueSubmit2(vk_hLogicalQueue, 1, &vk_submissionInfo, VK_NULL_HANDLE);
				if (vk_eResultSubmit != VK_SUCCESS) {
					RE_ERROR("Failed to submit a subtask to the logical queue ", vk_hLogicalQueue, " at index ", uLogicalQueueIndex, " (code ", vk_eResultSubmit, ")");
					return false;
				}
				vk_a2InternalSemaphoreSubmissionInfo[(uCurrentSemaphoreInfoIndex + 1) % uSemaphoreInfoCount].value = vk_a2InternalSemaphoreSubmissionInfo[uCurrentSemaphoreInfoIndex].value + 1;
				vk_submissionInfo.waitSemaphoreInfoCount = 1;
				vk_submissionInfo.pWaitSemaphoreInfos = &vk_a2InternalSemaphoreSubmissionInfo[uCurrentSemaphoreInfoIndex];
				std_commandBufferSubmissionInfos.clear();
				uCurrentCommandPoolIndex = std_commandPoolIndexPerCommandBuffer[uFunctionIndex];
				uCurrentSemaphoreInfoIndex = (uCurrentSemaphoreInfoIndex + 1) % uSemaphoreInfoCount;
				vk_eSemaphoreWaitFlags = 0;
			}
			std_commandBufferSubmissionInfos.emplace_back();
			std_commandBufferSubmissionInfos.back().sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			std_commandBufferSubmissionInfos.back().pNext = nullptr;
			std_commandBufferSubmissionInfos.back().commandBuffer = std_commandBuffers[uFunctionIndex];
			std_commandBufferSubmissionInfos.back().deviceMask = 1;
			if (uFunctionIndex > 0)
				vk_eSemaphoreWaitFlags |= vk_paeInternSemaphoreWaits[uFunctionIndex - 1];
		}

		PRINT_DEBUG_CLASS("Submitting last subtask");
		vk_submissionInfo.commandBufferInfoCount = std_commandBufferSubmissionInfos.size();
		vk_submissionInfo.pCommandBufferInfos = std_commandBufferSubmissionInfos.data();
		vk_submissionInfo.signalSemaphoreInfoCount = u32SemaphoresToSignal;
		vk_submissionInfo.pSignalSemaphoreInfos = vk_paSemaphoresToSignal;
		const unsigned uLogicalQueueIndex = std_queueIndexPerCommandPool[std_commandPoolIndexPerCommandBuffer[uFunctionsCount - 1]];
		const VkQueue vk_hLogicalQueue = std_queues[uLogicalQueueIndex];
		const VkResult vk_eResultLastSubmit = vkQueueSubmit2(vk_hLogicalQueue, 1, &vk_submissionInfo, vk_hFenceToSignal);
		if (vk_eResultLastSubmit != VK_SUCCESS) {
			RE_ERROR("Failed to submit last subtask to the logical queue ", vk_hLogicalQueue, " at index ", uLogicalQueueIndex, " (code ", vk_eResultLastSubmit, ")");
			return false;
		}
		return true;
	}

	void VulkanTask::reset_all(VkCommandPoolResetFlags vk_eResetFlags) const {
		for (unsigned uCommandPoolIndex = 0; uCommandPoolIndex < uCommandPoolCount; uCommandPoolIndex++)
			vkResetCommandPool(vk_hDevice, std_commandPools[uCommandPoolIndex], vk_eResetFlags);
	}

	VkCommandPool VulkanTask::command_pool_of_function(unsigned uFunctionIndex) const {
		return std_commandPools[std_commandPoolIndexPerCommandBuffer[uFunctionIndex]];
	}

	unsigned VulkanTask::function_count() const {
		return uFunctionsCount;
	}

	unsigned VulkanTask::logical_queue_index_for_function(unsigned uFunctionIndex) const {
		return std_queueIndexPerCommandPool[std_commandPoolIndexPerCommandBuffer[uFunctionIndex]];
	}

	unsigned VulkanTask::logical_queue_index_for_presentation() const {
		return uLogicalPresentQueueIndex;
	}

	VulkanQueueCollection VulkanTask::queues_of_functions(const unsigned *pauFunctionIndices, unsigned uFunctionIndexCount, bool bIncludePresentation) const {
		PRINT_DEBUG_CLASS("Collecting queue information, that are used by functions listed in ", pauFunctionIndices, "containing ", uFunctionIndexCount, " indices");
		std::vector<unsigned> std_logicalQueueIndicesNeeded;
		std_logicalQueueIndicesNeeded.reserve(std::min(uFunctionIndexCount, static_cast<uint32_t>(uCommandPoolCount)) + bool_to_int(bIncludePresentation));
		for (unsigned uFunctionIndex = 0; uFunctionIndex < uFunctionIndexCount; uFunctionIndex++) {
			PRINT_DEBUG_CLASS("Identifying queue used in at function index ", pauFunctionIndices[uFunctionIndex]);
			const unsigned uLogicalQueueIndex = std_queueIndexPerCommandPool[std_commandPoolIndexPerCommandBuffer[pauFunctionIndices[uFunctionIndex]]];
			if (std::find(std_logicalQueueIndicesNeeded.begin(), std_logicalQueueIndicesNeeded.end(), uLogicalQueueIndex) == std_logicalQueueIndicesNeeded.end()) {
				PRINT_DEBUG_CLASS("Adding new logical queue index ", uLogicalQueueIndex);
				std_logicalQueueIndicesNeeded.push_back(uLogicalQueueIndex);
			}
		}
		if (bIncludePresentation
				&& uLogicalPresentQueueIndex != RE_VK_LOGICAL_QUEUE_IGNORED
				&& std::find(std_logicalQueueIndicesNeeded.begin(), std_logicalQueueIndicesNeeded.end(), uLogicalPresentQueueIndex) == std_logicalQueueIndicesNeeded.end()) {
			PRINT_DEBUG_CLASS("Adding logical queue index ", uLogicalPresentQueueIndex, " for presentation");
			std_logicalQueueIndicesNeeded.push_back(uLogicalPresentQueueIndex);
		}
		PRINT_DEBUG_CLASS("Returning queue information");
		const unsigned uQueueCount = static_cast<unsigned>(std_logicalQueueIndicesNeeded.size());
		VulkanQueueCollection queueCollection;
		queueCollection.std_queueFamilyIndices = std::make_unique<uint32_t[]>(uQueueCount);
		queueCollection.std_logicalQueueIndices = std::make_unique<unsigned[]>(uQueueCount);
		queueCollection.vk_eSharingMode = uQueueCount == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
		queueCollection.uQueueCount = uQueueCount;
		for (unsigned uQueueNeededIndex = 0; uQueueNeededIndex < uQueueCount; uQueueNeededIndex++) {
			PRINT_DEBUG_CLASS("Copying index of the needed logical queue at ", uQueueNeededIndex);
			const unsigned uLogicalQueueIndex = std_logicalQueueIndicesNeeded[uQueueNeededIndex];
			queueCollection.std_logicalQueueIndices[uQueueNeededIndex] = uLogicalQueueIndex;
			queueCollection.std_queueFamilyIndices[uQueueNeededIndex] = std_queueFamilyIndices[uLogicalQueueIndex];
		}
		return queueCollection;
	}

	bool VulkanTask::valid() const {
		return static_cast<bool>(std_commandBuffers);
	}

}
