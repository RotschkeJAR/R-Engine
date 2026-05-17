#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	VkDescriptorSet vk_hSpriteDescSet;

	bool create_sprite_descriptor_sets() {
		PRINT_DEBUG("Allocating Vulkan descriptor set for using textures");
		const VkDescriptorSetAllocateInfo vk_textureDescSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hTextureDescPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &vk_hSpriteDescSetLayout
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_textureDescSetAllocInfo, &vk_hSpriteDescSet) == VK_SUCCESS)
			return true;
		else
			RE_FATAL_ERROR("Failed to allocate Vulkan descriptor set for textures");
		return false;
	}

	void destroy_sprite_descriptor_sets() {
	}

}
