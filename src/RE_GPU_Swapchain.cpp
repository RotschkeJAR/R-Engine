#include "RE_GPU_Internal.hpp"
#include "RE_Renderer.hpp"
#include "RE_Window.hpp"

namespace RE {

	VkPresentModeKHR vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR, vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
	VkSwapchainKHR vk_hSwapchain = VK_NULL_HANDLE;
	VkFormat vk_eSwapchainImageFormat;
	VkExtent2D vk_swapchainResolution;
	std::unique_ptr<VkImage[]> vk_pahSwapchainImages;
	std::unique_ptr<VkImageView[]> vk_pahSwapchainImageViews;
	uint32_t u32SwapchainImageCount;
	
	bool create_swapchain() {
		// Create actual sweapchain
		const VkSwapchainKHR vk_hOldSwapchain = vk_hSwapchain;
		if (vk_hOldSwapchain != VK_NULL_HANDLE) {
			swapchain_destroyed_renderer();
			for (uint32_t u32SwapchainImageIndex = 0; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++) {
				PRINT_DEBUG("Destroying Vulkan image view for swapchain image at index ", u32SwapchainImageIndex);
				vkDestroyImageView(vk_hDevice, vk_pahSwapchainImageViews[u32SwapchainImageIndex], nullptr);
			}
			vk_pahSwapchainImages.reset();
			vk_pahSwapchainImageViews.reset();
		}
		if (vk_surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() || vk_surfaceCapabilities.currentExtent.height != std::numeric_limits<uint32_t>::max() || !vk_surfaceCapabilities.currentExtent.width || !vk_surfaceCapabilities.currentExtent.height)
			vk_swapchainResolution = vk_surfaceCapabilities.currentExtent;
		else {
			vk_swapchainResolution.width = std::clamp<uint32_t>(windowSize[0], vk_surfaceCapabilities.minImageExtent.width, vk_surfaceCapabilities.maxImageExtent.width);
			vk_swapchainResolution.height = std::clamp<uint32_t>(windowSize[1], vk_surfaceCapabilities.minImageExtent.height, vk_surfaceCapabilities.maxImageExtent.height);
		}
		PRINT_DEBUG("Creating swapchain");
		VkSwapchainCreateInfoKHR vk_swapchainCreateInfo;
		vk_swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		vk_swapchainCreateInfo.pNext = nullptr;
		vk_swapchainCreateInfo.flags = 0;
		vk_swapchainCreateInfo.surface = vk_hSurface;
		vk_swapchainCreateInfo.minImageCount = std::clamp(vk_surfaceCapabilities.minImageCount + 1, vk_surfaceCapabilities.minImageCount, vk_surfaceCapabilities.maxImageCount > 0 ? vk_surfaceCapabilities.maxImageCount : std::numeric_limits<uint32_t>::max());
		vk_swapchainCreateInfo.imageFormat = vk_paSurfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].format;
		vk_swapchainCreateInfo.imageColorSpace = vk_paSurfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].colorSpace;
		vk_swapchainCreateInfo.imageExtent = vk_swapchainResolution;
		vk_swapchainCreateInfo.imageArrayLayers = 1;
		vk_swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (u8LogicalQueueCount > 1) {
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_swapchainCreateInfo.queueFamilyIndexCount = u8LogicalQueueCount;
			vk_swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.get();
		} else
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vk_swapchainCreateInfo.preTransform = vk_surfaceCapabilities.currentTransform;
		vk_swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		vk_swapchainCreateInfo.clipped = VK_TRUE;
		vk_swapchainCreateInfo.oldSwapchain = vk_hOldSwapchain;
		if (bVsyncEnabled)
			vk_swapchainCreateInfo.presentMode = vk_ePresentModeVsync;
		else
			vk_swapchainCreateInfo.presentMode = vk_ePresentModeNoVsync;
		if (vkCreateSwapchainKHR(vk_hDevice, &vk_swapchainCreateInfo, nullptr, &vk_hSwapchain) != VK_SUCCESS) {
			RE_ERROR("Failed creating Vulkan swapchain");
			return false;
		}
		vk_eSwapchainImageFormat = vk_paSurfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].format;
		if (vk_hOldSwapchain != VK_NULL_HANDLE) {
			PRINT_DEBUG("Destroying old swapchain");
			vkDestroySwapchainKHR(vk_hDevice, vk_hOldSwapchain, nullptr);
		}

		// Create swapchain image views
		PRINT_DEBUG("Getting handles to swapchain images");
		vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, nullptr);
		vk_pahSwapchainImages = std::make_unique<VkImage[]>(u32SwapchainImageCount);
		vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, vk_pahSwapchainImages.get());
		vk_pahSwapchainImageViews = std::make_unique<VkImageView[]>(u32SwapchainImageCount);
		uint32_t u32SwapchainImageCreateIndex = 0;
		while (u32SwapchainImageCreateIndex < u32SwapchainImageCount) {
			PRINT_DEBUG("Creating Vulkan image view for swapchain image at index ", u32SwapchainImageCreateIndex);
			if (create_vulkan_image_view(vk_pahSwapchainImages[u32SwapchainImageCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, &vk_pahSwapchainImageViews[u32SwapchainImageCreateIndex])) {
				u32SwapchainImageCreateIndex++;
				continue;
			} else
				RE_FATAL_ERROR("Failed to create Vulkan image view at swapchain image index ", u32SwapchainImageCreateIndex);
			break;
		}
		if (u32SwapchainImageCreateIndex != u32SwapchainImageCount) {
			for (uint32_t u32SwapchainImageDeleteIndex = 0; u32SwapchainImageDeleteIndex < u32SwapchainImageCreateIndex; u32SwapchainImageDeleteIndex++) {
				PRINT_DEBUG("Destroying Vulkan immage view for swapchain image at index ", u32SwapchainImageDeleteIndex, " for previous failure");
				vkDestroyImageView(vk_hDevice, vk_pahSwapchainImageViews[u32SwapchainImageDeleteIndex], nullptr);
			}
			vk_pahSwapchainImages.reset();
			vk_pahSwapchainImageViews.reset();
			PRINT_DEBUG("Destroying recently created swapchain");
			vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr);
			vk_hSwapchain = VK_NULL_HANDLE;
			return false;
		}
		
		return true;
	}
	
	void destroy_swapchain() {
		swapchain_destroyed_renderer();
		for (uint32_t u32SwapchainImageIndex = 0; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++) {
			PRINT_DEBUG("Destroying Vulkan image view for swapchain image at index ", u32SwapchainImageIndex);
			vkDestroyImageView(vk_hDevice, vk_pahSwapchainImageViews[u32SwapchainImageIndex], nullptr);
		}
		vk_pahSwapchainImages.reset();
		vk_pahSwapchainImageViews.reset();
		PRINT_DEBUG("Destroying swapchain");
		vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr);
		vk_hSwapchain = VK_NULL_HANDLE;
	}

	bool recreate_swapchain() {
		PRINT_DEBUG("Recreating swapchain");
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities);
		WAIT_FOR_IDLE_VULKAN_DEVICE();
		if (create_swapchain()) {
			if (swapchain_created_renderer())
				return true;
			else
				RE_ERROR("Failed notifying the renderer for recreated Vulkan swapchain");
			destroy_swapchain();
		} else
			RE_ERROR("Failed recreating the Vulkan swapchain");
		return false;
	}

}
