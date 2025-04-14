#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan.hpp"
#include "RE_Vulkan_Command Buffer.hpp"
#include "RE_Vulkan_Queue.hpp"

namespace RE {

#define RE_VK_QUEUE_COUNT 3U
#define RE_VK_QUEUE_GRAPHICS_INDEX 0U
#define RE_VK_QUEUE_PRESENT_INDEX 1U
#define RE_VK_QUEUE_TRANSFER_INDEX 2U

#define RE_VK_COMMAND_POOL_COUNT 2U
#define RE_VK_COMMAND_POOL_GRAPHICS_INDEX 0U
#define RE_VK_COMMAND_POOL_TRANSFER_INDEX 1U

	// Attributes initialized at beginning and rarely changed
	extern VkDevice vk_hDevice;
	extern Vulkan_Queue *pDeviceQueues[RE_VK_QUEUE_COUNT];
	extern VkFormat vk_eSwapchainImageFormat;
	extern VkExtent2D vk_swapchainResolution;
	extern VkImageView *vk_pSwapchainImageViews;
	extern Vulkan_CommandPool *pCommandPools[RE_VK_COMMAND_POOL_COUNT];

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
			void recreate_swapchain();

		public:
			static RenderSystem* pInstance;

			RenderSystem();
			~RenderSystem();
			void refresh();
			void window_resize_event();
			bool is_valid();
	};

	void enable_vsync(bool bEnableVsync);
	bool is_vsync_enabled();

#define WAIT_FOR_IDLE_VULKAN_DEVICE() CATCH_SIGNAL(vkDeviceWaitIdle(vk_hDevice))

}

#endif /* __RE_RENDER_SYSTEM_H__ */
