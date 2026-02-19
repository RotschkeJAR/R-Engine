#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	VkDescriptorSet vk_ahRawGameObjectBufferDescSets[RE_VK_FRAMES_IN_FLIGHT];

	bool create_raw_game_object_buffer_descriptor_sets() {
		VkDescriptorSetLayout vk_ahSetLayouts[RE_VK_FRAMES_IN_FLIGHT];
		std::fill(std::begin(vk_ahSetLayouts), std::end(vk_ahSetLayouts), vk_hRawGameObjectBufferDescSetLayout);
		const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hPersistentDescPool,
			.descriptorSetCount = RE_VK_FRAMES_IN_FLIGHT,
			.pSetLayouts = vk_ahSetLayouts
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, vk_ahRawGameObjectBufferDescSets) == VK_SUCCESS) {
			VkDescriptorBufferInfo vk_aBufferInfos[RE_VK_FRAMES_IN_FLIGHT];
			VkWriteDescriptorSet vk_aWriteSets[RE_VK_FRAMES_IN_FLIGHT];
			for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
				vk_aBufferInfos[u8FrameInFlightIndex].buffer = aRawGameObjectBuffers[u8FrameInFlightIndex].get();
				vk_aBufferInfos[u8FrameInFlightIndex].offset = 0;
				vk_aBufferInfos[u8FrameInFlightIndex].range = VK_WHOLE_SIZE;
				vk_aWriteSets[u8FrameInFlightIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				vk_aWriteSets[u8FrameInFlightIndex].pNext = nullptr;
				vk_aWriteSets[u8FrameInFlightIndex].dstSet = vk_ahRawGameObjectBufferDescSets[u8FrameInFlightIndex];
				vk_aWriteSets[u8FrameInFlightIndex].dstBinding = 0;
				vk_aWriteSets[u8FrameInFlightIndex].dstArrayElement = 0;
				vk_aWriteSets[u8FrameInFlightIndex].descriptorCount = 1;
				vk_aWriteSets[u8FrameInFlightIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				vk_aWriteSets[u8FrameInFlightIndex].pImageInfo = nullptr;
				vk_aWriteSets[u8FrameInFlightIndex].pBufferInfo = &vk_aBufferInfos[u8FrameInFlightIndex];
				vk_aWriteSets[u8FrameInFlightIndex].pTexelBufferView = nullptr;
			}
			vkUpdateDescriptorSets(vk_hDevice, RE_VK_FRAMES_IN_FLIGHT, vk_aWriteSets, 0, nullptr);
			return true;
		}
		return false;
	}

	void destroy_raw_game_object_buffer_descriptor_sets() {
	}

}