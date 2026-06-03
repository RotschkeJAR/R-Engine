#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

#ifdef RE_OS_LINUX

	VkDescriptorSet vk_ahCursorDescSets[RE_VK_FRAMES_IN_FLIGHT];

	bool create_cursor_descriptor_sets() {
		PRINT_DEBUG("Allocating descriptor sets for cursor");
		VkDescriptorSetLayout vk_ahDescSetLayouts[RE_VK_FRAMES_IN_FLIGHT];
		std::fill(std::begin(vk_ahDescSetLayouts), std::end(vk_ahDescSetLayouts), vk_hCursorDescSetLayout);
		const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hPersistentDescPool,
			.descriptorSetCount = RE_VK_FRAMES_IN_FLIGHT,
			.pSetLayouts = vk_ahDescSetLayouts
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, vk_ahCursorDescSets) == VK_SUCCESS) {
			PRINT_DEBUG("Writing to all descriptor sets for cursor");
			VkDescriptorBufferInfo vk_bufferDescs[RE_VK_FRAMES_IN_FLIGHT];
			VkWriteDescriptorSet vk_writeSets[RE_VK_FRAMES_IN_FLIGHT];
			for (uint16_t u16DescSetIndex = 0; u16DescSetIndex < RE_VK_FRAMES_IN_FLIGHT; u16DescSetIndex++) {
				vk_writeSets[u16DescSetIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				vk_writeSets[u16DescSetIndex].pNext = nullptr;
				vk_writeSets[u16DescSetIndex].dstSet = vk_ahCursorDescSets[u16DescSetIndex];
				vk_writeSets[u16DescSetIndex].dstBinding = 0;
				vk_writeSets[u16DescSetIndex].dstArrayElement = 0;
				vk_writeSets[u16DescSetIndex].descriptorCount = 1;
				vk_writeSets[u16DescSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				vk_writeSets[u16DescSetIndex].pImageInfo = nullptr;
				vk_writeSets[u16DescSetIndex].pBufferInfo = &vk_bufferDescs[u16DescSetIndex];
				vk_writeSets[u16DescSetIndex].pTexelBufferView = nullptr;
				vk_bufferDescs[u16DescSetIndex].buffer = vk_hCursorBuffer;
				vk_bufferDescs[u16DescSetIndex].offset = next_multiple_inclusive<VkDeviceSize>(sizeof(CursorShaderData), vk_uniformBufferAlignment) * u16DescSetIndex;
				vk_bufferDescs[u16DescSetIndex].range = sizeof(CursorShaderData);
			}
			vkUpdateDescriptorSets(vk_hDevice, RE_VK_FRAMES_IN_FLIGHT, vk_writeSets, 0, nullptr);
			return true;
		} else
			RE_FATAL_ERROR("Failed to allocate Vulkan descriptor sets for cursor");
		return false;
	}

	void destroy_cursor_descriptor_sets() {
	}

#else

	bool create_cursor_descriptor_sets() {
		return true;
	}

	void destroy_cursor_descriptor_sets() {
	}

#endif

}
