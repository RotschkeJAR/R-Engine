#include "RE_GPU_Internal.hpp"

namespace RE {

	VkDeviceSize vk_maxBufferSize;
	VkDeviceSize vk_maxMemorySize;
	VkDeviceSize vk_uniformBufferAlignment;
	VkDeviceSize vk_storageBufferAlignment;
	VkDeviceSize vk_optimalBufferCopyAlignmentOffset;
	VkSampleCountFlags vk_mSupportedMsaaSamples;
	uint64_t u64MaxTimelineSemaphoreDifference;
	uint32_t u32MaxDescsPerSet;
	uint32_t u32MaxImage2DSize;
	uint32_t u32MaxImageArrays;
	uint32_t u32MaxUniformBufferRange;
	uint32_t u32MaxStorageBufferRange;
	uint32_t u32MaxPushConstantsBytes;
	uint32_t u32MaxMemoryAllocs;
	uint32_t u32MaxSamplerAllocs;
	uint32_t u32MaxBoundDescSets;
	uint32_t u32MaxPerDescSetUniformBuffers;
	uint32_t u32MaxPerDescSetStorageBuffers;
	uint32_t u32MaxPerDescSetSampledImages;
	uint32_t u32MaxPerStageUniformBuffers;
	uint32_t u32MaxPerStageStorageBuffers;
	uint32_t u32MaxPerStageSampledImages;
	uint32_t u32MaxPerStageResources;
	uint32_t a3u32MaxComputeWorkGroupCounts[3];
	uint32_t u32MaxComputeWorkGroupInvocations;
	uint32_t u32MaxIndexedDrawCount;
	uint32_t u32MaxIndirectCount;
	float f32MaxSamplerAnisotropy;
	uint32_t u32MappedMemoryAlignment;
	uint8_t m8EnabledFeatures;
	
	void fetch_gpu_constrains() {
		{ // Hard-/Software Limits
			VkPhysicalDeviceVulkan13Properties vk_physicalDeviceProperties_1_3;
			vk_physicalDeviceProperties_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
			vk_physicalDeviceProperties_1_3.pNext = nullptr;
			VkPhysicalDeviceVulkan12Properties vk_physicalDeviceProperties_1_2;
			vk_physicalDeviceProperties_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
			vk_physicalDeviceProperties_1_2.pNext = &vk_physicalDeviceProperties_1_3;
			VkPhysicalDeviceVulkan11Properties vk_physicalDeviceProperties_1_1;
			vk_physicalDeviceProperties_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
			vk_physicalDeviceProperties_1_1.pNext = &vk_physicalDeviceProperties_1_2;
			VkPhysicalDeviceProperties2 vk_physicalDeviceProperties_1_0;
			vk_physicalDeviceProperties_1_0.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			vk_physicalDeviceProperties_1_0.pNext = &vk_physicalDeviceProperties_1_1;
			const VkPhysicalDeviceProperties &vk_physicalDeviceProperties = vk_physicalDeviceProperties_1_0.properties;
			const VkPhysicalDeviceLimits &vk_physicalDeviceLimits = vk_physicalDeviceProperties.limits;
			[[maybe_unused]] const VkPhysicalDeviceSparseProperties &vk_physicalDeviceSparseProperties = vk_physicalDeviceProperties.sparseProperties;
			vkGetPhysicalDeviceProperties2(SELECTED_PHYSICAL_VULKAN_DEVICE, &vk_physicalDeviceProperties_1_0);

			vk_maxBufferSize = vk_physicalDeviceProperties_1_3.maxBufferSize;
			vk_maxMemorySize = vk_physicalDeviceProperties_1_1.maxMemoryAllocationSize;
			vk_uniformBufferAlignment = vk_physicalDeviceLimits.minUniformBufferOffsetAlignment;
			vk_storageBufferAlignment = vk_physicalDeviceLimits.minStorageBufferOffsetAlignment;
			vk_optimalBufferCopyAlignmentOffset = vk_physicalDeviceLimits.optimalBufferCopyOffsetAlignment;
			vk_mSupportedMsaaSamples = vk_physicalDeviceLimits.framebufferColorSampleCounts
					& vk_physicalDeviceLimits.framebufferDepthSampleCounts
					& vk_physicalDeviceLimits.framebufferStencilSampleCounts;
			{
				constexpr VkFormat vk_aeDepthStencilFormats[] = {
					VK_FORMAT_D16_UNORM,
					VK_FORMAT_X8_D24_UNORM_PACK32,
					VK_FORMAT_D32_SFLOAT,
					VK_FORMAT_S8_UINT,
					VK_FORMAT_D16_UNORM_S8_UINT,
					VK_FORMAT_D24_UNORM_S8_UINT,
					VK_FORMAT_D32_SFLOAT_S8_UINT
				};
				VkPhysicalDeviceImageFormatInfo2 vk_depthStencilImageFormat;
				vk_depthStencilImageFormat.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
				vk_depthStencilImageFormat.pNext = nullptr;
				vk_depthStencilImageFormat.type = VK_IMAGE_TYPE_2D;
				vk_depthStencilImageFormat.tiling = VK_IMAGE_TILING_OPTIMAL;
				vk_depthStencilImageFormat.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				vk_depthStencilImageFormat.flags = 0;
				VkImageFormatProperties2 vk_depthStencilImageFormatProperties;
				vk_depthStencilImageFormatProperties.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
				vk_depthStencilImageFormatProperties.pNext = nullptr;
				for (const VkFormat vk_eDepthStencilFormat : vk_aeDepthStencilFormats) {
					switch (vkGetPhysicalDeviceImageFormatProperties2(SELECTED_PHYSICAL_VULKAN_DEVICE, &vk_depthStencilImageFormat, &vk_depthStencilImageFormatProperties)) {
						case VK_SUCCESS:
							vk_mSupportedMsaaSamples &= vk_depthStencilImageFormatProperties.imageFormatProperties.sampleCounts;
							break;
						case VK_ERROR_FORMAT_NOT_SUPPORTED:
							break;
						default:
							RE_ERROR("Failed to fetch image format properties of depth-stencil-format ", std::hex, vk_eDepthStencilFormat);
					}
				}
			}
			u64MaxTimelineSemaphoreDifference = vk_physicalDeviceProperties_1_2.maxTimelineSemaphoreValueDifference;
			u32MaxDescsPerSet = vk_physicalDeviceProperties_1_1.maxPerSetDescriptors;
			u32MaxImage2DSize = vk_physicalDeviceLimits.maxImageDimension2D;
			u32MaxImageArrays = vk_physicalDeviceLimits.maxImageArrayLayers;
			u32MaxUniformBufferRange = vk_physicalDeviceLimits.maxUniformBufferRange;
			u32MaxStorageBufferRange = vk_physicalDeviceLimits.maxStorageBufferRange;
			u32MaxPushConstantsBytes = vk_physicalDeviceLimits.maxPushConstantsSize;
			u32MaxMemoryAllocs = vk_physicalDeviceLimits.maxMemoryAllocationCount;
			u32MaxSamplerAllocs = vk_physicalDeviceLimits.maxSamplerAllocationCount;
			u32MaxBoundDescSets = vk_physicalDeviceLimits.maxBoundDescriptorSets;
			u32MaxPerDescSetUniformBuffers = vk_physicalDeviceLimits.maxDescriptorSetUniformBuffers;
			u32MaxPerDescSetStorageBuffers = vk_physicalDeviceLimits.maxDescriptorSetStorageBuffers;
			u32MaxPerDescSetSampledImages = vk_physicalDeviceLimits.maxDescriptorSetSampledImages;
			u32MaxPerStageUniformBuffers = vk_physicalDeviceLimits.maxPerStageDescriptorUniformBuffers;
			u32MaxPerStageStorageBuffers = vk_physicalDeviceLimits.maxPerStageDescriptorStorageBuffers;
			u32MaxPerStageSampledImages = vk_physicalDeviceLimits.maxPerStageDescriptorSampledImages;
			u32MaxPerStageResources = vk_physicalDeviceLimits.maxPerStageResources;
			for (uint8_t u8AxisIndex = 0; u8AxisIndex < 3; u8AxisIndex++)
				a3u32MaxComputeWorkGroupCounts[u8AxisIndex] = vk_physicalDeviceLimits.maxComputeWorkGroupCount[u8AxisIndex];
			u32MaxComputeWorkGroupInvocations = vk_physicalDeviceLimits.maxComputeWorkGroupInvocations;
			u32MaxIndexedDrawCount = vk_physicalDeviceLimits.maxDrawIndexedIndexValue;
			u32MaxIndirectCount = vk_physicalDeviceLimits.maxDrawIndirectCount;
			f32MaxSamplerAnisotropy = vk_physicalDeviceLimits.maxSamplerAnisotropy;
			u32MappedMemoryAlignment = vk_physicalDeviceLimits.minMemoryMapAlignment;
		}

		{ // Features
			VkPhysicalDeviceVulkan13Features vk_physicalDeviceFeatures_1_3;
			vk_physicalDeviceFeatures_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
			vk_physicalDeviceFeatures_1_3.pNext = nullptr;
			VkPhysicalDeviceVulkan12Features vk_physicalDeviceFeatures_1_2;
			vk_physicalDeviceFeatures_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
			vk_physicalDeviceFeatures_1_2.pNext = &vk_physicalDeviceFeatures_1_3;
			VkPhysicalDeviceVulkan11Features vk_physicalDeviceFeatures_1_1;
			vk_physicalDeviceFeatures_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
			vk_physicalDeviceFeatures_1_1.pNext = &vk_physicalDeviceFeatures_1_2;
			VkPhysicalDeviceFeatures2 vk_physicalDeviceFeatures_1_0;
			vk_physicalDeviceFeatures_1_0.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			vk_physicalDeviceFeatures_1_0.pNext = &vk_physicalDeviceFeatures_1_1;
			const VkPhysicalDeviceFeatures &vk_physicalDeviceFeatures = vk_physicalDeviceFeatures_1_0.features;
			vkGetPhysicalDeviceFeatures2(SELECTED_PHYSICAL_VULKAN_DEVICE, &vk_physicalDeviceFeatures_1_0);

			m8EnabledFeatures = 0;
			if (vk_physicalDeviceFeatures.sparseBinding)
				m8EnabledFeatures |= ENABLED_FEATURE_SPARSE_BINDING_BIT;
			if (vk_physicalDeviceFeatures.sparseResidencyBuffer)
				m8EnabledFeatures |= ENABLED_FEATURE_SPARSE_RESIDENCY_BUFFER_BIT;
			if (vk_physicalDeviceFeatures.sparseResidencyImage2D)
				m8EnabledFeatures |= ENABLED_FEATURE_SPARSE_RESIDENCY_IMAGE_2D_BIT;
			if (vk_physicalDeviceFeatures.sparseResidencyAliased)
				m8EnabledFeatures |= ENABLED_FEATURE_SPARSE_ALIASING_BIT;
			if (vk_physicalDeviceFeatures_1_3.synchronization2)
				m8EnabledFeatures |= ENABLED_FEATURE_SYNCHRONIZATION_2_BIT;
			if (vk_physicalDeviceFeatures.sampleRateShading)
				m8EnabledFeatures |= ENABLED_FEATURE_SAMPLE_RATE_SHADING_BIT;
			if (vk_physicalDeviceFeatures.samplerAnisotropy)
				m8EnabledFeatures |= ENABLED_FEATURE_ANISOTROPIC_FILTERING_BIT;
			if (vk_physicalDeviceFeatures_1_3.dynamicRendering)
				m8EnabledFeatures |= ENABLED_FEATURE_DYNAMIC_RENDERING_BIT;
		}
	}

}
