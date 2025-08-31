#include "RE_Render System_Internal.hpp"

namespace RE {

#define ALLOWED_DEPTH_STENCIL_BUFFER_FORMAT_COUNT 3
#define ALLOWED_DEPTH_STENCIL_BUFFER_FORMATS {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT}
	
	VkPhysicalDevice *vk_pahPhysicalDevicesAvailable = nullptr;
	uint32_t u32PhysicalDevicesAvailableCount = 0;
	uint32_t u32IndexToSelectedPhysicalDevice = 0;
	VkPhysicalDeviceProperties vk_physicalDeviceProperties = {};
	VkPhysicalDeviceFeatures vk_physicalDeviceFeatures = {};
	VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties = {};

	static bool is_vulkan_physical_device_suitable(const VkPhysicalDevice vk_hPhysicalDevice) {
		std::queue<std::string> missingFeatures, optionalFeaturesMissing, discrepantFeatures;

		// Fetch general information about the GPU
		VkPhysicalDeviceProperties vk_thisPhysicalDeviceProperties;
		vkGetPhysicalDeviceProperties(vk_hPhysicalDevice, &vk_thisPhysicalDeviceProperties);

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
		VkPhysicalDeviceFeatures vk_physicalDeviceFeaturesAvailable;
		vkGetPhysicalDeviceFeatures(vk_hPhysicalDevice, &vk_physicalDeviceFeaturesAvailable);

		// Check if there are surface formats defined
		uint32_t u32PhysicalDeviceSurfaceFormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDevice, vk_hSurface, &u32PhysicalDeviceSurfaceFormatCount, nullptr);
		if (!u32PhysicalDeviceSurfaceFormatCount)
			missingFeatures.emplace("No surface formats available");

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
		if (vk_physicalDeviceFeaturesAvailable.sampleRateShading == VK_FALSE)
			optionalFeaturesMissing.emplace("Sample shading is not supported on this GPU");

		// Check if anisotropy is supported
		if (vk_physicalDeviceFeaturesAvailable.samplerAnisotropy == VK_FALSE)
			optionalFeaturesMissing.emplace("Anisotropic filtering is not supported on this GPU");

		// Check if required minimum for sampled images is supported
		if (vk_thisPhysicalDeviceProperties.limits.maxPerStageDescriptorSamplers < RE_VK_MAX_SAMPLED_IMAGES)
			discrepantFeatures.push(append_to_string("The maximum amount of sample objects bound to a pipeline stage simultaneously should be equal or greater than ", RE_VK_MAX_SAMPLED_IMAGES, ", but is ", vk_thisPhysicalDeviceProperties.limits.maxPerStageDescriptorSamplers));
		if (vk_thisPhysicalDeviceProperties.limits.maxPerStageDescriptorSampledImages < RE_VK_MAX_SAMPLED_IMAGES)
			discrepantFeatures.push(append_to_string("The maximum amount of sampled images bound to a pipeline stage simultaneously should be equal or greater than ", RE_VK_MAX_SAMPLED_IMAGES, ", ", vk_thisPhysicalDeviceProperties.limits.maxPerStageDescriptorSampledImages));

		// Check if the required extensions exist
		bool bSwapchainExtists = false;
		for (uint32_t u32PhysicalDeviceExtensionIndex = 0; u32PhysicalDeviceExtensionIndex < u32PhysicalDeviceExtensionCount; u32PhysicalDeviceExtensionIndex++) {
			if (!bSwapchainExtists && are_string_contents_equal(vk_paPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
				bSwapchainExtists = true;
			if (bSwapchainExtists)
				break;
		}
		if (!bSwapchainExtists)
			missingFeatures.emplace("The swapchain extension doesn't exist on this GPU");

		// Check if required queues exist
		PUSH_TO_CALLSTACKTRACE(does_gpu_have_necessary_queues(vk_hPhysicalDevice, missingFeatures));

		// Check if depth & stencil buffers are supported
		/*constexpr VkFormat vk_aeDepthStencilFormats[ALLOWED_DEPTH_STENCIL_BUFFER_FORMAT_COUNT] = ALLOWED_DEPTH_STENCIL_BUFFER_FORMATS;
		const VkFormat vk_eSupportedDepthStencilBufferFormat = PUSH_TO_CALLSTACKTRACE_AND_RETURN(find_supported_image_format_on_physical_vulkan_device(vk_hPhysicalDevice, ALLOWED_DEPTH_STENCIL_BUFFER_FORMAT_COUNT, vk_aeDepthStencilFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT), VkFormat);
		if (vk_eSupportedDepthStencilBufferFormat == VK_FORMAT_UNDEFINED)
			missingFeatures.emplace("Unseperated depth and stencil buffers aren't supported");*/

		if (!discrepantFeatures.empty()) {
			println_colored(append_to_string("Physical Vulkan device ", vk_thisPhysicalDeviceProperties.deviceName, " has discrepancy/-ies:").c_str(), RE_TERMINAL_COLOR_BRIGHT_RED, false, true);
			do {
				println_colored(append_to_string(" - ", discrepantFeatures.front()).c_str(), RE_TERMINAL_COLOR_BRIGHT_RED, false, true);
				discrepantFeatures.pop();
			} while (!discrepantFeatures.empty());
		}
		if (!optionalFeaturesMissing.empty()) {
			println_colored(append_to_string("Physical Vulkan device ", vk_thisPhysicalDeviceProperties.deviceName, " does not fulfill the optional requirement(s):").c_str(), RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, true);
			do {
				println_colored(append_to_string(" - ", optionalFeaturesMissing.front()).c_str(), RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, true);
				optionalFeaturesMissing.pop();
			} while (!optionalFeaturesMissing.empty());
		}
		if (!missingFeatures.empty()) {
			println_colored(append_to_string("Physical Vulkan device ", vk_thisPhysicalDeviceProperties.deviceName, " has been discarded for not fulfilling the requirement(s):").c_str(), RE_TERMINAL_COLOR_RED, false, true);
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
		vk_pahPhysicalDevicesAvailable = new VkPhysicalDevice[u32PhysicalDevicesAvailableCount];
		uint32_t u32CurrentIndex = 0;
		do {
			vk_pahPhysicalDevicesAvailable[u32CurrentIndex] = suitablePhysicalDevices.front();
			u32CurrentIndex++;
			suitablePhysicalDevices.pop();
		} while (!suitablePhysicalDevices.empty());
		return true;
	}

	void free_physical_vulkan_device_list() {
		DELETE_ARRAY_SAFELY(vk_pahPhysicalDevicesAvailable);
		u32PhysicalDevicesAvailableCount = 0;
		u32IndexToSelectedPhysicalDevice = 0;
	}

	void select_best_physical_vulkan_device() {
		int32_t i32BestDeviceScore = std::numeric_limits<int32_t>::min();
		for (uint32_t u32PhysicalDeviceAvailableIndex = 0; u32PhysicalDeviceAvailableIndex < u32PhysicalDevicesAvailableCount; u32PhysicalDeviceAvailableIndex++) {
			VkPhysicalDeviceProperties vk_thisPhysicalDeviceProperties;
			vkGetPhysicalDeviceProperties(vk_pahPhysicalDevicesAvailable[u32PhysicalDeviceAvailableIndex], &vk_thisPhysicalDeviceProperties);
	
			int32_t i32CurrentDeviceScore = 0;
			switch (vk_thisPhysicalDeviceProperties.deviceType) {
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					i32CurrentDeviceScore += 1500;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					break;
				default:
					i32CurrentDeviceScore -= 2500;
					break;
			}
			i32CurrentDeviceScore += static_cast<int32_t>(std::clamp<uint32_t>(vk_thisPhysicalDeviceProperties.limits.maxImageDimension2D, 0U, 8192U)) * 1500 / 8192 - 499;
			i32CurrentDeviceScore += PUSH_TO_CALLSTACKTRACE_AND_RETURN(rate_gpu_queues(vk_pahPhysicalDevicesAvailable[u32PhysicalDeviceAvailableIndex]), int32_t);
			i32CurrentDeviceScore += static_cast<int32_t>(std::clamp<uint32_t>(std::min(vk_thisPhysicalDeviceProperties.limits.maxPerStageDescriptorSamplers, vk_thisPhysicalDeviceProperties.limits.maxPerStageDescriptorSampledImages), 16U, RE_VK_MAX_SAMPLED_IMAGES) - 16U) * 1500 / RE_VK_MAX_SAMPLED_IMAGES - 499;
			if (i32CurrentDeviceScore > i32BestDeviceScore) {
				i32BestDeviceScore = i32CurrentDeviceScore;
				u32IndexToSelectedPhysicalDevice = u32PhysicalDeviceAvailableIndex;
			}
		}
		vkGetPhysicalDeviceProperties(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], &vk_physicalDeviceProperties);
		vkGetPhysicalDeviceFeatures(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], &vk_physicalDeviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], &vk_physicalDeviceMemoryProperties);
	}

}
