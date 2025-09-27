#include "RE_Renderer_Internal.hpp"

namespace RE {

#define RE_VK_MAX_SAMPLED_IMAGES 0x7FFF

	std::array<VulkanTask, 1> textureCreationTasks;
	
	bool does_gpu_support_textures(const VkPhysicalDevice vk_hPhysicalDevice, const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits, std::queue<std::string> &rMissingFeatures, std::queue<std::string> &rDiscrepantFeatures) {
		constexpr VkFormatFeatureFlags vk_eRequiredTextureFormatFeatures = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
		VkFormatProperties2 vk_textureFormatFeatures;
		vk_textureFormatFeatures.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_textureFormatFeatures.pNext = nullptr;
		VkPhysicalDeviceImageFormatInfo2 vk_imageFormat;
		vk_imageFormat.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
		vk_imageFormat.pNext = nullptr;
		vk_imageFormat.type = VK_IMAGE_TYPE_2D;
		vk_imageFormat.tiling = VK_IMAGE_TILING_OPTIMAL;
		vk_imageFormat.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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
			vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eTextureFormat, &vk_textureFormatFeatures);
			if ((vk_textureFormatFeatures.formatProperties.optimalTilingFeatures & vk_eRequiredTextureFormatFeatures) == vk_eRequiredTextureFormatFeatures) {
				vk_imageFormat.format = vk_eTextureFormat;
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
		return (static_cast<int32_t>(std::clamp<uint32_t>(std::min(vk_rPhysicalDeviceLimits.maxPerStageDescriptorSamplers, vk_rPhysicalDeviceLimits.maxPerStageDescriptorSampledImages), 16, RE_VK_MAX_SAMPLED_IMAGES) - 16) * 1500 / RE_VK_MAX_SAMPLED_IMAGES - 499) + (static_cast<int32_t>(std::clamp<uint32_t>(vk_rPhysicalDeviceLimits.maxImageDimension2D, 0, 8192)) * 1500 / 8192 - 499);
	}

	bool create_texture_descriptor_sets() {
		const uint8_t au8LogicalQueueIndices[] = {
			u8LogicalQueueCount,
			get_render_graphics_queue_logical_index()
		};
		constexpr VkQueueFlagBits vk_aeQueueTypes[] = {
			VK_QUEUE_TRANSFER_BIT,
			VK_QUEUE_GRAPHICS_BIT
		};
		constexpr uint32_t au32SeparatedWorks[] = { 0, 1 };
		const VulkanTask_Queues textureCreationQueues = {
			.pau8LogicalQueueIndices = au8LogicalQueueIndices,
			.vk_paeQueueTypes = vk_aeQueueTypes,
			.pau32StrictSeparationIds = au32SeparatedWorks,
			.u32FunctionsCount = sizeof(au8LogicalQueueIndices) / sizeof(au8LogicalQueueIndices[0])
		};
		if (textureCreationTasks[0].init(textureCreationQueues, false)) {
			uint16_t u16TextureCreationTaskInitIndex = 1;
			while (u16TextureCreationTaskInitIndex < textureCreationTasks.size()) {
				if (!textureCreationTasks[u16TextureCreationTaskInitIndex].init(textureCreationTasks[0]))
					break;
				u16TextureCreationTaskInitIndex++;
			}
			if (u16TextureCreationTaskInitIndex == textureCreationTasks.size())
				return true;
			for (uint16_t u16TextureCreationTaskDestroyIndex = 0; u16TextureCreationTaskDestroyIndex < u16TextureCreationTaskInitIndex; u16TextureCreationTaskDestroyIndex++)
				textureCreationTasks[u16TextureCreationTaskDestroyIndex].destroy();
		}
		return false;
	}

	void destroy_texture_descriptor_sets() {
		for (VulkanTask &rTask : textureCreationTasks)
			rTask.destroy();
	}

}
