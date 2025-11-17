#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_RenderElement.hpp"
#include "RE_RenderPipelines.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {

#define RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE 2
#define RE_VK_SWAPCHAIN_SEMAPHORE_COUNT (u32SwapchainImageCount * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE)

	const Camera *pActiveCamera = nullptr;

	ScreenPercentageSettings screenPercentageSettings;
	
	VkPipelineLayout vk_hWorldPipelineLayout;

	float fSampleShadingRate = 0.2f;
	bool bRenderPipelinesDirty = false;

	bool init_renderer() {
		if (init_general_transfer_task()) {
			Vulkan_Buffer stagingRectBuffer;
			if (init_render_elements(stagingRectBuffer)) {
				if (create_render_tasks()) {
					if (create_render_buffers()) {
						if (create_descriptor_sets()) {
							if (create_renderpass()) {
								PRINT_DEBUG("Creating Vulkan pipeline layout");
								const VkDescriptorSetLayout vk_ahWorldPipelineLayoutDescriptors[] = {
									vk_hCameraDescLayout,
									vk_hTextureDescLayout
								};
								const VkPipelineLayoutCreateInfo vk_worldPipelineLayoutCreateInfo = {
									.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
									.setLayoutCount = sizeof(vk_ahWorldPipelineLayoutDescriptors) / sizeof(vk_ahWorldPipelineLayoutDescriptors[0]),
									.pSetLayouts = vk_ahWorldPipelineLayoutDescriptors
								};
								if (vkCreatePipelineLayout(vk_hDevice, &vk_worldPipelineLayoutCreateInfo, nullptr, &vk_hWorldPipelineLayout) == VK_SUCCESS) {
									if (create_render_pipelines()) {
										PRINT_DEBUG("Waiting for pending transfer tasks to finish");
										wait_for_transfer(std::numeric_limits<uint64_t>::max());
										return true;
									}
									PRINT_DEBUG("Destroying Vulkan pipeline layout due to failure initializing the whole renderer");
									vkDestroyPipelineLayout(vk_hDevice, vk_hWorldPipelineLayout, nullptr);
								} else
									RE_FATAL_ERROR("Failed creating a Vulkan graphics pipeline for rendering objects in world");
								destroy_renderpass();
							}
							destroy_descriptor_sets();
						}
						destroy_render_buffers();
					}
					destroy_render_tasks();
				}
				PRINT_DEBUG("Waiting for pending transfer task to finish before destroying further due to failure creating essential resources");
				wait_for_transfer(std::numeric_limits<uint64_t>::max());
				destroy_render_elements();
			}
			destroy_general_transfer_task();
		}
		return false;
	}
	
	void destroy_renderer() {
		PRINT_DEBUG("Destroying renderer");
		destroy_render_pipelines();
		vkDestroyPipelineLayout(vk_hDevice, vk_hWorldPipelineLayout, nullptr);
		destroy_renderpass();
		destroy_descriptor_sets();
		destroy_render_buffers();
		destroy_render_elements();
		destroy_general_transfer_task();
	}

	void render() {
		PRINT_DEBUG("Invoking render-procedure");
		if (bRenderPipelinesDirty) {
			recreate_render_pipelines();
			bRenderPipelinesDirty = false;
		}
		wait_for_transfer(std::numeric_limits<uint64_t>::max());
		vkWaitForFences(vk_hDevice, 1, &renderFences[u8CurrentFrameInFlightIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
		u8CurrentFrameInFlightIndex = (u8CurrentFrameInFlightIndex + 1) % RE_VK_FRAMES_IN_FLIGHT;
	}

	bool swapchain_created_renderer() {
		PRINT_DEBUG("Creating swapchain-related Vulkan objects in Renderer");
		return create_render_image_resources();
	}

	void swapchain_destroyed_renderer() {
		PRINT_DEBUG("Destroying swapchain-related Vulkan objects in Renderer");
		destroy_render_image_resources();
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
