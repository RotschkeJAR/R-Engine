#include "RE_Renderer_Descriptor Set Pools.hpp"

namespace RE {

	VkDescriptorPool vk_hPersistentDescPool,
		vk_hTextureDescPool;

	bool create_descriptor_set_pools() {
		if (are_vulkan_features_enabled<ENABLED_FEATURE_UPDATE_DESCRIPTOR_SAMPLED_IMAGE_AFTER_BIND_BIT>()) {
			PRINT_DEBUG("Creating persistent Vulkan descriptor pool");
			const VkDescriptorPoolSize vk_aPersistentPoolSizes[] = {
				{
					.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
#ifdef RE_OS_LINUX
					.descriptorCount = get_max_camera_count() * RE_VK_FRAMES_IN_FLIGHT + 1U
#else
					.descriptorCount = get_max_camera_count() * RE_VK_FRAMES_IN_FLIGHT
#endif
				}, {
					.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					.descriptorCount = 3U * RE_VK_FRAMES_IN_FLIGHT
				},
#ifdef RE_OS_LINUX
				{
					.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1U
				}
#endif
			};
			const VkDescriptorPoolCreateInfo vk_persistentPoolCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.maxSets = 4U + (get_max_camera_count() + 1U) * RE_VK_FRAMES_IN_FLIGHT,
				.poolSizeCount = sizeof(vk_aPersistentPoolSizes) / sizeof(vk_aPersistentPoolSizes[0]),
				.pPoolSizes = vk_aPersistentPoolSizes
			};
			if (vkCreateDescriptorPool(vk_hDevice, &vk_persistentPoolCreateInfo, nullptr, &vk_hPersistentDescPool) == VK_SUCCESS) {
				PRINT_DEBUG("Creating persistent Vulkan descriptor pool for textures only");
				const VkDescriptorPoolSize vk_aTexturePoolSizes[] = {
					{
						.type = VK_DESCRIPTOR_TYPE_SAMPLER,
						.descriptorCount = get_max_sprite_layout_count()
					}, {
						.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
						.descriptorCount = get_max_texture_count()
					}
				};
				const VkDescriptorPoolCreateInfo vk_texturePoolCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
					.pNext = nullptr,
					.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
					.maxSets = 1,
					.poolSizeCount = sizeof(vk_aTexturePoolSizes) / sizeof(vk_aTexturePoolSizes[0]),
					.pPoolSizes = vk_aTexturePoolSizes
				};
				if (vkCreateDescriptorPool(vk_hDevice, &vk_texturePoolCreateInfo, nullptr, &vk_hTextureDescPool) == VK_SUCCESS)
					return true;
				else
					RE_FATAL_ERROR("Failed to create a Vulkan descriptor pool for textures only");
			} else
				RE_FATAL_ERROR("Failed to create a persistent Vulkan descriptor pool");
		} else {
			PRINT_DEBUG("Creating persistent Vulkan descriptor pool containing textures aswell");
			const VkDescriptorPoolSize vk_aPersistentPoolSizes[] = {
				{
					.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
#ifdef RE_OS_LINUX
					.descriptorCount = get_max_camera_count() * RE_VK_FRAMES_IN_FLIGHT + 1U
#else
					.descriptorCount = get_max_camera_count() * RE_VK_FRAMES_IN_FLIGHT
#endif
				}, {
					.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					.descriptorCount = 3U * RE_VK_FRAMES_IN_FLIGHT
				}, {
					.type = VK_DESCRIPTOR_TYPE_SAMPLER,
					.descriptorCount = get_max_sprite_layout_count()
				}, {
					.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
					.descriptorCount = get_max_texture_count()
				},
#ifdef RE_OS_LINUX
				{
					.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1U
				}
#endif
			};
			const VkDescriptorPoolCreateInfo vk_persistentPoolCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.maxSets = static_cast<uint32_t>(5 + get_max_camera_count() * RE_VK_FRAMES_IN_FLIGHT),
				.poolSizeCount = sizeof(vk_aPersistentPoolSizes) / sizeof(vk_aPersistentPoolSizes[0]),
				.pPoolSizes = vk_aPersistentPoolSizes
			};
			if (vkCreateDescriptorPool(vk_hDevice, &vk_persistentPoolCreateInfo, nullptr, &vk_hPersistentDescPool) == VK_SUCCESS) {
				vk_hTextureDescPool = vk_hPersistentDescPool;
				return true;
			} else
				RE_FATAL_ERROR("Failed to create a persistent Vulkan descriptor pool containing textures aswell");
		}
		return false;
	}

	void destroy_descriptor_set_pools() {
		PRINT_DEBUG("Destroying all Vulkan descriptor set pools");
		if (vk_hPersistentDescPool != vk_hTextureDescPool)
			vkDestroyDescriptorPool(vk_hDevice, vk_hTextureDescPool, nullptr);
		vkDestroyDescriptorPool(vk_hDevice, vk_hPersistentDescPool, nullptr);
	}

}
