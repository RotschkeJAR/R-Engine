#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {
	
	VkDescriptorSet vk_ahSortableDepthDescSets[RE_VK_FRAMES_IN_FLIGHT];

	bool create_sortable_depth_desc_sets() {
		PRINT_DEBUG("Allocating descriptor sets for sortable depth");
		VkDescriptorSetLayout vk_ahDescSetLayouts[RE_VK_FRAMES_IN_FLIGHT];
		std::fill(std::begin(vk_ahDescSetLayouts), std::end(vk_ahDescSetLayouts), vk_hSortableDepthDescSetLayout);
		const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = vk_hPersistentDescPool,
			.descriptorSetCount = RE_VK_FRAMES_IN_FLIGHT,
			.pSetLayouts = vk_ahDescSetLayouts
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, vk_ahSortableDepthDescSets) == VK_SUCCESS) {
			PRINT_DEBUG("Writing to all descriptor sets for sortable depth");
			VkDescriptorBufferInfo vk_aBufferDescs[RE_VK_FRAMES_IN_FLIGHT];
			VkWriteDescriptorSet vk_aWriteSets[RE_VK_FRAMES_IN_FLIGHT];
			for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
				vk_aWriteSets[u8FrameInFlightIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				vk_aWriteSets[u8FrameInFlightIndex].pNext = nullptr;
				vk_aWriteSets[u8FrameInFlightIndex].dstSet = vk_ahSortableDepthDescSets[u8FrameInFlightIndex];
				vk_aWriteSets[u8FrameInFlightIndex].dstBinding = 0;
				vk_aWriteSets[u8FrameInFlightIndex].dstArrayElement = 0;
				vk_aWriteSets[u8FrameInFlightIndex].descriptorCount = 1;
				vk_aWriteSets[u8FrameInFlightIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				vk_aWriteSets[u8FrameInFlightIndex].pImageInfo = nullptr;
				vk_aWriteSets[u8FrameInFlightIndex].pBufferInfo = &vk_aBufferDescs[u8FrameInFlightIndex];
				vk_aWriteSets[u8FrameInFlightIndex].pTexelBufferView = nullptr;
				vk_aBufferDescs[u8FrameInFlightIndex].offset = 0;
				vk_aBufferDescs[u8FrameInFlightIndex].range = VK_WHOLE_SIZE;
				vk_aBufferDescs[u8FrameInFlightIndex].buffer = vk_ahSortableDepthBuffers[u8FrameInFlightIndex];
			}
			vkUpdateDescriptorSets(vk_hDevice, sizeof(vk_aWriteSets) / sizeof(vk_aWriteSets[0]), vk_aWriteSets, 0, nullptr);
			return true;
		} else
			RE_FATAL_ERROR("Failed allocate Vulkan descriptor sets for sortabloe depth");
		return false;
	}

	void destroy_sortable_depth_desc_sets() {
	}

}
