#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	VkDescriptorPool vk_hPermanentDescPool;

	bool create_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan descriptor set pool for permanent uniforms");
		constexpr VkDescriptorPoolSize vk_aDescPoolSizes[] = {
			{
				.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 2 * RE_VK_FRAMES_IN_FLIGHT
			}, {
				.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 2 * RE_VK_FRAMES_IN_FLIGHT
			}, {
				.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = RE_VK_MAX_SAMPLED_IMAGES
			}
		};
		const VkDescriptorPoolCreateInfo vk_permanentDescPoolInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = RE_VK_FRAMES_IN_FLIGHT + 1,
			.poolSizeCount = sizeof(vk_aDescPoolSizes) / sizeof(vk_aDescPoolSizes[0]),
			.pPoolSizes = vk_aDescPoolSizes
		};
		if (vkCreateDescriptorPool(vk_hDevice, &vk_permanentDescPoolInfo, nullptr, &vk_hPermanentDescPool) == VK_SUCCESS) {
			if (create_camera_descriptor_sets()) {
				if (create_computing_descriptor_sets()) {
					if (create_texture_descriptor_sets())
						return true;
					destroy_computing_descriptor_sets();
				}
				destroy_camera_descriptor_sets();
			}
			vkDestroyDescriptorPool(vk_hDevice, vk_hPermanentDescPool, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create Vulkan descriptor pool for permanently used sets");
		return false;
	}

	void destroy_descriptor_sets() {
		PRINT_DEBUG("Destroying Vulkan descriptor sets");
		destroy_texture_descriptor_sets();
		destroy_camera_descriptor_sets();
		vkDestroyDescriptorPool(vk_hDevice, vk_hPermanentDescPool, nullptr);
	}

}
