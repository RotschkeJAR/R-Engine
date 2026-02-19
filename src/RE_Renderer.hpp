#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Vulkan_Device.hpp"
#include "RE_GPU.hpp"

namespace RE {

	struct alignas(16) GameObjectInstanceData final {
		float a4fPosition[4];
		float a4fRotation[4];
		float a4fScale[4];
		float a4fColor[4];
		uint32_t u32TextureId;
	};

#define RE_VK_FRAMES_IN_FLIGHT 2

	extern VkPipelineLayout vk_hWorldPipelineLayout;
	extern float fSampleShadingRate;
	extern uint8_t u8CurrentFrameInFlightIndex;

	bool init_renderer();
	void destroy_renderer();
	void render();
	bool swapchain_created_renderer();
	void swapchain_destroyed_renderer();
	bool wait_for_rendering_finished();

	// Render Images
	void get_queues_for_swapchain_images(std::vector<uint32_t> &rRenderTaskQueueIndices);

	// Render buffer
	extern GameObjectInstanceData *paRenderBufferInstanceData;

	// Depth-stencil buffers
	extern std::vector<VkFormat> availableDepthStencilFormats;
	extern uint8_t u8IndexToSelectedDepthStencilFormat;
	extern bool bStencilsEnabled, bSeparateStencilsSupported;
	bool does_gpu_support_depth_stencil_images(VkPhysicalDevice vk_hPhysicalDevice, std::queue<std::string> &rMissingFeatures);
	int32_t rate_gpu_depth_stencil_image_formats(VkPhysicalDevice vk_hPhysicalDevice, VkSampleCountFlags &vk_reMsaaAvailable);
	void find_suitable_depth_stencil_formats();
	void discard_incompatible_msaa_modes_for_depth_stencil_images(VkSampleCountFlags &vk_reMsaaAvailable);

	// Descriptor sets
#define RE_VK_UNIFORM_BINDING_CAMERA 0
#define RE_VK_UNIFORM_BINDING_TEXTURE 0

	// Textures
#define RE_VK_MAX_SAMPLED_IMAGES 0x7FFF
	extern VkDescriptorSet vk_hTextureDescSet;
	bool does_gpu_support_textures(VkPhysicalDevice vk_hPhysicalDevice, const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits, std::queue<std::string> &rMissingFeatures, std::queue<std::string> &rDiscrepantFeatures);
	int32_t rate_gpu_texture_capacity(const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits);

	// MSAA
	extern VkSampleCountFlags vk_eAllowedMsaaSamples;
	extern VkSampleCountFlagBits vk_eMsaaCount;

}

#endif /* __RE_RENDERER_H__ */
