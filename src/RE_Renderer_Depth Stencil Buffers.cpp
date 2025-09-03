#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT * 2> vk_ahDepthStencilImages;
	static std::array<VkDeviceMemory, vk_ahDepthStencilImages.size()> vk_ahDepthStencilImageMemories;
	std::array<VkImageView, vk_ahDepthStencilImages.size()> vk_ahDepthStencilImageViews;
	static std::vector<VkFormat> vk_aeAvailableDepthStencilFormats;
	uint8_t u8IndexToSelectedDepthStencilFormat;
	bool bStencilsEnabled = false;

	static bool are_depth_stencil_images_separated(const VkFormat vk_eFormat) {
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
			[[unlikely]] default:
				RE_FATAL_ERROR("Unknown Vulkan format used to determine separation between depth and stencil data");
				PUSH_TO_CALLSTACKTRACE(std::abort());
				return false;
		}
	}

	static void get_format_properties(const VkFormat vk_eFormat, VkFormatProperties2 *const vk_pFormatProperties, void *const pNext) {
		vk_pFormatProperties->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_pFormatProperties->pNext = pNext;
		vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDeviceSelected, vk_eFormat, vk_pFormatProperties);
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
		VkFormatProperties2 vk_imageFormatProperties;
		vk_imageFormatProperties.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_imageFormatProperties.pNext = nullptr;
		for (const VkFormat vk_eDesiredFormat : vk_aeDesiredFormats) {
			vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eDesiredFormat, &vk_imageFormatProperties);
			if (((vk_imageFormatProperties.formatProperties.linearTilingFeatures | vk_imageFormatProperties.formatProperties.optimalTilingFeatures) & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0)
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
					[[unlikely]] default:
						RE_FATAL_ERROR("Unknown Vulkan format used to distinguish between depth and stencils");
						PUSH_TO_CALLSTACKTRACE(std::abort());
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
			VkFormatProperties2 vk_depthStencilFormatProperties;
			get_format_properties(vk_eDepthStencilFormat, &vk_depthStencilFormatProperties, nullptr);
			if (((vk_depthStencilFormatProperties.formatProperties.optimalTilingFeatures | vk_depthStencilFormatProperties.formatProperties.linearTilingFeatures) & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0)
				vk_aeAvailableDepthStencilFormats.push_back(vk_eDepthStencilFormat);
		}
		VkFormatProperties2 vk_stencilFormatProperties;
		get_format_properties(VK_FORMAT_S8_UINT, &vk_stencilFormatProperties, nullptr);
		if (((vk_stencilFormatProperties.formatProperties.linearTilingFeatures | vk_stencilFormatProperties.formatProperties.optimalTilingFeatures) & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0) {
			constexpr VkFormat vk_aeAllDepthFormats[] = {
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_X8_D24_UNORM_PACK32,
				VK_FORMAT_D16_UNORM
			};
			for (const VkFormat vk_eDepthFormat : vk_aeAllDepthFormats) {
				VkFormatProperties2 vk_depthFormatProperties;
				get_format_properties(vk_eDepthFormat, &vk_depthFormatProperties, nullptr);
				if (((vk_depthFormatProperties.formatProperties.optimalTilingFeatures | vk_depthFormatProperties.formatProperties.linearTilingFeatures) & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0)
					vk_aeAvailableDepthStencilFormats.push_back(vk_eDepthFormat);
			}
		}
		vk_aeAvailableDepthStencilFormats.shrink_to_fit();

		if (bStencilsEnabled) {
			u8IndexToSelectedDepthStencilFormat = 0;
			const VkFormat vk_eSelectedDepthFormat = vk_aeAvailableDepthStencilFormats[u8IndexToSelectedDepthStencilFormat];
			if (are_depth_stencil_images_separated(vk_eSelectedDepthFormat)) {
				VkImageTiling vk_eDepthImageTiling, vk_eStencilImageTiling;
				VkFormatProperties2 vk_formatProperties;
				get_format_properties(VK_FORMAT_S8_UINT, &vk_formatProperties, nullptr);
				vk_eStencilImageTiling = (vk_formatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0 ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
				vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDeviceSelected, vk_eSelectedDepthFormat, &vk_formatProperties);
				vk_eDepthImageTiling = (vk_formatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0 ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
				// TODO: create separate depth & stencil images
			} else {
				VkFormatProperties2 vk_depthStencilFormatProperties;
				get_format_properties(vk_eSelectedDepthFormat, &vk_depthStencilFormatProperties, nullptr);
				const VkImageTiling vk_eDepthStencilTiling = (vk_depthStencilFormatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0 ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
				// TODO: create depth-stencil images
			}
		} else {
			constexpr VkFormat vk_aeDepthOnlyFormats[] = {
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_X8_D24_UNORM_PACK32,
				VK_FORMAT_D16_UNORM
			};
			uint8_t u8SelectedDepthOnlyFormatIndex;
			for (uint8_t u8DepthOnlyFormatIndex = 0; u8DepthOnlyFormatIndex < sizeof(vk_aeDepthOnlyFormats) / sizeof(vk_aeDepthOnlyFormats[0]); u8DepthOnlyFormatIndex++)
				if (std::find(vk_aeAvailableDepthStencilFormats.rbegin(), vk_aeAvailableDepthStencilFormats.rend(), vk_aeDepthOnlyFormats[u8DepthOnlyFormatIndex]) != vk_aeAvailableDepthStencilFormats.rend()) {
					u8SelectedDepthOnlyFormatIndex = u8DepthOnlyFormatIndex;
					break;
				}
			VkFormatProperties2 vk_depthOnlyFormatProperties;
			get_format_properties(vk_aeDepthOnlyFormats[u8SelectedDepthOnlyFormatIndex], &vk_depthOnlyFormatProperties, nullptr);
			const VkImageTiling vk_eDepthOnlyImageTiling = (vk_depthOnlyFormatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
			// TODO: create depth-only images
		}
		return true;
	}

	void destroy_depth_stencil_buffers() {
		const uint8_t u8ImageCount = vk_ahDepthStencilImages.size() / (are_depth_stencil_images_separated(vk_aeAvailableDepthStencilFormats[u8IndexToSelectedDepthStencilFormat]) ? 1 : 2);
		for (uint8_t u8ImageIndex = 0; u8ImageIndex < u8ImageCount; u8ImageIndex++) {
			vkDestroyImageView(vk_hDevice, vk_ahDepthStencilImageViews[u8ImageIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8ImageIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8ImageIndex], nullptr);
		}
	}

	void enable_stencil_buffer(const bool bEnable) {
		RE_NOTE("Stencil buffers are currently unused and the engine doesn't provide any use cases for them. They will be kept disabled to save memory");
	}

	bool are_stencil_buffers_enabled() {
		return bStencilsEnabled;
	}

}
