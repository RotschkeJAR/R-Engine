#include "RE_Renderer_Descriptor Set Pools.hpp"

namespace RE {

	VkDescriptorPool vk_hPersistentDescPool;
	
	bool create_descriptor_set_pools() {
		PRINT_DEBUG("Creating persistent Vulkan descriptor pool");
		constexpr VkDescriptorPoolSize vk_aPersistentPoolSizes[] = {
			{
				.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 2
			}, {
				.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 6
			}, {
				.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = RE_VK_MAX_SAMPLED_IMAGES
			}
		};
		const VkDescriptorPoolCreateInfo vk_persistentPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.maxSets = 5,
			.poolSizeCount = sizeof(vk_aPersistentPoolSizes) / sizeof(vk_aPersistentPoolSizes[0]),
			.pPoolSizes = vk_aPersistentPoolSizes
		};
		if (vkCreateDescriptorPool(vk_hDevice, &vk_persistentPoolCreateInfo, nullptr, &vk_hPersistentDescPool) == VK_SUCCESS)
			return true;
		else
			RE_FATAL_ERROR("Failed to create a persistent Vulkan descriptor pool");
		return false;
	}

	void destroy_descriptor_set_pools() {
		PRINT_DEBUG("Destroying all Vulkan descriptor pools");
		vkDestroyDescriptorPool(vk_hDevice, vk_hPersistentDescPool, nullptr);
	}

}
