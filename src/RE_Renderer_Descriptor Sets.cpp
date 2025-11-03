#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	VkDescriptorSetLayout vk_hPermanentDescLayout;
	VkDescriptorPool vk_hPermanentDescPool;
	VkDescriptorSet vk_ahPermanentDescSets[RE_VK_FRAMES_IN_FLIGHT];

	bool create_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan descriptor set layout used for permanent uniforms");
		constexpr VkDescriptorSetLayoutBinding vk_aPermanentDescLayoutBindings[] = {
			{
				.binding = RE_VK_UNIFORM_BINDING_CAMERA,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
				.pImmutableSamplers = nullptr
			}, {
				.binding = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
				.pImmutableSamplers = nullptr
			}
		};
		if (create_descriptor_set_layout(sizeof(vk_aPermanentDescLayoutBindings) / sizeof(vk_aPermanentDescLayoutBindings[0]), vk_aPermanentDescLayoutBindings, 0, &vk_hPermanentDescLayout)) {
			PRINT_DEBUG("Creating Vulkan descriptor set pool for permanent uniforms");
			constexpr uint32_t u32DescriptorTypesRequired = 2;
			constexpr VkDescriptorPoolSize vk_aDescPoolSizes[u32DescriptorTypesRequired] = {
				{
					.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.descriptorCount = RE_VK_FRAMES_IN_FLIGHT
				}, {
					.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
					.descriptorCount = RE_VK_FRAMES_IN_FLIGHT
				}
			};
			if (create_descriptor_pool(u32DescriptorTypesRequired, vk_aDescPoolSizes, RE_VK_FRAMES_IN_FLIGHT, &vk_hPermanentDescPool)) {
				PRINT_DEBUG("Allocating all permanent Vulkan descriptor set");
				VkDescriptorSetLayout vk_aDescSetLayouts[RE_VK_FRAMES_IN_FLIGHT];
				std::fill(std::begin(vk_aDescSetLayouts), std::end(vk_aDescSetLayouts), vk_hPermanentDescLayout);
				if (alloc_descriptor_set(vk_hPermanentDescPool, RE_VK_FRAMES_IN_FLIGHT, vk_aDescSetLayouts, vk_ahPermanentDescSets)) {
					if (create_camera_descriptor_sets()) {
						if (create_texture_descriptor_sets())
							return true;
						destroy_camera_descriptor_sets();
					}
				} else
					RE_FATAL_ERROR("Failed to allocate permanent essential Vulkan descriptor sets in pool ", vk_hPermanentDescPool);
				vkDestroyDescriptorPool(vk_hDevice, vk_hPermanentDescPool, nullptr);
			} else
				RE_FATAL_ERROR("Failed to create Vulkan descriptor pool for permanent sets");
			vkDestroyDescriptorSetLayout(vk_hDevice, vk_hPermanentDescLayout, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout for permanent descriptor sets");
		return false;
	}

	void destroy_descriptor_sets() {
		PRINT_DEBUG("Destroying Vulkan descriptor sets");
		destroy_texture_descriptor_sets();
		destroy_camera_descriptor_sets();
		vkDestroyDescriptorPool(vk_hDevice, vk_hPermanentDescPool, nullptr);
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hPermanentDescLayout, nullptr);
	}

}
