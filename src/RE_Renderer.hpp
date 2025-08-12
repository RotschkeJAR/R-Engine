#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Render System.hpp"
#include "RE_Texture.hpp"

namespace RE {

#define RE_VK_RENDERABLE_RECTANGLES_COUNT 16384U
#define RE_VK_FRAMES_IN_FLIGHT 2

#define RE_VK_GAME_OBJECT_SUPBASS 0

#define RE_VK_SEMAPHORES_PER_FRAME_COUNT 1
#define RE_VK_TRANSFER_GAME_OBJECT_VERTICES_SEMAPHORE_INDEX 0
#define RE_VK_RENDER_SEMAPHORE_COUNT (RE_VK_FRAMES_IN_FLIGHT * RE_VK_SEMAPHORES_PER_FRAME_COUNT)

#define RE_VK_COUNT_OF_CAMERA_UNIFORM_DESCRIPTOR 1
#define RE_VK_COUNT_OF_TEXTURE_DESCRIPTOR 32
#define RE_VK_TOTAL_COUNT_OF_DESCRIPTORS (RE_VK_COUNT_OF_CAMERA_UNIFORM_DESCRIPTOR + RE_VK_COUNT_OF_TEXTURE_DESCRIPTOR)

#define RE_VK_TOTAL_COUNT_OF_DESCRIPTOR_SETS RE_VK_FRAMES_IN_FLIGHT

	extern const Camera *pActiveCamera;
	extern VkViewport vk_cameraViewport;
	extern VkRect2D vk_cameraScissor;
	extern VkExtent2D vk_worldRenderImageExtent;

	extern VkSampleCountFlagBits vk_eMsaaCount;

	extern VkBool32 vk_bSampleShadingEnabled;
	extern float fSampleShadingRate;

	extern VkBuffer vk_hRectIndexBuffer;
	extern VkDeviceMemory vk_hRectIndexBufferMemory;

	extern VkDescriptorSetLayout vk_hWorldDescriptorSetLayout;
	extern std::array<VkDescriptorSet, RE_VK_TOTAL_COUNT_OF_DESCRIPTOR_SETS> vk_ahDescriptorSets;

	extern VkPipelineLayout vk_hWorldBasicPipelineLayout;

	extern VkRenderPass vk_hWorldRenderPass;
	extern std::array<VkFramebuffer, RE_VK_FRAMES_IN_FLIGHT> vk_ahWorldFramebuffers;

	extern std::array<VkSemaphore, RE_VK_RENDER_SEMAPHORE_COUNT> vk_ahRenderSemaphores;

	extern uint8_t u8CurrentFrameInFlightIndex;

	void update_texture_descriptor_set(uint8_t u8FrameInFlightIndex, uint32_t u32TextureCount, const TextureContainer *const *papTextureContainers);
	bool init_renderer();
	void destroy_renderer();
	void render();
	bool swapchain_created_renderer();
	void swapchain_destroyed_renderer();
	void attach_camera(const Camera *pCamera);

}

#endif /* __RE_RENDERER_H__ */
