#ifndef __RE_RENDER_SYSTEM_INTERNAL_H__
#define __RE_RENDER_SYSTEM_INTERNAL_H__

#include "RE_Render System.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Renderer.hpp"

namespace RE {

#define RE_VK_QUEUE_PRESENT_BIT (VK_QUEUE_OPTICAL_FLOW_BIT_NV << 1)

#define SWAPCHAIN_DIRTY_BIT 0
#define VSYNC_SETTING_BIT 1
	extern uint8_t u8RenderSystemFlags;

	// Surface
	extern VkSurfaceKHR vk_hSurface;
	extern VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
	extern std::unique_ptr<VkSurfaceFormatKHR[]> vk_paSurfaceFormatsAvailable;
	extern uint32_t u32SurfaceFormatsAvailableCount;
	extern uint32_t u32IndexToSelectedSurfaceFormat;

	// Physical Device
	bool alloc_physical_vulkan_device_list();
	void free_physical_vulkan_device_list();
	void select_best_physical_vulkan_device();
	
	// Scheduler
	extern std::unique_ptr<VkQueue[]> vk_pahQueues;
	extern std::unique_ptr<VkQueueFlags[]> vk_paeQueueTypes;
	extern std::unique_ptr<uint32_t[]> pau32QueueIndices;
	extern std::unique_ptr<VkCommandPool[]> vk_pahCommandPools;
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
