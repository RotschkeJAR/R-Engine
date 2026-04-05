#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrappers.hpp"

namespace RE {

	VkSwapchainKHR vk_hSwapchain = VK_NULL_HANDLE;
	VkFormat vk_eSwapchainImageFormat;
	VkExtent2D vk_swapchainResolution;
	std::unique_ptr<VkImage[]> swapchainImages;
	std::unique_ptr<VkImageView[]> swapchainImageViews;
	uint32_t u32SwapchainImageCount;
	bool bSwapchainDirty = false;
	
	bool create_swapchain() {
		// Create actual sweapchain
		const VkSwapchainKHR vk_hOldSwapchain = vk_hSwapchain;
		if (vk_hOldSwapchain != VK_NULL_HANDLE) {
			PRINT_DEBUG("Cleaning up ressources used for outdated swapchain");
			swapchain_destroyed_renderer();
			for (uint32_t u32ImageViewDestroyIndex = 0; u32ImageViewDestroyIndex < u32SwapchainImageCount; u32ImageViewDestroyIndex++) {
				PRINT_DEBUG("Destroying Vulkan image view for swapchain image at index ", u32ImageViewDestroyIndex);
				vkDestroyImageView(vk_hDevice, swapchainImageViews[u32ImageViewDestroyIndex], nullptr);
			}
			swapchainImages.reset();
			swapchainImageViews.reset();
		}
		PRINT_DEBUG("Computing information for Vulkan swapchain creation");
		if (vk_surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() || vk_surfaceCapabilities.currentExtent.height != std::numeric_limits<uint32_t>::max() || !vk_surfaceCapabilities.currentExtent.width || !vk_surfaceCapabilities.currentExtent.height)
			vk_swapchainResolution = vk_surfaceCapabilities.currentExtent;
		else {
			vk_swapchainResolution.width = std::clamp<uint32_t>(windowSize[0], vk_surfaceCapabilities.minImageExtent.width, vk_surfaceCapabilities.maxImageExtent.width);
			vk_swapchainResolution.height = std::clamp<uint32_t>(windowSize[1], vk_surfaceCapabilities.minImageExtent.height, vk_surfaceCapabilities.maxImageExtent.height);
		}
		std::vector<uint32_t> queuesToShareAcross;
		get_queues_for_swapchain_images(queuesToShareAcross);
		PRINT_DEBUG("Creating swapchain");
		const VkSwapchainCreateInfoKHR vk_swapchainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = vk_hSurface,
			.minImageCount = std::clamp<uint32_t>(
					vk_surfaceCapabilities.minImageCount + 1,
					vk_surfaceCapabilities.minImageCount,
					vk_surfaceCapabilities.maxImageCount > 0 ? vk_surfaceCapabilities.maxImageCount : std::numeric_limits<uint32_t>::max()
			),
			.imageFormat = surfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].format,
			.imageColorSpace = surfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].colorSpace,
			.imageExtent = vk_swapchainResolution,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.imageSharingMode = queuesToShareAcross.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
			.queueFamilyIndexCount = static_cast<uint32_t>(queuesToShareAcross.size()),
			.pQueueFamilyIndices = queuesToShareAcross.data(),
			.preTransform = vk_surfaceCapabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = bVsyncEnabled ? vk_ePresentModeVsync : vk_ePresentModeNoVsync,
			.clipped = VK_TRUE,
			.oldSwapchain = vk_hOldSwapchain
		};
		if (vkCreateSwapchainKHR(vk_hDevice, &vk_swapchainCreateInfo, nullptr, &vk_hSwapchain) == VK_SUCCESS) {
			vk_eSwapchainImageFormat = surfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].format;
			if (vk_hOldSwapchain != VK_NULL_HANDLE) {
				PRINT_DEBUG("Destroying old swapchain");
				vkDestroySwapchainKHR(vk_hDevice, vk_hOldSwapchain, nullptr);
			}

			// Create swapchain image views
			PRINT_DEBUG("Getting handles to swapchain images");
			vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, nullptr);
			swapchainImages = std::make_unique<VkImage[]>(u32SwapchainImageCount);
			vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, swapchainImages.get());
			swapchainImageViews = std::make_unique<VkImageView[]>(u32SwapchainImageCount);
			uint32_t u32SwapchainImageCreateIndex = 0;
			while (u32SwapchainImageCreateIndex < u32SwapchainImageCount) {
				PRINT_DEBUG("Creating Vulkan image view for swapchain image at index ", u32SwapchainImageCreateIndex);
				if (create_vulkan_image_view(
						0, 
						swapchainImages[u32SwapchainImageCreateIndex], 
						VK_IMAGE_VIEW_TYPE_2D, 
						vk_eSwapchainImageFormat, 
						VkComponentMapping {
							.r = VK_COMPONENT_SWIZZLE_IDENTITY,
							.g = VK_COMPONENT_SWIZZLE_IDENTITY,
							.b = VK_COMPONENT_SWIZZLE_IDENTITY,
							.a = VK_COMPONENT_SWIZZLE_IDENTITY
						}, 
						VkImageSubresourceRange {
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.baseMipLevel = 0,
							.levelCount = 1,
							.baseArrayLayer = 0,
							.layerCount = 1
						}, 
						&swapchainImageViews[u32SwapchainImageCreateIndex])) {
					u32SwapchainImageCreateIndex++;
					continue;
				} else
					RE_FATAL_ERROR("Failed to create Vulkan image view at swapchain image index ", u32SwapchainImageCreateIndex);
				break;
			}
			if (u32SwapchainImageCreateIndex == u32SwapchainImageCount) {
				swapchain_created_renderer();
				return true;
			}
			for (uint32_t u32SwapchainImageDeleteIndex = 0; u32SwapchainImageDeleteIndex < u32SwapchainImageCreateIndex; u32SwapchainImageDeleteIndex++) {
				PRINT_DEBUG("Destroying Vulkan immage view for swapchain image at index ", u32SwapchainImageDeleteIndex, " for previous failure");
				vkDestroyImageView(vk_hDevice, swapchainImageViews[u32SwapchainImageDeleteIndex], nullptr);
			}
			swapchainImages.reset();
			swapchainImageViews.reset();
			PRINT_DEBUG("Destroying recently created swapchain");
			vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr);
			vk_hSwapchain = VK_NULL_HANDLE;
		} else
			RE_FATAL_ERROR("Failed creating Vulkan swapchain");
		return false;
	}
	
	void destroy_swapchain() {
		for (uint32_t u32ImageViewDestroyIndex = 0; u32ImageViewDestroyIndex < u32SwapchainImageCount; u32ImageViewDestroyIndex++) {
			PRINT_DEBUG("Destroying Vulkan image view for swapchain image at index ", u32ImageViewDestroyIndex);
			vkDestroyImageView(vk_hDevice, swapchainImageViews[u32ImageViewDestroyIndex], nullptr);
		}
		swapchainImages.reset();
		swapchainImageViews.reset();
		PRINT_DEBUG("Destroying swapchain");
		vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr);
		vk_hSwapchain = VK_NULL_HANDLE;
		swapchain_destroyed_renderer();
	}

	bool recreate_swapchain() {
		PRINT_DEBUG("Recreating swapchain");
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(SELECTED_PHYSICAL_VULKAN_DEVICE, vk_hSurface, &vk_surfaceCapabilities);
		wait_for_rendering_finished();
		if (create_swapchain()) {
			return true;
		} else
			RE_ERROR("Failed recreating the Vulkan swapchain");
		return false;
	}

	void mark_swapchain_dirty() {
		bSwapchainDirty = true;
	}

}
