#include "RE_Renderer_Internal.hpp"
#include "RE_Main.hpp"

namespace RE {

	ScreenPercentageSettings screenPercentageSettings;
	VkImage vk_hRenderImages;
	VkDeviceMemory vk_hRenderImageMemories;
	VkImageView vk_ahRenderImageViews[RE_VK_FRAMES_IN_FLIGHT];
	
	static bool create_render_images(const VkExtent3D &vk_rRenderImageExtent3D) {
		if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL && vk_eMsaaCount == VK_SAMPLE_COUNT_1_BIT)
			return true;
		PRINT_DEBUG("Creating Vulkan image array as target for rendering");
		if (create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, vk_rRenderImageExtent3D, 1, RE_VK_FRAMES_IN_FLIGHT, vk_eMsaaCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRenderImages, &vk_hRenderImageMemories)) {
			size_t renderImageCreateIndex = 0;
			while (renderImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				PRINT_DEBUG("Creating Vulkan image view at index ", renderImageCreateIndex, " for render image array");
				if (create_vulkan_image_view(vk_hRenderImages, VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, renderImageCreateIndex, 1, &vk_ahRenderImageViews[renderImageCreateIndex])) {
					renderImageCreateIndex++;
					continue;
				} else
					RE_FATAL_ERROR("Failed creating Vulkan image view for render image at index ", renderImageCreateIndex);
				break;
			}
			if (renderImageCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
				return true;
			for (size_t renderImageDestroyIndex = 0; renderImageDestroyIndex < renderImageCreateIndex; renderImageDestroyIndex++) {
				PRINT_DEBUG("Destroying Vulkan image view at index ", renderImageDestroyIndex, " due to the failure creating all image views pointing at the render image array");
				vkDestroyImageView(vk_hDevice, vk_ahRenderImageViews[renderImageDestroyIndex], nullptr);
			}
			PRINT_DEBUG("Destroying Vulkan image ", vk_hRenderImages, "and its memory ", vk_hRenderImageMemories, " due to failure creating its image views");
			vkFreeMemory(vk_hDevice, vk_hRenderImageMemories, nullptr);
			vkDestroyImage(vk_hDevice, vk_hRenderImages, nullptr);
		} else
			RE_FATAL_ERROR("Failed creating render image");
		return false;
	}

	static void destroy_render_images() {
		if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL && vk_eMsaaCount == VK_SAMPLE_COUNT_1_BIT)
			return;
		for (size_t renderImageDestroyIndex = 0; renderImageDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; renderImageDestroyIndex++) {
			PRINT_DEBUG("Destroying Vulkan image view at index ", renderImageDestroyIndex, " pointing at render image array");
			vkDestroyImageView(vk_hDevice, vk_ahRenderImageViews[renderImageDestroyIndex], nullptr);
		}
		PRINT_DEBUG("Destroying Vulkan image ", vk_hRenderImages, " and its memory ", vk_hRenderImageMemories);
		vkFreeMemory(vk_hDevice, vk_hRenderImageMemories, nullptr);
		vkDestroyImage(vk_hDevice, vk_hRenderImages, nullptr);
	}

	bool create_render_image_resources() {
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
					for (uint8_t u8FramesInFlightIndex = 0; u8FramesInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FramesInFlightIndex++) {
						renderTasks[u8FramesInFlightIndex].record(RENDER_TASK_SUBINDEX_IMAGE_TRANSFER, 0, [](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
							
						});
					}
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

	void destroy_render_image_resources() {
		PRINT_DEBUG("Destroying render image resources");
		destroy_render_images();
		destroy_singlesampled_images();
		destroy_depth_stencil_images();
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

}
