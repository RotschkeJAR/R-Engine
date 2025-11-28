#ifndef __RE_RENDERER_INTERNAL_H__
#define __RE_RENDERER_INTERNAL_H__

#include "RE_Renderer.hpp"
#include "RE_GPU.hpp"
#include "RE_Texture.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_RenderPipelines.hpp"

namespace RE {

	extern bool bRenderPipelinesDirty;

	// Render image
	extern VkImage vk_hRenderImages;
	extern VkImageView vk_ahRenderImageViews[RE_VK_FRAMES_IN_FLIGHT];
	extern VkExtent2D vk_renderImageSize;
	bool create_render_image_resources();
	void destroy_render_image_resources();
	bool record_command_buffer_transfering_render_image();

	// Render task
#define RENDER_TASK_SUBINDEX_BUFFER_TRANSFER 0
#define RENDER_TASK_SUBINDEX_RENDERING 1
#define RENDER_TASK_SUBINDEX_IMAGE_TRANSFER 2
	extern std::array<VulkanTask, RE_VK_FRAMES_IN_FLIGHT> renderTasks;
	extern std::array<VkFence, RE_VK_FRAMES_IN_FLIGHT> renderFences;
	extern uint8_t u8CurrentFrameInFlightIndex;
	bool create_render_tasks();
	void destroy_render_tasks();

	// Render buffer
	extern VkBuffer vk_hRenderBuffer;
	extern VkDrawIndexedIndirectCommand *vk_pRenderBufferDrawCommand;
	extern GameObjectInstanceData *paRenderBufferInstanceData;
	bool create_render_buffers();
	void destroy_render_buffers();

	// MSAA
	extern VkSampleCountFlagBits vk_eMsaaCount;
	extern VkImage vk_hSingleSampledWorldRenderImages;
	bool create_singlesampled_images(bool bResolvingRequired, bool bBlittingRequired);
	void destroy_singlesampled_images();

	// Depth-stencil images
	extern VkImage vk_a2hDepthStencilImages[2];
	extern VkImageView vk_a4hDepthStencilImageViews[4];
	bool create_depth_stencil_images(VulkanTask &rDepthStencilImageLayoutTransitionTask, VkFence vk_hDepthStencilImageLayoutTransitionFence);
	void destroy_depth_stencil_images();

	// Descriptor Sets
	extern VkDescriptorPool vk_hPermanentDescPool;
	bool create_descriptor_sets();
	void destroy_descriptor_sets();

	// Camera
	extern VkDescriptorSetLayout vk_hCameraDescLayout;
	extern std::array<VkDescriptorSet, RE_VK_FRAMES_IN_FLIGHT> cameraDescSets;

	// Texture
	extern VkDescriptorSetLayout vk_hTextureDescLayout;

	// Render Pass
	bool create_renderpass();
	void destroy_renderpass();

	// General transfer task
	bool init_general_transfer_task();
	void destroy_general_transfer_task();

}

#endif /* __RE_RENDERER_INTERNAL_H__ */
