#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	std::unique_ptr<VkDescriptorSet[]> cameraDescSets;

	bool create_camera_descriptor_sets() {
		PRINT_DEBUG("Allocating descriptor sets for camera");
		const uint16_t u16DescSetCount = RE_VK_FRAMES_IN_FLIGHT * get_max_camera_count();
		cameraDescSets = std::make_unique<VkDescriptorSet[]>(u16DescSetCount);
		std::unique_ptr<VkDescriptorSetLayout[]> descSetLayouts = std::make_unique<VkDescriptorSetLayout[]>(u16DescSetCount);
		std::fill(descSetLayouts.get(), descSetLayouts.get() + u16DescSetCount, vk_hCameraDescSetLayout);
		const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hPersistentDescPool,
			.descriptorSetCount = u16DescSetCount,
			.pSetLayouts = descSetLayouts.get()
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, vk_ahRenderContentDescSets) == VK_SUCCESS) {
			PRINT_DEBUG("Writing to all descriptor sets for camera");
			std::unique_ptr<VkDescriptorBufferInfo[]> bufferDescs = std::make_unique<VkDescriptorBufferInfo[]>(u16DescSetCount);
			std::unique_ptr<VkWriteDescriptorSet[]> writeSets = std::make_unique<VkWriteDescriptorSet[]>(u16DescSetCount);
			for (uint16_t u16DescSetIndex = 0; u16DescSetIndex < u16DescSetCount; u16DescSetIndex++) {
				writeSets[u16DescSetIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeSets[u16DescSetIndex].pNext = nullptr;
				writeSets[u16DescSetIndex].dstSet = cameraDescSets[u16DescSetIndex];
				writeSets[u16DescSetIndex].dstBinding = 0;
				writeSets[u16DescSetIndex].dstArrayElement = 0;
				writeSets[u16DescSetIndex].descriptorCount = 1;
				writeSets[u16DescSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				writeSets[u16DescSetIndex].pImageInfo = nullptr;
				writeSets[u16DescSetIndex].pBufferInfo = &bufferDescs[u16DescSetIndex];
				writeSets[u16DescSetIndex].pTexelBufferView = nullptr;
				bufferDescs[u16DescSetIndex].buffer = vk_hCameraBuffer;
				bufferDescs[u16DescSetIndex].offset = next_multiple_inclusive<VkDeviceSize>(sizeof(CameraShaderData), vk_uniformBufferAlignment) * u16DescSetIndex;
				bufferDescs[u16DescSetIndex].range = sizeof(CameraShaderData);
			}
			vkUpdateDescriptorSets(vk_hDevice, u16DescSetCount, writeSets.get(), 0, nullptr);
			return true;
		} else
			RE_FATAL_ERROR("Failed to allocate Vulkan descriptor sets for camera");
		return false;
	}

	void destroy_camera_descriptor_sets() {
	}

}
