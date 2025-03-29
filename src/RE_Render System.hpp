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

			// Attributes initialized at beginning and rarely changed
			VkPhysicalDevice *vk_phPhysicalDevicesAvailable;
			uint32_t u32PhysicalDevicesAvailableCount;
			VkDevice vk_hDevice;
			VkQueue vk_hQueues[RE_VK_QUEUE_COUNT];
			uint32_t u32QueueIndices[RE_VK_QUEUE_COUNT];
			VkSurfaceKHR vk_hSurface;
			VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
			VkSurfaceFormatKHR *vk_peSurfaceFormatsAvailable;
			uint32_t u32SurfaceFormatsAvailableCount;
			VkPresentModeKHR vk_ePresentModeVsync, vk_ePresentModeNoVsync;
			VkSwapchainKHR vk_hSwapchain;
			VkFormat vk_eSwapchainImageFormat;
			VkExtent2D vk_swapchainResolution;
			VkImage *vk_pSwapchainImages;
			VkImageView *vk_pSwapchainImageViews;
			uint32_t u32SwapchainImageCount;
			VkShaderModule vk_hVertexShader, vk_hFragmentShader;
			VkPipelineLayout vk_hPipelineLayout;
			VkRenderPass vk_hRenderPass;
			VkPipeline vk_hGraphicsPipeline;
			VkFramebuffer *vk_phSwapchainFramebuffers; // size equals swapchain image count
			VkBuffer vk_hVertexStagingBuffer;
			VkDeviceMemory vk_hVertexStagingBufferMemory;
			VkCommandPool vk_hCommandPools[RE_VK_COMMAND_POOL_COUNT];
			VkCommandBuffer *vk_phCommandBuffersGraphics, *vk_phCommandBuffersTransfer;
			VkSemaphore vk_hSemaphores[RE_VK_SEMAPHORE_COUNT];
			VkFence vk_hFences[RE_VK_FENCE_COUNT];

			// Runtime changing variables
			bool bUseOtherFrame;

			// Configurable settings
			VkPhysicalDevice vk_hPhysicalDeviceSelected;
			VkSurfaceFormatKHR vk_surfaceFormatSelected;
			VkPresentModeKHR vk_ePresentModeSelected;
			bool bVsyncEnabled;

			// Abstract functions
			bool create_buffer(VkDeviceSize vk_size, VkBufferUsageFlags vk_usage, VkSharingMode vk_sharingMode, uint32_t u32QueueFamilyIndexCount, const uint32_t* pu32QueueFamilyIndices, VkMemoryPropertyFlags vk_properties, VkBuffer& vk_rBuffer, VkDeviceMemory& vk_rDeviceMemory);

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

}

#endif /* __RE_RENDER_SYSTEM_H__ */
