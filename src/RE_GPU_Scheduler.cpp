#include "RE_GPU_Internal.hpp"

namespace RE {

	std::unique_ptr<uint32_t[]> queueFamilyIndices;
	std::unique_ptr<VkQueue[]> vk_pahQueues;
	std::unique_ptr<VkQueueFlags[]> vk_paeQueueTypes;
	std::vector<bool> presentationAvailablePerQueue;
	uint8_t u8LogicalQueueCount;

	constexpr VkQueueFlags vk_aeRecommendedQueueTypes[] = {
		VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, // Rendering
		VK_QUEUE_TRANSFER_BIT, // Transfer buffer data or images
		VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT // General computing
	};
	constexpr uint8_t u8RecommendedQueueTypesCount = sizeof(vk_aeRecommendedQueueTypes) / sizeof(vk_aeRecommendedQueueTypes[0]);
#define QUEUE_INDEX_RENDERING 0
#define QUEUE_INDEX_TRANSFER 1
#define QUEUE_INDEX_GENERAL_COMPUTING 2

	void create_queue_create_infos(const float *pfPriority, std::vector<VkDeviceQueueCreateInfo> &rLogicalQueueCreateInfos) {
		PRINT_DEBUG("Fetching information about available queues on physical Vulkan device");
		uint32_t u32QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties2(SELECTED_PHYSICAL_VULKAN_DEVICE, &u32QueueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties2> vk_paQueueFamilyProperties;
		vk_paQueueFamilyProperties.resize(u32QueueFamilyCount);
		for (VkQueueFamilyProperties2 &vk_rQueueFamilyPropertiesStructure : vk_paQueueFamilyProperties) {
			vk_rQueueFamilyPropertiesStructure.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
			vk_rQueueFamilyPropertiesStructure.pNext = nullptr;
		}
		vkGetPhysicalDeviceQueueFamilyProperties2(SELECTED_PHYSICAL_VULKAN_DEVICE, &u32QueueFamilyCount, vk_paQueueFamilyProperties.data());
		bool abRecommendedQueueTypesExisting[u8RecommendedQueueTypesCount] = {};
		abRecommendedQueueTypesExisting[QUEUE_INDEX_TRANSFER] = true;
		std::vector<uint32_t> selectedQueues;
		selectedQueues.reserve(u32QueueFamilyCount);

		PRINT_DEBUG("Sorting queue families into types");
		std::vector<uint32_t> graphicsQueues, presentQueues, computeQueues, transferQueues;
		graphicsQueues.reserve(u32QueueFamilyCount);
		presentQueues.reserve(u32QueueFamilyCount);
		computeQueues.reserve(u32QueueFamilyCount);
		transferQueues.reserve(u32QueueFamilyCount);
		for (uint32_t i = 0; i < u32QueueFamilyCount; i++) {
			if ((vk_paQueueFamilyProperties[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				graphicsQueues.push_back(i);
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, i, vk_hSurface, &vk_bPresentingSupported);
			if (vk_bPresentingSupported == VK_TRUE)
				presentQueues.push_back(i);
			if ((vk_paQueueFamilyProperties[i].queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
				computeQueues.push_back(i);
			if ((vk_paQueueFamilyProperties[i].queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
				transferQueues.push_back(i);
		}
		const uint8_t u8MaximumIntersections = std::max(graphicsQueues.size(), std::max(presentQueues.size(), std::max(computeQueues.size(), transferQueues.size())));

		{
			PRINT_DEBUG("Finding best transfer queue");
			uint32_t u32BestQueue;
			uint8_t u8LeastSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
			for (const uint32_t u32QueueIndex : transferQueues) {
				const uint8_t u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~vk_aeRecommendedQueueTypes[QUEUE_INDEX_TRANSFER]));
				if (u8SideFeaturesCount < u8LeastSideFeaturesInQueue) {
					u32BestQueue = u32QueueIndex;
					u8LeastSideFeaturesInQueue = u8SideFeaturesCount;
				}
			}
			selectedQueues.push_back(u32BestQueue);
		}

		PRINT_DEBUG("Finding intersections to get perfect fitting queues for certain purposes");
		std::vector<uint32_t> renderQueues;
		renderQueues.reserve(u8MaximumIntersections);
		{
			std::vector<uint32_t> graphicsComputeQueues;
			graphicsComputeQueues.reserve(u8MaximumIntersections);
			std::set_intersection(graphicsQueues.begin(), graphicsQueues.end(), computeQueues.begin(), computeQueues.end(), std::back_inserter(graphicsComputeQueues));
			std::set_intersection(graphicsComputeQueues.begin(), graphicsComputeQueues.end(), transferQueues.begin(), transferQueues.end(), std::back_inserter(renderQueues));
		}
		std::vector<uint32_t> generalComputationQueues;
		generalComputationQueues.reserve(u8MaximumIntersections);
		std::set_intersection(computeQueues.begin(), computeQueues.end(), transferQueues.begin(), transferQueues.end(), std::back_inserter(generalComputationQueues));

		if (!renderQueues.empty()) {
			PRINT_DEBUG("Finding best render queue");
			abRecommendedQueueTypesExisting[QUEUE_INDEX_RENDERING] = true;
			uint32_t u32BestQueue;
			uint8_t u8LeastSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
			for (const uint32_t u32QueueIndex : renderQueues) {
				const uint8_t u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~vk_aeRecommendedQueueTypes[QUEUE_INDEX_RENDERING]));
				if (u8SideFeaturesCount < u8LeastSideFeaturesInQueue) {
					u8LeastSideFeaturesInQueue = u8SideFeaturesCount;
					u32BestQueue = u32QueueIndex;
				}
			}
			if (std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
				selectedQueues.push_back(u32BestQueue);
		}
		if (!generalComputationQueues.empty()) {
			PRINT_DEBUG("Finding best computation & transfer queue");
			abRecommendedQueueTypesExisting[QUEUE_INDEX_GENERAL_COMPUTING] = true;
			uint32_t u32BestQueue;
			uint8_t u8LeastSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
			for (const uint32_t u32QueueIndex : generalComputationQueues) {
				const uint8_t u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~vk_aeRecommendedQueueTypes[QUEUE_INDEX_GENERAL_COMPUTING]));
				if (u8SideFeaturesCount < u8LeastSideFeaturesInQueue) {
					u8LeastSideFeaturesInQueue = u8SideFeaturesCount;
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
					PRINT_DEBUG("Searching for queues suitable for rendering");
					{
						uint32_t u32BestQueue;
						uint8_t u8LeastSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
						for (const uint32_t u32QueueIndex : graphicsQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~VK_QUEUE_GRAPHICS_BIT));
							if (u8SideFeaturesCount < u8LeastSideFeaturesInQueue) {
								u8LeastSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8LeastSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
						for (const uint32_t u32QueueIndex : computeQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~VK_QUEUE_COMPUTE_BIT));
							if (u8SideFeaturesCount < u8LeastSideFeaturesInQueue) {
								u8LeastSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8LeastSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
					}
					break;
				case QUEUE_INDEX_GENERAL_COMPUTING:
					PRINT_DEBUG("Searching for queues suitable for general computation");
					{
						uint32_t u32BestQueue;
						uint8_t u8LeastSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
						for (const uint32_t u32QueueIndex : graphicsQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~VK_QUEUE_COMPUTE_BIT));
							if (u8SideFeaturesCount < u8LeastSideFeaturesInQueue) {
								u8LeastSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8LeastSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
						for (const uint32_t u32QueueIndex : computeQueues) {
							uint8_t u8SideFeaturesCount;
							u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~VK_QUEUE_TRANSFER_BIT));
							if (u8SideFeaturesCount < u8LeastSideFeaturesInQueue) {
								u8LeastSideFeaturesInQueue = u8SideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (u8LeastSideFeaturesInQueue < std::numeric_limits<uint8_t>::max() && std::find(selectedQueues.begin(), selectedQueues.end(), u32BestQueue) == selectedQueues.end())
							selectedQueues.push_back(u32BestQueue);
					}
					break;
			}
		}

		PRINT_DEBUG("Checking for any presentation support amongst the as yet selected queues");
		bool bPresentQueueAmongstSelectedQueue = false;
		for (const uint32_t u32SelectedQueueIndex : selectedQueues) {
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, u32SelectedQueueIndex, vk_hSurface, &vk_bPresentingSupported);
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
				uint8_t u8LeastSideFeaturesInQueue = std::numeric_limits<uint8_t>::max();
				for (const uint32_t u32QueueIndex : goodPresentQueues) {
					uint8_t u8SideFeaturesCount = std::popcount<VkQueueFlags>(vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags);
					if (u8SideFeaturesCount < u8LeastSideFeaturesInQueue) {
						u32BestQueue = u32QueueIndex;
						u8LeastSideFeaturesInQueue = u8SideFeaturesCount;
					}
				}
				selectedQueues.push_back(u32BestQueue);
			} else
				selectedQueues.push_back(goodPresentQueues[0]);
		}
		
		PRINT_DEBUG("Generating queue create info-structs");
		u8LogicalQueueCount = selectedQueues.size();
		vk_pahQueues = std::make_unique<VkQueue[]>(u8LogicalQueueCount);
		vk_paeQueueTypes = std::make_unique<VkQueueFlags[]>(u8LogicalQueueCount);
		queueFamilyIndices = std::make_unique<uint32_t[]>(u8LogicalQueueCount);
		rLogicalQueueCreateInfos.resize(u8LogicalQueueCount);
		presentationAvailablePerQueue.reserve(u8LogicalQueueCount);
		uint8_t u8LogicalQueueCreateIndex = 0;
		for (const uint32_t u32QueueIndex : selectedQueues) {
			vk_paeQueueTypes[u8LogicalQueueCreateIndex] = vk_paQueueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags;
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, u32QueueIndex, vk_hSurface, &vk_bPresentingSupported);
			presentationAvailablePerQueue.push_back(vk_bPresentingSupported == VK_TRUE);

			queueFamilyIndices[u8LogicalQueueCreateIndex] = u32QueueIndex;

			rLogicalQueueCreateInfos[u8LogicalQueueCreateIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			rLogicalQueueCreateInfos[u8LogicalQueueCreateIndex].pNext = nullptr;
			rLogicalQueueCreateInfos[u8LogicalQueueCreateIndex].flags = 0;
			rLogicalQueueCreateInfos[u8LogicalQueueCreateIndex].queueFamilyIndex = u32QueueIndex;
			rLogicalQueueCreateInfos[u8LogicalQueueCreateIndex].queueCount = 1;
			rLogicalQueueCreateInfos[u8LogicalQueueCreateIndex].pQueuePriorities = pfPriority;

			u8LogicalQueueCreateIndex++;
		}
	}

	bool setup_logical_device_queues() {
		VkDeviceQueueInfo2 vk_queueInfoGet;
		vk_queueInfoGet.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
		vk_queueInfoGet.pNext = nullptr;
		vk_queueInfoGet.flags = 0;
		vk_queueInfoGet.queueIndex = 0;
		for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++) {
			PRINT_DEBUG("Getting logical queue at index ", u8LogicalQueueIndex);
			vk_queueInfoGet.queueFamilyIndex = queueFamilyIndices[u8LogicalQueueIndex];
			vkGetDeviceQueue2(vk_hDevice, &vk_queueInfoGet, &vk_pahQueues[u8LogicalQueueIndex]);
		}
		return true;
	}

	void destroy_logical_device_queues() {
		vk_pahQueues.reset();
		vk_paeQueueTypes.reset();
		queueFamilyIndices.reset();
	}

	VkQueue get_present_queue(const uint8_t u8PreferredQueueIndex) {
		if (presentationAvailablePerQueue[u8PreferredQueueIndex])
			return vk_pahQueues[u8PreferredQueueIndex];
		for (uint8_t u8LogicalQueueIndex = 0; u8LogicalQueueIndex < u8LogicalQueueCount; u8LogicalQueueIndex++)
			if (presentationAvailablePerQueue[u8LogicalQueueIndex])
				return vk_pahQueues[u8LogicalQueueIndex];
		return VK_NULL_HANDLE;
	}

}
