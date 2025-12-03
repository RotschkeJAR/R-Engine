#include "RE_Renderer_Internal.hpp"

namespace RE {

	VkDescriptorSetLayout vk_hTextureDescLayout;
	VkDescriptorSet vk_hTextureDescSet;
	
	bool does_gpu_support_textures(const VkPhysicalDevice vk_hPhysicalDevice, const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits, std::queue<std::string> &rMissingFeatures, std::queue<std::string> &rDiscrepantFeatures) {
		PRINT_DEBUG("Checking if physical Vulkan device ", vk_hPhysicalDevice, " supports textures");
		constexpr VkFormatFeatureFlags vk_eRequiredTextureFormatFeatures = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
		VkFormatProperties2 vk_textureFormatFeatures;
		vk_textureFormatFeatures.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_textureFormatFeatures.pNext = nullptr;
		VkPhysicalDeviceImageFormatInfo2 vk_imageFormat;
		vk_imageFormat.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
		vk_imageFormat.pNext = nullptr;
		vk_imageFormat.type = VK_IMAGE_TYPE_2D;
		vk_imageFormat.tiling = VK_IMAGE_TILING_OPTIMAL;
		vk_imageFormat.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		vk_imageFormat.flags = 0;
		VkImageFormatProperties2 vk_imageFormatProperties;
		vk_imageFormatProperties.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
		vk_imageFormatProperties.pNext = nullptr;
		constexpr VkFormat vk_aeTextureFormats[] = {
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FORMAT_B8G8R8A8_SRGB,
			VK_FORMAT_R8G8B8_SRGB,
			VK_FORMAT_B8G8R8_SRGB,
			VK_FORMAT_R8G8_SRGB,
			VK_FORMAT_R8_SRGB
		};
		bool a4bTextureFormatsSupported[4] = {};
		for (const VkFormat vk_eTextureFormat : vk_aeTextureFormats) {
			if ((vk_eTextureFormat == VK_FORMAT_B8G8R8A8_SRGB && a4bTextureFormatsSupported[0]) || (vk_eTextureFormat == VK_FORMAT_B8G8R8_SRGB && a4bTextureFormatsSupported[1]))
				continue;
			PRINT_DEBUG("Querying properties for Vulkan texture format ", std::hex, vk_eTextureFormat);
			vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eTextureFormat, &vk_textureFormatFeatures);
			if ((vk_textureFormatFeatures.formatProperties.optimalTilingFeatures & vk_eRequiredTextureFormatFeatures) == vk_eRequiredTextureFormatFeatures) {
				vk_imageFormat.format = vk_eTextureFormat;
				PRINT_DEBUG("Querying properties of Vulkan images with the format");
				switch (vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDevice, &vk_imageFormat, &vk_imageFormatProperties)) {
					case VK_SUCCESS:
						switch (vk_eTextureFormat) {
							case VK_FORMAT_R8G8B8A8_SRGB:
							case VK_FORMAT_B8G8R8A8_SRGB:
								a4bTextureFormatsSupported[0] = true;
								break;
							case VK_FORMAT_R8G8B8_SRGB:
							case VK_FORMAT_B8G8R8_SRGB:
								a4bTextureFormatsSupported[1] = true;
								break;
							case VK_FORMAT_R8G8_SRGB:
								a4bTextureFormatsSupported[2] = true;
								break;
							case VK_FORMAT_R8_SRGB:
								a4bTextureFormatsSupported[3] = true;
								break;
							default:
								RE_FATAL_ERROR("Unknown Vulkan format used to determine GPU's suitability for textures");
								std::abort();
						}
						break;
					case VK_ERROR_FORMAT_NOT_SUPPORTED:
						PRINT_DEBUG("Vulkan format is not supported for images by the GPU");
						break;
					default:
						return false;
				}
			}
		}

		if (!a4bTextureFormatsSupported[0])
			rMissingFeatures.emplace("The required format features for textures or texture creation with RGBA- and BGRA-channels aren't supported, which is mandatory for common GPUs");
		if (!a4bTextureFormatsSupported[1])
			rDiscrepantFeatures.emplace("The required format features for textures or texture creation with RGB- and BGR-channels aren't supported");
		if (!a4bTextureFormatsSupported[2])
			rDiscrepantFeatures.emplace("The required format features for textures or texture creation with RG-channels (grayscale with alpha) aren't supported");
		if (!a4bTextureFormatsSupported[3])
			rDiscrepantFeatures.emplace("The required format features for textures or texture creation with R-channel only (grayscale) aren't supported");

		if (std::min(vk_rPhysicalDeviceLimits.maxPerStageDescriptorSamplers, vk_rPhysicalDeviceLimits.maxPerStageDescriptorSampledImages) < RE_VK_MAX_SAMPLED_IMAGES)
			rDiscrepantFeatures.push(append_to_string("The maximum amount of bound sampler objects/sampled images should be at least ", std::hex, RE_VK_MAX_SAMPLED_IMAGES, std::dec));

		return a4bTextureFormatsSupported[0];
	}

	int32_t rate_gpu_texture_capacity(const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits) {
		const int32_t i32Score = (static_cast<int32_t>(std::clamp<uint32_t>(std::min(vk_rPhysicalDeviceLimits.maxMemoryAllocationCount, std::min(vk_rPhysicalDeviceLimits.maxSamplerAllocationCount, std::min(vk_rPhysicalDeviceLimits.maxPerStageDescriptorSamplers, std::min(vk_rPhysicalDeviceLimits.maxPerStageDescriptorSampledImages, std::min(vk_rPhysicalDeviceLimits.maxDescriptorSetSamplers, vk_rPhysicalDeviceLimits.maxDescriptorSetSampledImages))))), 16, RE_VK_MAX_SAMPLED_IMAGES) - 16) * 1500 / RE_VK_MAX_SAMPLED_IMAGES - 499) + (static_cast<int32_t>(std::clamp<uint32_t>(vk_rPhysicalDeviceLimits.maxImageDimension2D, 0, 8192)) * 1500 / 8192 - 499);
		PRINT_DEBUG("Rated GPU's texture capacity with score ", i32Score);
		return i32Score;
	}

	bool create_texture_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan descriptor set layout for textures");
		constexpr VkDescriptorBindingFlags vk_eBindingFlags = VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
		const VkDescriptorSetLayoutBindingFlagsCreateInfo vk_bindingFlagsInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
			.bindingCount = 1,
			.pBindingFlags = &vk_eBindingFlags
		};
		const VkDescriptorSetLayoutBinding vk_textureDescBinding = {
			.binding = RE_VK_UNIFORM_BINDING_TEXTURE,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = RE_VK_MAX_SAMPLED_IMAGES,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
		};
		const VkDescriptorSetLayoutCreateInfo vk_textureDescLayoutInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = &vk_bindingFlagsInfo,
			.bindingCount = 1,
			.pBindings = &vk_textureDescBinding
		};
		if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_textureDescLayoutInfo, nullptr, &vk_hTextureDescLayout) == VK_SUCCESS) {
			PRINT_DEBUG("Allocating Vulkan descriptor set for using textures");
			const VkDescriptorSetAllocateInfo vk_textureDescSetAllocInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.descriptorPool = vk_hPermanentDescPool,
				.descriptorSetCount = 1,
				.pSetLayouts = &vk_hTextureDescLayout
			};
			if (vkAllocateDescriptorSets(vk_hDevice, &vk_textureDescSetAllocInfo, &vk_hTextureDescSet) == VK_SUCCESS)
				return true;
			else
				RE_FATAL_ERROR("Failed allocating Vulkan descriptor set with layout ", vk_hTextureDescLayout, " in pool ", vk_hPermanentDescPool, " for textures");
			PRINT_DEBUG("Destroying Vulkan descriptor set layout due to failure allocating the texture descriptor set");
			vkDestroyDescriptorSetLayout(vk_hDevice, vk_hTextureDescLayout, nullptr);
		} else
			RE_FATAL_ERROR("Failed creating Vulkan descriptor set layout for textures");
		return false;
	}

	void destroy_texture_descriptor_sets() {
		PRINT_DEBUG("Destroying Vulkan descriptor set layout ", vk_hTextureDescLayout, " used for textures");
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hTextureDescLayout, nullptr);
	}

}
