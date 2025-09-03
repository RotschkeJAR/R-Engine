#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	VkDescriptorSetLayout vk_hDescriptorSetLayout;
	VkDescriptorPool vk_hDescriptorPool;
	std::array<VkDescriptorSet, RE_VK_FRAMES_IN_FLIGHT> vk_ahDescriptorSets;
	std::array<VkBuffer, vk_ahDescriptorSets.size()> vk_ahDescriptorBuffers;
	std::array<VkDeviceMemory, vk_ahDescriptorBuffers.size()> vk_ahDescriptorBufferMemories;

	bool create_descriptor_sets() {
		/*constexpr VkDescriptorSetLayoutBinding vk_aDescSetLayoutBindings[] = {
			{
				.binding = ,
				.descriptorType = ,
				.descriptorCount = ,
				.stageFlags = 
			}, {
				.binding = ,
				.descriptorType = ,
				.descriptorCount = ,
				.stageFlags = 
			}
		};
		constexpr VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = sizeof(vk_aDescSetLayoutBindings) / sizeof(vk_aDescSetLayoutBindings[0]),
			.pBindings = vk_aDescSetLayoutBindings
		}
		if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_descSetLayoutCreateInfo, nullptr, &vk_hDescriptorSetLayout) == VK_SUCCESS) {
			constexpr VkDescriptorPoolSize vk_aDescPoolSizes[] = {
				{
					.type = ,
					.descriptorCount = 
				}, {
					.type = ,
					.descriptorCount = 
				}
			};
			constexpr VkDescriptorPoolCreateInfo vk_descriptorPoolCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.maxSets = ,
				.poolSizeCount = sizeof(vk_aDescPoolSizes) / sizeof(vk_aDescPoolSizes[0]),
				.pPoolSizes = vk_aDescPoolSizes
			};
			if (vkCreateDescriptorPool(vk_hDevice, &vk_descriptorPoolCreateInfo, nullptr, &vk_hDescriptorPool) == VK_SUCCESS) {
				std::array<VkDescriptorSetLayout, vk_ahDescriptorSets.size()> vk_ahDescSetLayouts;
				vk_ahDescSetLayouts.fill(vk_hDescriptorSetLayout);
				const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
					.descriptorPool = vk_hDescriptorPool,
					.descriptorSetCount = vk_ahDescSetLayouts.size(),
					.pSetLayouts = vk_ahDescSetLayouts.data()
				};
				if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, nullptr, vk_ahDescriptorSets.data()) == VK_SUCCESS) {
					// TODO: create uniform buffers
					return true;
				} else
					RE_FATAL_ERROR("Failed to allocate Vulkan descriptor sets");
			} else
				RE_FATAL_ERROR("Failed to create Vulkan descriptor pool");
		} else
			RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout");*/
		return false;
	}

	void destroy_descriptor_sets() {
		vkFreeDescriptorSets(vk_hDevice, vk_hDescriptorPool, vk_ahDescriptorSets.size(), vk_ahDescriptorSets.data());
		for (uint8_t u8DescriptorBufferIndex = 0; u8DescriptorBufferIndex < vk_ahDescriptorBuffers.size(); u8DescriptorBufferIndex++) {
			vkFreeMemory(vk_hDevice, vk_ahDescriptorBufferMemories[u8DescriptorBufferIndex], nullptr);
			vkDestroyBuffer(vk_hDevice, vk_ahDescriptorBuffers[u8DescriptorBufferIndex], nullptr);
		}
		vkDestroyDescriptorPool(vk_hDevice, vk_hDescriptorPool, nullptr);
	}

}
