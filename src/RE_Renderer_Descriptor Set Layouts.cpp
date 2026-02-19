#include "RE_Renderer_Descriptor Set Layouts.hpp"

namespace RE {
	
	VkDescriptorSetLayout vk_ahDescriptorSetLayouts[RE_VK_DESC_SET_LAYOUT_COUNT];

	bool create_descriptor_set_layouts() {
		constexpr VkDescriptorSetLayoutBinding vk_aVertexCompute2StorageBuffer1UniformBufferLayoutBindings[] = {
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = nullptr
			}, {
				.binding = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = nullptr
			}, {
				.binding = 2,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = nullptr
			}
		};
		static_assert(sizeof(vk_aVertexCompute2StorageBuffer1UniformBufferLayoutBindings) / sizeof(vk_aVertexCompute2StorageBuffer1UniformBufferLayoutBindings[0]) == RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_2_STORAGE_BUFFER_1_UNIFORM_BUFFER_BINDING_COUNT);
		const VkDescriptorSetLayoutCreateInfo vk_vertexCompute2StorageBufferLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_2_STORAGE_BUFFER_1_UNIFORM_BUFFER_BINDING_COUNT,
			.pBindings = vk_aVertexCompute2StorageBuffer1UniformBufferLayoutBindings
		};
		if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_vertexCompute2StorageBufferLayoutCreateInfo, nullptr, &vk_ahDescriptorSetLayouts[RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_2_STORAGE_BUFFER_1_UNIFORM_BUFFER_INDEX]) == VK_SUCCESS) {
			constexpr VkDescriptorSetLayoutBinding vk_aComputeStorageBufferLayoutBindings[] = {
				{
					.binding = 0,
					.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
					.pImmutableSamplers = nullptr
				}
			};
			static_assert(sizeof(vk_aComputeStorageBufferLayoutBindings) / sizeof(vk_aComputeStorageBufferLayoutBindings[0]) == RE_VK_DESC_SET_LAYOUT_COMPUTE_STORAGE_BUFFER_BINDING_COUNT);
			const VkDescriptorSetLayoutCreateInfo vk_computeStorageBufferLayoutCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.bindingCount = RE_VK_DESC_SET_LAYOUT_COMPUTE_STORAGE_BUFFER_BINDING_COUNT,
				.pBindings = vk_aComputeStorageBufferLayoutBindings
			};
			if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_computeStorageBufferLayoutCreateInfo, nullptr, &vk_ahDescriptorSetLayouts[RE_VK_DESC_SET_LAYOUT_COMPUTE_STORAGE_BUFFER_INDEX]) == VK_SUCCESS) {
				constexpr VkDescriptorSetLayoutBinding vk_aVertexComputeUniformBufferLayoutBindings[] = {
					{
						.binding = 0,
						.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						.descriptorCount = 1,
						.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
						.pImmutableSamplers = nullptr
					}
				};
				static_assert(sizeof(vk_aVertexComputeUniformBufferLayoutBindings) / sizeof(vk_aVertexComputeUniformBufferLayoutBindings[0]) == RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_UNIFORM_BUFFER_BINDING_COUNT);
				const VkDescriptorSetLayoutCreateInfo vk_vertexComputeUniformBufferLayoutCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.bindingCount = RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_UNIFORM_BUFFER_BINDING_COUNT,
					.pBindings = vk_aVertexComputeUniformBufferLayoutBindings
				};
				if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_vertexComputeUniformBufferLayoutCreateInfo, nullptr, &vk_ahDescriptorSetLayouts[RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_UNIFORM_BUFFER_INDEX]) == VK_SUCCESS) {
					constexpr VkDescriptorBindingFlags vk_amFragmentSampledImagesLayoutBindingFlags[] = {
						VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
					};
					static_assert(sizeof(vk_amFragmentSampledImagesLayoutBindingFlags) / sizeof(vk_amFragmentSampledImagesLayoutBindingFlags[0]) == RE_VK_DESC_SET_LAYOUT_FRAGMENT_SAMPLED_IMAGES_BINDING_COUNT);
					const VkDescriptorSetLayoutBindingFlagsCreateInfo vk_fragmentSampledImagesLayoutBindingCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
						.pNext = nullptr,
						.bindingCount = RE_VK_DESC_SET_LAYOUT_FRAGMENT_SAMPLED_IMAGES_BINDING_COUNT,
						.pBindingFlags = vk_amFragmentSampledImagesLayoutBindingFlags
					};
					constexpr VkDescriptorSetLayoutBinding vk_aFragmentSampledImagesLayoutBindings[] = {
						{
							.binding = 0,
							.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
							.descriptorCount = RE_VK_MAX_SAMPLED_IMAGES,
							.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
							.pImmutableSamplers = nullptr
						}
					};
					static_assert(sizeof(vk_aFragmentSampledImagesLayoutBindings) / sizeof(vk_aFragmentSampledImagesLayoutBindings[0]) == RE_VK_DESC_SET_LAYOUT_FRAGMENT_SAMPLED_IMAGES_BINDING_COUNT);
					const VkDescriptorSetLayoutCreateInfo vk_fragmentSampledImagesLayoutCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
						.pNext = &vk_fragmentSampledImagesLayoutBindingCreateInfo,
						.flags = 0,
						.bindingCount = RE_VK_DESC_SET_LAYOUT_FRAGMENT_SAMPLED_IMAGES_BINDING_COUNT,
						.pBindings = vk_aFragmentSampledImagesLayoutBindings
					};
					if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_fragmentSampledImagesLayoutCreateInfo, nullptr, &vk_ahDescriptorSetLayouts[RE_VK_DESC_SET_LAYOUT_FRAGMENT_SAMPLED_IMAGES_INDEX]) == VK_SUCCESS)
						return true;
					else
						RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout containing ", RE_VK_MAX_SAMPLED_IMAGES, " combined image sampler/-s used in fragment-shaders");
					vkDestroyDescriptorSetLayout(vk_hDevice, vk_ahDescriptorSetLayouts[RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_UNIFORM_BUFFER_INDEX], nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout containing 1 uniform buffer used in compute- and vertex-shaders");
				vkDestroyDescriptorSetLayout(vk_hDevice, vk_ahDescriptorSetLayouts[RE_VK_DESC_SET_LAYOUT_COMPUTE_STORAGE_BUFFER_INDEX], nullptr);
			} else
				RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout containing 1 storage buffer used in compute-shaders");
			vkDestroyDescriptorSetLayout(vk_hDevice, vk_ahDescriptorSetLayouts[RE_VK_DESC_SET_LAYOUT_VERTEX_COMPUTE_2_STORAGE_BUFFER_1_UNIFORM_BUFFER_INDEX], nullptr);
		} else
			RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout containing 2 storage buffer and 1 uniform buffer used in compute- and vertex-shader");
		return false;
	}

	void destroy_descriptor_set_layouts() {
		PRINT_DEBUG("Destroying all Vulkan descriptor set layouts");
		for (const VkDescriptorSetLayout vk_hDescriptorSetLayout : vk_ahDescriptorSetLayouts)
			vkDestroyDescriptorSetLayout(vk_hDevice, vk_hDescriptorSetLayout, nullptr);
	}

}
