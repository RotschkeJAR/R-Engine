#include "RE_Render System_Internal.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

	VkQueue *vk_pahQueues = nullptr;
	VkQueueFlags *vk_paeQueueTypes = nullptr;
	uint32_t *pau32QueueIndices = nullptr;
	VkCommandPool *vk_pahCommandPools = nullptr;
	uint8_t u8LogicalQueueCount = 0;

	constexpr VkQueueFlags vk_aeRecommendedQueueTypes[] = {
		VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, // Rendering
		VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT, // Image transfer and optional mipmap generation
		VK_QUEUE_TRANSFER_BIT, // Transfer buffer data or images
		VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT // General computing
	};
	constexpr uint8_t u8RecommendedQueueTypesCount = sizeof(vk_aeRecommendedQueueTypes) / sizeof(VkQueueFlags);
#define QUEUE_INDEX_RENDERING 0
#define QUEUE_INDEX_IMAGE_TRANSFER 1
#define QUEUE_INDEX_TRANSFER 2
#define QUEUE_INDEX_GENERAL_COMPUTING 3

	void does_gpu_have_necessary_queues(const VkPhysicalDevice vk_hPhysicalDevice, std::queue<std::string> &rMissingFeatures) {
		uint32_t u32PhysicalDeviceQueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> vk_paPhysicalDeviceQueueFamilyProperties;
		vk_paPhysicalDeviceQueueFamilyProperties.resize(u32PhysicalDeviceQueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, vk_paPhysicalDeviceQueueFamilyProperties.data());

		VkQueueFlags vk_eAllExistingQueues = 0;
		bool bPresentingSupported = false;
		for (uint32_t u32QueueIndex = 0; u32QueueIndex < u32PhysicalDeviceQueueFamilyCount; u32QueueIndex++) {
			vk_eAllExistingQueues |= vk_paPhysicalDeviceQueueFamilyProperties[u32QueueIndex].queueFlags;
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDevice, u32QueueIndex, vk_hSurface, &vk_bPresentingSupported);
			bPresentingSupported = bPresentingSupported || vk_bPresentingSupported == VK_TRUE;
		}

		if ((vk_eAllExistingQueues & VK_QUEUE_GRAPHICS_BIT) == 0)
			rMissingFeatures.emplace("No queue supports graphics rendering tasks");
		if ((vk_eAllExistingQueues & VK_QUEUE_COMPUTE_BIT) == 0)
			rMissingFeatures.emplace("No queue supports computing tasks");
		if ((vk_eAllExistingQueues & VK_QUEUE_TRANSFER_BIT) == 0)
			rMissingFeatures.emplace("No queue supports transfer tasks");
		if (!bPresentingSupported)
			rMissingFeatures.emplace("No queue supports presenting images");
	}

	[[nodiscard]]
	int32_t rate_gpu_queues(const VkPhysicalDevice vk_hPhysicalDevice) {
		uint32_t u32PhysicalDeviceQueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> vk_paPhysicalDeviceQueueFamilyProperties;
		vk_paPhysicalDeviceQueueFamilyProperties.resize(u32PhysicalDeviceQueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, vk_paPhysicalDeviceQueueFamilyProperties.data());
		int32_t i32Score = 0;
		for (const VkQueueFamilyProperties vk_physicalDeviceQueueFamilyProperties : vk_paPhysicalDeviceQueueFamilyProperties)
			for (const VkQueueFlags vk_eRecommendedQueueType : vk_aeRecommendedQueueTypes)
				if ((vk_physicalDeviceQueueFamilyProperties.queueFlags & vk_eRecommendedQueueType) == vk_eRecommendedQueueType)
					i32Score += 100 - count_true_bits<VkQueueFlags>(vk_physicalDeviceQueueFamilyProperties.queueFlags ^ vk_eRecommendedQueueType) * 2;
		return i32Score;
	}

	void create_queue_create_infos(const float &fPriority, std::vector<VkDeviceQueueCreateInfo> &vk_rpaLogicalQueueCreateInfos) {
		uint32_t u32QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], &u32QueueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> vk_paQueueFamilyProperties;
		vk_paQueueFamilyProperties.resize(u32QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], &u32QueueFamilyCount, vk_paQueueFamilyProperties.data());
		bool abRecommendedQueueTypesExisting[u8RecommendedQueueTypesCount] = {};
		abRecommendedQueueTypesExisting[QUEUE_INDEX_TRANSFER] = true;
		std::vector<uint32_t> selectedQueues;
		selectedQueues.reserve(u32QueueFamilyCount);

		std::vector<uint32_t> graphicsQueues, presentQueues, computeQueues, transferQueues;
		graphicsQueues.reserve(u32QueueFamilyCount);
		presentQueues.reserve(u32QueueFamilyCount);
		computeQueues.reserve(u32QueueFamilyCount);
		transferQueues.reserve(u32QueueFamilyCount);
		for (uint32_t i = 0; i < u32QueueFamilyCount; i++) {
			if ((vk_paQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				graphicsQueues.push_back(i);
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], i, vk_hSurface, &vk_bPresentingSupported);
			if (vk_bPresentingSupported == VK_TRUE)
				presentQueues.push_back(i);
			if ((vk_paQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
				computeQueues.push_back(i);
			if ((vk_paQueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
				transferQueues.push_back(i);
		}
		const uint8_t u8MaximumIntersections = std::max(graphicsQueues.size(), std::max(presentQueues.size(), std::max(computeQueues.size(), transferQueues.size())));

		{
			uint32_t u32BestQueue;
			uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
			for (const uint32_t u32QueueIndex : transferQueues) {
				const uint8_t u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags ^ vk_aeRecommendedQueueTypes[QUEUE_INDEX_TRANSFER]);
				if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
					u32BestQueue = u32QueueIndex;
					u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
				}
			}
			selectedQueues.push_back(u32BestQueue);
		}

		std::vector<uint32_t> renderQueues;
		renderQueues.reserve(u8MaximumIntersections);
		std::set_intersection(graphicsQueues.begin(), graphicsQueues.end(), computeQueues.begin(), computeQueues.end(), std::back_inserter(renderQueues));
		std::vector<uint32_t> imageTransferQueues;
		imageTransferQueues.reserve(u8MaximumIntersections);
		std::set_intersection(graphicsQueues.begin(), graphicsQueues.end(), transferQueues.begin(), transferQueues.end(), std::back_inserter(imageTransferQueues));
		std::vector<uint32_t> generalComputationQueues;
		generalComputationQueues.reserve(u8MaximumIntersections);
		std::set_intersection(computeQueues.begin(), computeQueues.end(), transferQueues.begin(), transferQueues.end(), std::back_inserter(generalComputationQueues));

		if (!renderQueues.empty()) {
			abRecommendedQueueTypesExisting[QUEUE_INDEX_RENDERING] = true;
			uint32_t u32BestQueue;
			uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
			for (const uint32_t u32QueueIndex : renderQueues) {
				const uint8_t u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags ^ vk_aeRecommendedQueueTypes[QUEUE_INDEX_RENDERING]);
				if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
					u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
					u32BestQueue = u32QueueIndex;
				}
			}
			if (std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
				selectedQueues.push_back(u32BestQueue);
		}
		if (!imageTransferQueues.empty()) {
			abRecommendedQueueTypesExisting[QUEUE_INDEX_IMAGE_TRANSFER] = true;
			uint32_t u32BestQueue;
			uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
			for (const uint32_t u32QueueIndex : imageTransferQueues) {
				const uint8_t u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags ^ vk_aeRecommendedQueueTypes[QUEUE_INDEX_IMAGE_TRANSFER]);
				if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
					u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
					u32BestQueue = u32QueueIndex;
				}
			}
			if (std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
				selectedQueues.push_back(u32BestQueue);
		}
		if (!generalComputationQueues.empty()) {
			abRecommendedQueueTypesExisting[QUEUE_INDEX_GENERAL_COMPUTING] = true;
			uint32_t u32BestQueue;
			uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
			for (const uint32_t u32QueueIndex : generalComputationQueues) {
				const uint8_t u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags ^ vk_aeRecommendedQueueTypes[QUEUE_INDEX_GENERAL_COMPUTING]);
				if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
					u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
					u32BestQueue = u32QueueIndex;
				}
			}
			if (std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
				selectedQueues.push_back(u32BestQueue);
		}

		for (uint8_t u8RecommendedQueueTypesIndex = 0; u8RecommendedQueueTypesIndex < u8RecommendedQueueTypesCount; u8RecommendedQueueTypesIndex++) {
			if (abRecommendedQueueTypesExisting[u8RecommendedQueueTypesIndex])
				continue;
			switch (u8RecommendedQueueTypesIndex) {
				case QUEUE_INDEX_RENDERING:
					{
						uint32_t u32BestQueue;
						uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
						for (const uint32_t u32QueueIndex : graphicsQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags ^ VK_QUEUE_GRAPHICS_BIT);
							if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
								u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8MinimalAmountOfSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
						for (const uint32_t u32QueueIndex : computeQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags ^ VK_QUEUE_COMPUTE_BIT);
							if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
								u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8MinimalAmountOfSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
					}
					break;
				case QUEUE_INDEX_IMAGE_TRANSFER:
					{
						uint32_t u32BestQueue;
						uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
						for (const uint32_t u32QueueIndex : graphicsQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags ^ VK_QUEUE_GRAPHICS_BIT);
							if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
								u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8MinimalAmountOfSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
						for (const uint32_t u32QueueIndex : computeQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags ^ VK_QUEUE_TRANSFER_BIT);
							if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
								u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8MinimalAmountOfSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
					}
					break;
				case QUEUE_INDEX_GENERAL_COMPUTING:
					{
						uint32_t u32BestQueue;
						uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
						for (const uint32_t u32QueueIndex : graphicsQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags ^ VK_QUEUE_COMPUTE_BIT);
							if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
								u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8MinimalAmountOfSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
						for (const uint32_t u32QueueIndex : computeQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags ^ VK_QUEUE_TRANSFER_BIT);
							if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
								u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8MinimalAmountOfSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
					}
					break;
			}
		}

		bool bPresentQueueAmongstSelectedQueue = false;
		for (const uint32_t u32SelectedQueueIndex : selectedQueues) {
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], u32SelectedQueueIndex, vk_hSurface, &vk_bPresentingSupported);
			if (vk_bPresentingSupported == VK_TRUE) {
				bPresentQueueAmongstSelectedQueue = true;
				break;
			}
		}
		if (!bPresentQueueAmongstSelectedQueue) {
			std::vector<uint32_t> goodPresentQueues;
			goodPresentQueues.reserve(presentQueues.size());
			if (!renderQueues.empty())
				std::set_intersection(presentQueues.begin(), presentQueues.end(), renderQueues.begin(), renderQueues.end(), std::back_inserter(goodPresentQueues));
			else if (!graphicsQueues.empty())
				std::set_intersection(presentQueues.begin(), presentQueues.end(), graphicsQueues.begin(), graphicsQueues.end(), std::back_inserter(goodPresentQueues));
			else
				goodPresentQueues.push_back(presentQueues[0]);
			if (goodPresentQueues.size() > 1) {
				uint32_t u32BestQueue;
				uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
				for (const uint32_t u32QueueIndex : goodPresentQueues) {
					uint8_t u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags);
					if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
						u32BestQueue = u32QueueIndex;
						u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
					}
				}
				selectedQueues.push_back(u32BestQueue);
			} else
				selectedQueues.push_back(goodPresentQueues[0]);
		}
		
		u8LogicalQueueCount = selectedQueues.size();
		vk_pahQueues = new VkQueue[u8LogicalQueueCount];
		vk_paeQueueTypes = new VkQueueFlags[u8LogicalQueueCount];
		pau32QueueIndices = new uint32_t[u8LogicalQueueCount];
		vk_pahCommandPools = new VkCommandPool[u8LogicalQueueCount * 2];
		vk_rpaLogicalQueueCreateInfos.resize(u8LogicalQueueCount);
		uint32_t u32LogicalQueueCreateIndex = 0;
		for (const uint32_t u32QueueIndex : selectedQueues) {
			vk_paeQueueTypes[u32LogicalQueueCreateIndex] = vk_paQueueFamilyProperties[u32QueueIndex].queueFlags;
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], u32LogicalQueueCreateIndex, vk_hSurface, &vk_bPresentingSupported);
			if (vk_bPresentingSupported == VK_TRUE)
				vk_paeQueueTypes[u32LogicalQueueCreateIndex] |= RE_VK_QUEUE_PRESENT_BIT;

			pau32QueueIndices[u32LogicalQueueCreateIndex] = u32QueueIndex;

			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].pNext = nullptr;
			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].flags = 0;
			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].queueFamilyIndex = u32QueueIndex;
			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].queueCount = 1;
			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].pQueuePriorities = &fPriority;

			u32LogicalQueueCreateIndex++;
		}
	}

	bool setup_logical_device_interfaces() {
		for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++)
			vkGetDeviceQueue(vk_hDevice, pau32QueueIndices[u8LogicalQueueIndex], 0, &vk_pahQueues[u8LogicalQueueIndex]);
		VkCommandPoolCreateInfo vk_commandPoolCreateInfo;
		vk_commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		vk_commandPoolCreateInfo.pNext = nullptr;
		for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++) {
			vk_commandPoolCreateInfo.queueFamilyIndex = pau32QueueIndices[u8LogicalQueueIndex];
			vk_commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_pahCommandPools[u8LogicalQueueIndex * 2]);
			vk_commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_pahCommandPools[u8LogicalQueueIndex * 2 + 1]);
		}
		return true;
	}

	void destroy_logical_device_interfaces() {
		for (uint8_t u8CommandPoolIndex = 0; u8CommandPoolIndex < u8LogicalQueueCount * 2; u8CommandPoolIndex++)
			vkDestroyCommandPool(vk_hDevice, vk_pahCommandPools[u8CommandPoolIndex], nullptr);
		delete[] vk_pahQueues;
		delete[] vk_paeQueueTypes;
		delete[] pau32QueueIndices;
		delete[] vk_pahCommandPools;
		vk_pahQueues = nullptr;
		vk_paeQueueTypes = nullptr;
		pau32QueueIndices = nullptr;
		vk_pahCommandPools = nullptr;
		u8LogicalQueueCount = 0;
	}



	VulkanTask::VulkanTask(const uint32_t u32FunctionsCount, const VkQueueFlagBits *const vk_paeQueueTypePerFunctionRequiredInOrder, const bool bRequiresPresenting) : paFunctions(new std::function<void (VkCommandBuffer)>[u32FunctionsCount]), u32FunctionsCount(u32FunctionsCount) {
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
			queueIndicesPerCommandBuffer.get()[0] = u8BestQueue;
			commandPoolPerCommandBuffer = std::make_shared<VkCommandPool[]>(u32CommandBufferCount);
			commandPoolPerCommandBuffer.get()[0] = vk_pahCommandPools[u8BestQueue];
			vk_pahCommandBuffers = new VkCommandBuffer[u32CommandBufferCount];
			PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer.get()[0], VK_COMMAND_BUFFER_LEVEL_PRIMARY, u32CommandBufferCount, &vk_pahCommandBuffers[0]));
			vk_pahInternalSemaphores = nullptr;
			commandBufferIndicesPerFunction = std::make_shared<uint32_t[]>(u8BestQueue);
			commandBufferIndicesPerFunction.get()[0] = 0;
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

	VulkanTask::VulkanTask(const VulkanTask &rCopy) : queueIndicesPerCommandBuffer(rCopy.queueIndicesPerCommandBuffer), commandPoolPerCommandBuffer(rCopy.commandPoolPerCommandBuffer), vk_pahCommandBuffers(new VkCommandBuffer[rCopy.u32CommandBufferCount]), vk_pahInternalSemaphores(rCopy.u32CommandBufferCount > 0 ? new VkSemaphore[rCopy.u32CommandBufferCount - 1] : nullptr), commandBufferIndicesPerFunction(rCopy.commandBufferIndicesPerFunction), paFunctions(new std::function<void (VkCommandBuffer)>[rCopy.u32FunctionsCount]), u32FunctionsCount(rCopy.u32FunctionsCount), u32CommandBufferCount(rCopy.u32CommandBufferCount), u8PresentQueueIndex(rCopy.u8PresentQueueIndex) {
		for (uint32_t u32CommandBufferIndex = 0; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++)
			PUSH_TO_CALLSTACKTRACE(alloc_vulkan_command_buffers(commandPoolPerCommandBuffer.get()[u32CommandBufferIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, u32CommandBufferCount, &vk_pahCommandBuffers[u32CommandBufferIndex]));
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
			vkFreeCommandBuffers(vk_hDevice, commandPoolPerCommandBuffer.get()[u32CommandBufferIndex], u32CommandBufferCount, &vk_pahCommandBuffers[u32CommandBufferIndex]);
			if (u32CommandBufferIndex > 0)
				vkDestroySemaphore(vk_hDevice, vk_pahInternalSemaphores[u32CommandBufferIndex - 1], nullptr);
		}
		delete[] vk_pahCommandBuffers;
		if (vk_pahInternalSemaphores)
			delete[] vk_pahInternalSemaphores;
		delete[] paFunctions;
	}

	void VulkanTask::submit(const uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *const vk_pahSemaphoresToWaitFor, const VkPipelineStageFlags *const vk_pahStagesToWaitAt, const uint32_t u32SemaphoresToSignal, const VkSemaphore *const vk_pahSemaphoresToSignal, const VkFence vk_hFenceToSignal) {
		if (u32CommandBufferCount == 1)
			PUSH_TO_CALLSTACKTRACE(submit_to_vulkan_queue(vk_pahQueues[queueIndicesPerCommandBuffer.get()[0]], u32SemaphoresToWaitForCount, vk_pahSemaphoresToWaitFor, vk_pahStagesToWaitAt, 1, vk_pahCommandBuffers, u32SemaphoresToSignal, vk_pahSemaphoresToSignal, vk_hFenceToSignal));
		else {
			PUSH_TO_CALLSTACKTRACE(submit_to_vulkan_queue(vk_pahQueues[queueIndicesPerCommandBuffer.get()[0]], u32SemaphoresToWaitForCount, vk_pahSemaphoresToWaitFor, vk_pahStagesToWaitAt, 1, &vk_pahCommandBuffers[0], 1, &vk_pahInternalSemaphores[0], VK_NULL_HANDLE));
			constexpr VkPipelineStageFlags vk_ePipelineStageToWaitAt = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			for (uint32_t u32CommandBufferIndex = 1; u32CommandBufferIndex < u32CommandBufferCount - 1; u32CommandBufferIndex++)
				PUSH_TO_CALLSTACKTRACE(submit_to_vulkan_queue(vk_pahQueues[queueIndicesPerCommandBuffer.get()[u32CommandBufferIndex]], 1, &vk_pahInternalSemaphores[u32CommandBufferIndex - 1], &vk_ePipelineStageToWaitAt, 1, &vk_pahCommandBuffers[u32CommandBufferIndex], 1, &vk_pahInternalSemaphores[u32CommandBufferIndex], VK_NULL_HANDLE));
			PUSH_TO_CALLSTACKTRACE(submit_to_vulkan_queue(vk_pahQueues[queueIndicesPerCommandBuffer.get()[u32CommandBufferCount - 1]], 1, &vk_pahInternalSemaphores[u32CommandBufferCount - 2], &vk_ePipelineStageToWaitAt, 1, &vk_pahCommandBuffers[u32CommandBufferCount - 1], u32SemaphoresToSignal, vk_pahSemaphoresToSignal, vk_hFenceToSignal));
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
