#ifndef __RE_RENDERER_SWAPCHAIN_H__
#define __RE_RENDERER_SWAPCHAIN_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	extern VkSwapchainKHR vk_hSwapchain;
	extern VkFormat vk_eSwapchainImageFormat;
	extern VkExtent2D vk_swapchainResolution;
	extern std::unique_ptr<VkImage[]> swapchainImages;
	extern std::unique_ptr<VkImageView[]> swapchainImageViews;
	extern uint32_t u32SwapchainImageCount;
	extern bool bSwapchainDirty;
	
	bool create_swapchain();
	void destroy_swapchain();
	bool recreate_swapchain();

}

#endif /* __RE_RENDERER_SWAPCHAIN_H__ */
