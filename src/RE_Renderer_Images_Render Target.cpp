#include "RE_Renderer_Images_Internal.hpp"

namespace RE {
	
	VkImage vk_hRenderTargetImage = VK_NULL_HANDLE;
	std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> renderTargetImageViews;

	bool create_render_target_image() {
		PRINT_DEBUG("Fetching details about occupied queues for the render target");
		const uint32_t au32QueueIndices[] = {
			RENDER_TASK_SUBINDEX_RENDERING,
			RENDER_TASK_SUBINDEX_IMAGE_BLIT
		};
		const VulkanQueueCollection occupiedQueuesInfo = renderTasks[0].queues_of_functions(au32QueueIndices, sizeof(au32QueueIndices) / sizeof(au32QueueIndices[0]));
		PRINT_DEBUG("Creating Vulkan image used as render target");
		if (create_vulkan_image(0,
				VK_IMAGE_TYPE_2D,
				vk_eSwapchainImageFormat,
				VkExtent3D {
					.width = renderImageSize[0],
					.height = renderImageSize[1],
					.depth = 1
				},
				1,
				RE_VK_FRAMES_IN_FLIGHT,
				vk_eMsaaCount,
				VK_IMAGE_TILING_OPTIMAL,
				vk_mRenderTargetImageUsages,
				occupiedQueuesInfo.u8QueueCount,
				occupiedQueuesInfo.queueFamilyIndices.get(),
				VK_IMAGE_LAYOUT_UNDEFINED,
				&vk_hRenderTargetImage))
			return true;
		else
			RE_FATAL_ERROR("Failed to create Vulkan image used as render target");
		return false;
	}

	bool create_render_target_image_views() {
		PRINT_DEBUG("Setting data structures up for creating Vulkan image views pointing at the render target");
		VkImageViewCreateInfo vk_viewCreateInfo;
		vk_viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vk_viewCreateInfo.pNext = nullptr;
		vk_viewCreateInfo.flags = 0;
		vk_viewCreateInfo.image = vk_hRenderTargetImage;
		vk_viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		vk_viewCreateInfo.format = vk_eSwapchainImageFormat;
		vk_viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vk_viewCreateInfo.subresourceRange.baseMipLevel = 0;
		vk_viewCreateInfo.subresourceRange.levelCount = 1;
		vk_viewCreateInfo.subresourceRange.layerCount = 1;
		uint8_t u8FrameInFlightCreateIndex = 0;
		while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			PRINT_DEBUG("Creating Vulkan image view at index ", u8FrameInFlightCreateIndex, " pointing at image ", vk_hRenderTargetImage, " used as render target");
			vk_viewCreateInfo.subresourceRange.baseArrayLayer = u8FrameInFlightCreateIndex;
			const VkResult vk_eResult = vkCreateImageView(vk_hDevice, &vk_viewCreateInfo, nullptr, &renderTargetImageViews[u8FrameInFlightCreateIndex]);
			if (vk_eResult == VK_SUCCESS) {
				u8FrameInFlightCreateIndex++;
				continue;
			} else
				RE_FATAL_ERROR("Failed to create Vulkan image view at index ", u8FrameInFlightCreateIndex, " pointing at image ", vk_hRenderTargetImage, " used as render target (Vulkan error code: ", std::hex, vk_eResult, ")");
			break;
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		PRINT_DEBUG("Destroying Vulkan image views pointing at the render target due to failure creating all image views");
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++) {
			vkDestroyImageView(vk_hDevice, renderTargetImageViews[u8FrameInFlightDestroyIndex], nullptr);
			renderTargetImageViews[u8FrameInFlightDestroyIndex] = VK_NULL_HANDLE;
		}
		vk_hRenderTargetImage = VK_NULL_HANDLE;
		renderTargetImageViews.fill(VK_NULL_HANDLE);
		return false;
	}

	void destroy_render_target_image() {
		PRINT_DEBUG("Destroying Vulkan image ", vk_hRenderTargetImage, " used as render target");
		vkDestroyImage(vk_hDevice, vk_hRenderTargetImage, nullptr);
		vk_hRenderTargetImage = VK_NULL_HANDLE;
	}

	void destroy_render_target_image_views() {
		PRINT_DEBUG("Destroying Vulkan image views used as render targets");
		for (const VkImageView vk_hRenderTargetImageView : renderTargetImageViews)
			vkDestroyImageView(vk_hDevice, vk_hRenderTargetImageView, nullptr);
		renderTargetImageViews.fill(VK_NULL_HANDLE);
	}

}
