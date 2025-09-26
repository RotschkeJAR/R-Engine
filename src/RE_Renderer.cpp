#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_RenderElement.hpp"
#include "RE_RenderPipelines.hpp"

#include <thread>

namespace RE {

	typedef uint16_t REindex_t;
#define RE_VK_RECT_INDEX_BUFFER_SIZE (RE_VK_RENDERABLE_RECTANGLES_COUNT * 6)
#define RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES (RE_VK_RECT_INDEX_BUFFER_SIZE * sizeof(REindex_t))

#define RE_VK_VIEW_MATRIX_SIZE (4 * 4)
#define RE_VK_VIEW_MATRIX_OFFSET 0
#define RE_VK_PROJECTION_MATRIX_SIZE (4 * 4)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE

#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE (RE_VK_VIEW_MATRIX_SIZE + RE_VK_PROJECTION_MATRIX_SIZE)
#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES (RE_VK_CAMERA_UNIFORM_BUFFER_SIZE * sizeof(float))

#define RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE 2
#define RE_VK_SWAPCHAIN_SEMAPHORE_COUNT (u32SwapchainImageCount * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE)

	const Camera *pActiveCamera = nullptr;

	ScreenPercentageSettings screenPercentageSettings(RE_SCREEN_PERCENTAGE_MODE_NORMAL);

	VkBool32 vk_bSampleShadingEnabled = VK_FALSE;
	float fSampleShadingRate = 0.2f;

	std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT> vk_ahRenderImages;
	std::array<VkDeviceMemory, RE_VK_FRAMES_IN_FLIGHT> vk_ahRenderImageMemories;
	std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> vk_ahRenderImageViews;

	std::array<VulkanTask, RE_VK_FRAMES_IN_FLIGHT> renderTasks;
	std::array<VkFence, renderTasks.size()> renderFences;
	std::vector<VkSemaphore> swapchainSemaphores;

	VkPipelineLayout vk_hWorldPipelineLayout;

	uint32_t u32NextSwapchainSemaphoreIndex = 0;
	uint8_t u8CurrentFrameInFlightIndex = 0;

	static bool create_render_images(const VkExtent3D &vk_rRenderImageExtent3D) {
		if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL && vk_eMsaaCount == VK_SAMPLE_COUNT_1_BIT)
			return true;
		size_t renderImageCreateIndex = 0;
		while (renderImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, vk_rRenderImageExtent3D, 1, 1, vk_eMsaaCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahRenderImages[renderImageCreateIndex], &vk_ahRenderImageMemories[renderImageCreateIndex]), bool)) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image_view(vk_ahRenderImages[renderImageCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, &vk_ahRenderImageViews[renderImageCreateIndex]), bool)) {
					renderImageCreateIndex++;
					continue;
				}
				vkFreeMemory(vk_hDevice, vk_ahRenderImageMemories[renderImageCreateIndex], nullptr);
				vkDestroyImage(vk_hDevice, vk_ahRenderImages[renderImageCreateIndex], nullptr);
			}
			break;
		}
		if (renderImageCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		for (size_t renderImageDestroyIndex = 0; renderImageDestroyIndex < renderImageCreateIndex; renderImageDestroyIndex++) {
			vkDestroyImageView(vk_hDevice, vk_ahRenderImageViews[renderImageDestroyIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahRenderImageMemories[renderImageDestroyIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahRenderImages[renderImageDestroyIndex], nullptr);
		}
		return false;
	}

	static void destroy_render_images() {
		if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL && vk_eMsaaCount == VK_SAMPLE_COUNT_1_BIT)
			return;
		for (size_t renderImageDestroyIndex = 0; renderImageDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; renderImageDestroyIndex++) {
			vkDestroyImageView(vk_hDevice, vk_ahRenderImageViews[renderImageDestroyIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahRenderImageMemories[renderImageDestroyIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahRenderImages[renderImageDestroyIndex], nullptr);
		}
	}

	uint8_t get_render_graphics_queue_logical_index() {
		return renderTasks[0].get_logical_queue_index_for_function(1);
	}

	bool init_renderer() {
		constexpr VkQueueFlagBits vk_aeQueuesUsedForRendering[] = {VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_TRANSFER_BIT};
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(renderTasks[0].init(sizeof(vk_aeQueuesUsedForRendering) / sizeof(vk_aeQueuesUsedForRendering[0]), vk_aeQueuesUsedForRendering, false), bool)) {
			size_t renderTaskCreateIndex = 1;
			while (renderTaskCreateIndex < renderTasks.size()) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(renderTasks[renderTaskCreateIndex].init(renderTasks[0]), bool)) {
					renderTaskCreateIndex++;
					continue;
				}
				break;
			}
			if (renderTaskCreateIndex == renderTasks.size()) {
				const VkFenceCreateInfo vk_fenceCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
					.flags = VK_FENCE_CREATE_SIGNALED_BIT
				};
				size_t fenceCreateIndex = 0;
				while (fenceCreateIndex < renderFences.size()) {
					if (vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, &renderFences[fenceCreateIndex]) == VK_SUCCESS) {
						fenceCreateIndex++;
						continue;
					}
					break;
				}
				if (fenceCreateIndex == static_cast<uint8_t>(renderFences.size())) {
					Vulkan_Buffer stagingRectBuffer;
					VulkanTask rectBufferCreateTask;
					Vulkan_Fence stagingRectBufferTransferFence;
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_render_elements(stagingRectBuffer, rectBufferCreateTask, stagingRectBufferTransferFence), bool)) {
						if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_descriptor_sets(), bool)) {
							if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_renderpass(), bool)) {
								const VkPipelineLayoutCreateInfo vk_worldPipelineLayoutCreateInfo = {
									.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
								};
								if (vkCreatePipelineLayout(vk_hDevice, &vk_worldPipelineLayoutCreateInfo, nullptr, &vk_hWorldPipelineLayout) == VK_SUCCESS) {
									if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_render_pipelines(), bool)) {
										stagingRectBufferTransferFence.wait_for();
										return true;
									}
									vkDestroyPipelineLayout(vk_hDevice, vk_hWorldPipelineLayout, nullptr);
								}
								PUSH_TO_CALLSTACKTRACE(destroy_renderpass());
							}
							PUSH_TO_CALLSTACKTRACE(destroy_descriptor_sets());
						}
						stagingRectBufferTransferFence.wait_for();
						PUSH_TO_CALLSTACKTRACE(destroy_render_elements());
					}
				} else
					RE_FATAL_ERROR("Failed to create Vulkan fence at index ", fenceCreateIndex, " to wait on rendering to finish");
				for (size_t fenceDestroyIndex = 0; fenceDestroyIndex < fenceCreateIndex; fenceDestroyIndex++)
					vkDestroyFence(vk_hDevice, renderFences[fenceDestroyIndex], nullptr);
			} else
				RE_FATAL_ERROR("Failed to initialize render task at index ", renderTaskCreateIndex);
			for (size_t renderTaskDestroyIndex = 1; renderTaskDestroyIndex < renderTaskCreateIndex; renderTaskDestroyIndex++)
				renderTasks[renderTaskDestroyIndex].destroy();
			renderTasks[0].destroy();
		} else
			RE_FATAL_ERROR("Failed to initialize render task at index 0");
		return false;
	}
	
	void destroy_renderer() {
		PUSH_TO_CALLSTACKTRACE(destroy_render_pipelines());
		vkDestroyPipelineLayout(vk_hDevice, vk_hWorldPipelineLayout, nullptr);
		PUSH_TO_CALLSTACKTRACE(destroy_renderpass());
		PUSH_TO_CALLSTACKTRACE(destroy_descriptor_sets());
		PUSH_TO_CALLSTACKTRACE(destroy_render_elements());
		for (const VkFence &vk_rhFence : renderFences)
			vkDestroyFence(vk_hDevice, vk_rhFence, nullptr);
		for (VulkanTask &rRenderTask : renderTasks)
			rRenderTask.destroy();
	}

	void render() {

	}

	bool swapchain_created_renderer() {
		VkExtent3D vk_renderImageExtent3D;
		vk_renderImageExtent3D.depth = 1;
		switch (screenPercentageSettings.eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				vk_renderImageExtent3D.width = vk_swapchainResolution.width;
				vk_renderImageExtent3D.height = vk_swapchainResolution.height;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				vk_renderImageExtent3D.width = static_cast<uint32_t>(std::round(vk_swapchainResolution.width * screenPercentageSettings.fScale));
				vk_renderImageExtent3D.height = static_cast<uint32_t>(std::round(vk_swapchainResolution.height * screenPercentageSettings.fScale));
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				vk_renderImageExtent3D.width = screenPercentageSettings.constSize[0];
				vk_renderImageExtent3D.height = screenPercentageSettings.constSize[1];
				break;
		}
		VulkanTask depthImageLayoutTransitionTask;
		Vulkan_Fence depthStencilImageLayoutTransitionFence;
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_depth_stencil_images(vk_renderImageExtent3D, depthImageLayoutTransitionTask, depthStencilImageLayoutTransitionFence.get_fence()), bool)) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_singlesampled_images(vk_renderImageExtent3D), bool)) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_render_images(vk_renderImageExtent3D), bool)) {
					depthStencilImageLayoutTransitionFence.wait_for();
					return true;
				}
				PUSH_TO_CALLSTACKTRACE(destroy_singlesampled_images());
			}
			depthStencilImageLayoutTransitionFence.wait_for();
			PUSH_TO_CALLSTACKTRACE(destroy_depth_stencil_images());
		}
		return false;
	}

	void swapchain_destroyed_renderer() {
		PUSH_TO_CALLSTACKTRACE(destroy_render_images());
		PUSH_TO_CALLSTACKTRACE(destroy_depth_stencil_images());
	}

	void attach_camera(const Camera *const pCamera) {
		pActiveCamera = pCamera;
	}

	void set_screen_percentage_settings(const ScreenPercentageSettings &rNewSettings) {
		bool bRequiresRecreatingRenderPass;
		switch (rNewSettings.eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL)
					return;
				bRequiresRecreatingRenderPass = true;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_SCALED && screenPercentageSettings.fScale == rNewSettings.fScale)
					return;
				bRequiresRecreatingRenderPass = screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE && screenPercentageSettings.constSize == rNewSettings.constSize)
					return;
				bRequiresRecreatingRenderPass = screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				break;
		}
		screenPercentageSettings = rNewSettings;
		if (vk_hDevice == VK_NULL_HANDLE)
			return;
		WAIT_FOR_IDLE_VULKAN_DEVICE();
	}

	[[nodiscard]]
	ScreenPercentageSettings get_screen_percentage_settings() {
		return screenPercentageSettings;
	}

	void enable_sample_shading(const bool bEnable) {
		if (is_sample_shading_enabled() == bEnable)
			return;
		vk_bSampleShadingEnabled = bEnable ? VK_TRUE : VK_FALSE;
		if (vk_hDevice == VK_NULL_HANDLE)
			return;
		WAIT_FOR_IDLE_VULKAN_DEVICE();
	}

	[[nodiscard]]
	bool is_sample_shading_enabled() {
		return vk_bSampleShadingEnabled == VK_TRUE;
	}

	void set_sample_shading_rate(const float fNewSampleShadingRate) {
		if (fSampleShadingRate == fNewSampleShadingRate)
			return;
		if (fNewSampleShadingRate < 0.0f || fNewSampleShadingRate > 1.0f) {
			RE_ERROR("Sample shading rate should be in range between 0 and 1, but was ", fNewSampleShadingRate, ". Request to change it has been discarded");
			return;
		} else if (fNewSampleShadingRate == 0.0f) {
			RE_WARNING("Sample shading rate of zero disables sample shading. The engine will ignore this new rate and disable it instead");
			vk_bSampleShadingEnabled = VK_FALSE;
		} else
			fSampleShadingRate = fNewSampleShadingRate;
		if (vk_hDevice == VK_NULL_HANDLE)
			return;
		WAIT_FOR_IDLE_VULKAN_DEVICE();
	}

	[[nodiscard]]
	float get_sample_shading_rate() {
		return fSampleShadingRate;
	}

}
