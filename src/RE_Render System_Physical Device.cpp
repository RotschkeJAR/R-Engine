#include "RE_Render System_Internal.hpp"

namespace RE {
	
	std::unique_ptr<VkPhysicalDevice[]> vk_pahPhysicalDevicesAvailable;
	VkPhysicalDevice vk_hPhysicalDeviceSelected;
	uint32_t u32PhysicalDevicesAvailableCount = 0;
	VkPhysicalDeviceProperties vk_physicalDeviceProperties;
	VkPhysicalDeviceFeatures vk_physicalDeviceFeatures;
	VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties;

	static bool is_vulkan_physical_device_suitable(const VkPhysicalDevice vk_hPhysicalDevice) {
		std::queue<std::string> missingFeatures, optionalFeaturesMissing, discrepantFeatures;

		// Fetch general information about the GPU
		VkPhysicalDeviceProperties2 vk_thisPhysicalDeviceProperties;
		vk_thisPhysicalDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		vk_thisPhysicalDeviceProperties.pNext = nullptr;
		vkGetPhysicalDeviceProperties2(vk_hPhysicalDevice, &vk_thisPhysicalDeviceProperties);

		// Fetch extensions-data about the GPU
		uint32_t u32PhysicalDeviceExtensionCount;
		vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32PhysicalDeviceExtensionCount, nullptr);
		std::vector<VkExtensionProperties> vk_paPhysicalDeviceExtensionProperties;
		vk_paPhysicalDeviceExtensionProperties.resize(u32PhysicalDeviceExtensionCount);
		vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32PhysicalDeviceExtensionCount, vk_paPhysicalDeviceExtensionProperties.data());

		// Fetch surface-data about the GPU
		VkSurfaceCapabilitiesKHR vk_physicalDeviceSurfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDevice, vk_hSurface, &vk_physicalDeviceSurfaceCapabilities);

		// Fetch supported GPU features
		VkPhysicalDeviceFeatures2 vk_physicalDeviceFeaturesAvailable;
		vk_physicalDeviceFeaturesAvailable.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		vk_physicalDeviceFeaturesAvailable.pNext = nullptr;
		vkGetPhysicalDeviceFeatures2(vk_hPhysicalDevice, &vk_physicalDeviceFeaturesAvailable);

		// Check if required Vulkan version is supported
		const uint32_t u32VulkanMajorVersion = VK_API_VERSION_MAJOR(RE_VK_API_VERSION), 
			u32VulkanMinorVersion = VK_API_VERSION_MINOR(RE_VK_API_VERSION), 
			u32PhysicalDeviceVulkanMajorVersion = VK_API_VERSION_MAJOR(vk_thisPhysicalDeviceProperties.properties.apiVersion), 
			u32PhysicalDeviceVulkanMinorVersion = VK_API_VERSION_MINOR(vk_thisPhysicalDeviceProperties.properties.apiVersion);
		if ((u32PhysicalDeviceVulkanMajorVersion == u32VulkanMajorVersion && u32PhysicalDeviceVulkanMinorVersion < u32VulkanMinorVersion) || u32PhysicalDeviceVulkanMajorVersion < u32VulkanMajorVersion)
			missingFeatures.push(append_to_string("The physical device should support at least Vulkan ", u32VulkanMajorVersion, ".", u32VulkanMinorVersion, "; it's latest version is ", u32PhysicalDeviceVulkanMajorVersion, ".", u32PhysicalDeviceVulkanMinorVersion));
		else {
			// Check if there are surface formats defined
			uint32_t u32PhysicalDeviceSurfaceFormatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDevice, vk_hSurface, &u32PhysicalDeviceSurfaceFormatCount, nullptr);
			if (!u32PhysicalDeviceSurfaceFormatCount)
				missingFeatures.emplace("No surface formats available");

			VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDevice, vk_hSurface, &vk_surfaceCapabilities);
			if ((vk_surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) == 0)
				missingFeatures.emplace("The presentation engine doesn't support presenting opaque swapchain images for correct output");

			// Check if there are present modes defined
			uint32_t u32PhysicalDevicePresentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDevice, vk_hSurface, &u32PhysicalDevicePresentModeCount, nullptr);
			if (!u32PhysicalDevicePresentModeCount)
				missingFeatures.emplace("No presentation modes available");

			// Check if swapchain images support the required image usages
			constexpr VkImageUsageFlags vk_eSwapchainImageUsagesRequired = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			if ((vk_physicalDeviceSurfaceCapabilities.supportedUsageFlags & vk_eSwapchainImageUsagesRequired) != vk_eSwapchainImageUsagesRequired)
				missingFeatures.emplace("Swapchain images neither support being used for color attachments nor/or transfer operations");

			// Check if sample shading is supported
			if (vk_physicalDeviceFeaturesAvailable.features.sampleRateShading == VK_FALSE)
				optionalFeaturesMissing.emplace("Sample shading is not supported on this GPU");

			// Check if anisotropy is supported
			if (vk_physicalDeviceFeaturesAvailable.features.samplerAnisotropy == VK_FALSE)
				optionalFeaturesMissing.emplace("Anisotropic filtering is not supported on this GPU");

			// Check if the required extensions exist
			bool bSwapchainExtists = false;
			for (uint32_t u32PhysicalDeviceExtensionIndex = 0; u32PhysicalDeviceExtensionIndex < u32PhysicalDeviceExtensionCount; u32PhysicalDeviceExtensionIndex++)
				if (are_string_contents_equal(vk_paPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
					bSwapchainExtists = true;
					break;
				}
			if (!bSwapchainExtists)
				missingFeatures.emplace("The swapchain extension doesn't exist on this GPU");

			// Check if required queues exist
			PUSH_TO_CALLSTACKTRACE(does_gpu_have_necessary_queues(vk_hPhysicalDevice, missingFeatures));

			// Check if textures are supported
			PUSH_TO_CALLSTACKTRACE(does_gpu_support_textures(vk_hPhysicalDevice, vk_thisPhysicalDeviceProperties.properties.limits, missingFeatures, discrepantFeatures));

			// Check if depth & stencil buffers are supported
			PUSH_TO_CALLSTACKTRACE(does_gpu_support_depth_stencil_images(vk_hPhysicalDevice, missingFeatures));
		}

		if (!discrepantFeatures.empty()) {
			println_colored(append_to_string("Physical Vulkan device ", vk_thisPhysicalDeviceProperties.properties.deviceName, " has discrepancy/-ies:").c_str(), RE_TERMINAL_COLOR_BRIGHT_RED, false, true);
			do {
				println_colored(append_to_string(" - ", discrepantFeatures.front()).c_str(), RE_TERMINAL_COLOR_BRIGHT_RED, false, true);
				discrepantFeatures.pop();
			} while (!discrepantFeatures.empty());
		}
		if (!optionalFeaturesMissing.empty()) {
			println_colored(append_to_string("Physical Vulkan device ", vk_thisPhysicalDeviceProperties.properties.deviceName, " does not fulfill the optional requirement(s):").c_str(), RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, true);
			do {
				println_colored(append_to_string(" - ", optionalFeaturesMissing.front()).c_str(), RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, true);
				optionalFeaturesMissing.pop();
			} while (!optionalFeaturesMissing.empty());
		}
		if (!missingFeatures.empty()) {
			println_colored(append_to_string("Physical Vulkan device ", vk_thisPhysicalDeviceProperties.properties.deviceName, " has been discarded for the following reason(s):").c_str(), RE_TERMINAL_COLOR_RED, false, true);
			do {
				println_colored(append_to_string(" - ", missingFeatures.front()).c_str(), RE_TERMINAL_COLOR_RED, false, true);
				missingFeatures.pop();
			} while (!missingFeatures.empty());
			return false;
		}
		return true;
	}

	bool alloc_physical_vulkan_device_list() {
		uint32_t u32TotalPhysicalDeviceCount;
		vkEnumeratePhysicalDevices(vk_hInstance, &u32TotalPhysicalDeviceCount, nullptr);
		if (!u32TotalPhysicalDeviceCount) {
			RE_FATAL_ERROR("There aren't any physical devices supporting Vulkan or has Vulkan drivers installed");
			return false;
		}
		std::vector<VkPhysicalDevice> vk_pahTotalPhysicalDevice;
		vk_pahTotalPhysicalDevice.resize(u32TotalPhysicalDeviceCount);
		vkEnumeratePhysicalDevices(vk_hInstance, &u32TotalPhysicalDeviceCount, vk_pahTotalPhysicalDevice.data());
		std::queue<VkPhysicalDevice> suitablePhysicalDevices;
		for (const VkPhysicalDevice vk_hPhysicalDevice : vk_pahTotalPhysicalDevice)
			if (is_vulkan_physical_device_suitable(vk_hPhysicalDevice))
				suitablePhysicalDevices.push(vk_hPhysicalDevice);
		u32PhysicalDevicesAvailableCount = suitablePhysicalDevices.size();
		if (!u32PhysicalDevicesAvailableCount) {
			RE_FATAL_ERROR("There aren't any physical Vulkan devices supporting the necessary features");
			return false;
		}
		vk_pahPhysicalDevicesAvailable = std::make_unique<VkPhysicalDevice[]>(u32PhysicalDevicesAvailableCount);
		uint32_t u32CurrentIndex = 0;
		do {
			vk_pahPhysicalDevicesAvailable[u32CurrentIndex] = suitablePhysicalDevices.front();
			u32CurrentIndex++;
			suitablePhysicalDevices.pop();
		} while (!suitablePhysicalDevices.empty());
		return true;
	}

	void free_physical_vulkan_device_list() {
		vk_pahPhysicalDevicesAvailable.reset();
	}

	void select_best_physical_vulkan_device() {
		{ // Algorithm to select best GPU
			int32_t i32BestDeviceScore = std::numeric_limits<int32_t>::min();
			VkPhysicalDeviceProperties2 vk_thisPhysicalDeviceProperties;
			vk_thisPhysicalDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			vk_thisPhysicalDeviceProperties.pNext = nullptr;
			for (uint32_t u32PhysicalDeviceAvailableIndex = 0; u32PhysicalDeviceAvailableIndex < u32PhysicalDevicesAvailableCount; u32PhysicalDeviceAvailableIndex++) {
				vkGetPhysicalDeviceProperties2(vk_pahPhysicalDevicesAvailable[u32PhysicalDeviceAvailableIndex], &vk_thisPhysicalDeviceProperties);
	
				int32_t i32CurrentDeviceScore = 0;
				switch (vk_thisPhysicalDeviceProperties.properties.deviceType) {
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						i32CurrentDeviceScore += 1500;
						break;
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						break;
					default:
						i32CurrentDeviceScore -= 2500;
						break;
				}
				i32CurrentDeviceScore += PUSH_TO_CALLSTACKTRACE_AND_RETURN(rate_gpu_queues(vk_pahPhysicalDevicesAvailable[u32PhysicalDeviceAvailableIndex]), int32_t);
				i32CurrentDeviceScore += PUSH_TO_CALLSTACKTRACE_AND_RETURN(rate_gpu_depth_stencil_image_formats(vk_pahPhysicalDevicesAvailable[u32PhysicalDeviceAvailableIndex]), int32_t);
				i32CurrentDeviceScore += PUSH_TO_CALLSTACKTRACE_AND_RETURN(rate_gpu_texture_capacity(vk_thisPhysicalDeviceProperties.properties.limits), int32_t);
				if (i32CurrentDeviceScore > i32BestDeviceScore) {
					i32BestDeviceScore = i32CurrentDeviceScore;
					vk_hPhysicalDeviceSelected = vk_pahPhysicalDevicesAvailable[u32PhysicalDeviceAvailableIndex];
				}
			}
		}

		VkPhysicalDeviceProperties2 vk_physicalDeviceProperties2;
		vk_physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		vk_physicalDeviceProperties2.pNext = nullptr;
		vkGetPhysicalDeviceProperties2(vk_hPhysicalDeviceSelected, &vk_physicalDeviceProperties2);
		vk_physicalDeviceProperties = vk_physicalDeviceProperties2.properties;
		VkPhysicalDeviceFeatures2 vk_physicalDeviceFeatures2;
		vk_physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		vk_physicalDeviceFeatures2.pNext = nullptr;
		vkGetPhysicalDeviceFeatures2(vk_hPhysicalDeviceSelected, &vk_physicalDeviceFeatures2);
		vk_physicalDeviceFeatures = vk_physicalDeviceFeatures2.features;
		VkPhysicalDeviceMemoryProperties2 vk_physicalDeviceMemoryProperties2;
		vk_physicalDeviceMemoryProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
		vk_physicalDeviceMemoryProperties2.pNext = nullptr;
		vkGetPhysicalDeviceMemoryProperties2(vk_hPhysicalDeviceSelected, &vk_physicalDeviceMemoryProperties2);
		vk_physicalDeviceMemoryProperties = vk_physicalDeviceMemoryProperties2.memoryProperties;
	}

}
