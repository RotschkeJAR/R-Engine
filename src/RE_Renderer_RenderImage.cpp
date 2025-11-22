#include "RE_Renderer_Internal.hpp"

namespace RE {

	VkImage vk_hRenderImages;
	VkDeviceMemory vk_hRenderImageMemories;
	std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> renderImageViews;
	
	static bool create_render_images() {
		if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL && vk_eMsaaCount == VK_SAMPLE_COUNT_1_BIT)
			return true;
		PRINT_DEBUG("Creating Vulkan image array as target for rendering");
		if (create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, vk_rRenderImageExtent3D, 1, RE_VK_FRAMES_IN_FLIGHT, vk_eMsaaCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRenderImages, &vk_hRenderImageMemories)) {
			size_t renderImageCreateIndex = 0;
			while (renderImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				PRINT_DEBUG("Creating Vulkan image view at index ", renderImageCreateIndex, " for render image array");
				if (create_vulkan_image_view(vk_hRenderImages, VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, renderImageCreateIndex, 1, &renderImageViews[renderImageCreateIndex])) {
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
				vkDestroyImageView(vk_hDevice, renderImageViews[renderImageDestroyIndex], nullptr);
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
			vkDestroyImageView(vk_hDevice, renderImageViews[renderImageDestroyIndex], nullptr);
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

}
