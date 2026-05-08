#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	VkDescriptorSet vk_hCameraDescSet[RE_VK_FRAMES_IN_FLIGHT];

	bool create_camera_descriptor_sets() {
		PRINT_DEBUG("Allocating descriptor sets for camera");
		VkDescriptorSetLayout vk_ahDescSetLayouts[RE_VK_FRAMES_IN_FLIGHT];
		std::fill(std::begin(vk_ahDescSetLayouts), std::end(vk_ahDescSetLayouts), vk_hCameraDescSetLayout);
		const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hPersistentDescPool,
			.descriptorSetCount = RE_VK_FRAMES_IN_FLIGHT,
			.pSetLayouts = vk_ahDescSetLayouts
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, vk_ahRenderContentDescSets) == VK_SUCCESS) {
			PRINT_DEBUG("Writing to all descriptor sets for camera");
			VkDescriptorBufferInfo vk_aBufferDescs[RE_VK_FRAMES_IN_FLIGHT];
			VkWriteDescriptorSet vk_aWriteSets[RE_VK_FRAMES_IN_FLIGHT];
			for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
				vk_aWriteSets[u8FrameInFlightIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				vk_aWriteSets[u8FrameInFlightIndex].pNext = nullptr;
				vk_aWriteSets[u8FrameInFlightIndex].dstSet = vk_hCameraDescSet[u8FrameInFlightIndex];
				vk_aWriteSets[u8FrameInFlightIndex].dstBinding = 0;
				vk_aWriteSets[u8FrameInFlightIndex].dstArrayElement = 0;
				vk_aWriteSets[u8FrameInFlightIndex].descriptorCount = 1;
				vk_aWriteSets[u8FrameInFlightIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				vk_aWriteSets[u8FrameInFlightIndex].pImageInfo = nullptr;
				vk_aWriteSets[u8FrameInFlightIndex].pBufferInfo = &vk_aBufferDescs[u8FrameInFlightIndex];
				vk_aWriteSets[u8FrameInFlightIndex].pTexelBufferView = nullptr;
				vk_aBufferDescs[u8FrameInFlightIndex].buffer = vk_ahCameraBuffers[u8FrameInFlightIndex];
				vk_aBufferDescs[u8FrameInFlightIndex].offset = 0;
				vk_aBufferDescs[u8FrameInFlightIndex].range = VK_WHOLE_SIZE;
			}
			vkUpdateDescriptorSets(vk_hDevice, RE_VK_FRAMES_IN_FLIGHT, vk_aWriteSets, 0, nullptr);
			return true;
		} else
			RE_FATAL_ERROR("Failed to allocate Vulkan descriptor sets for camera");
		return false;
	}

	void destroy_camera_descriptor_sets() {
	}

}
