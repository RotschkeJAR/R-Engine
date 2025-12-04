#ifndef __RE_RENDERER_INTERNAL_H__
#define __RE_RENDERER_INTERNAL_H__

#include "RE_Renderer.hpp"
#include "RE_GPU.hpp"
#include "RE_Texture.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"

namespace RE {

	extern Color backgroundClearColor;

	// Presentation
#define RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE 2
#define RE_VK_SWAPCHAIN_SEMAPHORE_COUNT (u32SwapchainImageCount * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE)
	extern VkQueue vk_hPresentQueue;
	extern std::vector<VkSemaphore> swapchainSemaphores;
	extern uint32_t u32CurrentSwapchainSemaphoreIndex, u32SwapchainImageIndex;
	bool setup_presentation();
	void destroy_presentation();
	bool acquire_next_swapchain_image(bool &rbSkipRendering);
	bool present_swapchain_image();

	// Render image
	extern ScreenPercentageSettings screenPercentageSettings;
	extern VkImage vk_hRenderImages;
	extern VkImageView vk_ahRenderImageViews[RE_VK_FRAMES_IN_FLIGHT];
	extern VkExtent2D vk_renderImageSize;
	bool create_render_image_resources();
	void destroy_render_image_resources();
	bool record_cmd_blitting_render_image();

	// Render pipeline
	extern VkPipelineLayout vk_hGraphicsPipelineLayout;
	extern VkPipeline vk_hGraphicsPipeline;
	bool create_render_pipelines();
	bool recreate_render_pipelines();
	void destroy_render_pipelines();

	// Render task
#define RENDER_TASK_SUBINDEX_BUFFER_TRANSFER 0
#define RENDER_TASK_SUBINDEX_RENDERING 1
#define RENDER_TASK_SUBINDEX_IMAGE_BLIT 2
	extern std::array<VulkanTask, RE_VK_FRAMES_IN_FLIGHT> renderTasks;
	extern std::array<VkFence, RE_VK_FRAMES_IN_FLIGHT> renderFences;
	extern uint8_t u8CurrentFrameInFlightIndex;
	bool create_render_tasks();
	void destroy_render_tasks();

	// Render buffer
	extern VkBuffer vk_hRenderBuffer;
	extern VkDrawIndexedIndirectCommand *vk_pRenderBufferDrawCommand;
	bool create_render_buffers();
	void destroy_render_buffers();
	bool record_cmd_transfer_buffer();

	// MSAA
	extern VkImage vk_hSingleSampledWorldRenderImages;
	extern VkImageView vk_ahSingleSampledWorldRenderImageViews[RE_VK_FRAMES_IN_FLIGHT];
	bool create_singlesampled_images(const std::vector<uint32_t> &rRenderQueuesFamilyIndices, bool bResolvingRequired, bool bBlittingRequired);
	void destroy_singlesampled_images();

	// Depth-stencil images
#define DEPTH_IMAGE_INDEX 0
#define STENCIL_IMAGE_INDEX 1
	extern VkImage vk_a2hDepthStencilImages[2];
	extern VkImageView vk_a4hDepthStencilImageViews[4];
	bool create_depth_stencil_images(VulkanTask &rDepthStencilImageLayoutTransitionTask, VkFence vk_hDepthStencilImageLayoutTransitionFence);
	void destroy_depth_stencil_images();
	bool are_depth_stencil_images_separated(VkFormat vk_eFormat);

	// Descriptor Sets
	extern VkDescriptorPool vk_hPermanentDescPool;
	bool create_descriptor_sets();
	void destroy_descriptor_sets();

	// Camera
	extern VkDescriptorSetLayout vk_hCameraDescLayout;
	extern std::array<VkDescriptorSet, RE_VK_FRAMES_IN_FLIGHT> cameraDescSets;
	extern VkRect2D vk_cameraProjectionOnscreen;
	void calculate_camera_matrices();

	// Texture
	extern VkDescriptorSetLayout vk_hTextureDescLayout;
	extern VkDescriptorSet vk_hTextureDescSet;

	// Render Pass
	bool create_renderpass();
	void destroy_renderpass();
	void record_cmd_begin_renderpass(VkCommandBuffer vk_hCommandBuffer);
	void record_cmd_end_renderpass(VkCommandBuffer vk_hCommandBuffer);

	// General transfer task
	bool init_general_transfer_task();
	void destroy_general_transfer_task();

}

#endif /* __RE_RENDERER_INTERNAL_H__ */
