#ifndef __RE_RENDER_SYSTEM_INTERNAL_H__
#define __RE_RENDER_SYSTEM_INTERNAL_H__

#include "RE_Render System.hpp"

namespace RE {

#define RE_VK_QUEUE_PRESENT_BIT (VK_QUEUE_OPTICAL_FLOW_BIT_NV << 1)

#define SWAPCHAIN_DIRTY_BIT 0
#define VSYNC_SETTING_BIT 1
	extern uint8_t u8RenderSystemFlags;

	// Surface
	extern VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
	extern VkSurfaceFormatKHR vk_surfaceFormatSelected;

	// Physical Device
	bool alloc_physical_vulkan_device_list();
	void free_physical_vulkan_device_list();
	void select_best_physical_vulkan_device();
	
	// Scheduler
	extern uint32_t *pau32QueueIndices;
	extern uint8_t u8LogicalQueueCount;
	void does_gpu_have_necessary_queues(VkPhysicalDevice vk_hPhysicalDevice, std::queue<std::string> &rMissingFeatures);
	[[nodiscard]]
	int32_t rate_gpu_queues(VkPhysicalDevice vk_hPhysicalDevice);
	bool setup_logical_device_interfaces();
	void destroy_logical_device_interfaces();

	// Swapchain
	extern VkPresentModeKHR vk_ePresentModeVsync, vk_ePresentModeNoVsync;
	bool create_swapchain();
	void destroy_swapchain();
	bool recreate_swapchain();

}

#endif /* __RE_RENDER_SYSTEM_INTERNAL_H__ */
