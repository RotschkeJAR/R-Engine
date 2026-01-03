#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

#define RE_VK_BINDING_COUNT 2
#define RE_VK_OBJECT_COUNT_BUFFER_BINDING_INDEX 0
#define RE_VK_PROCESSABLE_BUFFER_BINDING_INDEX 1

#define RE_VK_PROCESSABLE_BUFFER_COUNT 2
#define RE_VK_RAW_RENDER_BUFFER_INDEX 0
#define RE_VK_SORTABLE_DEPTH_BUFFER_INDEX 1

	VkDescriptorSetLayout vk_hProcessingDescLayout;
	VkDescriptorSet vk_ahProcessingDescSets[RE_VK_FRAMES_IN_FLIGHT];
	static std::array<uint32_t*, RE_VK_FRAMES_IN_FLIGHT> objectCountUniforms;
	
	bool create_processing_descriptor_sets(const UniformBufferInfo &rUniformBufferInfo) {
		const VkDescriptorSetLayoutBinding vk_aDescBindings[] = {
			{
				.binding = RE_VK_OBJECT_COUNT_BUFFER_BINDING_INDEX,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = nullptr
			}, {
				.binding = RE_VK_PROCESSABLE_BUFFER_BINDING_INDEX,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = RE_VK_PROCESSABLE_BUFFER_COUNT,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = nullptr
			}
		};
		const VkDescriptorSetLayoutCreateInfo vk_descLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = sizeof(vk_aDescBindings) / sizeof(vk_aDescBindings[0]),
			.pBindings = vk_aDescBindings
		};
		if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_descLayoutCreateInfo, nullptr, &vk_hProcessingDescLayout) == VK_SUCCESS) {
			VkDescriptorSetLayout vk_ahLayouts[RE_VK_FRAMES_IN_FLIGHT];
			std::fill(std::begin(vk_ahLayouts), std::end(vk_ahLayouts), vk_hProcessingDescLayout);
			const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.descriptorPool = vk_hPermanentDescPool,
				.descriptorSetCount = sizeof(vk_ahLayouts) / sizeof(vk_ahLayouts[0]),
				.pSetLayouts = vk_ahLayouts
			};
			if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, vk_ahProcessingDescSets) == VK_SUCCESS) {
				VkDescriptorBufferInfo vk_aaBufferDescs[RE_VK_FRAMES_IN_FLIGHT][RE_VK_PROCESSABLE_BUFFER_COUNT];
				VkDescriptorBufferInfo vk_objectCountBufferDescs[RE_VK_FRAMES_IN_FLIGHT];
				VkWriteDescriptorSet vk_aWriteSets[RE_VK_BINDING_COUNT * RE_VK_FRAMES_IN_FLIGHT];
				for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
					vk_objectCountBufferDescs[u8FrameInFlightIndex].buffer = vk_aahUniformBuffers[RE_VK_OBJECT_COUNT_UNIFORM_BUFFER_INDEX][u8FrameInFlightIndex];
					vk_objectCountBufferDescs[u8FrameInFlightIndex].offset = 0;
					vk_objectCountBufferDescs[u8FrameInFlightIndex].range = VK_WHOLE_SIZE;
					for (uint32_t u32BindingIndex = 0; u32BindingIndex < RE_VK_BINDING_COUNT; u32BindingIndex++) {
						vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + u32BindingIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + u32BindingIndex].pNext = nullptr;
						vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + u32BindingIndex].dstSet = vk_ahProcessingDescSets[u8FrameInFlightIndex];
						vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + u32BindingIndex].dstBinding = u32BindingIndex;
						vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + u32BindingIndex].dstArrayElement = 0;
						vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + u32BindingIndex].pImageInfo = nullptr;
						vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + u32BindingIndex].pTexelBufferView = nullptr;
						switch (u32BindingIndex) {
							case RE_VK_OBJECT_COUNT_BUFFER_BINDING_INDEX:
								vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + RE_VK_OBJECT_COUNT_BUFFER_BINDING_INDEX].descriptorCount = 1;
								vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + RE_VK_OBJECT_COUNT_BUFFER_BINDING_INDEX].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
								vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + RE_VK_OBJECT_COUNT_BUFFER_BINDING_INDEX].pBufferInfo = &vk_objectCountBufferDescs[u8FrameInFlightIndex];
								break;
							case RE_VK_PROCESSABLE_BUFFER_BINDING_INDEX:
								for (uint32_t u32ProcessableBufferIndex = 0; u32ProcessableBufferIndex < RE_VK_PROCESSABLE_BUFFER_COUNT; u32ProcessableBufferIndex++) {
									vk_aaBufferDescs[u8FrameInFlightIndex * RE_VK_BINDING_COUNT][u32ProcessableBufferIndex].offset = 0;
									vk_aaBufferDescs[u8FrameInFlightIndex * RE_VK_BINDING_COUNT][u32ProcessableBufferIndex].range = VK_WHOLE_SIZE;
									switch (u32ProcessableBufferIndex) {
										case RE_VK_RAW_RENDER_BUFFER_INDEX:
											vk_aaBufferDescs[u8FrameInFlightIndex * RE_VK_BINDING_COUNT][RE_VK_RAW_RENDER_BUFFER_INDEX].buffer = aRenderBuffers[u8FrameInFlightIndex].get();
											break;
										case RE_VK_SORTABLE_DEPTH_BUFFER_INDEX:
											vk_aaBufferDescs[u8FrameInFlightIndex * RE_VK_BINDING_COUNT][RE_VK_SORTABLE_DEPTH_BUFFER_INDEX].buffer = aSortableDepthBuffers[u8FrameInFlightIndex].get();
											break;
										default:
											RE_ABORT("Unknown buffer index within the Vulkan descriptor sets for processing: ", u32ProcessableBufferIndex);
									}
								}
								vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + RE_VK_PROCESSABLE_BUFFER_BINDING_INDEX].descriptorCount = RE_VK_PROCESSABLE_BUFFER_COUNT;
								vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + RE_VK_PROCESSABLE_BUFFER_BINDING_INDEX].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
								vk_aWriteSets[u8FrameInFlightIndex * RE_VK_BINDING_COUNT + RE_VK_PROCESSABLE_BUFFER_BINDING_INDEX].pBufferInfo = vk_aaBufferDescs[u8FrameInFlightIndex];
								break;
							default:
								RE_ABORT("Unknown binding index within the Vulkan descriptor sets for processing: ", u32BindingIndex);
						}
					}
				}
				vkUpdateDescriptorSets(vk_hDevice, sizeof(vk_aWriteSets) / sizeof(vk_aWriteSets[0]), vk_aWriteSets, 0, nullptr);
				for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
					objectCountUniforms[u8FrameInFlightIndex] = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(rUniformBufferInfo.pUniformBuffersContent) + rUniformBufferInfo.vk_aObjectCountFirstBytes[u8FrameInFlightIndex]);
					PRINT_LN(rUniformBufferInfo.vk_aObjectCountFirstBytes[u8FrameInFlightIndex]);
				}
				return true;
			}
			vkDestroyDescriptorSetLayout(vk_hDevice, vk_hProcessingDescLayout, nullptr);
		}
		return false;
	}

	void destroy_processing_descriptor_sets() {
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hProcessingDescLayout, nullptr);
	}

}
