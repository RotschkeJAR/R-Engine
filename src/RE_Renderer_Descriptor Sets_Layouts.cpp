#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {
	
	VkDescriptorSetLayout vk_ahDescriptorSetLayouts[RE_VK_DESC_SET_LAYOUT_COUNT];

	bool create_descriptor_set_layouts() {
		constexpr uint32_t au32BindingCounts[] = {
			RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_2_STORAGE_BUFFER_BINDING_COUNT, 
			RE_VK_DESC_SET_LAYOUT_COMPUTE_STORAGE_BUFFER_BINDING_COUNT, 
			RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_UNIFORM_BUFFER_BINDING_COUNT, 
			RE_VK_DESC_SET_LAYOUT_FRAGMENT_SAMPLED_IMAGES_BINDING_COUNT
		};
		constexpr uint32_t u32MaxBindings = *std::max_element(std::begin(au32BindingCounts), std::end(au32BindingCounts));
		VkDescriptorSetLayoutBinding vk_aDescLayoutBindings[u32MaxBindings];
		VkDescriptorBindingFlags vk_amBindingFlags[u32MaxBindings];
		VkDescriptorSetLayoutBindingFlagsCreateInfo vk_bindingFlagsInfo;
		vk_bindingFlagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		vk_bindingFlagsInfo.pNext = nullptr;
		vk_bindingFlagsInfo.pBindingFlags = vk_amBindingFlags;
		VkDescriptorSetLayoutCreateInfo vk_descLayoutCreateInfo;
		vk_descLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		vk_descLayoutCreateInfo.flags = 0;
		vk_descLayoutCreateInfo.pBindings = vk_aDescLayoutBindings;

		uint32_t u32DescLayoutCreateIndex = 0;
		while (u32DescLayoutCreateIndex < RE_VK_DESC_SET_LAYOUT_COUNT) {
			PRINT_DEBUG("Filling information of descriptor set layout at index ", u32DescLayoutCreateIndex, " in");
			switch (u32DescLayoutCreateIndex) {
				case RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_2_STORAGE_BUFFER_INDEX:
					vk_descLayoutCreateInfo.pNext = nullptr;
					vk_descLayoutCreateInfo.bindingCount = RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_2_STORAGE_BUFFER_BINDING_COUNT;
					vk_aDescLayoutBindings[0].binding = 0;
					vk_aDescLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					vk_aDescLayoutBindings[0].descriptorCount = 1;
					vk_aDescLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
					vk_aDescLayoutBindings[0].pImmutableSamplers = nullptr;
					vk_aDescLayoutBindings[1].binding = 1;
					vk_aDescLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					vk_aDescLayoutBindings[1].descriptorCount = 1;
					vk_aDescLayoutBindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
					vk_aDescLayoutBindings[1].pImmutableSamplers = nullptr;
					break;
				case RE_VK_DESC_SET_LAYOUT_COMPUTE_STORAGE_BUFFER_INDEX:
					vk_descLayoutCreateInfo.pNext = nullptr;
					vk_descLayoutCreateInfo.bindingCount = RE_VK_DESC_SET_LAYOUT_COMPUTE_STORAGE_BUFFER_BINDING_COUNT;
					vk_aDescLayoutBindings[0].binding = 0;
					vk_aDescLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					vk_aDescLayoutBindings[0].descriptorCount = 1;
					vk_aDescLayoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
					vk_aDescLayoutBindings[0].pImmutableSamplers = nullptr;
					break;
				case RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_UNIFORM_BUFFER_INDEX:
					vk_descLayoutCreateInfo.pNext = nullptr;
					vk_descLayoutCreateInfo.bindingCount = RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_UNIFORM_BUFFER_BINDING_COUNT;
					vk_aDescLayoutBindings[0].binding = 0;
					vk_aDescLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					vk_aDescLayoutBindings[0].descriptorCount = 1;
					vk_aDescLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
					vk_aDescLayoutBindings[0].pImmutableSamplers = nullptr;
					break;
				case RE_VK_DESC_SET_LAYOUT_FRAGMENT_SAMPLED_IMAGES_INDEX:
					vk_descLayoutCreateInfo.pNext = &vk_bindingFlagsInfo;
					vk_descLayoutCreateInfo.bindingCount = RE_VK_DESC_SET_LAYOUT_FRAGMENT_SAMPLED_IMAGES_BINDING_COUNT;
					vk_aDescLayoutBindings[0].binding = 0;
					vk_aDescLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					vk_aDescLayoutBindings[0].descriptorCount = RE_VK_MAX_SAMPLED_IMAGES;
					vk_aDescLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
					vk_aDescLayoutBindings[0].pImmutableSamplers = nullptr;
					vk_bindingFlagsInfo.bindingCount = RE_VK_DESC_SET_LAYOUT_FRAGMENT_SAMPLED_IMAGES_BINDING_COUNT;
					vk_amBindingFlags[0] = VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
					break;
				default:
					RE_ABORT("Unknown index for unique descriptor set layout: ", u32DescLayoutCreateIndex);
			}
			PRINT_DEBUG("Creating Vulkan descriptor set layout");
			if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_descLayoutCreateInfo, nullptr, &vk_ahDescriptorSetLayouts[u32DescLayoutCreateIndex]) != VK_SUCCESS) {
				RE_FATAL_ERROR("Failed to create a unique descriptor set layout in Vulkan at index ", u32DescLayoutCreateIndex);
				break;
			}
			u32DescLayoutCreateIndex++;
		}
		if (u32DescLayoutCreateIndex == RE_VK_DESC_SET_LAYOUT_COUNT)
			return true;
		for (uint32_t u32DescLayoutDestroyIndex = 0; u32DescLayoutDestroyIndex < u32DescLayoutCreateIndex; u32DescLayoutDestroyIndex++) {
			PRINT_DEBUG("Destroying Vulkan descriptor set layout at index ", u32DescLayoutDestroyIndex, " due to failure creating all layouts");
			vkDestroyDescriptorSetLayout(vk_hDevice, vk_ahDescriptorSetLayouts[u32DescLayoutDestroyIndex], nullptr);
		}
		return false;
	}

	void destroy_descriptor_set_layouts() {
		PRINT_DEBUG("Destroying all Vulkan descriptor set layouts");
		for (const VkDescriptorSetLayout vk_hDescriptorSetLayout : vk_ahDescriptorSetLayouts)
			vkDestroyDescriptorSetLayout(vk_hDevice, vk_hDescriptorSetLayout, nullptr);
	}

}
