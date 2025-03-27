#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan.hpp"

namespace RE {

#define RE_VK_QUEUE_COUNT 2U
#define RE_VK_COMMAND_BUFFER_COUNT 1U
#define RE_VK_SEMAPHORE_COUNT 2U
	
	class RenderSystem {
		private:
			bool bValid;

			// Handles making rendering in Vulkan work
			VkPhysicalDevice *vk_phPhysicalDevicesAvailable;
			uint32_t u32PhysicalDevicesAvailableCount;
			VkDevice vk_hDevice;
			VkQueue vk_hQueues[RE_VK_QUEUE_COUNT];
			uint32_t u32QueueIndices[2];
			VkSurfaceKHR vk_hSurface;
			VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
			VkSurfaceFormatKHR *vk_peSurfaceFormatsAvailable;
			uint32_t u32SurfaceFormatsAvailableCount;
			VkPresentModeKHR vk_ePresentModeVsync, vk_ePresentModeNoVsync;
			VkSwapchainKHR vk_hSwapchain;
			VkFormat vk_eSwapchainImageFormat;
			VkExtent2D vk_swapchainResolution;
			VkImage *vk_pSwapchainImages;
			VkImageView *vk_pSwapchainImageView;
			uint32_t u32SwapchainImageCount;
			VkShaderModule vk_hVertexShader, vk_hFragmentShader;
			VkPipelineLayout vk_hPipelineLayout;
			VkRenderPass vk_hRenderPass;
			VkPipeline vk_hGraphicsPipeline;
			VkFramebuffer *vk_phSwapchainFramebuffers; // size equals swapchain image count
			VkCommandPool vk_hCommandPool;
			VkCommandBuffer *vk_phCommandBuffers;
			VkSemaphore vk_hSemaphores[RE_VK_SEMAPHORE_COUNT];
			VkFence vk_hFence;

			// Configurable settings
			VkPhysicalDevice vk_hPhysicalDeviceSelected;
			VkSurfaceFormatKHR vk_surfaceFormatSelected;
			VkPresentModeKHR vk_ePresentModeSelected;
			bool bVsyncEnabled;

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
			void wait_for_idle_device();
			bool is_valid();
	};

}

#endif /* __RE_RENDER_SYSTEM_H__ */
