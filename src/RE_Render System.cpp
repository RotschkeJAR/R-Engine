#include "RE_Render System.hpp"
#include "RE_Renderer.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

#include <queue>

namespace RE {

#define ALLOWED_DEPTH_STENCIL_BUFFER_FORMAT_COUNT 3
#define ALLOWED_DEPTH_STENCIL_BUFFER_FORMATS {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT}

	// Attributes initialized at beginning and rarely changed
	VkPhysicalDevice *vk_pahPhysicalDevicesAvailable = nullptr;
	uint32_t u32PhysicalDevicesAvailableCount = 0;
	VkPhysicalDevice vk_hPhysicalDeviceSelected = VK_NULL_HANDLE;
	VkPhysicalDeviceLimits vk_physicalDeviceLimits = {};
	VkPhysicalDeviceFeatures vk_physicalDeviceFeatures = {};
	VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties = {};
	std::array<VkQueue, RE_VK_QUEUE_COUNT> vk_ahDeviceQueueFamilies = {};
	std::array<uint32_t, RE_VK_QUEUE_COUNT> au32DeviceQueueFamilyIndices = {};
	VkSurfaceKHR vk_hSurface = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR vk_surfaceCapabilities = {};
	uint32_t u32SurfaceFormatsAvailableCount = 0;
	VkSurfaceFormatKHR *vk_paSurfaceFormatsAvailable = nullptr;
	VkSurfaceFormatKHR vk_surfaceFormatSelected = {};
	VkPresentModeKHR vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR, vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
	VkSwapchainKHR vk_hSwapchain = VK_NULL_HANDLE;
	VkFormat vk_eSwapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D vk_swapchainResolution = {};
	uint32_t u32SwapchainImageCount = 0;
	VkImage *vk_pahSwapchainImages = nullptr;
	VkImageView *vk_pahSwapchainImageViews = nullptr;
	std::array<VkCommandPool, RE_VK_COMMAND_POOL_COUNT> vk_ahCommandPools = {};
	VkCommandBuffer vk_hDummyTransferCommandBuffer = VK_NULL_HANDLE;
	VkFormat vk_eDepthStencilBufferFormat = VK_FORMAT_UNDEFINED;

#define SWAPCHAIN_DIRTY_BIT 0
#define VSYNC_SETTING_BIT 1
#define GRAPHICS_QUEUE_SUPPORTS_TRANSFER_BIT 2
#define COMPUTE_QUEUE_SUPPORTS_TRANSFER_BIT 3
	uint8_t u8RenderSystemFlags = 1 << VSYNC_SETTING_BIT;

	static void select_best_physical_vulkan_device() {
		int32_t i32BestDeviceScore = std::numeric_limits<int32_t>::min();
		for (uint32_t i = 0; i < u32PhysicalDevicesAvailableCount; i++) {
			VkPhysicalDeviceProperties vk_physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(vk_pahPhysicalDevicesAvailable[i], &vk_physicalDeviceProperties);
			
			int32_t i32CurrentDeviceScore = 0;
			switch (vk_physicalDeviceProperties.deviceType) {
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					i32CurrentDeviceScore += 1000;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					break;
				default:
					i32CurrentDeviceScore -= 2500;
					break;
			}
			i32CurrentDeviceScore += vk_physicalDeviceProperties.limits.maxImageDimension2D;
			i32CurrentDeviceScore += static_cast<int32_t>(std::clamp<uint32_t>(std::min(vk_physicalDeviceProperties.limits.maxPerStageDescriptorSamplers, vk_physicalDeviceProperties.limits.maxPerStageDescriptorSampledImages), 16U, RE_VK_MAX_SAMPLED_IMAGES) - 16U) * 1500 / RE_VK_MAX_SAMPLED_IMAGES - 499;

			if (i32BestDeviceScore < i32CurrentDeviceScore) {
				i32BestDeviceScore = i32CurrentDeviceScore;
				vk_hPhysicalDeviceSelected = vk_pahPhysicalDevicesAvailable[i];
			}
		}
	}

	static void select_best_vulkan_surface_format() {
		int32_t i32BestSurfaceFormatScore = std::numeric_limits<int32_t>::min();
		for (uint32_t i = 0U; i < u32SurfaceFormatsAvailableCount; i++) {
			int32_t i32CurrentSurfaceFormatScore = 0;
			switch (vk_paSurfaceFormatsAvailable[i].format) {
				case VK_FORMAT_R8G8B8A8_UNORM:
				case VK_FORMAT_B8G8R8A8_UNORM:
					i32CurrentSurfaceFormatScore += 500;
					break;
				case VK_FORMAT_R8G8B8A8_SRGB:
				case VK_FORMAT_B8G8R8A8_SRGB:
					i32CurrentSurfaceFormatScore += 1000;
					break;
				default:
					i32CurrentSurfaceFormatScore -= 2000;
					break;
			}
			i32CurrentSurfaceFormatScore += (vk_paSurfaceFormatsAvailable[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ? -1 : 1) * 5000;
			if (i32BestSurfaceFormatScore < i32CurrentSurfaceFormatScore) {
				i32BestSurfaceFormatScore = i32CurrentSurfaceFormatScore;
				vk_surfaceFormatSelected = vk_paSurfaceFormatsAvailable[i];
			}
		}
	}
	
	static void destroy_vulkan_surface() {
		vkDestroySurfaceKHR(vk_hInstance, vk_hSurface, nullptr);
		DELETE_ARRAY_SAFELY(vk_paSurfaceFormatsAvailable);
		vk_hSurface = VK_NULL_HANDLE;
	}

	static void fetch_vulkan_surface_infos() {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32SurfaceFormatsAvailableCount, nullptr);
		DELETE_ARRAY_SAFELY(vk_paSurfaceFormatsAvailable);
		vk_paSurfaceFormatsAvailable = new VkSurfaceFormatKHR[u32SurfaceFormatsAvailableCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32SurfaceFormatsAvailableCount, vk_paSurfaceFormatsAvailable);
		uint32_t u32PresentModesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32PresentModesCount, nullptr);
		VkPresentModeKHR *const vk_paeAllPresentModes = new VkPresentModeKHR[u32PresentModesCount];
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32PresentModesCount, vk_paeAllPresentModes);

		vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR;
		vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
		for (uint32_t u32PresentModeIndex = 0U; u32PresentModeIndex < u32PresentModesCount; u32PresentModeIndex++) {
			switch (vk_paeAllPresentModes[u32PresentModeIndex]) {
				case VK_PRESENT_MODE_IMMEDIATE_KHR:
					vk_ePresentModeNoVsync = VK_PRESENT_MODE_IMMEDIATE_KHR;
					break;
				case VK_PRESENT_MODE_MAILBOX_KHR:
					vk_ePresentModeVsync = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				default:
					break;
			}
		}
		delete[] vk_paeAllPresentModes;
	}

	static bool alloc_physical_vulkan_device_list() {
		DEFINE_SIGNAL_GUARD(sigGuardAllocPhysicalDeviceList);
		uint32_t u32TotalPhysicalDeviceCount;
		vkEnumeratePhysicalDevices(vk_hInstance, &u32TotalPhysicalDeviceCount, nullptr);
		if (!u32TotalPhysicalDeviceCount) {
			RE_FATAL_ERROR("There aren't any physical devices supporting Vulkan");
			return false;
		}
		if (is_verbose_behaviour_enabled())
			PRINT_LN("Available GPUs:");
		VkPhysicalDevice *const vk_pahTotalPhysicalDevice = new VkPhysicalDevice[u32TotalPhysicalDeviceCount];
		vkEnumeratePhysicalDevices(vk_hInstance, &u32TotalPhysicalDeviceCount, vk_pahTotalPhysicalDevice);
		std::queue<VkPhysicalDevice> suitablePhysicalDevices;
		for (uint32_t u32PhysicalDeviceIndex = 0; u32PhysicalDeviceIndex < u32TotalPhysicalDeviceCount; u32PhysicalDeviceIndex++) {
			const VkPhysicalDevice vk_hPhysicalDevice = vk_pahTotalPhysicalDevice[u32PhysicalDeviceIndex];

			// Fetch general information about the GPU
			VkPhysicalDeviceProperties vk_physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(vk_hPhysicalDevice, &vk_physicalDeviceProperties);

			// Fetch extensions-data about the GPU
			uint32_t u32PhysicalDeviceExtensionCount;
			vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32PhysicalDeviceExtensionCount, nullptr);
			VkExtensionProperties *const vk_paPhysicalDeviceExtensionProperties = new VkExtensionProperties[u32PhysicalDeviceExtensionCount];
			vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32PhysicalDeviceExtensionCount, vk_paPhysicalDeviceExtensionProperties);

			// Fetch queue-data about the GPU
			uint32_t u32PhysicalDeviceQueueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, nullptr);
			VkQueueFamilyProperties *const vk_paPhysicalDeviceQueueFamilyProperties = new VkQueueFamilyProperties[u32PhysicalDeviceQueueFamilyCount];
			vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, vk_paPhysicalDeviceQueueFamilyProperties);

			// Fetch surface-data about the GPU
			VkSurfaceCapabilitiesKHR vk_physicalDeviceSurfaceCapabilities;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDevice, vk_hSurface, &vk_physicalDeviceSurfaceCapabilities);

			// Fetch supported GPU features
			VkPhysicalDeviceFeatures vk_physicalDeviceFeaturesAvailable;
			vkGetPhysicalDeviceFeatures(vk_hPhysicalDevice, &vk_physicalDeviceFeaturesAvailable);

			std::queue<std::string> missingFeatures, optionalFeaturesMissing, discrepantFeatures;

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
			if ((vk_physicalDeviceSurfaceCapabilities.supportedUsageFlags & (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)) == 0)
				missingFeatures.emplace("Swapchain images neither support being used for color attachments nor/or transfer operations");

			// Check if sample shading is supported
			if (vk_physicalDeviceFeaturesAvailable.sampleRateShading == VK_FALSE)
				optionalFeaturesMissing.emplace("Sample shading is not supported on this GPU");

			// Check if anisotropy is supported
			if (vk_physicalDeviceFeaturesAvailable.samplerAnisotropy == VK_FALSE)
				optionalFeaturesMissing.emplace("Anisotropic filtering is not supported on this GPU");

			// Check if required minimum for sampled images is supported
			if (vk_physicalDeviceProperties.limits.maxPerStageDescriptorSamplers < RE_VK_MAX_SAMPLED_IMAGES)
				discrepantFeatures.push(append_to_string("The maximum amount of sample objects bound to a pipeline stage simultaneously should be equal or greater than ", RE_VK_MAX_SAMPLED_IMAGES, ", but is ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorSamplers));
			if (vk_physicalDeviceProperties.limits.maxPerStageDescriptorSampledImages < RE_VK_MAX_SAMPLED_IMAGES)
				discrepantFeatures.push(append_to_string("The maximum amount of sampled images bound to a pipeline stage simultaneously should be equal or greater than ", RE_VK_MAX_SAMPLED_IMAGES, ", ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorSampledImages));

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
			bool bGraphicsQueueExists = false, bPresentQueueExists = false, bComputeQueueExists = false, bTransferQueueExists = false;
			for (uint32_t u32PhysicalDeviceQueueFamilyIndex = 0; u32PhysicalDeviceQueueFamilyIndex < u32PhysicalDeviceQueueFamilyCount; u32PhysicalDeviceQueueFamilyIndex++) {
				if (!bPresentQueueExists) {
					VkBool32 surfaceSupportExists;
					vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDevice, u32PhysicalDeviceQueueFamilyIndex, vk_hSurface, &surfaceSupportExists);
					if (surfaceSupportExists)
						bPresentQueueExists = true;
				}
				if (!bGraphicsQueueExists && (vk_paPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))) {
					bGraphicsQueueExists = true;
					bTransferQueueExists = true;
				} else if (!bComputeQueueExists && (vk_paPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))) {
					bComputeQueueExists = true;
					bTransferQueueExists = true;
				} else if (!bTransferQueueExists && (vk_paPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT))
					bTransferQueueExists = true;
				if (bGraphicsQueueExists && bPresentQueueExists && bComputeQueueExists && bTransferQueueExists)
					break;
			}
			if (!bGraphicsQueueExists)
				missingFeatures.emplace("A graphics queue supporting transfers either doesn't exist");
			if (!bPresentQueueExists)
				missingFeatures.emplace("The presentation queue doesn't exist");
			if (!bComputeQueueExists)
				missingFeatures.emplace("A computation queue supporting transfers either doesn't exist");
			if (!bTransferQueueExists)
				missingFeatures.emplace("A dedicated transfer queue or a graphics or computation queue supporting transfers aswell doesn't exist");

			// Check if depth & stencil buffers are supported
			constexpr VkFormat vk_aeDepthStencilFormats[ALLOWED_DEPTH_STENCIL_BUFFER_FORMAT_COUNT] = ALLOWED_DEPTH_STENCIL_BUFFER_FORMATS;
			const VkFormat vk_eSupportedDepthStencilBufferFormat = PUSH_TO_CALLSTACKTRACE_AND_RETURN(find_supported_image_format_on_physical_vulkan_device(vk_hPhysicalDevice, ALLOWED_DEPTH_STENCIL_BUFFER_FORMAT_COUNT, vk_aeDepthStencilFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT), VkFormat);
			if (vk_eSupportedDepthStencilBufferFormat == VK_FORMAT_UNDEFINED)
				missingFeatures.emplace("Unseperated depth and stencil buffers aren't supported");

			delete[] vk_paPhysicalDeviceExtensionProperties;
			delete[] vk_paPhysicalDeviceQueueFamilyProperties;
			if (!discrepantFeatures.empty()) {
				println_colored(append_to_string("Physical Vulkan device ", vk_physicalDeviceProperties.deviceName, " has discrepancy/-ies:").c_str(), RE_TERMINAL_COLOR_BRIGHT_RED, false, true);
				do {
					println_colored(append_to_string(" - ", discrepantFeatures.front()).c_str(), RE_TERMINAL_COLOR_BRIGHT_RED, false, true);
					discrepantFeatures.pop();
				} while (!discrepantFeatures.empty());
			}
			if (!optionalFeaturesMissing.empty()) {
				println_colored(append_to_string("Physical Vulkan device ", vk_physicalDeviceProperties.deviceName, " does not fulfill the optional requirement(s):").c_str(), RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, true);
				do {
					println_colored(append_to_string(" - ", optionalFeaturesMissing.front()).c_str(), RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, true);
					optionalFeaturesMissing.pop();
				} while (!optionalFeaturesMissing.empty());
			}
			if (!missingFeatures.empty()) {
				println_colored(append_to_string("Physical Vulkan device ", vk_physicalDeviceProperties.deviceName, " has been discarded for not fulfilling the requirement(s):").c_str(), RE_TERMINAL_COLOR_RED, false, true);
				do {
					println_colored(append_to_string(" - ", missingFeatures.front()).c_str(), RE_TERMINAL_COLOR_RED, false, true);
					missingFeatures.pop();
				} while (!missingFeatures.empty());
				continue;
			} else
				suitablePhysicalDevices.push(vk_hPhysicalDevice);
		}
		delete[] vk_pahTotalPhysicalDevice;
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

	static void free_physical_vulkan_device_list() {
		DELETE_ARRAY_SAFELY(vk_pahPhysicalDevicesAvailable);
	}

	static bool setup_interfaces_to_logical_vulkan_device() {
		{ // Sets bit, whether graphics- and compute-queue support transfers
			uint32_t u32PhysicalDeviceSelectedQueueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDeviceSelected, &u32PhysicalDeviceSelectedQueueFamilyCount, nullptr);
			VkQueueFamilyProperties *const vk_paPhysicalDeviceSelectedQueueFamilies = new VkQueueFamilyProperties[u32PhysicalDeviceSelectedQueueFamilyCount];
			vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDeviceSelected, &u32PhysicalDeviceSelectedQueueFamilyCount, vk_paPhysicalDeviceSelectedQueueFamilies);
			PUSH_TO_CALLSTACKTRACE(set_bits<uint8_t>(u8RenderSystemFlags, (vk_paPhysicalDeviceSelectedQueueFamilies[au32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX]].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT)) != 0, GRAPHICS_QUEUE_SUPPORTS_TRANSFER_BIT));
			PUSH_TO_CALLSTACKTRACE(set_bits<uint8_t>(u8RenderSystemFlags, (vk_paPhysicalDeviceSelectedQueueFamilies[au32DeviceQueueFamilyIndices[RE_VK_QUEUE_COMPUTE_INDEX]].queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT)) != 0,COMPUTE_QUEUE_SUPPORTS_TRANSFER_BIT));
			delete[] vk_paPhysicalDeviceSelectedQueueFamilies;
		}

		vkGetDeviceQueue(vk_hDevice, au32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX], 0, &vk_ahDeviceQueueFamilies[RE_VK_QUEUE_GRAPHICS_INDEX]);
		vkGetDeviceQueue(vk_hDevice, au32DeviceQueueFamilyIndices[RE_VK_QUEUE_PRESENT_INDEX], 0, &vk_ahDeviceQueueFamilies[RE_VK_QUEUE_PRESENT_INDEX]);
		vkGetDeviceQueue(vk_hDevice, au32DeviceQueueFamilyIndices[RE_VK_QUEUE_COMPUTE_INDEX], 0, &vk_ahDeviceQueueFamilies[RE_VK_QUEUE_COMPUTE_INDEX]);
		vkGetDeviceQueue(vk_hDevice, au32DeviceQueueFamilyIndices[RE_VK_QUEUE_TRANSFER_INDEX], 0, &vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX]);

		VkCommandPoolCreateInfo vk_commandPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = au32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX]
		};
		if (vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX]) == VK_SUCCESS) {
			vk_commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			if (vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_TRANSIENT_INDEX]) == VK_SUCCESS) {
				vk_commandPoolCreateInfo.queueFamilyIndex = au32DeviceQueueFamilyIndices[RE_VK_QUEUE_COMPUTE_INDEX];
				if (vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_ahCommandPools[RE_VK_COMMAND_POOL_COMPUTE_TRANSIENT_INDEX]) == VK_SUCCESS) {
					vk_commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
					if (vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_ahCommandPools[RE_VK_COMMAND_POOL_COMPUTE_PERSISTENT_INDEX]) == VK_SUCCESS) {
						vk_commandPoolCreateInfo.queueFamilyIndex = au32DeviceQueueFamilyIndices[RE_VK_QUEUE_TRANSFER_INDEX];
						if (vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX]) == VK_SUCCESS) {
							if (vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX]) == VK_SUCCESS) {
								if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_vulkan_command_buffers(vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &vk_hDummyTransferCommandBuffer), bool)) {
									if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(begin_recording_vulkan_command_buffer(vk_hDummyTransferCommandBuffer, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr), bool)) {
										if (vkEndCommandBuffer(vk_hDummyTransferCommandBuffer) == VK_SUCCESS)
											return true;
										else
											RE_FATAL_ERROR("Failed to finish recording Vulkan dummy command buffer");
									} else
										RE_FATAL_ERROR("Failed to begin recording Vulkan dummy command buffer");
								} else
									RE_FATAL_ERROR("Failed to allocate Vulkan dummy command buffer");
							} else
								RE_FATAL_ERROR("Failed to create Vulkan command pool for transient transfer command buffers");
							vkDestroyCommandPool(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX], nullptr);
						} else
							RE_FATAL_ERROR("Failed to create Vulkan command pool for persistent transfer command buffers");
						vkDestroyCommandPool(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_COMPUTE_PERSISTENT_INDEX], nullptr);
					} else
						RE_FATAL_ERROR("Failed to create Vulkan command pool for persistent compute command buffers");
					vkDestroyCommandPool(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_COMPUTE_TRANSIENT_INDEX], nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan command pool for transient compute command buffers");
				vkDestroyCommandPool(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_TRANSIENT_INDEX], nullptr);
			} else
				RE_FATAL_ERROR("Failed to create Vulkan command pool for transient graphics command buffers");
			vkDestroyCommandPool(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], nullptr);
		} else
			RE_FATAL_ERROR("Failed to create Vulkan command pool for persistent graphics command buffers");
		return false;
	}

	static void destroy_interfaces_to_logical_vulkan_device() {
		vk_hDummyTransferCommandBuffer = VK_NULL_HANDLE;
		for (uint8_t u8CommandPoolIndex = 0; u8CommandPoolIndex < RE_VK_COMMAND_POOL_COUNT; u8CommandPoolIndex++) {
			vkDestroyCommandPool(vk_hDevice, vk_ahCommandPools[u8CommandPoolIndex], nullptr);
			vk_ahCommandPools[u8CommandPoolIndex] = VK_NULL_HANDLE;
		}
		au32DeviceQueueFamilyIndices.fill(0);
		vk_ahDeviceQueueFamilies.fill(VK_NULL_HANDLE);
	}

	static bool create_swapchain() {
		// Create actual sweapchain
		const VkSwapchainKHR vk_hOldSwapchain = vk_hSwapchain;
		if (vk_hOldSwapchain) {
			PUSH_TO_CALLSTACKTRACE(swapchain_destroyed_renderer());
			for (uint32_t u32SwapchainImageIndex = 0; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++)
				vkDestroyImageView(vk_hDevice, vk_pahSwapchainImageViews[u32SwapchainImageIndex], nullptr);
			DELETE_ARRAY_SAFELY(vk_pahSwapchainImages);
			DELETE_ARRAY_SAFELY(vk_pahSwapchainImageViews);
		}
		if (vk_surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() || vk_surfaceCapabilities.currentExtent.height != std::numeric_limits<uint32_t>::max() || !vk_surfaceCapabilities.currentExtent.width || !vk_surfaceCapabilities.currentExtent.height)
			vk_swapchainResolution = vk_surfaceCapabilities.currentExtent;
		else {
			vk_swapchainResolution.width = std::clamp<uint32_t>(windowSize[0], vk_surfaceCapabilities.minImageExtent.width, vk_surfaceCapabilities.maxImageExtent.width);
			vk_swapchainResolution.height = std::clamp<uint32_t>(windowSize[1], vk_surfaceCapabilities.minImageExtent.height, vk_surfaceCapabilities.maxImageExtent.height);
		}
		VkSwapchainCreateInfoKHR vk_swapchainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = vk_hSurface,
			.minImageCount = std::clamp(vk_surfaceCapabilities.minImageCount + 1, vk_surfaceCapabilities.minImageCount, vk_surfaceCapabilities.maxImageCount > 0 ? vk_surfaceCapabilities.maxImageCount : std::numeric_limits<uint32_t>::max()),
			.imageFormat = vk_surfaceFormatSelected.format,
			.imageColorSpace = vk_surfaceFormatSelected.colorSpace,
			.imageExtent = vk_swapchainResolution,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.preTransform = vk_surfaceCapabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.clipped = VK_TRUE,
			.oldSwapchain = vk_hOldSwapchain
		};
		const uint32_t u32SwapchainRelevantQueueIndices[2] = {au32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX], au32DeviceQueueFamilyIndices[RE_VK_QUEUE_PRESENT_INDEX]};
		if (au32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX] != au32DeviceQueueFamilyIndices[RE_VK_QUEUE_PRESENT_INDEX]) {
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_swapchainCreateInfo.queueFamilyIndexCount = 2;
			vk_swapchainCreateInfo.pQueueFamilyIndices = u32SwapchainRelevantQueueIndices;
		} else
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (are_bits_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_BIT))
			vk_swapchainCreateInfo.presentMode = vk_ePresentModeVsync;
		else
			vk_swapchainCreateInfo.presentMode = vk_ePresentModeNoVsync;
		if (vkCreateSwapchainKHR(vk_hDevice, &vk_swapchainCreateInfo, nullptr, &vk_hSwapchain) != VK_SUCCESS) {
			RE_ERROR("Failed creating Vulkan swapchain");
			return false;
		}
		vk_eSwapchainImageFormat = vk_surfaceFormatSelected.format;
		if (vk_hOldSwapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(vk_hDevice, vk_hOldSwapchain, nullptr);
		vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, nullptr);
		vk_pahSwapchainImages = new VkImage[u32SwapchainImageCount];
		vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, vk_pahSwapchainImages);
		// End of creating actual swapchain

		// Create swapchain image views
		vk_pahSwapchainImageViews = new VkImageView[u32SwapchainImageCount];
		{
			uint32_t u32SwapchainImageCreateIndex = 0;
			while (u32SwapchainImageCreateIndex < u32SwapchainImageCount) {
				if (create_vulkan_image_view(vk_pahSwapchainImages[u32SwapchainImageCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, &vk_pahSwapchainImageViews[u32SwapchainImageCreateIndex])) {
					u32SwapchainImageCreateIndex++;
					continue;
				} else
					RE_FATAL_ERROR("Failed to create Vulkan image view at swapchain image index ", u32SwapchainImageCreateIndex);
				break;
			}
			if (u32SwapchainImageCreateIndex != u32SwapchainImageCount) {
				for (uint32_t u32SwapchainImageDeleteIndex = 0; u32SwapchainImageDeleteIndex < u32SwapchainImageCreateIndex; u32SwapchainImageDeleteIndex++)
					vkDestroyImageView(vk_hDevice, vk_pahSwapchainImageViews[u32SwapchainImageDeleteIndex], nullptr);
				DELETE_ARRAY_SAFELY(vk_pahSwapchainImages);
				DELETE_ARRAY_SAFELY(vk_pahSwapchainImageViews);
				vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr);
				vk_hSwapchain = VK_NULL_HANDLE;
				return false;
			}
		} // End of creating swapchain image views
		return true;
	}
	
	static void destroy_swapchain() {
		PUSH_TO_CALLSTACKTRACE(swapchain_destroyed_renderer());
		for (uint32_t u32SwapchainImageIndex = 0; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++)
			vkDestroyImageView(vk_hDevice, vk_pahSwapchainImageViews[u32SwapchainImageIndex], nullptr);
		DELETE_ARRAY_SAFELY(vk_pahSwapchainImages);
		DELETE_ARRAY_SAFELY(vk_pahSwapchainImageViews);
		vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr);
		vk_hSwapchain = VK_NULL_HANDLE;
	}

	static bool recreate_swapchain() {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities);
		WAIT_FOR_IDLE_VULKAN_DEVICE();
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_swapchain(), bool)) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(swapchain_created_renderer(), bool))
				return true;
			else
				RE_ERROR("Failed notifying the renderer for recreated Vulkan swapchain");
			PUSH_TO_CALLSTACKTRACE(destroy_swapchain());
		} else
			RE_ERROR("Failed recreating the Vulkan swapchain");
		return false;
	}

	bool init_render_system() {
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_surface(vk_hSurface), bool)) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_physical_vulkan_device_list(), bool)) {
				PUSH_TO_CALLSTACKTRACE(select_best_physical_vulkan_device());

				VkPhysicalDeviceProperties vk_physicalDeviceProperties;
				vkGetPhysicalDeviceProperties(vk_hPhysicalDeviceSelected, &vk_physicalDeviceProperties);
				PRINT_LN("Device selected: ", vk_physicalDeviceProperties.deviceName);
				vk_physicalDeviceLimits = vk_physicalDeviceProperties.limits;
				vkGetPhysicalDeviceFeatures(vk_hPhysicalDeviceSelected, &vk_physicalDeviceFeatures);
				vkGetPhysicalDeviceMemoryProperties(vk_hPhysicalDeviceSelected, &vk_physicalDeviceMemoryProperties);
				constexpr VkFormat vk_aeDepthStencilFormats[ALLOWED_DEPTH_STENCIL_BUFFER_FORMAT_COUNT] = ALLOWED_DEPTH_STENCIL_BUFFER_FORMATS;
				vk_eDepthStencilBufferFormat = PUSH_TO_CALLSTACKTRACE_AND_RETURN(find_supported_image_format(ALLOWED_DEPTH_STENCIL_BUFFER_FORMAT_COUNT, vk_aeDepthStencilFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT), VkFormat);

				PUSH_TO_CALLSTACKTRACE(fetch_vulkan_surface_infos());
				PUSH_TO_CALLSTACKTRACE(select_best_vulkan_surface_format());
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_logical_vulkan_device(), bool)) {
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(setup_interfaces_to_logical_vulkan_device(), bool)) {
						if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_swapchain(), bool))
							return true;
						PUSH_TO_CALLSTACKTRACE(destroy_interfaces_to_logical_vulkan_device());
					}
					PUSH_TO_CALLSTACKTRACE(destroy_logical_vulkan_device());
				}
				PUSH_TO_CALLSTACKTRACE(free_physical_vulkan_device_list());
			}
			PUSH_TO_CALLSTACKTRACE(destroy_vulkan_surface());
		}
		return false;
	}

	void destroy_render_system() {
		if (vk_hSwapchain)
			PUSH_TO_CALLSTACKTRACE(destroy_swapchain());
		PUSH_TO_CALLSTACKTRACE(destroy_interfaces_to_logical_vulkan_device());
		PUSH_TO_CALLSTACKTRACE(destroy_logical_vulkan_device());
		vk_hPhysicalDeviceSelected = VK_NULL_HANDLE;
		PUSH_TO_CALLSTACKTRACE(free_physical_vulkan_device_list());
		PUSH_TO_CALLSTACKTRACE(destroy_vulkan_surface());
	}

	bool refresh_swapchain() {
		if (are_bits_true<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_BIT)) {
			set_bits<uint8_t>(u8RenderSystemFlags, false, SWAPCHAIN_DIRTY_BIT);
			if (!PUSH_TO_CALLSTACKTRACE_AND_RETURN(recreate_swapchain(), bool))
				return false;
		}
		return true;
	}

	void mark_swapchain_dirty() {
		set_bits<uint8_t>(u8RenderSystemFlags, bRunning, SWAPCHAIN_DIRTY_BIT);
	}

	bool does_graphics_queue_support_transfer() {
		return are_bits_true<uint8_t>(u8RenderSystemFlags, GRAPHICS_QUEUE_SUPPORTS_TRANSFER_BIT);
	}

	void enable_vsync(const bool bEnableVsync) {
		if (are_bits_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_BIT) != bEnableVsync) {
			set_bits<uint8_t>(u8RenderSystemFlags, bEnableVsync, VSYNC_SETTING_BIT);
			set_bits<uint8_t>(u8RenderSystemFlags, bRunning, SWAPCHAIN_DIRTY_BIT);
		}
	}

	[[nodiscard]]
	bool is_vsync_enabled() {
		return are_bits_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_BIT);
	}

}
