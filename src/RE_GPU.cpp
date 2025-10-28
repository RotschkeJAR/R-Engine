#include "RE_GPU_Internal.hpp"
#include "RE_Renderer.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"

#include <queue>

namespace RE {

	VkSurfaceKHR vk_hSurface;
	VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
	std::unique_ptr<VkSurfaceFormatKHR[]> vk_paSurfaceFormatsAvailable;
	uint32_t u32SurfaceFormatsAvailableCount;
	uint32_t u32IndexToSelectedSurfaceFormat;

	bool bSwapchainDirty = false;
	bool bVsyncEnabled = true;

	static void select_best_vulkan_surface_format() {
		if (u32SurfaceFormatsAvailableCount == 1) {
			u32IndexToSelectedSurfaceFormat = 0;
			PRINT_DEBUG("Only one Vulkan surface format is available. Its color space is ", std::hex, vk_paSurfaceFormatsAvailable[0].colorSpace, " and format is ", vk_paSurfaceFormatsAvailable[0].format, std::dec);
		} else {
			PRINT_DEBUG("Selecting best Vulkan surface format");
			int32_t i32BestSurfaceFormatScore = std::numeric_limits<int32_t>::min();
			for (uint32_t u32SurfaceFormatIndex = 0; u32SurfaceFormatIndex < u32SurfaceFormatsAvailableCount; u32SurfaceFormatIndex++) {
				int32_t i32CurrentSurfaceFormatScore = 0;
				switch (vk_paSurfaceFormatsAvailable[u32SurfaceFormatIndex].format) {
					// SDR
					case VK_FORMAT_R8G8B8A8_UNORM:
					case VK_FORMAT_B8G8R8A8_UNORM:
						i32CurrentSurfaceFormatScore += 990;
						break;
					case VK_FORMAT_R8G8B8A8_SRGB:
					case VK_FORMAT_B8G8R8A8_SRGB:
						i32CurrentSurfaceFormatScore += 1000;
						break;
					case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
					case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
						i32CurrentSurfaceFormatScore += 960;
						break;

					// other or unknown
					default:
						i32CurrentSurfaceFormatScore -= 2000;
						break;
				}
				i32CurrentSurfaceFormatScore += (vk_paSurfaceFormatsAvailable[u32SurfaceFormatIndex].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ? -1 : 1) * 1000;
				if (i32BestSurfaceFormatScore < i32CurrentSurfaceFormatScore) {
					i32BestSurfaceFormatScore = i32CurrentSurfaceFormatScore;
					u32IndexToSelectedSurfaceFormat = u32SurfaceFormatIndex;
					PRINT_DEBUG("Found new best Vulkan surface format at index ", u32IndexToSelectedSurfaceFormat, ", which has color space ", std::hex, vk_paSurfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].colorSpace, " and format ", vk_paSurfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].format, std::dec, ". Its score was ", i32BestSurfaceFormatScore);
				}
			}
		}
	}
	
	static void destroy_vulkan_surface() {
		PRINT_DEBUG("Destroying Vulkan surface");
		vkDestroySurfaceKHR(vk_hInstance, vk_hSurface, nullptr);
		vk_paSurfaceFormatsAvailable.reset();
		vk_hSurface = VK_NULL_HANDLE;
	}

	static void fetch_vulkan_surface_infos() {
		PRINT_DEBUG("Fetching Vulkan surface capabilities");
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities);
		PRINT_DEBUG("Fetching Vulkan surface formats");
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32SurfaceFormatsAvailableCount, nullptr);
		vk_paSurfaceFormatsAvailable.reset();
		vk_paSurfaceFormatsAvailable = std::make_unique<VkSurfaceFormatKHR[]>(u32SurfaceFormatsAvailableCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32SurfaceFormatsAvailableCount, vk_paSurfaceFormatsAvailable.get());
		PRINT_DEBUG("Fetching Vulkan surface presentation modes");
		uint32_t u32PresentModesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32PresentModesCount, nullptr);
		std::vector<VkPresentModeKHR> allSupportedPresentModes;
		allSupportedPresentModes.resize(u32PresentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32PresentModesCount, allSupportedPresentModes.data());

		vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR;
		vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
		for (const VkPresentModeKHR vk_eSupportedPresentMode : allSupportedPresentModes)
			switch (vk_eSupportedPresentMode) {
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

	bool init_render_system() {
		if (create_vulkan_surface(vk_hSurface)) {
			if (alloc_physical_vulkan_device_list()) {
				select_best_physical_vulkan_device();
				fetch_vulkan_surface_infos();
				select_best_vulkan_surface_format();
				find_suitable_depth_stencil_formats();
				if (init_logical_vulkan_device()) {
					if (setup_logical_device_queues()) {
						if (create_swapchain())
							return true;
						destroy_logical_device_queues();
					}
					destroy_logical_vulkan_device();
				}
				free_physical_vulkan_device_list();
			}
			destroy_vulkan_surface();
		}
		return false;
	}

	void destroy_render_system() {
		if (vk_hSwapchain)
			destroy_swapchain();
		destroy_logical_device_queues();
		destroy_logical_vulkan_device();
		free_physical_vulkan_device_list();
		destroy_vulkan_surface();
	}

	bool refresh_swapchain() {
		if (bSwapchainDirty) {
			bSwapchainDirty = false;
			PRINT_DEBUG("Recreating Vulkan swapchain");
			if (!recreate_swapchain())
				return false;
		}
		return true;
	}

	void mark_swapchain_dirty() {
		bSwapchainDirty = true;
	}

	void enable_vsync(const bool bEnableVsync) {
		bVsyncEnabled = bEnableVsync;
		bSwapchainDirty = bRunning;
	}

	[[nodiscard]]
	bool is_vsync_enabled() {
		return bVsyncEnabled;
	}

}
