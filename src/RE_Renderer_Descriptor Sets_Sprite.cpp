#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	VkDescriptorSet vk_hTextureDescSet;

	bool create_texture_descriptor_sets() {
		PRINT_DEBUG("Allocating Vulkan descriptor set for using textures");
		const VkDescriptorSetAllocateInfo vk_textureDescSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hPersistentDescPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &vk_hTextureDescSetLayout
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_textureDescSetAllocInfo, &vk_hTextureDescSet) == VK_SUCCESS)
			return true;
		else
			RE_FATAL_ERROR("Failed allocating Vulkan descriptor set for textures");
		return false;
	}

	void destroy_texture_descriptor_sets() {
	}

}
