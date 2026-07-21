#ifndef __RE_RENDERER_PRESENTATION_H__
#define __RE_RENDERER_PRESENTATION_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {
	
#define RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE 2
#define RE_VK_SWAPCHAIN_SEMAPHORE_COUNT (u32SwapchainImageCount * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE)
	
	extern VkQueue vk_hPresentQueue;
	extern std::unique_ptr<VkSemaphore[]> swapchainSemaphores;
	extern uint32_t u32CurrentSwapchainSemaphoreIndex,
		u32CurrentSwapchainImageIndex;

	bool setup_presentation();
	void destroy_presentation();
	bool acquire_next_swapchain_image();
	bool present_swapchain_image();
	bool recreate_swapchain();

}

#endif /* __RE_RENDERER_PRESENTATION_H__ */
