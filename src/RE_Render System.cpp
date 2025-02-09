#include "RE_Render System.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	RenderSystem* RenderSystem::instance = nullptr;

	RenderSystem::RenderSystem() {
		if (RenderSystem::instance) {
			RE_ERROR("An instance of the render system has already been created. The new one won't be initialized and remains invalid");
			return;
		}
		RenderSystem::instance = this;

		valid = false;
		vsyncActive = false;
		vsyncMode = VK_PRESENT_MODE_FIFO_KHR;
		noVsync = VK_PRESENT_MODE_FIFO_KHR;
		surfaceFormat = {};
		swapchainImageSize = {};
		internalSwapchain = VK_NULL_HANDLE;
		swapchainImageCount = 0;
		swapchainImages = nullptr;
		swapchainImageViews = nullptr;

		if (!createSwapchain())
			return;
		if (!createImageViews())
			return;		
		valid = true;
	}

	RenderSystem::~RenderSystem() {
		if (RenderSystem::instance != this)
			return;
		RenderSystem::instance = nullptr;
		if (swapchainImageViews)
			for (uint32_t i = 0; i < swapchainImageCount; i++)
				if (swapchainImageViews[i] != VK_NULL_HANDLE)
					vkDestroyImageView(vkDevice, swapchainImageViews[i], nullptr);
		if (internalSwapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(vkDevice, internalSwapchain, nullptr);
		if (swapchainImages) {
			delete[] swapchainImages;
			delete[] swapchainImageViews;
		}
	}

	bool RenderSystem::createSwapchain() {
		for (uint32_t i = 0; i < vkPresentModesCount; i++) {
			if (vkPresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
				vsyncMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			else if (vkPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				noVsync = VK_PRESENT_MODE_MAILBOX_KHR;
		}
		bool foundFormat = false;
		for (uint32_t i = 0; i < vkSurfaceFormatsCount; i++) {
			if (vkSurfaceFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB && vkSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				surfaceFormat = vkSurfaceFormats[i];
				foundFormat = true;
				break;
			}
		}
		if (!foundFormat)
			surfaceFormat = vkSurfaceFormats[0];
		if (vkSurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() && vkSurfaceCapabilities.currentExtent.height != std::numeric_limits<uint32_t>::max())
			swapchainImageSize = vkSurfaceCapabilities.currentExtent;
		else {
			Vector<REushort, 2> winSize = Window::instance->getSize();
			swapchainImageSize = {static_cast<uint32_t>(winSize[0]), static_cast<uint32_t>(winSize[1])};
			swapchainImageSize.width = std::clamp(swapchainImageSize.width, vkSurfaceCapabilities.minImageExtent.width, vkSurfaceCapabilities.maxImageExtent.width);
			swapchainImageSize.height = std::clamp(swapchainImageSize.height, vkSurfaceCapabilities.minImageExtent.height, vkSurfaceCapabilities.maxImageExtent.height);
		}
		swapchainImageCount = vkSurfaceCapabilities.minImageCount + 1;
		if (vkSurfaceCapabilities.maxImageCount > 0 && swapchainImageCount > vkSurfaceCapabilities.maxImageCount)
			swapchainImageCount = vkSurfaceCapabilities.maxImageCount;

		VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		swapchainCreateInfo.surface = vkSurface;
		swapchainCreateInfo.minImageCount = swapchainImageCount;
		swapchainCreateInfo.imageFormat = surfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = swapchainImageSize;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		uint32_t queueFamilyIndices[] = {vkQueueIndices.graphicsFamily, vkQueueIndices.presentationFamily};
		if (vkQueueIndices.graphicsFamily != vkQueueIndices.presentationFamily) {
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.preTransform = vkSurfaceCapabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = vsyncActive ? vsyncMode : noVsync;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
		if (!CHECK_VK_RESULT(vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, nullptr, &internalSwapchain))) {
			RE_FATAL_ERROR("Failed creating swapchain in Vulkan");
			return false;
		}
		return true;
	}

	bool RenderSystem::createImageViews() {
		vkGetSwapchainImagesKHR(vkDevice, internalSwapchain, &swapchainImageCount, nullptr);
		swapchainImages = new VkImage[swapchainImageCount];
		swapchainImageViews = new VkImageView[swapchainImageCount];
		for (uint32_t i = 0; i < swapchainImageCount; i++)
			swapchainImageViews[i] = VK_NULL_HANDLE;
		vkGetSwapchainImagesKHR(vkDevice, internalSwapchain, &swapchainImageCount, swapchainImages);
		bool failure = false;
		for (uint32_t imageIndex = 0; imageIndex < swapchainImageCount; imageIndex++) {
			VkImageViewCreateInfo imgViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			imgViewCreateInfo.image = swapchainImages[imageIndex];
			imgViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imgViewCreateInfo.format = surfaceFormat.format;
			imgViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imgViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imgViewCreateInfo.subresourceRange.levelCount = 1;
			imgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imgViewCreateInfo.subresourceRange.layerCount = 1;
			if (!CHECK_VK_RESULT(vkCreateImageView(vkDevice, &imgViewCreateInfo, nullptr, &swapchainImageViews[imageIndex]))) {
				RE_FATAL_ERROR(appendStrings("Failed creating image view ", imageIndex, " out of ", swapchainImageCount));
				failure = true;
			}
		}
		return !failure;
	}

	void RenderSystem::windowResize(Vector<REushort, 2> newSize) {

	}

	bool RenderSystem::isValid() {
		return valid;
	}

}