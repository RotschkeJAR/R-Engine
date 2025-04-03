#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan.hpp"

namespace RE {

#define RE_VK_QUEUE_COUNT 3U
#define RE_VK_COMMAND_POOL_COUNT 2U
#define RE_VK_COMMAND_BUFFER_COUNT 1U
#define RE_VK_SEMAPHORE_COUNT 4U
#define RE_VK_FENCE_COUNT 2U

	typedef float REvertex;
#define RE_VK_RENDERABLE_OBJECTS_COUNT 1000U
#define RE_VK_VERTEX_COUNT (RE_VK_RENDERABLE_OBJECTS_COUNT * 4U)
#define RE_VK_VERTEX_POSITION_SIZE 3U
#define RE_VK_VERTEX_POSITION_SIZE_BYTES (RE_VK_VERTEX_POSITION_SIZE * sizeof(REvertex))
#define RE_VK_VERTEX_POSITION_OFFSET 0U
#define RE_VK_VERTEX_POSITION_OFFSET_BYTES (RE_VK_VERTEX_POSITION_OFFSET * sizeof(REvertex))
#define RE_VK_VERTEX_COLOR_SIZE 4U
#define RE_VK_VERTEX_COLOR_SIZE_BYTES (RE_VK_VERTEX_COLOR_SIZE * sizeof(REvertex))
#define RE_VK_VERTEX_COLOR_OFFSET RE_VK_VERTEX_POSITION_SIZE
#define RE_VK_VERTEX_COLOR_OFFSET_BYTES (RE_VK_VERTEX_COLOR_OFFSET * sizeof(REvertex))
#define RE_VK_VERTEX_TOTAL_SIZE 7U
#define RE_VK_VERTEX_TOTAL_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE * sizeof(REvertex))
	
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
			void upload_to_vertex_buffer(const REvertex *const pNewVertexBufferData, const uint32_t u32VertexCount);
			void window_resize_event();
			void wait_for_idle_device();
			bool is_valid();
	};

	void enable_vsync(bool bEnableVsync);
	bool is_vsync_enabled();

	VkPhysicalDevice get_vulkan_physical_device();
	VkDevice get_vulkan_device();

}

#endif /* __RE_RENDER_SYSTEM_H__ */
