#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

#define RE_VK_DESC_SET_TYPE_COUNT 2
#define RE_VK_DESC_SET_TYPE_SORTABLE_BUFFERS_INDEX 0
#define RE_VK_DESC_SET_TYPE_DEPTHS_INDEX 1

	VkDescriptorSet vk_ahSortableBuffersDescSets[RE_VK_FRAMES_IN_FLIGHT];
	VkDescriptorSet vk_ahStagingBufferDescSets[RE_VK_FRAMES_IN_FLIGHT];
	
	bool create_processing_descriptor_sets() {
		PRINT_DEBUG("Allocating descriptor sets for processing");
		constexpr uint32_t u32DescSetCount = RE_VK_FRAMES_IN_FLIGHT * RE_VK_DESC_SET_TYPE_COUNT;
		VkDescriptorSetLayout vk_ahDescSetLayouts[u32DescSetCount];
		std::fill(std::begin(vk_ahDescSetLayouts), std::begin(vk_ahDescSetLayouts) + RE_VK_FRAMES_IN_FLIGHT, vk_hSortableBuffersDescSetLayout);
		std::fill(std::begin(vk_ahDescSetLayouts) + RE_VK_FRAMES_IN_FLIGHT, std::end(vk_ahDescSetLayouts), vk_hStagingBufferDescSetLayout);
		const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hPermanentDescPool,
			.descriptorSetCount = u32DescSetCount,
			.pSetLayouts = vk_ahDescSetLayouts
		};
		VkDescriptorSet vk_ahDescSets[u32DescSetCount];
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, vk_ahDescSets) == VK_SUCCESS) {
			std::copy(std::begin(vk_ahDescSets), std::begin(vk_ahDescSets) + RE_VK_FRAMES_IN_FLIGHT, std::begin(vk_ahSortableBuffersDescSets));
			std::copy(std::begin(vk_ahDescSets) + RE_VK_FRAMES_IN_FLIGHT, std::end(vk_ahDescSets), std::begin(vk_ahStagingBufferDescSets));
			
			VkDescriptorBufferInfo vk_aaSortableBufferDescs[RE_VK_DESC_SET_SORTABLE_BUFFERS_BINDING_COUNT][RE_VK_FRAMES_IN_FLIGHT];
			VkDescriptorBufferInfo vk_aStagingBufferDescs[RE_VK_FRAMES_IN_FLIGHT];
			VkWriteDescriptorSet vk_aWriteSets[(RE_VK_DESC_SET_SORTABLE_BUFFERS_BINDING_COUNT + RE_VK_DESC_SET_STAGING_BUFFER_BINDING_COUNT) * RE_VK_FRAMES_IN_FLIGHT];
			uint32_t u32WriteDescSetIndex = 0;
			for (uint32_t u32DescSetTypeIndex = 0; u32DescSetTypeIndex < RE_VK_DESC_SET_TYPE_COUNT; u32DescSetTypeIndex++) {
				const uint32_t u32BindingCount = [&]() {
							switch (u32DescSetTypeIndex) {
								case RE_VK_DESC_SET_TYPE_SORTABLE_BUFFERS_INDEX:
									return RE_VK_DESC_SET_SORTABLE_BUFFERS_BINDING_COUNT;
								case RE_VK_DESC_SET_TYPE_DEPTHS_INDEX:
									return RE_VK_DESC_SET_STAGING_BUFFER_BINDING_COUNT;
								default:
									RE_ABORT("Unknown type index ", u32DescSetTypeIndex, " of the processing descriptor set");
							}
						} ();
				for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
					for (uint32_t u32BindingIndex = 0; u32BindingIndex < u32BindingCount; u32BindingIndex++) {
						PRINT_DEBUG("Setting write information for descriptor set at type index ", u32DescSetTypeIndex, ", frame-in-flight index ", u8FrameInFlightIndex, " and binding index ", u32BindingIndex);
						vk_aWriteSets[u32WriteDescSetIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						vk_aWriteSets[u32WriteDescSetIndex].pNext = nullptr;
						switch (u32DescSetTypeIndex) {
							case RE_VK_DESC_SET_TYPE_SORTABLE_BUFFERS_INDEX:
								vk_aWriteSets[u32WriteDescSetIndex].dstSet = vk_ahSortableBuffersDescSets[u8FrameInFlightIndex];
								vk_aWriteSets[u32WriteDescSetIndex].pBufferInfo = &vk_aaSortableBufferDescs[u32BindingIndex][u8FrameInFlightIndex];
								switch (u32BindingIndex) {
									case RE_VK_DESC_SET_SORTABLE_BUFFER_GAME_OBJECTS_BINDING_INDEX:
										vk_aaSortableBufferDescs[u32BindingIndex][u8FrameInFlightIndex].buffer = aGameObjectBuffers[u8FrameInFlightIndex].get();
										break;
									case RE_VK_DESC_SET_SORTABLE_BUFFER_DEPTHS_BINDING_INDEX:
										vk_aaSortableBufferDescs[u32BindingIndex][u8FrameInFlightIndex].buffer = aSortableDepthBuffers[u8FrameInFlightIndex].get();
										break;
									default:
										RE_ABORT("Unknown binding index ", u32BindingIndex, " of the descriptor set used for sortable buffers");
								}
								vk_aaSortableBufferDescs[u32BindingIndex][u8FrameInFlightIndex].offset = 0;
								vk_aaSortableBufferDescs[u32BindingIndex][u8FrameInFlightIndex].range = VK_WHOLE_SIZE;
								break;
							case RE_VK_DESC_SET_TYPE_DEPTHS_INDEX:
								vk_aWriteSets[u32WriteDescSetIndex].dstSet = vk_ahStagingBufferDescSets[u8FrameInFlightIndex];
								vk_aWriteSets[u32WriteDescSetIndex].pBufferInfo = &vk_aStagingBufferDescs[u8FrameInFlightIndex];
								vk_aStagingBufferDescs[u8FrameInFlightIndex].buffer = aRawGameObjectBuffers[u8FrameInFlightIndex];
								vk_aStagingBufferDescs[u8FrameInFlightIndex].offset = 0;
								vk_aStagingBufferDescs[u8FrameInFlightIndex].range = VK_WHOLE_SIZE;
								break;
							default:
								RE_ABORT("Unknown type index ", u32DescSetTypeIndex, " of the processing descriptor set");
						}
						vk_aWriteSets[u32WriteDescSetIndex].dstBinding = u32BindingIndex;
						vk_aWriteSets[u32WriteDescSetIndex].dstArrayElement = 0;
						vk_aWriteSets[u32WriteDescSetIndex].pImageInfo = nullptr;
						vk_aWriteSets[u32WriteDescSetIndex].pTexelBufferView = nullptr;
						vk_aWriteSets[u32WriteDescSetIndex].descriptorCount = 1;
						vk_aWriteSets[u32WriteDescSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
						u32WriteDescSetIndex++;
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

	void destroy_processing_descriptor_sets() {
	}

}
