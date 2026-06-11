#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {
	
	VkDescriptorSet vk_hCharacterDescSet;

	bool create_character_descriptor_sets() {
		PRINT_DEBUG("Allocating Vulkan descriptor set pointing at character image");
		const VkDescriptorSetAllocateInfo vk_allocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = vk_hPersistentDescPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &vk_hCharacterDescSetLayout
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_allocInfo, &vk_hCharacterDescSet) == VK_SUCCESS) {
			PRINT_DEBUG("Writing to Vulkan descriptor set ", vk_hCharacterDescSet, " to point at character image ", vk_hCharacterImageView);
			const VkDescriptorImageInfo vk_imageInfo = {
				.sampler = vk_hDefaultSampler,
				.imageView = vk_hCharacterImageView,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};
			const VkWriteDescriptorSet vk_writeInfo = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = vk_hCharacterDescSet,
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.pImageInfo = &vk_imageInfo,
				.pBufferInfo = nullptr,
				.pTexelBufferView = nullptr
			};
			vkUpdateDescriptorSets(vk_hDevice, 1, &vk_writeInfo, 0, nullptr);
			return true;
		} else
			RE_FATAL_ERROR("Failed to create Vulkan descriptor set pointing at character image");
		return false;
	}

	void destroy_character_descriptor_sets() {
	}

}
