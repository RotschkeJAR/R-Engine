#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Vulkan_Device.hpp"
#include "RE_GPU.hpp"

namespace RE {

#define RE_VK_FRAMES_IN_FLIGHT 2

	extern float fSampleShadingRate;
	extern VkPipelineLayout vk_hWorldPipelineLayout;
	extern uint8_t u8CurrentFrameInFlightIndex;

	extern const Camera *pActiveCamera;

	uint8_t get_render_graphics_queue_logical_index();
	bool init_renderer();
	void destroy_renderer();
	void render();
	bool swapchain_created_renderer();
	void swapchain_destroyed_renderer();
	bool wait_for_rendering_finished();
	void attach_camera(const Camera *pCamera);

	// Depth-stencil buffers
	extern std::vector<VkFormat> availableDepthStencilFormats;
	extern uint8_t u8IndexToSelectedDepthStencilFormat;
	extern bool bStencilsEnabled;
	bool does_gpu_support_depth_stencil_images(VkPhysicalDevice vk_hPhysicalDevice, std::queue<std::string> &rMissingFeatures);
	int32_t rate_gpu_depth_stencil_image_formats(VkPhysicalDevice vk_hPhysicalDevice, VkSampleCountFlags &vk_reMsaaAvailable);
	void find_suitable_depth_stencil_formats();
	void discard_incompatible_msaa_modes_for_depth_stencil_images(VkSampleCountFlags &vk_reMsaaAvailable);

	// Descriptor sets
#define RE_VK_UNIFORM_BINDING_CAMERA 0
#define RE_VK_UNIFORM_BINDING_TEXTURE 1

	// Textures
#define RE_VK_MAX_SAMPLED_IMAGES 0x7FFF
	extern VkDescriptorSet vk_hTextureDescSet;
	bool does_gpu_support_textures(VkPhysicalDevice vk_hPhysicalDevice, const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits, std::queue<std::string> &rMissingFeatures, std::queue<std::string> &rDiscrepantFeatures);
	int32_t rate_gpu_texture_capacity(const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits);

	// MSAA
	extern VkSampleCountFlags vk_eAllowedMsaaSamples;
	extern VkSampleCountFlagBits vk_eMsaaCount;

	// General transfer task
	bool submit_transfer_task(VkCommandBufferUsageFlags vk_eUsageFlags, std::function<void (VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue)> pTransferFunction, std::function<void (VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue)> pOwnershipAcquireFunction, uint32_t u32WaitSemaphoreCount, const VkSemaphoreSubmitInfo *vk_paWaitSemaphores, uint32_t u32SignalSemaphoreCount, const VkSemaphoreSubmitInfo *vk_paSignalSemaphores, VkFence vk_hFence);
	void wait_for_transfer(uint64_t u64Timeout);

}

#endif /* __RE_RENDERER_H__ */
