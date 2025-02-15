#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class RenderSystem {
		private:
			bool valid;
			bool vsyncActive;
			VkPresentModeKHR vsyncMode;
			VkPresentModeKHR noVsync;
			VkSurfaceFormatKHR internalSurfaceFormat;
			VkExtent2D internalSwapchainImageSize;
			VkSwapchainKHR internalSwapchain;
			uint32_t internalSwapchainImageCount;
			VkImage* internalSwapchainImages;
			VkImageView* internalSwapchainImageViews;
			VkShaderModule internalVertexShader, internalFragmentShader;
			VkRenderPass internalRenderPass;
			VkPipelineLayout internalPipelineLayout;
			VkPipeline internalPipeline;
			VkFramebuffer* internalFramebuffers;
			VkCommandPool internalCmdPool;
			VkCommandBuffer internalCmdBuffer;
			VkSemaphore internalImgAvailableSemaphore, internalRenderFinishedSemaphore;
			VkFence internalFence;

			bool createSwapchain();
			bool createImageViews();
			bool createShaders();
			bool createRenderPass();
			bool createPipeline();
			bool createFramebuffers();
			bool createCommands();
			bool createSync();
			bool recordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t imgIndex);

		public:
			static RenderSystem* instance;

			RenderSystem();
			~RenderSystem();
			void drawFrame();
			void windowResize(Vector<REushort, 2> newSize);
			bool isValid();
	};

}

#endif /* __RE_RENDER_SYSTEM_H__ */
