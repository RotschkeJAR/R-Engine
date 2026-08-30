#include "RE_GPU_Internal.hpp"

namespace RE {

	std::unique_ptr<uint32_t[]> std_queueFamilyIndices;
	std::unique_ptr<VkQueue[]> std_queues;
	std::unique_ptr<VkQueueFlags[]> std_queueTypes;
	std::vector<bool> std_presentationAvailablePerQueue;
	unsigned uLogicalQueueCount;

	constexpr VkQueueFlags vk_amRecommendedQueueTypes[] = {
		VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT,   // Rendering
		VK_QUEUE_TRANSFER_BIT,                                                  // Transfer buffer data or images
		VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT                            // General computing
	};
	constexpr unsigned uRecommendedQueueTypesCount = sizeof(vk_amRecommendedQueueTypes) / sizeof(vk_amRecommendedQueueTypes[0]);
#define QUEUE_INDEX_RENDERING          0
#define QUEUE_INDEX_TRANSFER           1
#define QUEUE_INDEX_GENERAL_COMPUTING  2

	bool setup_logical_device_queues() {
		VkDeviceQueueInfo2 vk_queueInfoGet;
		vk_queueInfoGet.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
		vk_queueInfoGet.pNext = nullptr;
		vk_queueInfoGet.flags = 0;
		vk_queueInfoGet.queueIndex = 0;
		for (unsigned uLogicalQueueIndex = 0; uLogicalQueueIndex < uLogicalQueueCount; uLogicalQueueIndex++) {
			PRINT_DEBUG("Getting logical queue at index ", uLogicalQueueIndex);
			vk_queueInfoGet.queueFamilyIndex = std_queueFamilyIndices[uLogicalQueueIndex];
			vkGetDeviceQueue2(vk_hDevice, &vk_queueInfoGet, &std_queues[uLogicalQueueIndex]);
		}
		return true;
	}

	void destroy_logical_device_queues() {
		std_queues.reset();
		std_queueTypes.reset();
		std_queueFamilyIndices.reset();
	}

	void create_device_queue_create_infos(const float *pfPriority, std::vector<VkDeviceQueueCreateInfo> &rLogicalQueueCreateInfos) {
		PRINT_DEBUG("Fetching information about available queues on physical Vulkan device");
		uint32_t u32QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties2(SELECTED_PHYSICAL_VULKAN_DEVICE, &u32QueueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties2> std_queueFamilyProperties;
		std_queueFamilyProperties.resize(u32QueueFamilyCount);
		for (VkQueueFamilyProperties2 &vk_rQueueFamilyPropertiesStructure : std_queueFamilyProperties) {
			vk_rQueueFamilyPropertiesStructure.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
			vk_rQueueFamilyPropertiesStructure.pNext = nullptr;
		}
		vkGetPhysicalDeviceQueueFamilyProperties2(SELECTED_PHYSICAL_VULKAN_DEVICE, &u32QueueFamilyCount, std_queueFamilyProperties.data());
		bool abRecommendedQueueTypesExisting[uRecommendedQueueTypesCount] = {};
		abRecommendedQueueTypesExisting[QUEUE_INDEX_TRANSFER] = true;
		std::vector<uint32_t> std_selectedQueues;
		std_selectedQueues.reserve(u32QueueFamilyCount);

		PRINT_DEBUG("Sorting queue families into types");
		std::vector<uint32_t> std_graphicsQueues,
			std_presentQueues,
			std_computeQueues,
			std_transferQueues;
		std_graphicsQueues.reserve(u32QueueFamilyCount);
		std_presentQueues.reserve(u32QueueFamilyCount);
		std_computeQueues.reserve(u32QueueFamilyCount);
		std_transferQueues.reserve(u32QueueFamilyCount);
		for (uint32_t u32QueueFamilyIndex = 0; u32QueueFamilyIndex < u32QueueFamilyCount; u32QueueFamilyIndex++) {
			if ((std_queueFamilyProperties[u32QueueFamilyIndex].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				std_graphicsQueues.push_back(u32QueueFamilyIndex);
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, u32QueueFamilyIndex, vk_hSurface, &vk_bPresentingSupported);
			if (vk_bPresentingSupported == VK_TRUE)
				std_presentQueues.push_back(u32QueueFamilyIndex);
			if ((std_queueFamilyProperties[u32QueueFamilyIndex].queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
				std_computeQueues.push_back(u32QueueFamilyIndex);
			if ((std_queueFamilyProperties[u32QueueFamilyIndex].queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
				std_transferQueues.push_back(u32QueueFamilyIndex);
		}
		const unsigned uMaximumIntersections = std::max(std::max(std_graphicsQueues.size(), std_presentQueues.size()), std::max(std_computeQueues.size(), std_transferQueues.size()));

		{
			PRINT_DEBUG("Finding best transfer queue");
			uint32_t u32BestQueue = 0;
			unsigned uLeastSideFeaturesInQueue = UINT_MAX;
			for (const uint32_t u32QueueIndex : std_transferQueues) {
				const uint8_t uSideFeaturesCount = std::popcount<VkQueueFlags>(std_queueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~vk_amRecommendedQueueTypes[QUEUE_INDEX_TRANSFER]));
				if (uSideFeaturesCount < uLeastSideFeaturesInQueue) {
					u32BestQueue = u32QueueIndex;
					uLeastSideFeaturesInQueue = uSideFeaturesCount;
				}
			}
			std_selectedQueues.push_back(u32BestQueue);
		}

		PRINT_DEBUG("Finding intersections to get perfect fitting queues for certain purposes");
		std::vector<uint32_t> std_renderQueues;
		std_renderQueues.reserve(uMaximumIntersections);
		{
			std::vector<uint32_t> std_graphicsComputeQueues;
			std_graphicsComputeQueues.reserve(uMaximumIntersections);
			std::set_intersection(std_graphicsQueues.begin(), std_graphicsQueues.end(), std_computeQueues.begin(), std_computeQueues.end(), std::back_inserter(std_graphicsComputeQueues));
			std::set_intersection(std_graphicsComputeQueues.begin(), std_graphicsComputeQueues.end(), std_transferQueues.begin(), std_transferQueues.end(), std::back_inserter(std_renderQueues));
		}
		std::vector<uint32_t> std_generalComputationQueues;
		std_generalComputationQueues.reserve(uMaximumIntersections);
		std::set_intersection(std_computeQueues.begin(), std_computeQueues.end(), std_transferQueues.begin(), std_transferQueues.end(), std::back_inserter(std_generalComputationQueues));

		if (!std_renderQueues.empty()) {
			PRINT_DEBUG("Finding best render queue");
			abRecommendedQueueTypesExisting[QUEUE_INDEX_RENDERING] = true;
			uint32_t u32BestQueue = 0;
			unsigned uLeastSideFeaturesInQueue = UINT_MAX;
			for (const uint32_t u32QueueIndex : std_renderQueues) {
				const unsigned uSideFeaturesCount = std::popcount<VkQueueFlags>(std_queueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~vk_amRecommendedQueueTypes[QUEUE_INDEX_RENDERING]));
				if (uSideFeaturesCount < uLeastSideFeaturesInQueue) {
					uLeastSideFeaturesInQueue = uSideFeaturesCount;
					u32BestQueue = u32QueueIndex;
				}
			}
			if (std::find(std_selectedQueues.begin(), std_selectedQueues.end(), u32BestQueue) == std_selectedQueues.end())
				std_selectedQueues.push_back(u32BestQueue);
		}
		if (!std_generalComputationQueues.empty()) {
			PRINT_DEBUG("Finding best computation & transfer queue");
			abRecommendedQueueTypesExisting[QUEUE_INDEX_GENERAL_COMPUTING] = true;
			uint32_t u32BestQueue;
			unsigned uLeastSideFeaturesInQueue = UINT_MAX;
			for (const uint32_t u32QueueIndex : std_generalComputationQueues) {
				const unsigned uSideFeaturesCount = std::popcount<VkQueueFlags>(std_queueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~vk_amRecommendedQueueTypes[QUEUE_INDEX_GENERAL_COMPUTING]));
				if (uSideFeaturesCount < uLeastSideFeaturesInQueue) {
					uLeastSideFeaturesInQueue = uSideFeaturesCount;
					u32BestQueue = u32QueueIndex;
				}
			}
			if (std::find(std_selectedQueues.begin(), std_selectedQueues.end(), u32BestQueue) == std_selectedQueues.end())
				std_selectedQueues.push_back(u32BestQueue);
		}

		for (unsigned uRecommendedQueueTypesIndex = 0; uRecommendedQueueTypesIndex < uRecommendedQueueTypesCount; uRecommendedQueueTypesIndex++) {
			if (abRecommendedQueueTypesExisting[uRecommendedQueueTypesIndex])
				continue;
			switch (uRecommendedQueueTypesIndex) {
				case QUEUE_INDEX_RENDERING:
					PRINT_DEBUG("Searching for queues suitable for rendering");
					{
						uint32_t u32BestQueue;
						unsigned uLeastSideFeaturesInQueue = UINT_MAX;
						for (const uint32_t u32QueueIndex : std_graphicsQueues) {
							const unsigned uSideFeaturesCount = std::popcount<VkQueueFlags>(std_queueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~VK_QUEUE_GRAPHICS_BIT));
							if (uSideFeaturesCount < uLeastSideFeaturesInQueue) {
								uLeastSideFeaturesInQueue = uSideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (uLeastSideFeaturesInQueue < UINT_MAX && std::find(std_selectedQueues.begin(), std_selectedQueues.end(), u32BestQueue) == std_selectedQueues.end())
							std_selectedQueues.push_back(u32BestQueue);
						for (const uint32_t u32QueueIndex : std_computeQueues) {
							const unsigned uSideFeaturesCount = std::popcount<VkQueueFlags>(std_queueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~VK_QUEUE_COMPUTE_BIT));
							if (uSideFeaturesCount < uLeastSideFeaturesInQueue) {
								uLeastSideFeaturesInQueue = uSideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (uLeastSideFeaturesInQueue < UINT_MAX && std::find(std_selectedQueues.begin(), std_selectedQueues.end(), u32BestQueue) == std_selectedQueues.end())
							std_selectedQueues.push_back(u32BestQueue);
					}
					break;
				case QUEUE_INDEX_GENERAL_COMPUTING:
					PRINT_DEBUG("Searching for queues suitable for general computation");
					{
						uint32_t u32BestQueue;
						unsigned uLeastSideFeaturesInQueue = UINT_MAX;
						for (const uint32_t u32QueueIndex : std_graphicsQueues) {
							const unsigned uSideFeaturesCount = std::popcount<VkQueueFlags>(std_queueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~VK_QUEUE_COMPUTE_BIT));
							if (uSideFeaturesCount < uLeastSideFeaturesInQueue) {
								uLeastSideFeaturesInQueue = uSideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (uLeastSideFeaturesInQueue < UINT_MAX && std::find(std_selectedQueues.begin(), std_selectedQueues.end(), u32BestQueue) == std_selectedQueues.end())
							std_selectedQueues.push_back(u32BestQueue);
						for (const uint32_t u32QueueIndex : std_computeQueues) {
							const unsigned uSideFeaturesCount = std::popcount<VkQueueFlags>(std_queueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags & (~VK_QUEUE_TRANSFER_BIT));
							if (uSideFeaturesCount < uLeastSideFeaturesInQueue) {
								uLeastSideFeaturesInQueue = uSideFeaturesCount;
								u32BestQueue = u32QueueIndex;
							}
						}
						if (uLeastSideFeaturesInQueue < UINT_MAX && std::find(std_selectedQueues.begin(), std_selectedQueues.end(), u32BestQueue) == std_selectedQueues.end())
							std_selectedQueues.push_back(u32BestQueue);
					}
					break;
			}
		}

		PRINT_DEBUG("Checking for any presentation support amongst the as yet selected queues");
		bool bPresentQueueAmongstSelectedQueue = false;
		for (const uint32_t u32SelectedQueueIndex : std_selectedQueues) {
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, u32SelectedQueueIndex, vk_hSurface, &vk_bPresentingSupported);
			if (vk_bPresentingSupported == VK_TRUE) {
				bPresentQueueAmongstSelectedQueue = true;
				break;
			}
		}
		if (!bPresentQueueAmongstSelectedQueue) {
			std::vector<uint32_t> std_goodPresentQueues;
			std_goodPresentQueues.reserve(std_presentQueues.size());
			if (!std_renderQueues.empty())
				std::set_intersection(std_presentQueues.begin(), std_presentQueues.end(), std_renderQueues.begin(), std_renderQueues.end(), std::back_inserter(std_goodPresentQueues));
			else if (!std_graphicsQueues.empty())
				std::set_intersection(std_presentQueues.begin(), std_presentQueues.end(), std_graphicsQueues.begin(), std_graphicsQueues.end(), std::back_inserter(std_goodPresentQueues));
			else
				std_goodPresentQueues.push_back(std_presentQueues[0]);
			if (std_goodPresentQueues.size() > 1) {
				uint32_t u32BestQueue = 0;
				unsigned uLeastSideFeaturesInQueue = UINT_MAX;
				for (const uint32_t u32QueueIndex : std_goodPresentQueues) {
					uint8_t uSideFeaturesCount = std::popcount<VkQueueFlags>(std_queueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags);
					if (uSideFeaturesCount < uLeastSideFeaturesInQueue) {
						u32BestQueue = u32QueueIndex;
						uLeastSideFeaturesInQueue = uSideFeaturesCount;
					}
				}
				std_selectedQueues.push_back(u32BestQueue);
			} else
				std_selectedQueues.push_back(std_goodPresentQueues[0]);
		}
		
		PRINT_DEBUG("Generating queue create info-structs");
		uLogicalQueueCount = std_selectedQueues.size();
		std_queues = std::make_unique<VkQueue[]>(uLogicalQueueCount);
		std_queueTypes = std::make_unique<VkQueueFlags[]>(uLogicalQueueCount);
		std_queueFamilyIndices = std::make_unique<uint32_t[]>(uLogicalQueueCount);
		rLogicalQueueCreateInfos.resize(uLogicalQueueCount);
		std_presentationAvailablePerQueue.reserve(uLogicalQueueCount);
		unsigned uLogicalQueueCreateIndex = 0;
		for (const uint32_t u32QueueIndex : std_selectedQueues) {
			std_queueTypes[uLogicalQueueCreateIndex] = std_queueFamilyProperties[u32QueueIndex].queueFamilyProperties.queueFlags;
			VkBool32 vk_bPresentingSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, u32QueueIndex, vk_hSurface, &vk_bPresentingSupported);
			std_presentationAvailablePerQueue.push_back(vk_bPresentingSupported == VK_TRUE);

			std_queueFamilyIndices[uLogicalQueueCreateIndex] = u32QueueIndex;

			rLogicalQueueCreateInfos[uLogicalQueueCreateIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			rLogicalQueueCreateInfos[uLogicalQueueCreateIndex].pNext = nullptr;
			rLogicalQueueCreateInfos[uLogicalQueueCreateIndex].flags = 0;
			rLogicalQueueCreateInfos[uLogicalQueueCreateIndex].queueFamilyIndex = u32QueueIndex;
			rLogicalQueueCreateInfos[uLogicalQueueCreateIndex].queueCount = 1;
			rLogicalQueueCreateInfos[uLogicalQueueCreateIndex].pQueuePriorities = pfPriority;

			uLogicalQueueCreateIndex++;
		}
	}

	VkQueue get_present_queue(const unsigned uPreferredQueueIndex) {
		if (std_presentationAvailablePerQueue[uPreferredQueueIndex])
			return std_queues[uPreferredQueueIndex];
		for (unsigned uLogicalQueueIndex = 0; uLogicalQueueIndex < uLogicalQueueCount; uLogicalQueueIndex++)
			if (std_presentationAvailablePerQueue[uLogicalQueueIndex])
				return std_queues[uLogicalQueueIndex];
		return VK_NULL_HANDLE;
	}

}
