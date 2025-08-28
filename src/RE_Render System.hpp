#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan_Device.hpp"

namespace RE {

#define RE_VK_COMMAND_POOL_COUNT 6
#define RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX 0
#define RE_VK_COMMAND_POOL_GRAPHICS_TRANSIENT_INDEX 1
#define RE_VK_COMMAND_POOL_COMPUTE_PERSISTENT_INDEX 2
#define RE_VK_COMMAND_POOL_COMPUTE_TRANSIENT_INDEX 3
#define RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX 4
#define RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX 5

#define RE_VK_MAX_SAMPLED_IMAGES 0x7FFF
	
	// Attributes initialized at beginning and rarely changed
	extern VkPhysicalDevice vk_hPhysicalDeviceSelected;
	extern VkPhysicalDeviceLimits vk_physicalDeviceLimits;
	extern VkPhysicalDeviceFeatures vk_physicalDeviceFeatures;
	extern VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties;
	extern VkSurfaceKHR vk_hSurface;
	extern VkSurfaceFormatKHR vk_surfaceFormatSelected;
	extern VkSwapchainKHR vk_hSwapchain;
	extern VkFormat vk_eSwapchainImageFormat;
	extern VkExtent2D vk_swapchainResolution;
	extern uint32_t u32SwapchainImageCount;
	extern VkImage *vk_pahSwapchainImages;
	extern VkImageView *vk_pahSwapchainImageViews;

	bool init_render_system();
	void destroy_render_system();
	bool refresh_swapchain();
	void mark_swapchain_dirty();
	bool does_graphics_queue_support_transfer();

}

#endif /* __RE_RENDER_SYSTEM_H__ */