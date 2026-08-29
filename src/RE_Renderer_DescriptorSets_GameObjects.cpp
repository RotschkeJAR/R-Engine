#include "RE_Renderer_DescriptorSets_Internal.hpp"

namespace RE {

	VkDescriptorSet vk_ahGameObjectsDescSets[RE_VK_FRAMES_IN_FLIGHT];
	
	bool create_game_objects_descriptor_sets() {
		PRINT_DEBUG("Allocating descriptor sets for game objects");
		VkDescriptorSetLayout vk_ahDescSetLayouts[RE_VK_FRAMES_IN_FLIGHT];
		std::fill(std::begin(vk_ahDescSetLayouts), std::end(vk_ahDescSetLayouts), vk_hGameObjectsDescSetLayout);
		const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hPersistentDescPool,
			.descriptorSetCount = RE_VK_FRAMES_IN_FLIGHT,
			.pSetLayouts = vk_ahDescSetLayouts
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, vk_ahGameObjectsDescSets) == VK_SUCCESS) {
			PRINT_DEBUG("Writing to all descriptor sets for game objects");
			VkDescriptorBufferInfo vk_aaBufferDescs[RE_VK_GAME_OBJECTS_DESC_SET_BINDING_COUNT][RE_VK_FRAMES_IN_FLIGHT];
			VkWriteDescriptorSet vk_aWriteSets[RE_VK_GAME_OBJECTS_DESC_SET_BINDING_COUNT * RE_VK_FRAMES_IN_FLIGHT];
			for (unsigned uFrameInFlightIndex = 0; uFrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; uFrameInFlightIndex++) {
				for (unsigned uBindingIndex = 0; uBindingIndex < RE_VK_GAME_OBJECTS_DESC_SET_BINDING_COUNT; uBindingIndex++) {
					const uint32_t u32WriteSetIndex = uFrameInFlightIndex * RE_VK_GAME_OBJECTS_DESC_SET_BINDING_COUNT + uBindingIndex;
					vk_aWriteSets[u32WriteSetIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					vk_aWriteSets[u32WriteSetIndex].pNext = nullptr;
					vk_aWriteSets[u32WriteSetIndex].dstSet = vk_ahGameObjectsDescSets[uFrameInFlightIndex];
					vk_aWriteSets[u32WriteSetIndex].dstBinding = uBindingIndex;
					vk_aWriteSets[u32WriteSetIndex].dstArrayElement = 0;
					vk_aWriteSets[u32WriteSetIndex].descriptorCount = 1;
					vk_aWriteSets[u32WriteSetIndex].pImageInfo = nullptr;
					vk_aWriteSets[u32WriteSetIndex].pBufferInfo = &vk_aaBufferDescs[uBindingIndex][uFrameInFlightIndex];
					vk_aWriteSets[u32WriteSetIndex].pTexelBufferView = nullptr;
					switch (uBindingIndex) {
						case RE_VK_GAME_OBJECTS_DESC_SET_INSTANCE_BINDING_INDEX:
							vk_aWriteSets[u32WriteSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
							vk_aaBufferDescs[uBindingIndex][uFrameInFlightIndex].buffer = vk_ahGameObjectsBuffers[uFrameInFlightIndex];
							vk_aaBufferDescs[uBindingIndex][uFrameInFlightIndex].offset = 0;
							vk_aaBufferDescs[uBindingIndex][uFrameInFlightIndex].range = VK_WHOLE_SIZE;
							break;
						case RE_VK_GAME_OBJECTS_DESC_SET_MODEL_MATRIX_BINDING_INDEX:
							vk_aWriteSets[u32WriteSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
							vk_aaBufferDescs[uBindingIndex][uFrameInFlightIndex].buffer = vk_ahGameObjectsModelMatrixBuffers[uFrameInFlightIndex];
							vk_aaBufferDescs[uBindingIndex][uFrameInFlightIndex].offset = 0;
							vk_aaBufferDescs[uBindingIndex][uFrameInFlightIndex].range = VK_WHOLE_SIZE;
							break;
						case RE_VK_GAME_OBJECTS_DESC_SET_TOTAL_COUNT_BINDING_INDEX:
							vk_aWriteSets[u32WriteSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
							vk_aaBufferDescs[uBindingIndex][uFrameInFlightIndex].buffer = vk_hGameObjectCountBuffer;
							vk_aaBufferDescs[uBindingIndex][uFrameInFlightIndex].offset = next_multiple_inclusive<VkDeviceSize>(sizeof(GameObjectCountShaderData), vk_uniformBufferAlignment) * uFrameInFlightIndex;
							vk_aaBufferDescs[uBindingIndex][uFrameInFlightIndex].range = sizeof(GameObjectCountShaderData);
							break;
						[[unlikely]] default:
							RE_ABORT("Unknown binding index: ", uBindingIndex);
					}
				}
			}
			vkUpdateDescriptorSets(vk_hDevice, sizeof(vk_aWriteSets) / sizeof(vk_aWriteSets[0]), vk_aWriteSets, 0, nullptr);
			return true;
		} else
			RE_FATAL_ERROR("Failed allocate Vulkan descriptor sets for game objects");
		return false;
	}

	void destroy_game_objects_descriptor_sets() {
	}

}
