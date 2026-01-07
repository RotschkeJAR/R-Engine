#ifndef __RE_RENDERER_INTERNAL_H__
#define __RE_RENDERER_INTERNAL_H__

#include "RE_Renderer.hpp"
#include "RE_GPU.hpp"
#include "RE_Texture.hpp"
#include "RE_Vulkan_Wrappers.hpp"

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
	bool acquire_next_swapchain_image();
	bool present_swapchain_image();

	// Render image
	extern ScreenPercentageSettings screenPercentageSettings;
	extern Vulkan_Image renderImages;
	extern Vulkan_ImageView aRenderImageViews[RE_VK_FRAMES_IN_FLIGHT];
	extern VkExtent2D vk_renderImageSize;
	bool create_render_image_resources();
	void destroy_render_image_resources();
	bool record_cmd_blitting_render_image();

	// Render pipeline
	extern VkPipelineLayout vk_hGraphicsPipelineLayout;
	extern VkPipeline vk_hGraphicsPipeline2D, vk_hGraphicsPipeline3D;
	bool create_render_pipelines();
	bool recreate_render_pipelines();
	void destroy_render_pipelines();

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

	// Uniform buffer
#define RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME 2
#define RE_VK_CAMERA_UNIFORM_BUFFER_INDEX 0
#define RE_VK_OBJECT_COUNT_UNIFORM_BUFFER_INDEX 1

	struct UniformBufferInfo final {
		void *pUniformBuffersContent;
		VkDeviceSize vk_aCameraFirstBytes[RE_VK_FRAMES_IN_FLIGHT];
		VkDeviceSize vk_aObjectCountFirstBytes[RE_VK_FRAMES_IN_FLIGHT];
	};

	extern VkBuffer vk_aahUniformBuffers[RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME][RE_VK_FRAMES_IN_FLIGHT];
	bool create_uniform_buffers(UniformBufferInfo &rUniformBufferInfo);
	void destroy_uniform_buffers();

	// Storage buffer
	extern Vulkan_Buffer aSortableDepthBuffers[RE_VK_FRAMES_IN_FLIGHT], aRenderBuffers[RE_VK_FRAMES_IN_FLIGHT];

	// Staging buffer
	extern Vulkan_Buffer aRawRenderBuffers[RE_VK_FRAMES_IN_FLIGHT];
	extern VkDrawIndexedIndirectCommand *vk_apRenderBufferDrawCommands[RE_VK_FRAMES_IN_FLIGHT];
	bool create_render_buffers();
	void destroy_render_buffers();
	bool record_cmd_transfer_buffer();

	// MSAA
	extern Vulkan_Image singleSampledWorldRenderImages;
	extern Vulkan_ImageView aSingleSampledWorldRenderImageViews[RE_VK_FRAMES_IN_FLIGHT];
	bool create_singlesampled_images(const std::vector<uint32_t> &rRenderQueuesFamilyIndices, bool bResolvingRequired, bool bBlittingRequired);
	void destroy_singlesampled_images();

	// Depth-stencil images
#define DEPTH_IMAGE_INDEX 0
#define STENCIL_IMAGE_INDEX 1
	extern Vulkan_Image a2DepthStencilImages[2];
	extern Vulkan_ImageView a4DepthStencilImageViews[4];
	bool create_depth_stencil_images(VulkanTask &rDepthStencilImageLayoutTransitionTask, VkFence vk_hDepthStencilImageLayoutTransitionFence);
	void destroy_depth_stencil_images();
	bool are_depth_stencil_images_separated(VkFormat vk_eFormat);

	// Descriptor Sets
	extern VkDescriptorPool vk_hPermanentDescPool;
	bool create_descriptor_sets(const UniformBufferInfo &rUniformBufferInfo);
	void destroy_descriptor_sets();

	// Camera
#define RE_VK_VIEW_MATRIX_SIZE (4 * 4)
#define RE_VK_VIEW_MATRIX_OFFSET 0
#define RE_VK_PROJECTION_MATRIX_SIZE (4 * 4)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE
#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE (RE_VK_VIEW_MATRIX_SIZE + RE_VK_PROJECTION_MATRIX_SIZE)
#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES (RE_VK_CAMERA_UNIFORM_BUFFER_SIZE * sizeof(float))
	extern VkDescriptorSetLayout vk_hCameraDescLayout;
	extern std::array<VkDescriptorSet, RE_VK_FRAMES_IN_FLIGHT> cameraDescSets;
	extern VkRect2D vk_cameraProjectionOnscreen;
	void calculate_camera_matrices();

	// Texture
	extern VkDescriptorSetLayout vk_hTextureDescLayout;
	extern VkDescriptorSet vk_hTextureDescSet;

	// Processing
#define RE_VK_OBJECT_COUNT_UNIFORM_BUFFER_SIZE 1
#define RE_VK_OBJECT_COUNT_UNIFORM_BUFFER_SIZE_BYTES (RE_VK_OBJECT_COUNT_UNIFORM_BUFFER_SIZE * sizeof(uint32_t))
	extern VkDescriptorSetLayout vk_hProcessingDescLayout;
	extern VkDescriptorSet vk_ahProcessingDescSets[RE_VK_FRAMES_IN_FLIGHT];

	// Render Pass
	bool create_renderpass();
	void destroy_renderpass();
	void record_cmd_begin_renderpass(VkCommandBuffer vk_hCommandBuffer);
	void record_cmd_end_renderpass(VkCommandBuffer vk_hCommandBuffer);

	// Ordering
	extern Vulkan_Buffer aSortableDepthBuffers[RE_VK_FRAMES_IN_FLIGHT];
	bool create_processing_pipelines();
	void destroy_processing_pipelines();
	void order_rendering(VkCommandBuffer vk_hCommandBuffer);

}

#endif /* __RE_RENDERER_INTERNAL_H__ */
