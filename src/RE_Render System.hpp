#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class RenderSystem {
		private:
			bool bValid;
			bool bVsyncActive;
			VkPresentModeKHR vk_eVsyncMode, vk_eNoVsync;
			VkSurfaceFormatKHR vk_internalSurfaceFormat;
			VkExtent2D vk_internalSwapchainImageSize;
			VkSwapchainKHR vk_hInternalSwapchain;
			uint32_t u32InternalSwapchainImageCount;
			VkImage* vk_pInternalSwapchainImages;
			VkImageView* vk_pInternalSwapchainImageViews;
			VkShaderModule vk_hInternalVertexShader, vk_hInternalFragmentShader;
			VkRenderPass vk_hInternalRenderPass;
			VkPipelineLayout vk_hInternalPipelineLayout;
			VkPipeline vk_hInternalPipeline;
			VkFramebuffer* vk_pInternalFramebuffers;
			VkCommandPool vk_hInternalCmdPool;
			VkCommandBuffer vk_hInternalCmdBuffer;
			VkSemaphore vk_hInternalImgAvailableSemaphore, vk_hInternalRenderFinishedSemaphore;
			VkFence vk_hInternalFence;

			bool create_swapchain();
			void clear_swapchain();
			bool create_shaders();
			bool create_renderpass();
			bool create_pipeline();
			bool create_framebuffers();
			bool create_command_buffers();
			bool create_sync_objects();
			bool record_command_buffer(VkCommandBuffer cmdBuffer, uint32_t imgIndex);

		public:
			static RenderSystem* pInstance;

			RenderSystem();
			~RenderSystem();
			void draw_frame();
			void window_resize_event(Vector<REushort, 2> newSize);
			bool is_valid();
	};

}

#endif /* __RE_RENDER_SYSTEM_H__ */
