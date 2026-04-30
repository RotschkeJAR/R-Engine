#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	VkDescriptorSet vk_ahRenderContentDescSets[RE_VK_FRAMES_IN_FLIGHT];
	
	bool create_render_content_descriptor_sets() {
		PRINT_DEBUG("Allocating descriptor sets for processing");
		VkDescriptorSetLayout vk_ahDescSetLayouts[RE_VK_FRAMES_IN_FLIGHT];
		std::fill(std::begin(vk_ahDescSetLayouts), std::end(vk_ahDescSetLayouts), vk_hRenderContentDescSetLayout);
		const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hPersistentDescPool,
			.descriptorSetCount = RE_VK_FRAMES_IN_FLIGHT,
			.pSetLayouts = vk_ahDescSetLayouts
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, vk_ahRenderContentDescSets) == VK_SUCCESS) {
			VkDescriptorBufferInfo vk_aaBufferDescs[RE_VK_DESC_SET_RENDER_CONTENT_BINDING_COUNT][RE_VK_FRAMES_IN_FLIGHT];
			VkWriteDescriptorSet vk_aWriteSets[RE_VK_DESC_SET_RENDER_CONTENT_BINDING_COUNT * RE_VK_FRAMES_IN_FLIGHT];
			for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
				for (uint32_t u32BindingIndex = 0; u32BindingIndex < RE_VK_DESC_SET_RENDER_CONTENT_BINDING_COUNT; u32BindingIndex++) {
					PRINT_DEBUG("Setting write information for descriptor set at binding ", u32BindingIndex, " and frame-in-flight index ", u8FrameInFlightIndex);
					const uint32_t u32WriteSetIndex = u8FrameInFlightIndex * RE_VK_DESC_SET_RENDER_CONTENT_BINDING_COUNT + u32BindingIndex;
					vk_aWriteSets[u32WriteSetIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					vk_aWriteSets[u32WriteSetIndex].pNext = nullptr;
					vk_aWriteSets[u32WriteSetIndex].dstSet = vk_ahRenderContentDescSets[u8FrameInFlightIndex];
					vk_aWriteSets[u32WriteSetIndex].dstBinding = u32BindingIndex;
					vk_aWriteSets[u32WriteSetIndex].dstArrayElement = 0;
					vk_aWriteSets[u32WriteSetIndex].descriptorCount = 1;
					vk_aWriteSets[u32WriteSetIndex].pImageInfo = nullptr;
					vk_aWriteSets[u32WriteSetIndex].pBufferInfo = &vk_aaBufferDescs[u32BindingIndex][u8FrameInFlightIndex];
					vk_aWriteSets[u32WriteSetIndex].pTexelBufferView = nullptr;
					vk_aaBufferDescs[u32BindingIndex][u8FrameInFlightIndex].offset = 0;
					vk_aaBufferDescs[u32BindingIndex][u8FrameInFlightIndex].range = VK_WHOLE_SIZE;
					switch (u32BindingIndex) {
						case RE_VK_DESC_SET_RENDER_CONTENT_GAME_OBJECTS_BINDING_INDEX:
							vk_aWriteSets[u32WriteSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
							vk_aaBufferDescs[u32BindingIndex][u8FrameInFlightIndex].buffer = vk_ahGameObjectBuffers[u8FrameInFlightIndex];
							break;
						case RE_VK_DESC_SET_RENDER_CONTENT_DEPTHS_BINDING_INDEX:
							vk_aWriteSets[u32WriteSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
							vk_aaBufferDescs[u32BindingIndex][u8FrameInFlightIndex].buffer = vk_ahSortableDepthBuffers[u8FrameInFlightIndex];
							break;
						case RE_VK_DESC_SET_RENDER_CONTENT_CAMERA_MATRICES_BINDING_INDEX:
							vk_aWriteSets[u32WriteSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
							vk_aaBufferDescs[u32BindingIndex][u8FrameInFlightIndex].buffer = vk_ahCameraBuffers[u8FrameInFlightIndex];
							break;
						default:
							RE_ABORT("Unknown binding index ", u32BindingIndex);
					}
				}
			}
			PRINT_DEBUG("Writing to all descriptor sets for processing");
			vkUpdateDescriptorSets(vk_hDevice, sizeof(vk_aWriteSets) / sizeof(vk_aWriteSets[0]), vk_aWriteSets, 0, nullptr);
			return true;
		} else
			RE_FATAL_ERROR("Failed allocating Vulkan descriptor sets for processing");
		return false;
	}

	void destroy_render_content_descriptor_sets() {
	}

}
