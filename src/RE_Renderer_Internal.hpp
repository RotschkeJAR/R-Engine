#ifndef __RE_RENDERER_INTERNAL_H__
#define __RE_RENDERER_INTERNAL_H__

#include "RE_Renderer.hpp"
#include "RE_GPU.hpp"
#include "RE_Texture.hpp"
#include "RE_Vulkan_Wrappers.hpp"

#include "RE_Renderer_Descriptor Sets.hpp"
#include "RE_Renderer_Descriptor Set Pools.hpp"
#include "RE_Renderer_Descriptor Set Layouts.hpp"
#include "RE_Renderer_Pipelines.hpp"
#include "RE_Renderer_Buffers.hpp"
#include "RE_Renderer_Camera.hpp"
#include "RE_Renderer_Images.hpp"

namespace RE {

	bool swapchain_created_renderer();
	void swapchain_destroyed_renderer();

	extern Color backgroundClearColor;

	// Swapchain
	extern VkSwapchainKHR vk_hSwapchain;
	extern VkFormat vk_eSwapchainImageFormat;
	extern VkExtent2D vk_swapchainResolution;
	extern std::unique_ptr<VkImage[]> swapchainImages;
	extern std::unique_ptr<VkImageView[]> swapchainImageViews;
	extern uint32_t u32SwapchainImageCount;
	extern bool bVsyncEnabled;
	bool create_swapchain();
	void destroy_swapchain();
	bool recreate_swapchain();

	// Presentation
#define RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE 2
#define RE_VK_SWAPCHAIN_SEMAPHORE_COUNT (u32SwapchainImageCount * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE)
	extern VkQueue vk_hPresentQueue;
	extern std::unique_ptr<VkSemaphore[]> swapchainSemaphores;
	extern uint32_t u32CurrentSwapchainSemaphoreIndex, u32SwapchainImageIndex;
	bool setup_presentation();
	void destroy_presentation();
	bool acquire_next_swapchain_image();
	bool present_swapchain_image();
	bool recreate_swapchain();

	// Render task
#define RENDER_TASK_SUBINDEX_BUFFER_TRANSFER 0
#define RENDER_TASK_SUBINDEX_PROCESSING 1
#define RENDER_TASK_SUBINDEX_RENDERING 2
#define RENDER_TASK_SUBINDEX_IMAGE_BLIT 3
	extern std::array<VulkanTask, RE_VK_FRAMES_IN_FLIGHT> renderTasks;
	extern std::array<VkFence, RE_VK_FRAMES_IN_FLIGHT> renderFences;
	extern uint8_t u8CurrentFrameInFlightIndex;
	bool create_render_tasks();
	void destroy_render_tasks();

	// Dynamic rendering
	void record_cmd_begin_dynamic_rendering(VkCommandBuffer vk_hCommandBuffer);
	void record_cmd_end_dynamic_rendering(VkCommandBuffer vk_hCommandBuffer);

}

#endif /* __RE_RENDERER_INTERNAL_H__ */
