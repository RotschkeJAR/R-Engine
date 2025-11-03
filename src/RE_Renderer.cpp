#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_RenderElement.hpp"
#include "RE_RenderPipelines.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"

#include <thread>

namespace RE {

	typedef uint16_t REindex_t;
#define RE_VK_RECT_INDEX_BUFFER_SIZE (RE_VK_RENDERABLE_RECTANGLES_COUNT * 6)
#define RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES (RE_VK_RECT_INDEX_BUFFER_SIZE * sizeof(REindex_t))

#define RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE 2
#define RE_VK_SWAPCHAIN_SEMAPHORE_COUNT (u32SwapchainImageCount * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE)

	const Camera *pActiveCamera = nullptr;

	ScreenPercentageSettings screenPercentageSettings(RE_SCREEN_PERCENTAGE_MODE_NORMAL);

	float fSampleShadingRate = 0.2f;

	std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT> vk_ahRenderImages;
	std::array<VkDeviceMemory, RE_VK_FRAMES_IN_FLIGHT> vk_ahRenderImageMemories;
	std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> vk_ahRenderImageViews;

	std::array<VulkanTask, RE_VK_FRAMES_IN_FLIGHT> renderTasks;
	std::array<VkFence, RE_VK_FRAMES_IN_FLIGHT> renderFences;
	std::vector<VkSemaphore> swapchainSemaphores;

	VkPipelineLayout vk_hWorldPipelineLayout;

	uint32_t u32NextSwapchainSemaphoreIndex = 0;
	uint8_t u8CurrentFrameInFlightIndex = 0;
	bool bRenderPipelinesDirty = false;

	static bool create_render_images(const VkExtent3D &vk_rRenderImageExtent3D) {
		if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL && vk_eMsaaCount == VK_SAMPLE_COUNT_1_BIT)
			return true;
		PRINT_DEBUG("Creating render images");
		size_t renderImageCreateIndex = 0;
		while (renderImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			PRINT_DEBUG("Creating Vulkan image at index ", renderImageCreateIndex);
			if (create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, vk_rRenderImageExtent3D, 1, 1, vk_eMsaaCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahRenderImages[renderImageCreateIndex], &vk_ahRenderImageMemories[renderImageCreateIndex])) {
				PRINT_DEBUG("Creating Vulkan image view for render image at index ", renderImageCreateIndex);
				if (create_vulkan_image_view(vk_ahRenderImages[renderImageCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, &vk_ahRenderImageViews[renderImageCreateIndex])) {
					renderImageCreateIndex++;
					continue;
				} else
					RE_FATAL_ERROR("Failed creating Vulkan image view for render image at index ", renderImageCreateIndex);
				PRINT_DEBUG("Destroying render image at index ", renderImageCreateIndex, " due to the failure of creating its Vulkan image view");
				vkFreeMemory(vk_hDevice, vk_ahRenderImageMemories[renderImageCreateIndex], nullptr);
				vkDestroyImage(vk_hDevice, vk_ahRenderImages[renderImageCreateIndex], nullptr);
			} else
				RE_FATAL_ERROR("Failed creating render image at index ", renderImageCreateIndex);
			break;
		}
		if (renderImageCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		for (size_t renderImageDestroyIndex = 0; renderImageDestroyIndex < renderImageCreateIndex; renderImageDestroyIndex++) {
			PRINT_DEBUG("Destroying render image and its Vulkan image view at index ", renderImageDestroyIndex, " due to the failure of the whole process to create render images and its Vulkan image views");
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
			PRINT_DEBUG("Destroying render image and its Vulkan image view at index ", renderImageDestroyIndex);
			vkDestroyImageView(vk_hDevice, vk_ahRenderImageViews[renderImageDestroyIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahRenderImageMemories[renderImageDestroyIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahRenderImages[renderImageDestroyIndex], nullptr);
		}
	}

	static bool create_render_image_resources() {
		PRINT_DEBUG("Calculating size of image being rendered to");
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
		if (create_depth_stencil_images(vk_renderImageExtent3D, depthImageLayoutTransitionTask, depthStencilImageLayoutTransitionFence.get_fence())) {
			if (create_singlesampled_images(vk_renderImageExtent3D)) {
				if (create_render_images(vk_renderImageExtent3D)) {
					PRINT_DEBUG("Waiting for Vulkan fence signaling completion of image layout transfer of all depth images");
					depthStencilImageLayoutTransitionFence.wait_for();
					return true;
				}
				destroy_singlesampled_images();
			}
			PRINT_DEBUG("Waiting for Vulkan fence signaling completion of image layout transfer of all depth images due to failure initializing all swapchain-related Vulkan objects");
			depthStencilImageLayoutTransitionFence.wait_for();
			destroy_depth_stencil_images();
		}
		return false;
	}

	static void destroy_render_image_resources() {
		PRINT_DEBUG("Destroying render image resources");
		destroy_render_images();
		destroy_singlesampled_images();
		destroy_depth_stencil_images();
	}

	uint8_t get_render_graphics_queue_logical_index() {
		return renderTasks[0].get_logical_queue_index_for_function(1);
	}

	bool init_renderer() {
		PRINT_DEBUG("Initializing first render task");
		constexpr VkQueueFlagBits vk_aeQueuesUsedForRendering[] = {VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_TRANSFER_BIT};
		if (renderTasks[0].init(sizeof(vk_aeQueuesUsedForRendering) / sizeof(vk_aeQueuesUsedForRendering[0]), vk_aeQueuesUsedForRendering, false)) {
			size_t renderTaskCreateIndex = 1;
			while (renderTaskCreateIndex < renderTasks.size()) {
				PRINT_DEBUG("Initializing render task at index ", renderTaskCreateIndex, " by using the first render task's precomputed data");
				if (renderTasks[renderTaskCreateIndex].init(renderTasks[0])) {
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
					PRINT_DEBUG("Creating Vulkan fence at index ", fenceCreateIndex, " for render tasks");
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
					if (init_render_elements(stagingRectBuffer, rectBufferCreateTask, stagingRectBufferTransferFence)) {
						if (create_descriptor_sets()) {
							if (create_renderpass()) {
								PRINT_DEBUG("Creating Vulkan pipeline layout");
								const VkDescriptorSetLayout vk_ahWorldPipelineLayoutDescriptors[] = {
									vk_hPermanentDescLayout
								};
								const VkPipelineLayoutCreateInfo vk_worldPipelineLayoutCreateInfo = {
									.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
									.setLayoutCount = sizeof(vk_ahWorldPipelineLayoutDescriptors) / sizeof(vk_ahWorldPipelineLayoutDescriptors[0]),
									.pSetLayouts = vk_ahWorldPipelineLayoutDescriptors
								};
								if (vkCreatePipelineLayout(vk_hDevice, &vk_worldPipelineLayoutCreateInfo, nullptr, &vk_hWorldPipelineLayout) == VK_SUCCESS) {
									if (create_render_pipelines()) {
										if (init_game_object_render_batches()) {
											PRINT_DEBUG("Waiting for Vulkan fence to signal completion of transferring Vulkan buffer containing rectangle data to GPU");
											stagingRectBufferTransferFence.wait_for();
											return true;
										}
										destroy_render_pipelines();
									}
									PRINT_DEBUG("Destroying Vulkan pipeline layout due to failure initializing the whole renderer");
									vkDestroyPipelineLayout(vk_hDevice, vk_hWorldPipelineLayout, nullptr);
								} else
									RE_FATAL_ERROR("Failed creating a Vulkan graphics pipeline for rendering objects in world");
								destroy_renderpass();
							}
							destroy_descriptor_sets();
						}
						PRINT_DEBUG("Waiting for Vulkan fence to signal completion of transferring Vulkan buffer containing rectangle data to GPU due to failure initializing the whole renderer");
						stagingRectBufferTransferFence.wait_for();
						destroy_render_elements();
					}
				} else
					RE_FATAL_ERROR("Failed to create Vulkan fence at index ", fenceCreateIndex, " to wait on rendering to finish");
				for (size_t fenceDestroyIndex = 0; fenceDestroyIndex < fenceCreateIndex; fenceDestroyIndex++) {
					PRINT_DEBUG("Destroying Vulkan fence ", renderFences[fenceDestroyIndex], " at index ", fenceDestroyIndex, " due to failure initializing the whole renderer");
					vkDestroyFence(vk_hDevice, renderFences[fenceDestroyIndex], nullptr);
				}
			} else
				RE_FATAL_ERROR("Failed to initialize render task at index ", renderTaskCreateIndex);
			for (size_t renderTaskDestroyIndex = 0; renderTaskDestroyIndex < renderTaskCreateIndex; renderTaskDestroyIndex++) {
				PRINT_DEBUG("Destroying render task at index ", renderTaskDestroyIndex);
				renderTasks[renderTaskDestroyIndex].destroy();
			}
		} else
			RE_FATAL_ERROR("Failed to initialize render task at index 0");
		return false;
	}
	
	void destroy_renderer() {
		PRINT_DEBUG("Destroying renderer");
		destroy_game_object_render_batches();
		destroy_render_pipelines();
		vkDestroyPipelineLayout(vk_hDevice, vk_hWorldPipelineLayout, nullptr);
		destroy_renderpass();
		destroy_descriptor_sets();
		destroy_render_elements();
		for (size_t fenceDestroyIndex = 0; fenceDestroyIndex < renderFences.size(); fenceDestroyIndex++) {
			PRINT_DEBUG("Destroying Vulkan fence ", renderFences[fenceDestroyIndex], " at index ", fenceDestroyIndex);
			vkDestroyFence(vk_hDevice, renderFences[fenceDestroyIndex], nullptr);
		}
		for (size_t renderTaskDestroyIndex = 0; renderTaskDestroyIndex < renderTasks.size(); renderTaskDestroyIndex++) {
			PRINT_DEBUG("Destroying render task at index ", renderTaskDestroyIndex);
			renderTasks[renderTaskDestroyIndex].destroy();
		}
	}

	void render() {
		PRINT_DEBUG("Invoking render-procedure");
		if (bRenderPipelinesDirty) {
			recreate_render_pipelines();
			bRenderPipelinesDirty = false;
		}
	}

	bool swapchain_created_renderer() {
		PRINT_DEBUG("Creating swapchain-related Vulkan objects in Renderer");
		return create_render_image_resources();
	}

	void swapchain_destroyed_renderer() {
		PRINT_DEBUG("Destroying swapchain-related Vulkan objects in Renderer");
		destroy_render_image_resources();
	}

	bool wait_for_rendering_finished() {
		PRINT_DEBUG("Waiting for all render process finished");
		return vkWaitForFences(vk_hDevice, renderFences.size(), renderFences.data(), VK_TRUE, std::numeric_limits<uint64_t>::max()) == VK_SUCCESS;
	}

	void attach_camera(const Camera *const pCamera) {
		pActiveCamera = pCamera;
	}

	void set_screen_percentage_settings(const ScreenPercentageSettings &rNewSettings) {
		if (screenPercentageSettings.eMode == rNewSettings.eMode)
			switch (rNewSettings.eMode) {
				case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
					return;
				case RE_SCREEN_PERCENTAGE_MODE_SCALED:
					if (screenPercentageSettings.fScale == rNewSettings.fScale)
						return;
					break;
				case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
					if (screenPercentageSettings.constSize == rNewSettings.constSize)
						return;
					break;
			}
		PRINT_DEBUG("Updating screen percentage settings");
		screenPercentageSettings = rNewSettings;
		if (!bSwapchainDirty && bRunning) {
			PRINT_DEBUG("Immediatly applying new screen percentage settings");
			destroy_render_image_resources();
			create_render_image_resources();
		}
	}

	[[nodiscard]]
	ScreenPercentageSettings get_screen_percentage_settings() {
		return screenPercentageSettings;
	}

	[[nodiscard]]
	bool is_sample_shading_enabled() {
		return fSampleShadingRate > 0.0f && fSampleShadingRate <= 1.0f;
	}

	void set_sample_shading_rate(const float fNewSampleShadingRate) {
		if (fSampleShadingRate == fNewSampleShadingRate)
			return;
		if (fNewSampleShadingRate < 0.0f || fNewSampleShadingRate > 1.0f) {
			RE_ERROR("Sample shading rate should be in range between 0 and 1, but was ", fNewSampleShadingRate, ". Request to change it has been discarded");
			return;
		} else {
			PRINT_DEBUG("Setting sample shading rate to ", fNewSampleShadingRate);
			fSampleShadingRate = fNewSampleShadingRate;
			bRenderPipelinesDirty = bRunning;
		}
	}

	[[nodiscard]]
	float get_sample_shading_rate() {
		return fSampleShadingRate;
	}

}
