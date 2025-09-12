#include "RE_Renderer_Internal.hpp"

namespace RE {

#define RE_VK_MAX_SAMPLED_IMAGES 0x7FFF
	
	bool does_gpu_support_textures(const VkPhysicalDevice vk_hPhysicalDevice, const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits, std::queue<std::string> &rMissingFeatures, std::queue<std::string> &rDiscrepantFeatures) {
		constexpr VkFormatFeatureFlags vk_eRequiredTextureFormatFeatures = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
		VkFormatProperties2 vk_textureFormatFeatures;
		vk_textureFormatFeatures.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_textureFormatFeatures.pNext = nullptr;
		vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, VK_FORMAT_R8G8B8A8_SRGB, &vk_textureFormatFeatures);
		bool bSuitable = (vk_textureFormatFeatures.formatProperties.optimalTilingFeatures & vk_eRequiredTextureFormatFeatures) == vk_eRequiredTextureFormatFeatures;
		if (!bSuitable) {
			vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, VK_FORMAT_B8G8R8A8_SRGB, &vk_textureFormatFeatures);
			bSuitable = (vk_textureFormatFeatures.formatProperties.optimalTilingFeatures & vk_eRequiredTextureFormatFeatures) == vk_eRequiredTextureFormatFeatures;
			if (!bSuitable)
				rMissingFeatures.emplace("The required features for textures with RGBA- and BGRA-channels aren't supported");
		}
		vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, VK_FORMAT_R8G8B8_SRGB, &vk_textureFormatFeatures);
		if ((vk_textureFormatFeatures.formatProperties.optimalTilingFeatures & vk_eRequiredTextureFormatFeatures) != vk_eRequiredTextureFormatFeatures)
			rDiscrepantFeatures.emplace("The required features for textures with RGB-channels aren't supported");
		vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, VK_FORMAT_B8G8R8_SRGB, &vk_textureFormatFeatures);
		if ((vk_textureFormatFeatures.formatProperties.optimalTilingFeatures & vk_eRequiredTextureFormatFeatures) != vk_eRequiredTextureFormatFeatures)
			rDiscrepantFeatures.emplace("The required features for textures with BGR-channels aren't supported");
		vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, VK_FORMAT_R8G8_SRGB, &vk_textureFormatFeatures);
		if ((vk_textureFormatFeatures.formatProperties.optimalTilingFeatures & vk_eRequiredTextureFormatFeatures) != vk_eRequiredTextureFormatFeatures)
			rDiscrepantFeatures.emplace("The required features for textures with RG-channels aren't supported");
		vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, VK_FORMAT_R8_SRGB, &vk_textureFormatFeatures);
		if ((vk_textureFormatFeatures.formatProperties.optimalTilingFeatures & vk_eRequiredTextureFormatFeatures) != vk_eRequiredTextureFormatFeatures)
			rDiscrepantFeatures.emplace("The required features for textures with R-channel only aren't supported");
		if (std::min(vk_rPhysicalDeviceLimits.maxPerStageDescriptorSamplers, vk_rPhysicalDeviceLimits.maxPerStageDescriptorSampledImages) < RE_VK_MAX_SAMPLED_IMAGES)
			rDiscrepantFeatures.push(append_to_string("The maximum amount of bound sampler objects/sampled images should be at least ", hexadecimal_to_string(RE_VK_MAX_SAMPLED_IMAGES)));
		return bSuitable;
	}

	int32_t rate_gpu_texture_capacity(const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits) {
		return (static_cast<int32_t>(std::clamp<uint32_t>(std::min(vk_rPhysicalDeviceLimits.maxPerStageDescriptorSamplers, vk_rPhysicalDeviceLimits.maxPerStageDescriptorSampledImages), 16U, RE_VK_MAX_SAMPLED_IMAGES) - 16U) * 1500 / RE_VK_MAX_SAMPLED_IMAGES - 499) + (static_cast<int32_t>(std::clamp<uint32_t>(vk_rPhysicalDeviceLimits.maxImageDimension2D, 0U, 8192U)) * 1500 / 8192 - 499);
	}

}
