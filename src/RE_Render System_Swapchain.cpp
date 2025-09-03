#include "RE_Render System_Internal.hpp"
#include "RE_Renderer.hpp"
#include "RE_Window.hpp"

namespace RE {

	VkPresentModeKHR vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR, vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
	VkSwapchainKHR vk_hSwapchain = VK_NULL_HANDLE;
	VkFormat vk_eSwapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D vk_swapchainResolution = {};
	std::unique_ptr<VkImage[]> vk_pahSwapchainImages;
	std::unique_ptr<VkImageView[]> vk_pahSwapchainImageViews;
	uint32_t u32SwapchainImageCount = 0;
	
	bool create_swapchain() {
		// Create actual sweapchain
		const VkSwapchainKHR vk_hOldSwapchain = vk_hSwapchain;
		if (vk_hOldSwapchain) {
			PUSH_TO_CALLSTACKTRACE(swapchain_destroyed_renderer());
			for (uint32_t u32SwapchainImageIndex = 0; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++)
				vkDestroyImageView(vk_hDevice, vk_pahSwapchainImageViews[u32SwapchainImageIndex], nullptr);
			vk_pahSwapchainImages.reset();
			vk_pahSwapchainImageViews.reset();
		}
		if (vk_surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() || vk_surfaceCapabilities.currentExtent.height != std::numeric_limits<uint32_t>::max() || !vk_surfaceCapabilities.currentExtent.width || !vk_surfaceCapabilities.currentExtent.height)
			vk_swapchainResolution = vk_surfaceCapabilities.currentExtent;
		else {
			vk_swapchainResolution.width = std::clamp<uint32_t>(windowSize[0], vk_surfaceCapabilities.minImageExtent.width, vk_surfaceCapabilities.maxImageExtent.width);
			vk_swapchainResolution.height = std::clamp<uint32_t>(windowSize[1], vk_surfaceCapabilities.minImageExtent.height, vk_surfaceCapabilities.maxImageExtent.height);
		}
		VkSwapchainCreateInfoKHR vk_swapchainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = vk_hSurface,
			.minImageCount = std::clamp(vk_surfaceCapabilities.minImageCount + 1, vk_surfaceCapabilities.minImageCount, vk_surfaceCapabilities.maxImageCount > 0 ? vk_surfaceCapabilities.maxImageCount : std::numeric_limits<uint32_t>::max()),
			.imageFormat = vk_paSurfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].format,
			.imageColorSpace = vk_paSurfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].colorSpace,
			.imageExtent = vk_swapchainResolution,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.preTransform = vk_surfaceCapabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.clipped = VK_TRUE,
			.oldSwapchain = vk_hOldSwapchain
		};
		if (u8LogicalQueueCount > 1) {
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_swapchainCreateInfo.queueFamilyIndexCount = u8LogicalQueueCount;
			vk_swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.get();
		} else
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (are_bits_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_BIT))
			vk_swapchainCreateInfo.presentMode = vk_ePresentModeVsync;
		else
			vk_swapchainCreateInfo.presentMode = vk_ePresentModeNoVsync;
		if (vkCreateSwapchainKHR(vk_hDevice, &vk_swapchainCreateInfo, nullptr, &vk_hSwapchain) != VK_SUCCESS) {
			RE_ERROR("Failed creating Vulkan swapchain");
			return false;
		}
		vk_eSwapchainImageFormat = vk_paSurfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].format;
		if (vk_hOldSwapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(vk_hDevice, vk_hOldSwapchain, nullptr);

		// Create swapchain image views
		vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, nullptr);
		vk_pahSwapchainImages = std::make_unique<VkImage[]>(u32SwapchainImageCount);
		vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, vk_pahSwapchainImages.get());
		vk_pahSwapchainImageViews = std::make_unique<VkImageView[]>(u32SwapchainImageCount);
		uint32_t u32SwapchainImageCreateIndex = 0;
		while (u32SwapchainImageCreateIndex < u32SwapchainImageCount) {
			if (create_vulkan_image_view(vk_pahSwapchainImages[u32SwapchainImageCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, &vk_pahSwapchainImageViews[u32SwapchainImageCreateIndex])) {
				u32SwapchainImageCreateIndex++;
				continue;
			} else
				RE_FATAL_ERROR("Failed to create Vulkan image view at swapchain image index ", u32SwapchainImageCreateIndex);
			break;
		}
		if (u32SwapchainImageCreateIndex != u32SwapchainImageCount) {
			for (uint32_t u32SwapchainImageDeleteIndex = 0; u32SwapchainImageDeleteIndex < u32SwapchainImageCreateIndex; u32SwapchainImageDeleteIndex++)
				vkDestroyImageView(vk_hDevice, vk_pahSwapchainImageViews[u32SwapchainImageDeleteIndex], nullptr);
			vk_pahSwapchainImages.reset();
			vk_pahSwapchainImageViews.reset();
			vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr);
			vk_hSwapchain = VK_NULL_HANDLE;
			return false;
		}
		
		return true;
	}
	
	void destroy_swapchain() {
		PUSH_TO_CALLSTACKTRACE(swapchain_destroyed_renderer());
		for (uint32_t u32SwapchainImageIndex = 0; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++)
			vkDestroyImageView(vk_hDevice, vk_pahSwapchainImageViews[u32SwapchainImageIndex], nullptr);
		vk_pahSwapchainImages.reset();
		vk_pahSwapchainImageViews.reset();
		vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr);
		vk_hSwapchain = VK_NULL_HANDLE;
	}

	bool recreate_swapchain() {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities);
		WAIT_FOR_IDLE_VULKAN_DEVICE();
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_swapchain(), bool)) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(swapchain_created_renderer(), bool))
				return true;
			else
				RE_ERROR("Failed notifying the renderer for recreated Vulkan swapchain");
			PUSH_TO_CALLSTACKTRACE(destroy_swapchain());
		} else
			RE_ERROR("Failed recreating the Vulkan swapchain");
		return false;
	}

}
