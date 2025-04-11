#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan.hpp"
#include "RE_Rendering_Command Buffer.hpp"
#include "RE_Rendering_Queue.hpp"

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
	extern Rendering_Queue *pDeviceQueues[RE_VK_QUEUE_COUNT];
	extern VkFormat vk_eSwapchainImageFormat;
	extern VkExtent2D vk_swapchainResolution;
	extern Rendering_CommandPool *pCommandPools[RE_VK_COMMAND_POOL_COUNT];

	// Configurable settings
	extern VkPhysicalDevice vk_hPhysicalDeviceSelected;
	
	class RenderSystem {
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
			bool create_shaders();
			void destroy_shaders();
			bool create_pipeline_layout();
			void destroy_pipeline_layout();
			bool create_renderpass();
			void destroy_renderpass();
			bool create_graphics_pipeline();
			void destroy_graphics_pipeline();
			bool create_framebuffers();
			void destroy_framebuffers();
			bool create_vertex_buffer();
			void destroy_vertex_buffer();
			bool alloc_command_buffers();
			void free_command_buffers();
			bool record_command_buffers();
			bool create_sync_objects();
			void destroy_sync_objects();

		public:
			static RenderSystem* pInstance;

			RenderSystem();
			~RenderSystem();
			void draw_frame();
			void upload_to_vertex_buffer();
			void window_resize_event();
			bool is_valid();
	};

	void enable_vsync(bool bEnableVsync);
	bool is_vsync_enabled();

#define WAIT_FOR_IDLE_VULKAN_DEVICE() CATCH_SIGNAL(vkDeviceWaitIdle(vk_hDevice))

}

#endif /* __RE_RENDER_SYSTEM_H__ */
