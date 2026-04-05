#include "RE_GPU_Internal.hpp"

namespace RE {
	
	VkPresentModeKHR vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR, vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
	VkSurfaceKHR vk_hSurface;
	VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
	std::unique_ptr<VkSurfaceFormatKHR[]> surfaceFormatsAvailable;
	uint32_t u32SurfaceFormatsAvailableCount;

	// creating Vulkan surface is done by the window

	void destroy_vulkan_surface() {
		PRINT_DEBUG("Destroying Vulkan surface");
		vkDestroySurfaceKHR(vk_hInstance, vk_hSurface, nullptr);
		surfaceFormatsAvailable.reset();
	}

	void fetch_vulkan_surface_infos() {
		PRINT_DEBUG("Fetching Vulkan surface capabilities");
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, vk_hSurface, &vk_surfaceCapabilities);
		PRINT_DEBUG("Fetching Vulkan surface formats");
		vkGetPhysicalDeviceSurfaceFormatsKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, vk_hSurface, &u32SurfaceFormatsAvailableCount, nullptr);
		surfaceFormatsAvailable = std::make_unique<VkSurfaceFormatKHR[]>(u32SurfaceFormatsAvailableCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, vk_hSurface, &u32SurfaceFormatsAvailableCount, surfaceFormatsAvailable.get());
		PRINT_DEBUG("Fetching Vulkan surface presentation modes");
		uint32_t u32PresentModesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, vk_hSurface, &u32PresentModesCount, nullptr);
		std::unique_ptr<VkPresentModeKHR[]> allSupportedPresentModes = std::make_unique<VkPresentModeKHR[]>(u32PresentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, vk_hSurface, &u32PresentModesCount, allSupportedPresentModes.get());

		vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR;
		vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
		for (uint32_t u32SurfaceFormatIndex = 0; u32SurfaceFormatIndex < u32SurfaceFormatsAvailableCount; u32SurfaceFormatIndex++)
			switch (allSupportedPresentModes[u32SurfaceFormatIndex]) {
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

	void select_best_vulkan_surface_format() {
		PRINT_DEBUG("Selecting best Vulkan surface format");
		int32_t i32BestSurfaceFormatScore = std::numeric_limits<int32_t>::min();
		for (uint32_t u32SurfaceFormatIndex = 0; u32SurfaceFormatIndex < u32SurfaceFormatsAvailableCount; u32SurfaceFormatIndex++) {
			int32_t i32CurrentSurfaceFormatScore = 0;
			switch (surfaceFormatsAvailable[u32SurfaceFormatIndex].format) {
				// SDR
				case VK_FORMAT_R8G8B8A8_UNORM:
					i32CurrentSurfaceFormatScore += 990;
					break;
				case VK_FORMAT_B8G8R8A8_UNORM:
					i32CurrentSurfaceFormatScore += 989;
					break;
				case VK_FORMAT_R8G8B8A8_SRGB:
					i32CurrentSurfaceFormatScore += 1000;
					break;
				case VK_FORMAT_B8G8R8A8_SRGB:
					i32CurrentSurfaceFormatScore += 999;
					break;

				// other or unknown
				default:
					i32CurrentSurfaceFormatScore += -2000;
					break;
			}
			i32CurrentSurfaceFormatScore += surfaceFormatsAvailable[u32SurfaceFormatIndex].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ? 1000 : -1000;
			if (i32BestSurfaceFormatScore < i32CurrentSurfaceFormatScore) {
				i32BestSurfaceFormatScore = i32CurrentSurfaceFormatScore;
				u32IndexToSelectedSurfaceFormat = u32SurfaceFormatIndex;
				PRINT_DEBUG("Found new best Vulkan surface format at index ", u32IndexToSelectedSurfaceFormat, ", which has color space ", std::hex, surfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].colorSpace, " and format ", surfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].format, ".");
			}
		}
	}

}
