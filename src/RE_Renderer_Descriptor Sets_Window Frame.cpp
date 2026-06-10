#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

#ifdef RE_OS_LINUX

	VkDescriptorSet vk_hWindowFrameDescSet;

	bool create_window_frame_descriptor_sets() {
		PRINT_DEBUG("Allocating descriptor sets for cursor");
		const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hPersistentDescPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &vk_hWindowFrameDescSetLayout
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, &vk_hWindowFrameDescSet) == VK_SUCCESS) {
			PRINT_DEBUG("Writing to all descriptor sets for cursor");
			const VkDescriptorBufferInfo vk_bufferDesc = {
				.buffer = vk_hCursorBuffer,
				.offset = 0,
				.range = sizeof(CursorShaderData)
			};
			const VkDescriptorImageInfo vk_imageDesc = {
				.sampler = vk_hDefaultSampler,
				.imageView = vk_hWindowButtonImageView,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};
			const VkWriteDescriptorSet vk_aWriteSets[] = {
				{
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.pNext = nullptr,
					.dstSet = vk_hWindowFrameDescSet,
					.dstBinding = 0,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.pImageInfo = nullptr,
					.pBufferInfo = &vk_bufferDesc,
					.pTexelBufferView = nullptr
				}, {
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.pNext = nullptr,
					.dstSet = vk_hWindowFrameDescSet,
					.dstBinding = 1,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.pImageInfo = &vk_imageDesc,
					.pBufferInfo = nullptr,
					.pTexelBufferView = nullptr
				}
			};
			vkUpdateDescriptorSets(vk_hDevice, sizeof(vk_aWriteSets) / sizeof(vk_aWriteSets[0]), vk_aWriteSets, 0, nullptr);
			return true;
		} else
			RE_FATAL_ERROR("Failed to allocate Vulkan descriptor sets for cursor");
		return false;
	}

	void destroy_window_frame_descriptor_sets() {
	}

#else

	bool create_window_frame_descriptor_sets() {
		return true;
	}

	void destroy_window_frame_descriptor_sets() {
	}

#endif

}
