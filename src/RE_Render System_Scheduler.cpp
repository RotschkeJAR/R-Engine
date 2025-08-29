#include "RE_Render System_Scheduler.hpp"
#include "RE_Render System.hpp"

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
			VkBool32 vk_bSupporting;
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDevice, u32QueueIndex, vk_hSurface, &vk_bSupporting);
			bPresentingSupported = bPresentingSupported || vk_bSupporting == VK_TRUE;
		}

		if ((vk_eAllExistingQueues & VK_QUEUE_GRAPHICS_BIT) == 0)
			rMissingFeatures.emplace("No queue supporting graphics rendering tasks");
		if ((vk_eAllExistingQueues & VK_QUEUE_COMPUTE_BIT) == 0)
			rMissingFeatures.emplace("No queue supporting computing tasks");
		if ((vk_eAllExistingQueues & VK_QUEUE_TRANSFER_BIT) == 0)
			rMissingFeatures.emplace("No queue supporting transfer tasks");
		if (!bPresentingSupported)
			rMissingFeatures.emplace("No queue supporting presenting rendered images");

		delete[] vk_paPhysicalDeviceQueueFamilyProperties;
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
		vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDeviceSelected, &u32QueueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> vk_paQueueFamilyProperties;
		vk_paQueueFamilyProperties.resize(u32QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDeviceSelected, &u32QueueFamilyCount, vk_paQueueFamilyProperties.data());
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
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDeviceSelected, i, vk_hSurface, &vk_bPresentingSupported);
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
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDeviceSelected, u32SelectedQueueIndex, vk_hSurface, &vk_bPresentingSupported);
			if (vk_bPresentingSupported == VK_TRUE) {
				bPresentQueueAmongstSelectedQueue = true;
				break;
			}
		}
		if (!bPresentQueueAmongstSelectedQueue) {
			std::vector<uint32_t> bestPresentQueues;
			bestPresentQueues.reserve(presentQueues.size());
			if (!renderQueues.empty())
				std::set_intersection(presentQueues.begin(), presentQueues.end(), renderQueues.begin(), renderQueues.end(), std::back_inserter(bestPresentQueues));
			else if (!graphicsQueues.empty())
				std::set_intersection(presentQueues.begin(), presentQueues.end(), graphicsQueues.begin(), graphicsQueues.end(), std::back_inserter(bestPresentQueues));
			else
				bestPresentQueues.push_back(presentQueues[0]);
			if (bestPresentQueues.size() > 1) {
				uint32_t u32BestQueue;
				uint8_t u8MinimalAmountOfSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
				for (const uint32_t u32QueueIndex : bestPresentQueues) {
					uint8_t u8SideFeaturesCount = count_true_bits<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFlags);
					if (u8SideFeaturesCount < u8MinimalAmountOfSideFeaturesInQueue) {
						u32BestQueue = u32QueueIndex;
						u8MinimalAmountOfSideFeaturesInQueue = u8SideFeaturesCount;
					}
				}
				presentQueueIndex = u32BestQueue;
			} else
				presentQueueIndex = bestPresentQueues[0];
		}
		
		u8LogicalQueueCount = selectedQueues.size();
		vk_pahQueues = new VkQueue[u8LogicalQueueCount];
		vk_paeQueueTypes = new VkQueueFlags[u8LogicalQueueCount];
		pau32QueueIndices = new uint32_t[u8LogicalQueueCount];
		vk_pahCommandPools = new VkCommandPool[u8LogicalQueueCount * 2];
		vk_rpaLogicalQueueCreateInfos.resize(u8LogicalQueueCount);
		uint32_t u32LogicalQueueCreateIndex = 0;
		for (const uint32_t u32QueueIndex : selectedQueues) {
			vk_paeQueueTypes[u8LogicalQueueCount] = vk_paQueueFamilyProperties[u32QueueIndex];
			pau32QueueIndices[u8LogicalQueueCount] = u32QueueIndex;
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

}
