#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan.hpp"

namespace RE {

#define RE_VK_QUEUE_COUNT 3U
#define RE_VK_QUEUE_GRAPHICS_INDEX 0U
#define RE_VK_QUEUE_PRESENT_INDEX 1U
#define RE_VK_QUEUE_TRANSFER_INDEX 2U

#define RE_VK_COMMAND_POOL_COUNT 2U
#define RE_VK_COMMAND_POOL_GRAPHICS_INDEX 0U
#define RE_VK_COMMAND_POOL_TRANSFER_INDEX 1U

	// Attributes initialized at beginning and rarely changed
	extern VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties;
	extern VkDevice vk_hDevice;
	extern VkQueue vk_hDeviceQueueFamilies[RE_VK_QUEUE_COUNT];
	extern uint32_t u32DeviceQueueFamilyIndices[RE_VK_QUEUE_COUNT];
	extern VkSwapchainKHR vk_hSwapchain;
	extern VkFormat vk_eSwapchainImageFormat;
	extern VkExtent2D vk_swapchainResolution;
	extern VkImage *vk_phSwapchainImages;
	extern VkImageView *vk_phSwapchainImageViews;
	extern uint32_t u32SwapchainImageCount;
	extern VkCommandPool vk_hCommandPools[RE_VK_COMMAND_POOL_COUNT];

	// Configurable settings
	extern VkPhysicalDevice vk_hPhysicalDeviceSelected;
	
	class RenderSystem final {
		private:
			bool bValid;

			bool create_surface();
			void destroy_surface();
			void fetch_surface_infos();
			bool alloc_physical_device_list();
			void free_physical_device_list();
			bool create_device();
			void destroy_device();
			bool create_swapchain();
			void destroy_swapchain();
			bool recreate_swapchain();

		public:
			static RenderSystem* pInstance;

			RenderSystem();
			~RenderSystem();
			bool refresh();
			void window_resize_event();
			bool is_valid();
	};

	void enable_vsync(bool bEnableVsync);
	bool is_vsync_enabled();
	void bind_fps_to_vsync(bool bBindFpsToVsync);
	bool is_fps_bound_to_vsync();

#define WAIT_FOR_IDLE_VULKAN_DEVICE() CATCH_SIGNAL(vkDeviceWaitIdle(vk_hDevice))

}

#endif /* __RE_RENDER_SYSTEM_H__ */
