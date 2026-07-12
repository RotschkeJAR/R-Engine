#include "RE_GPU_Internal.hpp"

namespace RE {
	
	std::unique_ptr<PhysicalVulkanDeviceInfo[]> physicalDevicesAvailable;
	uint32_t u32PhysicalDevicesAvailableCount, u32IndexToSelectedPhysicalDevice;

	bool alloc_physical_vulkan_device_list() {
		PRINT_DEBUG("Getting all available physical Vulkan devices");
		uint32_t u32TotalPhysicalDeviceCount;
		vkEnumeratePhysicalDevices(vk_hInstance, &u32TotalPhysicalDeviceCount, nullptr);
		if (!u32TotalPhysicalDeviceCount) {
			RE_FATAL_ERROR("There aren't any physical devices supporting Vulkan or has Vulkan drivers installed");
			return false;
		}
		std::unique_ptr<VkPhysicalDevice[]> allPhysicalDevices = std::make_unique<VkPhysicalDevice[]>(u32TotalPhysicalDeviceCount);
		vkEnumeratePhysicalDevices(vk_hInstance, &u32TotalPhysicalDeviceCount, allPhysicalDevices.get());
		std::vector<PhysicalVulkanDeviceInfo> suitablePhysicalDevices;
		suitablePhysicalDevices.reserve(u32TotalPhysicalDeviceCount);
		for (uint32_t u32PhysicalDeviceIndex = 0; u32PhysicalDeviceIndex < u32TotalPhysicalDeviceCount; u32PhysicalDeviceIndex++) {
			VkPhysicalDeviceProperties2 vk_physicalDeviceProperties;
			vk_physicalDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			vk_physicalDeviceProperties.pNext = nullptr;
			vkGetPhysicalDeviceProperties2(allPhysicalDevices[u32PhysicalDeviceIndex], &vk_physicalDeviceProperties);
			PhysicalVulkanDeviceInfo physicalDeviceInfo;
			physicalDeviceInfo.vk_hPhysicalDevice = allPhysicalDevices[u32PhysicalDeviceIndex];
			physicalDeviceInfo.pacName = vk_physicalDeviceProperties.properties.deviceName;
			physicalDeviceInfo.vk_eType = vk_physicalDeviceProperties.properties.deviceType;
			if (is_physical_vulkan_device_suitable(physicalDeviceInfo.vk_hPhysicalDevice, &physicalDeviceInfo.i32Scoring))
				suitablePhysicalDevices.push_back(physicalDeviceInfo);
		}
		if (suitablePhysicalDevices.empty()) {
			RE_FATAL_ERROR("There aren't any physical Vulkan devices supporting the necessary features");
			return false;
		}
		PRINT_DEBUG("Sorting suitable physical Vulkan devices by score in an ascending order");
		std::sort(suitablePhysicalDevices.begin(),
				suitablePhysicalDevices.end(),
				[&](const PhysicalVulkanDeviceInfo &rDevice1, const PhysicalVulkanDeviceInfo &rDevice2) -> bool {
					return rDevice1.i32Scoring < rDevice2.i32Scoring;
				}
		);
		PRINT_DEBUG("Saving all suitable, available physical Vulkan devices");
		u32PhysicalDevicesAvailableCount = suitablePhysicalDevices.size();
		physicalDevicesAvailable = std::make_unique<PhysicalVulkanDeviceInfo[]>(u32PhysicalDevicesAvailableCount);
		uint32_t u32CurrentIndex = 0;
		while (!suitablePhysicalDevices.empty()) {
			physicalDevicesAvailable[u32CurrentIndex] = suitablePhysicalDevices.back();
			suitablePhysicalDevices.pop_back();
			u32CurrentIndex++;
		}
		u32IndexToSelectedPhysicalDevice = 0;
		alloc_gpu_memory_info();
		return true;
	}

	void free_physical_vulkan_device_list() {
		physicalDevicesAvailable.reset();
	}

	void select_physical_vulkan_device(const uint32_t u32PhysicalDeviceIndex) {
		if (u32IndexToSelectedPhysicalDevice == u32PhysicalDeviceIndex)
			return;
		PRINT_DEBUG("New physical Vulkan device at index ", u32IndexToSelectedPhysicalDevice, " selected");
		u32IndexToSelectedPhysicalDevice = u32PhysicalDeviceIndex;
		fetch_gpu_constrains();
	}

}
