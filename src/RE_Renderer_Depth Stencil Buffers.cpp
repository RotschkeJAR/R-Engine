#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT * 2> vk_ahDepthStencilImages;
	std::array<VkDeviceMemory, vk_ahDepthStencilImages.size()> vk_ahDepthStencilImageMemories;
	std::array<VkImageView, vk_ahDepthStencilImages.size()> vk_ahDepthStencilImageViews;
	std::vector<VkFormat> vk_aeAvailableDepthStencilFormats;
	uint8_t u8IndexToSelectedDepthStencilFormat;

	static bool are_depth_stencil_images_seperated(const VkFormat vk_eFormat) {
		switch (vk_eFormat) {
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
			case VK_FORMAT_D24_UNORM_S8_UINT:
			case VK_FORMAT_D16_UNORM_S8_UINT:
				return false;
			case VK_FORMAT_D32_SFLOAT:
			case VK_FORMAT_X8_D24_UNORM_PACK32:
			case VK_FORMAT_D16_UNORM:
			case VK_FORMAT_S8_UINT:
				return true;
			default:
				std::abort();
		}
	}

	bool does_gpu_support_depth_stencil_buffers(const VkPhysicalDevice vk_hPhysicalDevice, std::queue<std::string> &rMissingFeatures) {
		constexpr VkFormat vk_aeDesiredFormats[] = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D16_UNORM,
		};
		bool bSupportsDepthBuffers = false, bSupportsStencilBuffers = false;
		for (const VkFormat vk_eDesiredFormat : vk_aeDesiredFormats) {
			VkFormatProperties vk_bufferFormatProperties;
			vkGetPhysicalDeviceFormatProperties(vk_hPhysicalDevice, vk_eDesiredFormat, &vk_bufferFormatProperties);
			if (((vk_bufferFormatProperties.linearTilingFeatures | vk_bufferFormatProperties.optimalTilingFeatures) & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0)
				switch (vk_eDesiredFormat) {
					case VK_FORMAT_D32_SFLOAT_S8_UINT:
					case VK_FORMAT_D24_UNORM_S8_UINT:
					case VK_FORMAT_D16_UNORM_S8_UINT:
						bSupportsDepthBuffers = true;
						bSupportsStencilBuffers = true;
						break;
					case VK_FORMAT_S8_UINT:
						bSupportsStencilBuffers = true;
						break;
					case VK_FORMAT_D16_UNORM:
					case VK_FORMAT_X8_D24_UNORM_PACK32:
					case VK_FORMAT_D32_SFLOAT:
						bSupportsDepthBuffers = true;
						break;
					default:
						std::abort();
				}
			if (bSupportsDepthBuffers && bSupportsStencilBuffers)
				return true;
		}
		if (!bSupportsDepthBuffers)
			rMissingFeatures.emplace("Depth buffers aren't supported");
		if (!bSupportsStencilBuffers)
			rMissingFeatures.emplace("Stencil buffers aren't supported");
		return false;
	}

	bool create_depth_stencil_buffers() {
		vk_aeAvailableDepthStencilFormats.reserve(7);
		constexpr VkFormat vk_aeAllDepthStencilFormats[] = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT
		};
		for (const VkFormat vk_eDepthStencilFormat : vk_aeAllDepthStencilFormats) {
			VkFormatProperties vk_depthStencilFormatProperties;
			vkGetPhysicalDeviceFormatProperties(get_selected_physical_vulkan_device(), vk_eDepthStencilFormat, &vk_depthStencilFormatProperties);
			if (((vk_depthStencilFormatProperties.optimalTilingFeatures | vk_depthStencilFormatProperties.linearTilingFeatures) & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0)
				vk_aeAvailableDepthStencilFormats.push_back(vk_eDepthStencilFormat);
		}
		VkFormatProperties vk_stencilFormatProperties;
		vkGetPhysicalDeviceFormatProperties(get_selected_physical_vulkan_device(), VK_FORMAT_S8_UINT, &vk_stencilFormatProperties);
		if (((vk_stencilFormatProperties.linearTilingFeatures | vk_stencilFormatProperties.optimalTilingFeatures) & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0) {
			constexpr VkFormat vk_aeAllDepthFormats[] = {
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_X8_D24_UNORM_PACK32,
				VK_FORMAT_D16_UNORM
			};
			for (const VkFormat vk_eDepthFormat : vk_aeAllDepthFormats) {
				VkFormatProperties vk_depthFormatProperties;
				vkGetPhysicalDeviceFormatProperties(get_selected_physical_vulkan_device(), vk_eDepthFormat, &vk_depthFormatProperties);
				if (((vk_depthFormatProperties.optimalTilingFeatures | vk_depthFormatProperties.linearTilingFeatures) & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0)
					vk_aeAvailableDepthStencilFormats.push_back(vk_eDepthFormat);
			}
		}

		u8IndexToSelectedDepthStencilFormat = 0;
		if (are_depth_stencil_images_seperated(vk_aeAvailableDepthStencilFormats[u8IndexToSelectedDepthStencilFormat])) {
			// TODO: create depth-stencil images
		} else {
			// TODO: create seperate depth & stencil images
		}
		return true;
	}

	void destroy_depth_stencil_buffers() {
		const uint8_t u8ImageCount = vk_ahDepthStencilImages.size() / (are_depth_stencil_images_seperated(vk_aeAvailableDepthStencilFormats[u8IndexToSelectedDepthStencilFormat]) ? 1 : 2);
		for (uint8_t u8ImageIndex = 0; u8ImageIndex < u8ImageCount; u8ImageIndex++) {
			vkDestroyImageView(vk_hDevice, vk_ahDepthStencilImageViews[u8ImageIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8ImageIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8ImageIndex], nullptr);
		}
	}

}
