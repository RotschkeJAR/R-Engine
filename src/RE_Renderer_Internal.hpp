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
	extern std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> renderImageViews;
	bool create_render_image_resources();
	void destroy_render_image_resources();

	// Render task
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
	extern std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT> vk_ahSingleSampledWorldRenderImages;
	bool create_singlesampled_images(const VkExtent3D &vk_rSingleSampledImageExtent3D);
	void destroy_singlesampled_images();

	// Depth-stencil images
	extern std::array<VkImage, 2> vk_ahDepthStencilImages;
	extern std::array<VkImageView, vk_ahDepthStencilImages.size() * 2> vk_ahDepthStencilImageViews;
	bool create_depth_stencil_images(const VkExtent3D &vk_rDepthStencilImageExtent3D, VulkanTask &rDepthStencilImageLayoutTransitionTask, VkFence vk_hDepthStencilImageLayoutTransitionFence);
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
