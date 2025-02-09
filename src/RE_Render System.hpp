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
			VkSurfaceFormatKHR surfaceFormat;
			VkExtent2D swapchainImageSize;
			VkSwapchainKHR internalSwapchain;
			uint32_t swapchainImageCount;
			VkImage* swapchainImages;
			VkImageView* swapchainImageViews;

			bool createSwapchain();
			bool createImageViews();

		public:
			static RenderSystem* instance;

			RenderSystem();
			~RenderSystem();
			void windowResize(Vector<REushort, 2> newSize);
			bool isValid();
	};

}

#endif /* __RE_RENDER_SYSTEM_H__ */
