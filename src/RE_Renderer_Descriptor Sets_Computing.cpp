#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	VkDescriptorSetLayout vk_hComputePipelineDescLayout;
	VkDescriptorSet vk_ahComputePipelineDescriptorSets[RE_VK_FRAMES_IN_FLIGHT];
	
	bool create_computing_descriptor_sets() {
		const VkDescriptorSetLayoutBinding vk_aDescriptorBindings[] = {
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 2,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = nullptr
			}, {
				.binding = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = nullptr
			}
		};
		const VkDescriptorSetLayoutCreateInfo vk_descriptorLayoutCreateInfo = {
			.sType = VKSTRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = sizeof(vk_aDescriptorBindings) / sizeof(vk_aDescriptorBindings[0]),
			.pBindings = vk_aDescriptorBindings
		};
		if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_descriptorLayoutCreateInfo, nullptr, &vk_hComputePipelineDescLayout) == VK_SUCCESS) {
			const VkDescriptorSetLayout vk_aLayouts[RE_VK_FRAMES_IN_FLIGHT] = {
				vk_hComputePipelineDescLayout,
				vk_hComputePipelineDescLayout
			};
			const VkDescriptorSetAllocateInfo vk_descriptorSetAllocInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.flags = 0,
				.descriptorPool = vk_hPermanentDescPool,
				.descriptorSetCount = RE_VK_FRAMES_IN_FLIGHT,
				.pSetLayouts = vk_aLayouts
			};
			if (vkAllocateDescriptorSets(vk_hDevice, &vk_descriptorSetAllocInfo, vk_ahComputePipelineDescriptorSets) == VK_SUCCESS)
				return true;
			vkDestroyDescriptorSetLayout(vk_hDevice, vk_hComputePipelineDescLayout, nullptr);
		}
		return false;
	}

	void destroy_computing_descriptor_sets() {

	}

}
