#include "RE_Render System_Internal.hpp"
#include "RE_Renderer.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"

#include <queue>

namespace RE {

	VkSurfaceKHR vk_hSurface = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR vk_surfaceCapabilities = {};
	VkSurfaceFormatKHR *vk_paSurfaceFormatsAvailable = nullptr;
	VkSurfaceFormatKHR vk_surfaceFormatSelected = {};
	uint32_t u32SurfaceFormatsAvailableCount = 0;

	uint8_t u8RenderSystemFlags = 1 << VSYNC_SETTING_BIT;

	static void select_best_vulkan_surface_format() {
		int32_t i32BestSurfaceFormatScore = std::numeric_limits<int32_t>::min();
		for (uint32_t i = 0; i < u32SurfaceFormatsAvailableCount; i++) {
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
			i32CurrentSurfaceFormatScore += (vk_paSurfaceFormatsAvailable[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ? -1 : 1) * 1000;
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
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], vk_hSurface, &vk_surfaceCapabilities);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], vk_hSurface, &u32SurfaceFormatsAvailableCount, nullptr);
		DELETE_ARRAY_SAFELY(vk_paSurfaceFormatsAvailable);
		vk_paSurfaceFormatsAvailable = new VkSurfaceFormatKHR[u32SurfaceFormatsAvailableCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], vk_hSurface, &u32SurfaceFormatsAvailableCount, vk_paSurfaceFormatsAvailable);
		uint32_t u32PresentModesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], vk_hSurface, &u32PresentModesCount, nullptr);
		std::vector<VkPresentModeKHR> allSupportedPresentModes;
		allSupportedPresentModes.resize(u32PresentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice], vk_hSurface, &u32PresentModesCount, allSupportedPresentModes.data());

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
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_surface(vk_hSurface), bool)) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_physical_vulkan_device_list(), bool)) {
				PUSH_TO_CALLSTACKTRACE(select_best_physical_vulkan_device());
				PUSH_TO_CALLSTACKTRACE(fetch_vulkan_surface_infos());
				PUSH_TO_CALLSTACKTRACE(select_best_vulkan_surface_format());
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_logical_vulkan_device(), bool)) {
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(setup_logical_device_interfaces(), bool)) {
						if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_swapchain(), bool))
							return true;
						PUSH_TO_CALLSTACKTRACE(destroy_logical_device_interfaces());
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
		PUSH_TO_CALLSTACKTRACE(destroy_logical_device_interfaces());
		PUSH_TO_CALLSTACKTRACE(destroy_logical_vulkan_device());
		vk_pahPhysicalDevicesAvailable[u32IndexToSelectedPhysicalDevice] = VK_NULL_HANDLE;
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
