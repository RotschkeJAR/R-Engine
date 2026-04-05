#include "RE_Renderer_Images_Internal.hpp"

namespace RE {
	
	VkImage vk_hSinglesampledImage = VK_NULL_HANDLE;
	std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> singlesampledImageViews;

	bool create_singlesampled_image() {
		PRINT_DEBUG("Fetching details about occupied queues for the singlesampled render target");
		const uint32_t au32QueueIndices[] = {
			RENDER_TASK_SUBINDEX_RENDERING,
			RENDER_TASK_SUBINDEX_IMAGE_BLIT
		};
		const VulkanQueueCollection occupiedQueuesInfo = renderTasks[0].queues_of_functions(au32QueueIndices, sizeof(au32QueueIndices) / sizeof(au32QueueIndices[0]));
		PRINT_DEBUG("Creating Vulkan image used as singlesampled render target");
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
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				vk_mSinglesampledImageUsages,
				occupiedQueuesInfo.u8QueueCount,
				occupiedQueuesInfo.queueFamilyIndices.get(),
				VK_IMAGE_LAYOUT_UNDEFINED,
				&vk_hSinglesampledImage))
			return true;
		else
			RE_FATAL_ERROR("Failed to create Vulkan image used as singlesampled render target");
		return false;
	}

	bool create_singlesampled_image_views() {
		PRINT_DEBUG("Setting data structures up for creating Vulkan image views pointing at singlesampled image");
		VkImageViewCreateInfo vk_viewCreateInfo;
		vk_viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vk_viewCreateInfo.pNext = nullptr;
		vk_viewCreateInfo.flags = 0;
		vk_viewCreateInfo.image = vk_hSinglesampledImage;
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
		uint8_t u8FramesInFlightCreateIndex = 0;
		while (u8FramesInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			vk_viewCreateInfo.subresourceRange.baseArrayLayer = u8FramesInFlightCreateIndex;
			PRINT_DEBUG("Creating Vulkan image view at index ", u8FramesInFlightCreateIndex, " pointing at singlesampled image ", vk_hSinglesampledImage, " with format ", std::hex, vk_eSwapchainImageFormat);
			const VkResult vk_eResult = vkCreateImageView(vk_hDevice, &vk_viewCreateInfo, nullptr, &singlesampledImageViews[u8FramesInFlightCreateIndex]);
			if (vk_eResult == VK_SUCCESS) {
				u8FramesInFlightCreateIndex++;
				continue;
			} else
				RE_FATAL_ERROR("Failed to create Vulkan image view at index ", u8FramesInFlightCreateIndex, " pointing at singlesampled image ", vk_hSinglesampledImage, " with format ", std::hex, vk_eSwapchainImageFormat, " (Vulkan error code: ", vk_eResult, ")");
			break;
		}
		if (u8FramesInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		PRINT_DEBUG("Destroying Vulkan image view pointing at singlesampled image due to failure to create all image views");
		for (uint8_t u8FramesInFlightDestroyIndex = 0; u8FramesInFlightDestroyIndex < u8FramesInFlightCreateIndex; u8FramesInFlightDestroyIndex++) {
			vkDestroyImageView(vk_hDevice, singlesampledImageViews[u8FramesInFlightDestroyIndex], nullptr);
			singlesampledImageViews[u8FramesInFlightDestroyIndex] = VK_NULL_HANDLE;
		}
		vk_hSinglesampledImage = VK_NULL_HANDLE;
		singlesampledImageViews.fill(VK_NULL_HANDLE);
		return false;
	}

	void destroy_singlesampled_image() {
		PRINT_DEBUG("Destroying Vulkan image ", vk_hSinglesampledImage, " used as singlesampled render target");
		vkDestroyImage(vk_hDevice, vk_hSinglesampledImage, nullptr);
		vk_hSinglesampledImage = VK_NULL_HANDLE;
	}

	void destroy_singlesampled_image_views() {
		PRINT_DEBUG("Destroying Vulkan image views pointing at the singlesampled render target");
		for (const VkImageView vk_hSinglesampledView : singlesampledImageViews)
			vkDestroyImageView(vk_hDevice, vk_hSinglesampledView, nullptr);
		singlesampledImageViews.fill(VK_NULL_HANDLE);
	}

}
