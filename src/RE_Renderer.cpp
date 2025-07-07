#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Renderer_Opaque_GameObject.hpp"
#include "RE_Renderer_Transparent_GameObject.hpp"

#include <thread>

namespace RE {

#define GET_VULKAN_MATRIX_ELEMENT_INDEX(COLUMN, LINE) (COLUMN * 4U + LINE)

	enum ScreenPercentage {
		SCREEN_PERCENTAGE_CONST_IMG_SIZE,
		SCREEN_PERCENTAGE_SWAPCHAIN_EQUAL,
		SCREEN_PERCENTAGE_SWAPCHAIN_PERCENTAGE
	};

	typedef uint16_t REindex_t;
#define RE_VK_RECT_INDEX_BUFFER_SIZE (RE_VK_RENDERABLE_RECTANGLES_COUNT * 6U)
#define RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES (RE_VK_RECT_INDEX_BUFFER_SIZE * sizeof(REindex_t))

#define RE_VK_VIEW_MATRIX_SIZE (4U * 4U)
#define RE_VK_VIEW_MATRIX_OFFSET 0U
#define RE_VK_PROJECTION_MATRIX_SIZE (4U * 4U)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE

#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE (RE_VK_VIEW_MATRIX_SIZE + RE_VK_PROJECTION_MATRIX_SIZE)
#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES (RE_VK_CAMERA_UNIFORM_BUFFER_SIZE * sizeof(float))

#define RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE 2U
#define RE_VK_SWAPCHAIN_SEMAPHORE_COUNT (u32SwapchainImageCount * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE)

	Camera *pActiveCamera = nullptr;
	VkViewport vk_cameraViewport = {
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	VkRect2D vk_cameraScissor;
	VkExtent2D vk_worldRenderImageExtent = {
		.width = 600U,
		.height = 400U
	};

	ScreenPercentage eScreenPercentage = SCREEN_PERCENTAGE_SWAPCHAIN_EQUAL;
	float fScreenPercentage;

	VkBuffer vk_hRectIndexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vk_hRectIndexBufferMemory = VK_NULL_HANDLE;

	VkDescriptorSetLayout vk_hWorldCameraDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool vk_hWorldCameraDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet vk_ahWorldCameraDescriptorSets[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkBuffer vk_ahWorldCameraUniformBuffers[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkDeviceMemory vk_ahWorldCameraUniformBufferMemories[RE_VK_FRAMES_IN_FLIGHT] = {};
	float *apafCameraUniformData[RE_VK_FRAMES_IN_FLIGHT] = {};

	VkPipelineLayout vk_hWorldBasicPipelineLayout = VK_NULL_HANDLE;

	VkImage vk_ahWorldDepthStencilImages[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkDeviceMemory vk_ahWorldDepthStencilImageMemories[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkImageView vk_ahWorldDepthStencilImageViews[RE_VK_FRAMES_IN_FLIGHT] = {};

	VkRenderPass vk_hWorldRenderPass = VK_NULL_HANDLE;
	VkImage vk_ahWorldRenderImages[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkDeviceMemory vk_ahWorldRenderImageMemories[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkImageView vk_ahWorldRenderImageViews[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkFramebuffer vk_ahWorldFramebuffers[RE_VK_FRAMES_IN_FLIGHT] = {};

	VkFence vk_ahRenderFences[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkSemaphore *vk_pahSwapchainSemaphores = nullptr, vk_ahRenderSemaphores[RE_VK_RENDER_SEMAPHORE_COUNT] = {};

	VkCommandBuffer vk_ahRenderCommandBuffers[RE_VK_FRAMES_IN_FLIGHT] = {};

	uint32_t u32SwapchainRenderImageIndex = 0U;
	uint8_t u8CurrentFrameInFlightIndex = 0U;

	static bool create_world_render_images() {
		switch (eScreenPercentage) {
			case SCREEN_PERCENTAGE_SWAPCHAIN_EQUAL:
				vk_worldRenderImageExtent.width = vk_swapchainResolution.width;
				vk_worldRenderImageExtent.height = vk_swapchainResolution.height;
				break;
			case SCREEN_PERCENTAGE_SWAPCHAIN_PERCENTAGE:
				vk_worldRenderImageExtent.width = static_cast<uint32_t>(std::round(vk_swapchainResolution.width * fScreenPercentage));
				vk_worldRenderImageExtent.height = static_cast<uint32_t>(std::round(vk_swapchainResolution.height * fScreenPercentage));
				break;
			default:
				break;
		}
		const VkExtent3D vk_worldRenderImageExtent3D = {
			.width = vk_worldRenderImageExtent.width,
			.height = vk_worldRenderImageExtent.height,
			.depth = 1U
		};
		constexpr uint32_t u32WorldRenderImageQueueCount = 2U, au32WorldRenderImageQueues[u32WorldRenderImageQueueCount] = {RE_VK_QUEUE_GRAPHICS_INDEX, RE_VK_QUEUE_TRANSFER_INDEX};
		uint8_t u8WorldRenderImageCollectionCreateIndex = 0U;
		while (u8WorldRenderImageCollectionCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			if (create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, vk_worldRenderImageExtent3D, 1U, 1U, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, u32WorldRenderImageQueueCount, au32WorldRenderImageQueues, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], &vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex])) {
				if (CATCH_SIGNAL_AND_RETURN(create_vulkan_image_view(vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0U, 1U, 0U, 1U, &vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex]), bool)) {
					constexpr uint32_t u32DepthStencilImageQueueCount = 1U, au32DepthStencilImageQueues[u32DepthStencilImageQueueCount] = {RE_VK_QUEUE_GRAPHICS_INDEX};
					if (create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eDepthStencilBufferFormat, vk_worldRenderImageExtent3D, 1U, 1U, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, u32DepthStencilImageQueueCount, au32DepthStencilImageQueues, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionCreateIndex], &vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionCreateIndex])) {
						if (create_vulkan_image_view(vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eDepthStencilBufferFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0U, 1U, 0U, 1U, &vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionCreateIndex])) {
							VkCommandBuffer vk_hDepthStencilBufferTransitCommandBuffer;
							if (transit_image(RE_VK_QUEUE_GRAPHICS_INDEX, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, VK_ACCESS_NONE, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionCreateIndex], VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0U, 1U, 0U, 1U, &vk_hDepthStencilBufferTransitCommandBuffer, nullptr)) {
								constexpr uint32_t u32FramebufferImageViewCount = 2U;
								const VkImageView vk_ahFramebufferImageViews[u32FramebufferImageViewCount] = {vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex], vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionCreateIndex]};
								if (CATCH_SIGNAL_AND_RETURN(create_vulkan_framebuffer(0, vk_hWorldRenderPass, u32FramebufferImageViewCount, vk_ahFramebufferImageViews, vk_worldRenderImageExtent.width, vk_worldRenderImageExtent.height, 1U, &vk_ahWorldFramebuffers[u8WorldRenderImageCollectionCreateIndex]), bool)) {
									u8WorldRenderImageCollectionCreateIndex++;
									continue;
								} else
									RE_FATAL_ERROR(append_to_string("Failed creating Vulkan framebuffer at index ", u8WorldRenderImageCollectionCreateIndex, " for world rendering"));
							} else
								RE_FATAL_ERROR(append_to_string("Failed transiting depth & stencil buffer at index ", u8WorldRenderImageCollectionCreateIndex, " to its required image layout"));
							vkDestroyImageView(vk_hDevice, vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionCreateIndex], nullptr);
						} else
							RE_FATAL_ERROR(append_to_string("Failed to create Vulkan image view for the depth & stencil buffer at index ", u8WorldRenderImageCollectionCreateIndex));
						vkFreeMemory(vk_hDevice, vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionCreateIndex], nullptr);
						vkDestroyImage(vk_hDevice, vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionCreateIndex], nullptr);
						
					} else
						RE_FATAL_ERROR(append_to_string("Failed to create Vulkan image for depth & stencil buffer usage at index ", u8WorldRenderImageCollectionCreateIndex));
					vkDestroyImageView(vk_hDevice, vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex], nullptr);
				} else
					RE_FATAL_ERROR(append_to_string("Failed creating Vulkan GPU-side image view at index ", u8WorldRenderImageCollectionCreateIndex, " for world rendering"));
				vkFreeMemory(vk_hDevice, vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex], nullptr);
				vkDestroyImage(vk_hDevice, vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], nullptr);
			} else
				RE_FATAL_ERROR(append_to_string("Failed creating Vulkan GPU-side image at index ", u8WorldRenderImageCollectionCreateIndex, " for world rendering"));
			vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
			vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
			vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
			vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
			break;
		}
		if (u8WorldRenderImageCollectionCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		for (uint8_t u8WorldRenderImageCollectionDeleteIndex = 0U; u8WorldRenderImageCollectionDeleteIndex < u8WorldRenderImageCollectionCreateIndex; u8WorldRenderImageCollectionDeleteIndex++) {
			vkDestroyFramebuffer(vk_hDevice, vk_ahWorldFramebuffers[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkDestroyImageView(vk_hDevice, vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkDestroyImageView(vk_hDevice, vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vk_ahWorldFramebuffers[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
		}
		return false;
	}

	static void destroy_world_render_images() {
		for (uint32_t u8WorldRenderImageCollectionDeleteIndex = 0U; u8WorldRenderImageCollectionDeleteIndex < RE_VK_FRAMES_IN_FLIGHT; u8WorldRenderImageCollectionDeleteIndex++) {
			vkDestroyFramebuffer(vk_hDevice, vk_ahWorldFramebuffers[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkDestroyImageView(vk_hDevice, vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkDestroyImageView(vk_hDevice, vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
			vk_ahWorldFramebuffers[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
		}
	}

	static void recreate_world_render_images() {
		if (vk_hDevice != VK_NULL_HANDLE) {
			WAIT_FOR_IDLE_VULKAN_DEVICE();
			CATCH_SIGNAL(create_world_render_images());
			CATCH_SIGNAL(destroy_world_render_images());
		}
	}

	static void calculate_world_render_area_with_camera() {
		DEFINE_SIGNAL_GUARD(sigCamAccess);
		const float a2fCamScale[2] = {
			std::abs(pActiveCamera->view[0]),
			std::abs(pActiveCamera->view[1])
		};
		const float fCameraScale = std::abs(std::min(vk_worldRenderImageExtent.width / a2fCamScale[0], vk_worldRenderImageExtent.height / a2fCamScale[1]));
		vk_cameraViewport.width = a2fCamScale[0] * fCameraScale;
		vk_cameraViewport.height = a2fCamScale[1] * fCameraScale;
		vk_cameraViewport.x = (vk_worldRenderImageExtent.width - vk_cameraViewport.width) / 2.0f;
		vk_cameraViewport.y = (vk_worldRenderImageExtent.height - vk_cameraViewport.height) / 2.0f;
		vk_cameraScissor.offset.x = static_cast<int32_t>(std::round(vk_cameraViewport.x));
		vk_cameraScissor.offset.y = static_cast<int32_t>(std::round(vk_cameraViewport.y));
		vk_cameraScissor.extent.width = static_cast<int32_t>(std::round(vk_cameraViewport.width));
		vk_cameraScissor.extent.height = static_cast<int32_t>(std::round(vk_cameraViewport.height));
	}

	static void update_camera_descriptor_set(const uint8_t u8FrameInFlightIndex) {
		const VkDescriptorBufferInfo vk_cameraUniformBufferToDescriptorSet = {
			.buffer = vk_ahWorldCameraUniformBuffers[u8FrameInFlightIndex],
			.offset = 0UL,
			.range = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES
		};
		const VkWriteDescriptorSet vk_writeToCameraDescriptorSet = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = vk_ahWorldCameraDescriptorSets[u8FrameInFlightIndex],
			.dstBinding = 0U,
			.dstArrayElement = 0U,
			.descriptorCount = 1U,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &vk_cameraUniformBufferToDescriptorSet
		};
		vkUpdateDescriptorSets(vk_hDevice, 1U, &vk_writeToCameraDescriptorSet, 0U, nullptr);
	}

	static void update_camera_uniform_buffer() {
		DEFINE_SIGNAL_GUARD(sigCamAccess);
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 0)] = -pActiveCamera->position[0];
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 1)] = -pActiveCamera->position[1];
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 2)] = -pActiveCamera->position[2];
		const float a2fCamScale[2] = {
			std::abs(pActiveCamera->view[0]) / 2.0f,
			std::abs(pActiveCamera->view[1]) / 2.0f
		};
		const float fLeft = pActiveCamera->position[0] - a2fCamScale[0],
			fRight = pActiveCamera->position[0] + a2fCamScale[0],
			fTop = pActiveCamera->position[1] + a2fCamScale[1],
			fBottom = pActiveCamera->position[1] - a2fCamScale[1],
			fNear = 1.0f,
			fFar = -1.0f;
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(0, 0)] = 2.0f / (fRight - fLeft) * sign(pActiveCamera->view[0]);
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(1, 1)] = 2.0f / (fTop - fBottom) * sign(pActiveCamera->view[1]);
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(2, 2)] = 1.0f / (fNear - fFar);
		/*apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 0)] = -(fRight + fLeft) / (fRight - fLeft);
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 1)] = -(fTop + fBottom) / (fTop - fBottom);
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 2)] = fNear / (fFar - fNear);*/
		CATCH_SIGNAL(update_camera_descriptor_set(u8CurrentFrameInFlightIndex));
	}

	bool init_renderer() {
		constexpr uint32_t u32StagingIndexBufferQueueCount = 1U, au32StagingIndexBufferQueues[u32StagingIndexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX};
		Vulkan_Buffer stagingIndexBuffer(RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingIndexBufferQueueCount, au32StagingIndexBufferQueues, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (stagingIndexBuffer.is_valid()) {
			std::thread stagingIndexBufferDataInit([&]() {
				REindex_t *pau16RectIndices;
				stagingIndexBuffer.map_memory((void**) &pau16RectIndices);
				for (uint16_t u16RectNumber = 0U; u16RectNumber < RE_VK_RENDERABLE_RECTANGLES_COUNT; u16RectNumber++) {
					pau16RectIndices[u16RectNumber * 6U + 0U] = u16RectNumber * 4U + 0U;
					pau16RectIndices[u16RectNumber * 6U + 1U] = u16RectNumber * 4U + 1U;
					pau16RectIndices[u16RectNumber * 6U + 2U] = u16RectNumber * 4U + 2U;
					pau16RectIndices[u16RectNumber * 6U + 3U] = u16RectNumber * 4U + 2U;
					pau16RectIndices[u16RectNumber * 6U + 4U] = u16RectNumber * 4U + 3U;
					pau16RectIndices[u16RectNumber * 6U + 5U] = u16RectNumber * 4U + 0U;
				}
				stagingIndexBuffer.unmap_memory();
			});
			constexpr uint32_t u32IndexBufferQueueCount = 2U, au32IndexBufferQueues[u32IndexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX, RE_VK_QUEUE_GRAPHICS_INDEX};
			if (CATCH_SIGNAL_AND_RETURN(create_vulkan_buffer(RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, u32IndexBufferQueueCount, au32IndexBufferQueues, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRectIndexBuffer, &vk_hRectIndexBufferMemory), bool)) {
				Vulkan_CommandBuffer indexBufferDataTransferCommandBuffer(vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
				if (indexBufferDataTransferCommandBuffer.is_valid()) {
					if (indexBufferDataTransferCommandBuffer.begin_recording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
						constexpr VkBufferCopy vk_rectIndexBufferCopy = {
							.size = RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES
						};
						vkCmdCopyBuffer(indexBufferDataTransferCommandBuffer, stagingIndexBuffer, vk_hRectIndexBuffer, 1U, &vk_rectIndexBufferCopy);
						if (indexBufferDataTransferCommandBuffer.end_recording()) {
							Vulkan_Fence rectIndexBufferDataTransferFence;
							if (rectIndexBufferDataTransferFence.is_valid()) {
								constexpr uint32_t u32DescriptorSetLayoutBindingCount = 1U;
								constexpr VkDescriptorSetLayoutBinding vk_aDescriptorSetLayoutBindings[u32DescriptorSetLayoutBindingCount] = {
									{
										.binding = 0U,
										.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
										.descriptorCount = 1U,
										.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
										.pImmutableSamplers = nullptr
									}
								};
								if (CATCH_SIGNAL_AND_RETURN(create_descriptor_set_layout(u32DescriptorSetLayoutBindingCount, vk_aDescriptorSetLayoutBindings, 0, &vk_hWorldCameraDescriptorSetLayout), bool)) {
									constexpr uint32_t u32DescriptorPoolSizeCount = 1U;
									constexpr VkDescriptorPoolSize vk_aDescriptorPoolSizes[u32DescriptorPoolSizeCount] = {
										{
											.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
											.descriptorCount = RE_VK_FRAMES_IN_FLIGHT
										}
									};
									if (create_descriptor_pool(u32DescriptorPoolSizeCount, vk_aDescriptorPoolSizes, RE_VK_FRAMES_IN_FLIGHT, &vk_hWorldCameraDescriptorPool)) {
										VkDescriptorSetLayout vk_ahDescriptorSetLayouts[RE_VK_FRAMES_IN_FLIGHT];
										std::fill(std::begin(vk_ahDescriptorSetLayouts), std::end(vk_ahDescriptorSetLayouts), vk_hWorldCameraDescriptorSetLayout);
										if (alloc_descriptor_set(vk_hWorldCameraDescriptorPool, RE_VK_FRAMES_IN_FLIGHT, vk_ahDescriptorSetLayouts, vk_ahWorldCameraDescriptorSets)) {
											if (create_pipeline_layout(1U, &vk_hWorldCameraDescriptorSetLayout, 0U, nullptr, 0, &vk_hWorldBasicPipelineLayout)) {
												constexpr uint32_t u32WorldRenderPassAttachmentCount = 2U, u32WorldRenderPassSubpassCount = 1U, u32WorldRenderPassDependencyCount = 1U;
												const VkAttachmentDescription vk_worldRenderPassAttachments[u32WorldRenderPassAttachmentCount] = {
													{
														.format = vk_eSwapchainImageFormat,
														.samples = VK_SAMPLE_COUNT_1_BIT,
														.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
														.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
														.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
														.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
														.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
														.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
													}, {
														.format = vk_eDepthStencilBufferFormat,
														.samples = VK_SAMPLE_COUNT_1_BIT,
														.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
														.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
														.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
														.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
														.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
														.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
													}
												};
												const VkAttachmentReference vk_worldRenderPassColorAttachments[] = {
													{
														.attachment = 0U,
														.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
													}
												};
												const VkAttachmentReference vk_worldRenderPassDepthStencilAttachments[] = {
													{
														.attachment = 1U,
														.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
													}
												};
												const VkSubpassDescription vk_worldRenderSubpasses[u32WorldRenderPassSubpassCount] = {
													{
														.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
														.inputAttachmentCount = 0U,
														.pInputAttachments = nullptr,
														.colorAttachmentCount = 1U,
														.pColorAttachments = &vk_worldRenderPassColorAttachments[0],
														.pDepthStencilAttachment = &vk_worldRenderPassDepthStencilAttachments[0]
													}
												};
												const VkSubpassDependency vk_worldRenderDependencies[u32WorldRenderPassDependencyCount] = {
													{
														.srcSubpass = VK_SUBPASS_EXTERNAL,
														.dstSubpass = RE_VK_OPAQUE_GAME_OBJECT_SUPBASS,
														.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
														.srcAccessMask = VK_ACCESS_NONE,
														.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
														.dependencyFlags = 0
													}
												};
												const VkRenderPassCreateInfo vk_renderPassCreateInfo = {
													.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
													.attachmentCount = u32WorldRenderPassAttachmentCount,
													.pAttachments = vk_worldRenderPassAttachments,
													.subpassCount = u32WorldRenderPassSubpassCount,
													.pSubpasses = vk_worldRenderSubpasses,
													.dependencyCount = u32WorldRenderPassDependencyCount,
													.pDependencies = vk_worldRenderDependencies
												};
												if (vkCreateRenderPass(vk_hDevice, &vk_renderPassCreateInfo, nullptr, &vk_hWorldRenderPass) == VK_SUCCESS) {
													constexpr uint32_t u32CameraUniformBufferQueueCount = 1U, au32CameraUniformBufferQueues[u32CameraUniformBufferQueueCount] = {RE_VK_QUEUE_GRAPHICS_INDEX};
													const VkFenceCreateInfo vk_renderFenceCreateInfo = {
														.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
														.flags = VK_FENCE_CREATE_SIGNALED_BIT
													};
													uint8_t u8FrameInFlightCreateIndex = 0U;
													while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
														if (!create_vulkan_buffer(RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, u32CameraUniformBufferQueueCount, au32CameraUniformBufferQueues, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk_ahWorldCameraUniformBuffers[u8FrameInFlightCreateIndex], &vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightCreateIndex])) {
															RE_FATAL_ERROR(append_to_string("Failed to create Vulkan uniform buffer at index ", u8FrameInFlightCreateIndex));
															break;
														}
														vkMapMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightCreateIndex], 0UL, RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES, 0, (void**) &apafCameraUniformData[u8FrameInFlightCreateIndex]);
														std::fill(apafCameraUniformData[u8FrameInFlightCreateIndex], apafCameraUniformData[u8FrameInFlightCreateIndex] + RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES, 0.0f);
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(0, 0)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(1, 1)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(2, 2)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 3)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(0, 0)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(1, 1)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(2, 2)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 3)] = 1.0f;
														CATCH_SIGNAL(update_camera_descriptor_set(u8FrameInFlightCreateIndex));
														if (vkCreateFence(vk_hDevice, &vk_renderFenceCreateInfo, nullptr, &vk_ahRenderFences[u8FrameInFlightCreateIndex]) == VK_SUCCESS) {
															u8FrameInFlightCreateIndex++;
															continue;
														} else
															RE_FATAL_ERROR(append_to_string("Failed to create Vulkan render fence at index ", u8FrameInFlightCreateIndex));
														vkUnmapMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightCreateIndex]);
														vkFreeMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightCreateIndex], nullptr);
														vkDestroyBuffer(vk_hDevice, vk_ahWorldCameraUniformBuffers[u8FrameInFlightCreateIndex], nullptr);
														vk_ahWorldDepthStencilImageViews[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
														vk_ahWorldDepthStencilImageMemories[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
														vk_ahWorldDepthStencilImages[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
														apafCameraUniformData[u8FrameInFlightCreateIndex] = nullptr;
														vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
														vk_ahWorldCameraUniformBuffers[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
														break;
													}
													if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT) {
														const VkSemaphoreCreateInfo vk_renderSemaphoreCreateInfo = {
															.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
														};
														uint16_t u16RenderSemaphoreCreateIndex = 0U;
														while (u16RenderSemaphoreCreateIndex < RE_VK_RENDER_SEMAPHORE_COUNT) {
															if (vkCreateSemaphore(vk_hDevice, &vk_renderSemaphoreCreateInfo, nullptr, &vk_ahRenderSemaphores[u16RenderSemaphoreCreateIndex]) != VK_SUCCESS) {
																RE_FATAL_ERROR(append_to_string("Failed creating Vulkan semaphore at index ", u16RenderSemaphoreCreateIndex));
																break;
															}
															u16RenderSemaphoreCreateIndex++;
														}
														if (u16RenderSemaphoreCreateIndex == RE_VK_RENDER_SEMAPHORE_COUNT && CATCH_SIGNAL_AND_RETURN(alloc_vulkan_command_buffers(vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahRenderCommandBuffers), bool)) {
															const VkSubmitInfo vk_rectIndexBufferTransferSubmitInfo = {
																.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
																.commandBufferCount = 1U,
																.pCommandBuffers = indexBufferDataTransferCommandBuffer.get_command_buffer_ptr()
															};
															CATCH_SIGNAL(stagingIndexBufferDataInit.join());
															if (vkQueueSubmit(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 1U, &vk_rectIndexBufferTransferSubmitInfo, rectIndexBufferDataTransferFence) == VK_SUCCESS) {
																if (eScreenPercentage != SCREEN_PERCENTAGE_CONST_IMG_SIZE || CATCH_SIGNAL_AND_RETURN(create_world_render_images(), bool)) {
																	if (CATCH_SIGNAL_AND_RETURN(swapchain_created_renderer(), bool)) {
																		if (CATCH_SIGNAL_AND_RETURN(init_opaque_gameobject_renderer(), bool)) {
																			if (CATCH_SIGNAL_AND_RETURN(init_transparent_game_object_renderer(), bool)) {
																				rectIndexBufferDataTransferFence.wait_for();
																				return true;
																			}
																			CATCH_SIGNAL(destroy_opaque_gameobject_renderer());
																		}
																		CATCH_SIGNAL(swapchain_destroyed_renderer());
																	}
																	if (eScreenPercentage == SCREEN_PERCENTAGE_CONST_IMG_SIZE)
																		CATCH_SIGNAL(destroy_world_render_images());
																}
																rectIndexBufferDataTransferFence.wait_for();
															} else
																RE_FATAL_ERROR("Failed to submit task to the transfer queue for copying data to the Vulkan rectangle index buffer on the GPU");
															vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahRenderCommandBuffers);
															std::fill(std::begin(vk_ahRenderCommandBuffers), std::end(vk_ahRenderCommandBuffers), VK_NULL_HANDLE);
														}
														for (uint16_t u16RenderSemaphoreDeleteIndex = 0U; u16RenderSemaphoreDeleteIndex < u16RenderSemaphoreCreateIndex; u16RenderSemaphoreDeleteIndex++) {
															vkDestroySemaphore(vk_hDevice, vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex], nullptr);
															vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex] = VK_NULL_HANDLE;
														}
													}
													for (uint8_t u8FrameInFlightDeleteIndex = 0U; u8FrameInFlightDeleteIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDeleteIndex++) {
														vkUnmapMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex]);
														vkFreeMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex], nullptr);
														vkDestroyBuffer(vk_hDevice, vk_ahWorldCameraUniformBuffers[u8FrameInFlightDeleteIndex], nullptr);
														vkDestroyFence(vk_hDevice, vk_ahRenderFences[u8FrameInFlightDeleteIndex], nullptr);
														vk_ahWorldDepthStencilImageViews[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
														vk_ahWorldDepthStencilImageMemories[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
														vk_ahWorldDepthStencilImages[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
														apafCameraUniformData[u8FrameInFlightDeleteIndex] = nullptr;
														vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
														vk_ahWorldCameraUniformBuffers[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
														vk_ahRenderFences[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
													}
													vkDestroyRenderPass(vk_hDevice, vk_hWorldRenderPass, nullptr);
												} else
													RE_FATAL_ERROR("Failed to create Vulkan render pass");
												vkDestroyPipelineLayout(vk_hDevice, vk_hWorldBasicPipelineLayout, nullptr);
											} else
												RE_FATAL_ERROR("Failed to create basic Vulkan pipeline layout for rendering the world");
											vkFreeDescriptorSets(vk_hDevice, vk_hWorldCameraDescriptorPool, RE_VK_FRAMES_IN_FLIGHT, vk_ahWorldCameraDescriptorSets);
											std::fill(std::begin(vk_ahWorldCameraDescriptorSets), std::end(vk_ahWorldCameraDescriptorSets), VK_NULL_HANDLE);
										} else
											RE_FATAL_ERROR("Failed allocating Vulkan descriptor set for camera uniform buffer");
										vkDestroyDescriptorPool(vk_hDevice, vk_hWorldCameraDescriptorPool, nullptr);
									} else
										RE_FATAL_ERROR("Failed to create Vulkan descriptor pool for camera uniform buffer");
									vkDestroyDescriptorSetLayout(vk_hDevice, vk_hWorldCameraDescriptorSetLayout, nullptr);
								} else
									RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout for camera uniform buffer");
							} else
								RE_FATAL_ERROR("Failed to create Vulkan fence to wait on finishing transfer task for rectangle index buffer data");
						} else
							RE_FATAL_ERROR("Failed to finish recording Vulkan rectangle index buffer");
					} else
						RE_FATAL_ERROR("Failed to begin recording Vulkan rectangle index buffer");
				} else
					RE_FATAL_ERROR("Failed to allocate Vulkan command buffer to transfer data for the rectangle index buffer to the GPU");
				vkFreeMemory(vk_hDevice, vk_hRectIndexBufferMemory, nullptr);
				vkDestroyBuffer(vk_hDevice, vk_hRectIndexBuffer, nullptr);
			} else
				RE_FATAL_ERROR("Failed to create Vulkan rectangle index buffer on the GPU");
			if (stagingIndexBufferDataInit.joinable())
				stagingIndexBufferDataInit.join();
		} else
			RE_FATAL_ERROR("Failed to create Vulkan staging rectangle index buffer");
		vk_hWorldRenderPass = VK_NULL_HANDLE;
		vk_hWorldBasicPipelineLayout = VK_NULL_HANDLE;
		vk_hWorldCameraDescriptorPool = VK_NULL_HANDLE;
		vk_hWorldCameraDescriptorSetLayout = VK_NULL_HANDLE;
		vk_hRectIndexBufferMemory = VK_NULL_HANDLE;
		vk_hRectIndexBuffer = VK_NULL_HANDLE;
		return false;
	}
	
	void destroy_renderer() {
		CATCH_SIGNAL(destroy_transparent_game_object_renderer());
		CATCH_SIGNAL(destroy_opaque_gameobject_renderer());
		vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahRenderCommandBuffers);
		for (uint16_t u16RenderSemaphoreDeleteIndex = 0U; u16RenderSemaphoreDeleteIndex < RE_VK_RENDER_SEMAPHORE_COUNT; u16RenderSemaphoreDeleteIndex++) {
			vkDestroySemaphore(vk_hDevice, vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex], nullptr);
			vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex] = VK_NULL_HANDLE;
		}
		for (uint8_t u8FrameInFlightDeleteIndex = 0U; u8FrameInFlightDeleteIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightDeleteIndex++) {
			vkDestroyImageView(vk_hDevice, vk_ahWorldDepthStencilImageViews[u8FrameInFlightDeleteIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahWorldDepthStencilImageMemories[u8FrameInFlightDeleteIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahWorldDepthStencilImages[u8FrameInFlightDeleteIndex], nullptr);
			vkUnmapMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex]);
			vkFreeMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex], nullptr);
			vkDestroyBuffer(vk_hDevice, vk_ahWorldCameraUniformBuffers[u8FrameInFlightDeleteIndex], nullptr);
			vkDestroyFence(vk_hDevice, vk_ahRenderFences[u8FrameInFlightDeleteIndex], nullptr);
			vk_ahWorldDepthStencilImageViews[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImageMemories[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImages[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			apafCameraUniformData[u8FrameInFlightDeleteIndex] = nullptr;
			vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldCameraUniformBuffers[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vk_ahRenderFences[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
		}
		if (eScreenPercentage == SCREEN_PERCENTAGE_CONST_IMG_SIZE)
			CATCH_SIGNAL(destroy_world_render_images());
		vkDestroyRenderPass(vk_hDevice, vk_hWorldRenderPass, nullptr);
		vkDestroyPipelineLayout(vk_hDevice, vk_hWorldBasicPipelineLayout, nullptr);
		vkDestroyDescriptorPool(vk_hDevice, vk_hWorldCameraDescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hWorldCameraDescriptorSetLayout, nullptr);
		vkFreeMemory(vk_hDevice, vk_hRectIndexBufferMemory, nullptr);
		vkDestroyBuffer(vk_hDevice, vk_hRectIndexBuffer, nullptr);
		std::fill(std::begin(vk_ahRenderCommandBuffers), std::end(vk_ahRenderCommandBuffers), VK_NULL_HANDLE);
		vk_hWorldRenderPass = VK_NULL_HANDLE;
		vk_hWorldBasicPipelineLayout = VK_NULL_HANDLE;
		vk_hWorldCameraDescriptorSetLayout = VK_NULL_HANDLE;
		vk_hWorldCameraDescriptorPool = VK_NULL_HANDLE;
		vk_hRectIndexBufferMemory = VK_NULL_HANDLE;
		vk_hRectIndexBuffer = VK_NULL_HANDLE;
	}

	void render() {
		if (pActiveCamera)
			CATCH_SIGNAL(pActiveCamera->update());
		vkWaitForFences(vk_hDevice, 1U, &vk_ahRenderFences[u8CurrentFrameInFlightIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
		uint32_t u32NextSwapchainImageIndex;
		const VkResult vk_eSwapchainImageAcquireResult = vkAcquireNextImageKHR(vk_hDevice, vk_hSwapchain, std::numeric_limits<uint64_t>::max(), vk_pahSwapchainSemaphores[u32SwapchainRenderImageIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE], VK_NULL_HANDLE, &u32NextSwapchainImageIndex);
		switch (vk_eSwapchainImageAcquireResult) {
			case VK_SUBOPTIMAL_KHR:
			case VK_ERROR_OUT_OF_DATE_KHR:
				mark_swapchain_dirty();
				return;
			default:
				if (!check_vulkan_result(vk_eSwapchainImageAcquireResult, __FILE__, __func__, __LINE__)) {
					RE_FATAL_ERROR("Failed to get next presentable swapchain image index");
					return;
				}
				break;
		}
		if (pActiveCamera) {
			if (pActiveCamera->view[0] && pActiveCamera->view[1]) {
				CATCH_SIGNAL(calculate_world_render_area_with_camera());
				CATCH_SIGNAL(update_camera_uniform_buffer());
			} else {
				RE_WARNING(append_to_string("The camera's view shouldn't contain any zeros: ", pActiveCamera->view, "; using old camera data instead"));
				const VkCopyDescriptorSet vk_copyCameraDescriptorSet = {
					.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
					.srcSet = vk_ahWorldCameraDescriptorSets[u8CurrentFrameInFlightIndex == 0U ? (RE_VK_FRAMES_IN_FLIGHT - 1U) : (u8CurrentFrameInFlightIndex - 1U)],
					.srcBinding = 0U,
					.srcArrayElement = 0U,
					.dstSet = vk_ahWorldCameraDescriptorSets[u8CurrentFrameInFlightIndex],
					.dstBinding = 0U,
					.dstArrayElement = 0U,
					.descriptorCount = 1U
				};
				vkUpdateDescriptorSets(vk_hDevice, 0U, nullptr, 1U, &vk_copyCameraDescriptorSet);
			}
		}
		if (!CATCH_SIGNAL_AND_RETURN(render_opaque_gameobjects() && render_transparent_game_object(), bool))
			return;
		if (!begin_recording_vulkan_command_buffer(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
			RE_FATAL_ERROR("Failed beginning to record Vulkan command buffer for rendering everything");
			return;
		}
		constexpr uint32_t u32ClearValueCount = 2U;
		const VkClearValue vk_aClearValues[u32ClearValueCount] = {
			{
				.color = {
					.float32 = {
						0.0f,
						0.0f,
						0.0f,
						1.0f
					}
				}
			}, {
				.depthStencil = {
					.depth = 0.0f,
					.stencil = 0U
				}
			}
		};
		const VkRenderPassBeginInfo vk_renderPassInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = vk_hWorldRenderPass,
			.framebuffer = vk_ahWorldFramebuffers[u8CurrentFrameInFlightIndex],
			.renderArea = VkRect2D{
				VkOffset2D{0, 0}, 
				vk_worldRenderImageExtent
			},
			.clearValueCount = u32ClearValueCount,
			.pClearValues = vk_aClearValues
		};
		vkCmdBeginRenderPass(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], &vk_renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		vkCmdExecuteCommands(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_ahOpaqueGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex]);
		vkCmdExecuteCommands(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_ahTransparentGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex]);
		vkCmdEndRenderPass(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex]);
		CATCH_SIGNAL(vk_cmd_transit_image(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, vk_pahSwapchainImages[u32NextSwapchainImageIndex], VK_IMAGE_ASPECT_COLOR_BIT));
		const VkImageBlit vk_worldRenderImageBlit = {
			.srcSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0U,
				.baseArrayLayer = 0U,
				.layerCount = 1U
			},
			.srcOffsets = {
				{
					.x = 0,
					.y = static_cast<int32_t>(vk_worldRenderImageExtent.height),
					.z = 0
				}, {
					.x = static_cast<int32_t>(vk_worldRenderImageExtent.width),
					.y = 0,
					.z = 1
				}
			},
			.dstSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0U,
				.baseArrayLayer = 0U,
				.layerCount = 1U
			},
			.dstOffsets = {
				{
					.x = 0,
					.y = 0,
					.z = 0
				}, {
					.x = static_cast<int32_t>(vk_swapchainResolution.width),
					.y = static_cast<int32_t>(vk_swapchainResolution.height),
					.z = 1
				}
			}
		};
		vkCmdBlitImage(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], vk_ahWorldRenderImages[u8CurrentFrameInFlightIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vk_pahSwapchainImages[u32NextSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1U, &vk_worldRenderImageBlit, VK_FILTER_LINEAR);
		CATCH_SIGNAL(vk_cmd_transit_image(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, vk_pahSwapchainImages[u32NextSwapchainImageIndex], VK_IMAGE_ASPECT_COLOR_BIT));
		if (vkEndCommandBuffer(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex]) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed finishing to record Vulkan command buffer for rendering everything");
			return;
		}
		vkResetFences(vk_hDevice, 1U, &vk_ahRenderFences[u8CurrentFrameInFlightIndex]);
		constexpr uint32_t u32RenderSemaphoresToWaitForCount = 3U;
		constexpr VkPipelineStageFlags vk_aePipelinesToWaitForBeforeRendering[u32RenderSemaphoresToWaitForCount] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
			VK_PIPELINE_STAGE_TRANSFER_BIT, 
			VK_PIPELINE_STAGE_TRANSFER_BIT
		};
		const VkSemaphore vk_ahWaitForSemaphoresBeforeRendering[u32RenderSemaphoresToWaitForCount] = {
			vk_pahSwapchainSemaphores[u32SwapchainRenderImageIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE], 
			vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT + RE_VK_OPAQUE_GAME_OBJECT_SEMAPHORE_INDEX], 
			vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT + RE_VK_TRANSPARENT_GAME_OBJECT_SEMAPHORE_INDEX]
		};
		if (!CATCH_SIGNAL_AND_RETURN(submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_GRAPHICS_INDEX], u32RenderSemaphoresToWaitForCount, vk_ahWaitForSemaphoresBeforeRendering, vk_aePipelinesToWaitForBeforeRendering, 1U, &vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_pahSwapchainSemaphores[u32SwapchainRenderImageIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1U], vk_ahRenderFences[u8CurrentFrameInFlightIndex]), bool)) {
			RE_FATAL_ERROR("Failed submitting the task for rendering everything to the Vulkan GPU");
			return;
		}
		const VkPresentInfoKHR vk_presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1U,
			.pWaitSemaphores = &vk_pahSwapchainSemaphores[u32SwapchainRenderImageIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1U],
			.swapchainCount = 1U,
			.pSwapchains = &vk_hSwapchain,
			.pImageIndices = &u32NextSwapchainImageIndex
		};
		const VkResult vk_eSwapchainPresentResult = vkQueuePresentKHR(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_PRESENT_INDEX], &vk_presentInfo);
		switch (vk_eSwapchainPresentResult) {
			case VK_SUBOPTIMAL_KHR:
			case VK_ERROR_OUT_OF_DATE_KHR:
				mark_swapchain_dirty();
				break;
			default:
				if (!check_vulkan_result(vk_eSwapchainPresentResult, __FILE__, __func__, __LINE__))
					RE_FATAL_ERROR("Failed to submit presentable Vulkan swapchain image");
				break;
		}
		CATCH_SIGNAL(Window::pInstance->post_rendering_window_proc());
		u32SwapchainRenderImageIndex = (u32SwapchainRenderImageIndex + 1U) % u32SwapchainImageCount;
		u8CurrentFrameInFlightIndex = (u8CurrentFrameInFlightIndex + 1U) % RE_VK_FRAMES_IN_FLIGHT;
	}

	bool swapchain_created_renderer() {
		const VkSemaphoreCreateInfo vk_presentSemaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		vk_pahSwapchainSemaphores = new VkSemaphore[RE_VK_SWAPCHAIN_SEMAPHORE_COUNT];
		for (uint32_t u32PresentSemaphoreCreateIndex = 0U; u32PresentSemaphoreCreateIndex < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT; u32PresentSemaphoreCreateIndex++)
			if (vkCreateSemaphore(vk_hDevice, &vk_presentSemaphoreCreateInfo, nullptr, &vk_pahSwapchainSemaphores[u32PresentSemaphoreCreateIndex]) != VK_SUCCESS) {
				RE_FATAL_ERROR(append_to_string("Failed creating Vulkan semaphore at index ", u32PresentSemaphoreCreateIndex, " for synchronizing rendering procedures"));
				for (uint32_t u8WorldRenderImageCollectionDeleteIndex = 0U; u8WorldRenderImageCollectionDeleteIndex < u32PresentSemaphoreCreateIndex; u8WorldRenderImageCollectionDeleteIndex++)
					vkDestroySemaphore(vk_hDevice, vk_pahSwapchainSemaphores[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				DELETE_ARRAY_SAFELY(vk_pahSwapchainSemaphores);
				return false;
			}
		if (eScreenPercentage != SCREEN_PERCENTAGE_CONST_IMG_SIZE)
			CATCH_SIGNAL(create_world_render_images());
		if (!pActiveCamera) {
			vk_cameraViewport.width = vk_worldRenderImageExtent.width;
			vk_cameraViewport.height = vk_worldRenderImageExtent.height;
			vk_cameraViewport.x = 0.0f;
			vk_cameraViewport.y = 0.0f;
			vk_cameraScissor.offset.x = 0;
			vk_cameraScissor.offset.y = 0;
			vk_cameraScissor.extent.width = vk_worldRenderImageExtent.width;
			vk_cameraScissor.extent.height = vk_worldRenderImageExtent.height;
		}
		return true;
	}

	void swapchain_destroyed_renderer() {
		if (!vk_pahSwapchainSemaphores)
			return;
		for (uint32_t u8WorldRenderImageCollectionDeleteIndex = 0U; u8WorldRenderImageCollectionDeleteIndex < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT; u8WorldRenderImageCollectionDeleteIndex++)
			vkDestroySemaphore(vk_hDevice, vk_pahSwapchainSemaphores[u8WorldRenderImageCollectionDeleteIndex], nullptr);
		DELETE_ARRAY_SAFELY(vk_pahSwapchainSemaphores);
		if (eScreenPercentage != SCREEN_PERCENTAGE_CONST_IMG_SIZE)
			CATCH_SIGNAL(destroy_world_render_images());
	}

	void attach_camera(Camera *const pCamera) {
		pActiveCamera = pCamera;
		if (!pActiveCamera) {
			vk_cameraViewport.width = vk_worldRenderImageExtent.width;
			vk_cameraViewport.height = vk_worldRenderImageExtent.height;
			vk_cameraViewport.x = 0.0f;
			vk_cameraViewport.y = 0.0f;
			vk_cameraScissor.offset.x = 0;
			vk_cameraScissor.offset.y = 0;
			vk_cameraScissor.extent.width = vk_worldRenderImageExtent.width;
			vk_cameraScissor.extent.height = vk_worldRenderImageExtent.height;
		}
	}

	void set_const_screen_size(const uint32_t u32Width, const uint32_t u32Height) {
		eScreenPercentage = SCREEN_PERCENTAGE_CONST_IMG_SIZE;
		vk_worldRenderImageExtent.width = u32Width;
		vk_worldRenderImageExtent.height = u32Height;
		CATCH_SIGNAL(recreate_world_render_images());
	}

	void set_screen_percentage(const float fPercentage) {
		eScreenPercentage = fPercentage == 1.0f ? SCREEN_PERCENTAGE_SWAPCHAIN_EQUAL : SCREEN_PERCENTAGE_SWAPCHAIN_PERCENTAGE;
		fScreenPercentage = fPercentage;
		CATCH_SIGNAL(recreate_world_render_images());
	}

	void reset_screen_size() {
		eScreenPercentage = SCREEN_PERCENTAGE_SWAPCHAIN_EQUAL;
		CATCH_SIGNAL(recreate_world_render_images());
	}

}
