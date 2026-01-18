#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	VkDescriptorPool vk_hPermanentDescPool;

	consteval uint32_t count_permanent_descriptor_sets(const VkDescriptorType vk_eType) {
		uint32_t u32Counter = 0;
		for (uint32_t u32DescSetIndex = 0; u32DescSetIndex < 4; u32DescSetIndex++) {
			uint32_t u32DescCounter;
			const uint32_t u32DescSetLayoutIndex = [&]() {
						switch (u32DescSetIndex) {
							case 0:
								u32DescCounter = sizeof(vk_ahSortableBuffersDescSets) / sizeof(vk_ahSortableBuffersDescSets[0]);
								return RE_VK_DESC_SET_SORTABLE_BUFFERS_LAYOUT_INDEX;
							case 1:
								u32DescCounter = sizeof(vk_ahStagingBufferDescSets) / sizeof(vk_ahStagingBufferDescSets[0]);
								return RE_VK_DESC_SET_STAGING_BUFFER_LAYOUT_INDEX;
							case 2:
								u32DescCounter = sizeof(vk_ahCameraDescSets) / sizeof(vk_ahCameraDescSets[0]);
								return RE_VK_DESC_SET_CAMERA_LAYOUT_INDEX;
							case 3:
								u32DescCounter = RE_VK_MAX_SAMPLED_IMAGES;
								return RE_VK_DESC_SET_TEXTURE_LAYOUT_INDEX;
						}
					} ();
			switch (u32DescSetLayoutIndex) {
				case RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_2_STORAGE_BUFFER_INDEX:
					if (vk_eType != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
						continue;
					u32DescCounter *= 2;
					break;
				case RE_VK_DESC_SET_LAYOUT_COMPUTE_STORAGE_BUFFER_INDEX:
					if (vk_eType != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
						continue;
					break;
				case RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_UNIFORM_BUFFER_INDEX:
					if (vk_eType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
						continue;
					break;
				case RE_VK_DESC_SET_LAYOUT_FRAGMENT_SAMPLED_IMAGES_INDEX:
					if (vk_eType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
						continue;
					break;
			}
			u32Counter += u32DescCounter;
		}
		return u32Counter;
	}

	bool create_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan descriptor set pool for permanent uniforms");
		constexpr VkDescriptorPoolSize vk_aDescPoolSizes[] = {
			{
				.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = count_permanent_descriptor_sets(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			}, {
				.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = count_permanent_descriptor_sets(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
			}, {
				.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = count_permanent_descriptor_sets(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
			}
		};
		const VkDescriptorPoolCreateInfo vk_permanentDescPoolInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = sizeof(vk_ahCameraDescSets) / sizeof(vk_ahCameraDescSets[0])
					 + sizeof(vk_ahSortableBuffersDescSets) / sizeof(vk_ahSortableBuffersDescSets[0])
					 + sizeof(vk_ahStagingBufferDescSets) / sizeof(vk_ahStagingBufferDescSets[0])
					 + 1,
			.poolSizeCount = sizeof(vk_aDescPoolSizes) / sizeof(vk_aDescPoolSizes[0]),
			.pPoolSizes = vk_aDescPoolSizes
		};
		if (vkCreateDescriptorPool(vk_hDevice, &vk_permanentDescPoolInfo, nullptr, &vk_hPermanentDescPool) == VK_SUCCESS) {
			if (create_descriptor_set_layouts()) {
				if (create_camera_descriptor_sets()) {
					if (create_processing_descriptor_sets()) {
						if (create_texture_descriptor_sets())
							return true;
						destroy_processing_descriptor_sets();
					}
					destroy_camera_descriptor_sets();
				}
				destroy_descriptor_set_layouts();
			}
			vkDestroyDescriptorPool(vk_hDevice, vk_hPermanentDescPool, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create Vulkan descriptor pool for permanently used sets");
		return false;
	}

	void destroy_descriptor_sets() {
		PRINT_DEBUG("Destroying Vulkan descriptor sets");
		destroy_texture_descriptor_sets();
		destroy_processing_descriptor_sets();
		destroy_camera_descriptor_sets();
		destroy_descriptor_set_layouts();
		vkDestroyDescriptorPool(vk_hDevice, vk_hPermanentDescPool, nullptr);
	}

}
