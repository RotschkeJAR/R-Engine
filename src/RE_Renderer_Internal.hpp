#ifndef __RE_RENDERER_INTERNAL_H__
#define __RE_RENDERER_INTERNAL_H__

#include "RE_Renderer.hpp"
#include "RE_GPU.hpp"
#include "RE_Texture.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"

namespace RE {

	extern bool bRenderPipelinesDirty;

	// MSAA
	extern VkSampleCountFlagBits vk_eMsaaCount;
	extern std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT> vk_ahSingleSampledWorldRenderImages;
	bool create_singlesampled_images(const VkExtent3D &vk_rSingleSampledImageExtent3D);
	void destroy_singlesampled_images();

	// Depth-stencil images
	extern std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT * 2> vk_ahDepthStencilImages;
	extern std::array<VkImageView, vk_ahDepthStencilImages.size()> vk_ahDepthStencilImageViews;
	bool create_depth_stencil_images(const VkExtent3D &vk_rDepthStencilImageExtent3D, VulkanTask &rDepthStencilImageLayoutTransitionTask, VkFence vk_hDepthStencilImageLayoutTransitionFence);
	void destroy_depth_stencil_images();

	// Descriptor Sets
	extern VkDescriptorSetLayout vk_hCameraDescriptorSetLayout;
	extern VkDescriptorSet vk_ahCameraDescriptorSets[RE_VK_FRAMES_IN_FLIGHT];
	bool create_descriptor_sets();
	void destroy_descriptor_sets();

	// Render Pass
	bool create_renderpass();
	void destroy_renderpass();

}

#endif /* __RE_RENDERER_INTERNAL_H__ */
