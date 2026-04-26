#ifndef __RE_GPU_CONSTRAINS_H__
#define __RE_GPU_CONSTRAINS_H__

#include "RE_GPU_Vulkan Device.hpp"

namespace RE {

	enum VulkanEnabledFeatureBits : uint64_t {
		ENABLED_FEATURE_SPARSE_BINDING_BIT				= 0x1,
		ENABLED_FEATURE_SPARSE_RESIDENCY_BUFFER_BIT		= 0x2,
		ENABLED_FEATURE_SPARSE_RESIDENCY_IMAGE_2D_BIT	= 0x4,
		ENABLED_FEATURE_SPARSE_ALIASING_BIT				= 0x8,
		ENABLED_FEATURE_SYNCHRONIZATION_2_BIT			= 0x10,
		ENABLED_FEATURE_SAMPLE_RATE_SHADING_BIT			= 0x20,
		ENABLED_FEATURE_ANISOTROPIC_FILTERING_BIT		= 0x40,
		ENABLED_FEATURE_DYNAMIC_RENDERING_BIT			= 0x80
	};
	typedef uint64_t VulkanEnabledFeatureFlags;
	
	extern VulkanEnabledFeatureFlags mEnabledFeatures;
	extern VkDeviceSize vk_maxBufferSize;
	extern VkDeviceSize vk_maxMemorySize;
	extern VkDeviceSize vk_uniformBufferAlignment;
	extern VkDeviceSize vk_storageBufferAlignment;
	extern VkDeviceSize vk_optimalBufferCopyAlignmentOffset;
	extern VkSampleCountFlags vk_mSupportedMsaaSamples;
	extern uint64_t u64MaxTimelineSemaphoreDifference;
	extern uint32_t u32MaxDescsPerSet;
	extern uint32_t u32MaxImage2DSize;
	extern uint32_t u32MaxImageArrays;
	extern uint32_t u32MaxUniformBufferRange;
	extern uint32_t u32MaxStorageBufferRange;
	extern uint32_t u32MaxPushConstantsBytes;
	extern uint32_t u32MaxMemoryAllocs;
	extern uint32_t u32MaxSamplerAllocs;
	extern uint32_t u32MaxBoundDescSets;
	extern uint32_t u32MaxPerDescSetUniformBuffers;
	extern uint32_t u32MaxPerDescSetStorageBuffers;
	extern uint32_t u32MaxPerDescSetSampledImages;
	extern uint32_t u32MaxPerStageUniformBuffers;
	extern uint32_t u32MaxPerStageStorageBuffers;
	extern uint32_t u32MaxPerStageSampledImages;
	extern uint32_t u32MaxPerStageResources;
	extern uint32_t a3u32MaxComputeWorkGroupCounts[3];
	extern uint32_t u32MaxComputeWorkGroupInvocations;
	extern uint32_t u32MaxIndexedDrawCount;
	extern uint32_t u32MaxIndirectCount;
	extern float f32MaxSamplerAnisotropy;
	extern uint32_t u32MappedMemoryAlignment;

	template <VulkanEnabledFeatureBits... FeatureBits>
	inline bool are_vulkan_features_enabled() {
		const VulkanEnabledFeatureFlags mQueriedFeatureBits = (FeatureBits | ...);
		return (mEnabledFeatures & mQueriedFeatureBits) == mQueriedFeatureBits;
	}

}

#endif /* __RE_GPU_CONSTRAINS_H__ */
