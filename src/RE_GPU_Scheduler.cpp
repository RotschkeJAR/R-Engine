#include "RE_GPU_Internal.hpp"

namespace RE {

	std::unique_ptr<uint32_t[]> queueFamilyIndices;
	std::unique_ptr<VkQueue[]> vk_pahQueues;
	std::unique_ptr<VkQueueFlags[]> vk_paeQueueTypes;
	std::unique_ptr<VkCommandPool[]> vk_pahCommandPools;
	uint8_t u8LogicalQueueCount = 0;

	constexpr VkQueueFlags vk_aeRecommendedQueueTypes[] = {
		VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, // Rendering
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
		vkGetPhysicalDeviceQueueFamilyProperties2(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties2> vk_paPhysicalDeviceQueueFamilyProperties;
		vk_paPhysicalDeviceQueueFamilyProperties.resize(u32PhysicalDeviceQueueFamilyCount);
		for (VkQueueFamilyProperties2 &vk_rQueueFamilyPropertiesStructure : vk_paPhysicalDeviceQueueFamilyProperties) {
			vk_rQueueFamilyPropertiesStructure.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
			vk_rQueueFamilyPropertiesStructure.pNext = nullptr;
		}
		vkGetPhysicalDeviceQueueFamilyProperties2(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, vk_paPhysicalDeviceQueueFamilyProperties.data());

		VkQueueFlags vk_eAllExistingQueues = 0;
		bool bPresentingSupported = false;
		for (uint32_t u32QueueIndex = 0; u32QueueIndex < u32PhysicalDeviceQueueFamilyCount; u32QueueIndex++) {
			vk_eAllExistingQueues |= vk_paPhysicalDeviceQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags;
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
		vkGetPhysicalDeviceQueueFamilyProperties2(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties2> vk_paPhysicalDeviceQueueFamilyProperties;
		vk_paPhysicalDeviceQueueFamilyProperties.resize(u32PhysicalDeviceQueueFamilyCount);
		for (VkQueueFamilyProperties2 &vk_rQueueFamilyPropertiesStructure : vk_paPhysicalDeviceQueueFamilyProperties) {
			vk_rQueueFamilyPropertiesStructure.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
			vk_rQueueFamilyPropertiesStructure.pNext = nullptr;
		}
		vkGetPhysicalDeviceQueueFamilyProperties2(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, vk_paPhysicalDeviceQueueFamilyProperties.data());
		int32_t i32Score = 0;
		for (const VkQueueFamilyProperties2 vk_physicalDeviceQueueFamilyProperties : vk_paPhysicalDeviceQueueFamilyProperties)
			for (const VkQueueFlags vk_eRecommendedQueueType : vk_aeRecommendedQueueTypes)
				if ((vk_physicalDeviceQueueFamilyProperties.queueFamilyProperties.queueFlags & vk_eRecommendedQueueType) == vk_eRecommendedQueueType)
					i32Score += 100 - std::popcount<VkQueueFlags>(vk_physicalDeviceQueueFamilyProperties.queueFamilyProperties.queueFlags ^ vk_eRecommendedQueueType) * 2;
		return i32Score;
	}

	void create_queue_create_infos(const float *pfPriority, std::vector<VkDeviceQueueCreateInfo> &vk_rpaLogicalQueueCreateInfos) {
		uint32_t u32QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties2(vk_hPhysicalDeviceSelected, &u32QueueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties2> vk_paQueueFamilyProperties;
		vk_paQueueFamilyProperties.resize(u32QueueFamilyCount);
		for (VkQueueFamilyProperties2 &vk_rQueueFamilyPropertiesStructure : vk_paQueueFamilyProperties) {
			vk_rQueueFamilyPropertiesStructure.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
			vk_rQueueFamilyPropertiesStructure.pNext = nullptr;
		}
		vkGetPhysicalDeviceQueueFamilyProperties2(vk_hPhysicalDeviceSelected, &u32QueueFamilyCount, vk_paQueueFamilyProperties.data());
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
			if ((vk_paQueueFamilyProperties[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				graphicsQueues.push_back(i);
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDeviceSelected, i, vk_hSurface, &vk_bPresentingSupported);
			if (vk_bPresentingSupported == VK_TRUE)
				presentQueues.push_back(i);
			if ((vk_paQueueFamilyProperties[i].queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
				computeQueues.push_back(i);
			if ((vk_paQueueFamilyProperties[i].queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
				transferQueues.push_back(i);
		}
		const uint8_t u8MaximumIntersections = std::max(graphicsQueues.size(), std::max(presentQueues.size(), std::max(computeQueues.size(), transferQueues.size())));

		{
			uint32_t u32BestQueue;
			uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
			for (const uint32_t u32QueueIndex : transferQueues) {
				const uint8_t u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags ^ vk_aeRecommendedQueueTypes[QUEUE_INDEX_TRANSFER]);
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
				const uint8_t u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags ^ vk_aeRecommendedQueueTypes[QUEUE_INDEX_RENDERING]);
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
				const uint8_t u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags ^ vk_aeRecommendedQueueTypes[QUEUE_INDEX_IMAGE_TRANSFER]);
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
				const uint8_t u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags ^ vk_aeRecommendedQueueTypes[QUEUE_INDEX_GENERAL_COMPUTING]);
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
							u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags ^ VK_QUEUE_GRAPHICS_BIT);
							if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
								u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8MinimalAmountOfSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
						for (const uint32_t u32QueueIndex : computeQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags ^ VK_QUEUE_COMPUTE_BIT);
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
							u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags ^ VK_QUEUE_GRAPHICS_BIT);
							if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
								u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8MinimalAmountOfSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
						for (const uint32_t u32QueueIndex : computeQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags ^ VK_QUEUE_TRANSFER_BIT);
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
							u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags ^ VK_QUEUE_COMPUTE_BIT);
							if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
								u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8MinimalAmountOfSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
						for (const uint32_t u32QueueIndex : computeQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags ^ VK_QUEUE_TRANSFER_BIT);
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
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDeviceSelected, u32SelectedQueueIndex, vk_hSurface, &vk_bPresentingSupported);
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
					uint8_t u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags);
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
		vk_pahQueues = std::make_unique<VkQueue[]>(u8LogicalQueueCount);
		vk_paeQueueTypes = std::make_unique<VkQueueFlags[]>(u8LogicalQueueCount);
		queueFamilyIndices = std::make_unique<uint32_t[]>(u8LogicalQueueCount);
		vk_pahCommandPools = std::make_unique<VkCommandPool[]>(u8LogicalQueueCount * 2);
		vk_rpaLogicalQueueCreateInfos.resize(u8LogicalQueueCount);
		uint32_t u32LogicalQueueCreateIndex = 0;
		for (const uint32_t u32QueueIndex : selectedQueues) {
			vk_paeQueueTypes[u32LogicalQueueCreateIndex] = vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags;
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDeviceSelected, u32LogicalQueueCreateIndex, vk_hSurface, &vk_bPresentingSupported);
			if (vk_bPresentingSupported == VK_TRUE)
				vk_paeQueueTypes[u32LogicalQueueCreateIndex] |= RE_VK_QUEUE_PRESENT_BIT;

			queueFamilyIndices[u32LogicalQueueCreateIndex] = u32QueueIndex;

			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].pNext = nullptr;
			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].flags = 0;
			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].queueFamilyIndex = u32QueueIndex;
			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].queueCount = 1;
			vk_rpaLogicalQueueCreateInfos[u32LogicalQueueCreateIndex].pQueuePriorities = pfPriority;

			u32LogicalQueueCreateIndex++;
		}
	}

	bool setup_logical_device_queues() {
		VkDeviceQueueInfo2 vk_queueInfoGet;
		vk_queueInfoGet.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
		vk_queueInfoGet.pNext = nullptr;
		vk_queueInfoGet.flags = 0;
		vk_queueInfoGet.queueIndex = 0;
		for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++) {
			vk_queueInfoGet.queueFamilyIndex = queueFamilyIndices[u8LogicalQueueIndex];
			vkGetDeviceQueue2(vk_hDevice, &vk_queueInfoGet, &vk_pahQueues[u8LogicalQueueIndex]);
		}
		VkCommandPoolCreateInfo vk_commandPoolCreateInfo;
		vk_commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		vk_commandPoolCreateInfo.pNext = nullptr;
		for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++) {
			vk_commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices[u8LogicalQueueIndex];
			vk_commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_pahCommandPools[u8LogicalQueueIndex * COMMAND_POOLS_PER_QUEUE + COMMAND_POOL_OFFSET_NORMAL]);
			vk_commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_pahCommandPools[u8LogicalQueueIndex * COMMAND_POOLS_PER_QUEUE + COMMAND_POOL_OFFSET_TRANSIENT]);
		}
		return true;
	}

	void destroy_logical_device_queues() {
		for (uint8_t u8CommandPoolIndex = 0; u8CommandPoolIndex < u8LogicalQueueCount * 2; u8CommandPoolIndex++)
			vkDestroyCommandPool(vk_hDevice, vk_pahCommandPools[u8CommandPoolIndex], nullptr);
		vk_pahQueues.reset();
		vk_paeQueueTypes.reset();
		queueFamilyIndices.reset();
		vk_pahCommandPools.reset();
	}

}
