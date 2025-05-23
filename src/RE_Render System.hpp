#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan.hpp"

namespace RE {

#define RE_VK_QUEUE_COUNT 3U
#define RE_VK_QUEUE_GRAPHICS_INDEX 0U
#define RE_VK_QUEUE_PRESENT_INDEX 1U
#define RE_VK_QUEUE_TRANSFER_INDEX 2U

#define RE_VK_COMMAND_POOL_COUNT 4U
#define RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX 0U
#define RE_VK_COMMAND_POOL_GRAPHICS_TRANSIENT_INDEX 1U
#define RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX 2U
#define RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX 3U

	// Attributes initialized at beginning and rarely changed
	extern VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties;
	extern VkDevice vk_hDevice;
	extern VkQueue vk_hDeviceQueueFamilies[RE_VK_QUEUE_COUNT];
	extern uint32_t u32DeviceQueueFamilyIndices[RE_VK_QUEUE_COUNT];
	extern VkSwapchainKHR vk_hSwapchain;
	extern VkFormat vk_eSwapchainImageFormat;
	extern VkExtent2D vk_swapchainResolution;
	extern uint32_t u32SwapchainImageCount;
	extern VkImage *vk_phSwapchainImages;
	extern VkImageView *vk_phSwapchainImageViews;
	extern VkCommandPool vk_hCommandPools[RE_VK_COMMAND_POOL_COUNT];
	extern VkCommandBuffer vk_hDummyTransferCommandBuffer;

	// Configurable settings
	extern VkPhysicalDevice vk_hPhysicalDeviceSelected;

	bool init_render_system();
	void destroy_render_system();
	bool refresh_render_system();
	void mark_swapchain_dirty();

	void enable_vsync(bool bEnableVsync);
	bool is_vsync_enabled();
	void bind_fps_to_vsync(bool bBindFpsToVsync);
	bool is_fps_bound_to_vsync();

#define WAIT_FOR_IDLE_VULKAN_DEVICE() CATCH_SIGNAL(vkDeviceWaitIdle(vk_hDevice))

}

#endif /* __RE_RENDER_SYSTEM_H__ */
