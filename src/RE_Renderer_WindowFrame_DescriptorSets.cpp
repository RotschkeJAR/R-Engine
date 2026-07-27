#include "RE_Renderer_WindowFrame_DescriptorSets.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_WINDOW_FRAME

	VkDescriptorSetLayout vk_hWindowFrameDescSetLayout;
	VkDescriptorSet vk_hWindowFrameDescSet;

	bool create_window_frame_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan descriptor set layout for window frame");
		VkDescriptorSetLayoutSupport vk_setLayoutSupported;
		vk_setLayoutSupported.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT;
		vk_setLayoutSupported.pNext = nullptr;
		const VkBool32 &vk_rbLayoutSupported = vk_setLayoutSupported.supported;
		const VkDescriptorSetLayoutBinding vk_aLayoutBindings[] = {
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
				.pImmutableSamplers = nullptr
			}, {
				.binding = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
				.pImmutableSamplers = &vk_hDefaultSampler
			}
		};
		const VkDescriptorSetLayoutCreateInfo vk_layoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = sizeof(vk_aLayoutBindings) / sizeof(vk_aLayoutBindings[0]),
			.pBindings = vk_aLayoutBindings
		};
		vkGetDescriptorSetLayoutSupport(vk_hDevice, &vk_layoutCreateInfo, &vk_setLayoutSupported);
		if (vk_rbLayoutSupported && vkCreateDescriptorSetLayout(vk_hDevice, &vk_layoutCreateInfo, nullptr, &vk_hWindowFrameDescSetLayout) == VK_SUCCESS) {
			PRINT_DEBUG("Allocating descriptor sets for window frame");
			const VkDescriptorSetAllocateInfo vk_descSetAllocInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.descriptorPool = vk_hPersistentDescPool,
				.descriptorSetCount = 1,
				.pSetLayouts = &vk_hWindowFrameDescSetLayout
			};
			if (vkAllocateDescriptorSets(vk_hDevice, &vk_descSetAllocInfo, &vk_hWindowFrameDescSet) == VK_SUCCESS) {
				PRINT_DEBUG("Writing to all descriptor sets for window frame");
				const VkDescriptorBufferInfo vk_bufferDesc = {
					.buffer = vk_hWindowFrameBuffer,
					.offset = 0,
					.range = sizeof(WindowFrameUniformData)
				};
				const VkDescriptorImageInfo vk_aImageDesc[] = {
					{
						.sampler = vk_hDefaultSampler,
						.imageView = vk_hWindowButtonImageView,
						.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					}
				};
				const VkWriteDescriptorSet vk_aWriteSets[] = {
					{
						.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
						.pNext = nullptr,
						.dstSet = vk_hWindowFrameDescSet,
						.dstBinding = 0,
						.dstArrayElement = 0,
						.descriptorCount = 1,
						.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						.pImageInfo = nullptr,
						.pBufferInfo = &vk_bufferDesc,
						.pTexelBufferView = nullptr
					}, {
						.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
						.pNext = nullptr,
						.dstSet = vk_hWindowFrameDescSet,
						.dstBinding = 1,
						.dstArrayElement = 0,
						.descriptorCount = sizeof(vk_aImageDesc) / sizeof(vk_aImageDesc[0]),
						.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						.pImageInfo = vk_aImageDesc,
						.pBufferInfo = nullptr,
						.pTexelBufferView = nullptr
					}
				};
				vkUpdateDescriptorSets(vk_hDevice, sizeof(vk_aWriteSets) / sizeof(vk_aWriteSets[0]), vk_aWriteSets, 0, nullptr);
				return true;
			} else
				RE_FATAL_ERROR("Failed to allocate Vulkan descriptor sets for window frame");
			PRINT_DEBUG("Destroying Vulkan descriptor set layout ", vk_hWindowFrameDescSetLayout, " for failing creating descriptor sets for window frame");
			vkDestroyDescriptorSetLayout(vk_hDevice, vk_hWindowFrameDescSetLayout, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout for window frame rendering");
		return false;
	}

	void destroy_window_frame_descriptor_sets() {
		PRINT_DEBUG("Destroying Vulkan descriptor set layout ", vk_hWindowFrameDescSetLayout);
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hWindowFrameDescSetLayout, nullptr);
	}

#endif

}
