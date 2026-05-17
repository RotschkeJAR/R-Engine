#include "RE_Renderer_Descriptor Set Layouts.hpp"

namespace RE {

	VkDescriptorSetLayout vk_hGameObjectsDescSetLayout,
		vk_hSortableDepthDescSetLayout,
		vk_hCameraDescSetLayout,
		vk_hSpriteDescSetLayout;

	bool create_descriptor_set_layouts() {
		PRINT_DEBUG("Creating Vulkan descriptor set layout for game objects");
	    VkDescriptorSetLayoutSupport vk_setLayoutSupported;
		vk_setLayoutSupported.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT;
		vk_setLayoutSupported.pNext = nullptr;
		const VkBool32 &vk_rbLayoutSupported = vk_setLayoutSupported.supported;
		constexpr VkDescriptorSetLayoutBinding vk_aGameObjectsLayoutBindings[RE_VK_GAME_OBJECTS_DESC_SET_BINDING_COUNT] = {
			{
				.binding = RE_VK_GAME_OBJECTS_DESC_SET_INSTANCE_BINDING_INDEX,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = nullptr
			}, {
				.binding = RE_VK_GAME_OBJECTS_DESC_SET_MODEL_MATRIX_BINDING_INDEX,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = nullptr
			}
		};
		const VkDescriptorSetLayoutCreateInfo vk_gameObjectsLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = RE_VK_GAME_OBJECTS_DESC_SET_BINDING_COUNT,
			.pBindings = vk_aGameObjectsLayoutBindings
		};
		vkGetDescriptorSetLayoutSupport(vk_hDevice, &vk_gameObjectsLayoutCreateInfo, &vk_setLayoutSupported);
		if (vk_rbLayoutSupported && vkCreateDescriptorSetLayout(vk_hDevice, &vk_gameObjectsLayoutCreateInfo, nullptr, &vk_hGameObjectsDescSetLayout) == VK_SUCCESS) {
			PRINT_DEBUG("Creating Vulkan descriptor set layout for sortable depth");
			constexpr VkDescriptorSetLayoutBinding vk_aSortableDepthLayoutBindings[RE_VK_GAME_OBJECTS_DESC_SET_BINDING_COUNT] = {
				{
					.binding = 0,
					.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
					.pImmutableSamplers = nullptr
				}
			};
			const VkDescriptorSetLayoutCreateInfo vk_sortableDepthLayoutCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.bindingCount = 1,
				.pBindings = vk_aSortableDepthLayoutBindings
			};
			vkGetDescriptorSetLayoutSupport(vk_hDevice, &vk_sortableDepthLayoutCreateInfo, &vk_setLayoutSupported);
			if (vk_rbLayoutSupported && vkCreateDescriptorSetLayout(vk_hDevice, &vk_sortableDepthLayoutCreateInfo, nullptr, &vk_hSortableDepthDescSetLayout) == VK_SUCCESS) {
				PRINT_DEBUG("Creating Vulkan descriptor set layout for camera");
				constexpr VkDescriptorSetLayoutBinding vk_aCameraLayoutBindings[] = {
					{
						.binding = 0,
						.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						.descriptorCount = 1,
						.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
						.pImmutableSamplers = nullptr
					}
				};
				const VkDescriptorSetLayoutCreateInfo vk_cameraLayoutCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.bindingCount = 1,
					.pBindings = vk_aCameraLayoutBindings
				};
				vkGetDescriptorSetLayoutSupport(vk_hDevice, &vk_cameraLayoutCreateInfo, &vk_setLayoutSupported);
				if (vk_rbLayoutSupported && vkCreateDescriptorSetLayout(vk_hDevice, &vk_cameraLayoutCreateInfo, nullptr, &vk_hCameraDescSetLayout) == VK_SUCCESS) {
					PRINT_DEBUG("Creating Vulkan descriptor set layout for textures and sprite layouts");
					VkDescriptorBindingFlags vk_amSpriteLayoutBindingFlags[RE_VK_SPRITE_DESC_SET_BINDING_COUNT] = {
						VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT,
						VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
					};
					for (uint32_t u32BindingIndex = 0; u32BindingIndex < RE_VK_SPRITE_DESC_SET_BINDING_COUNT; u32BindingIndex++) {
						if (are_vulkan_features_enabled<ENABLED_FEATURE_UPDATE_UNUSED_DESCRIPTORS_WHILE_PENDING_BIT>())
							vk_amSpriteLayoutBindingFlags[u32BindingIndex] |= VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
						if (are_vulkan_features_enabled<ENABLED_FEATURE_UPDATE_DESCRIPTOR_SAMPLED_IMAGE_AFTER_BIND_BIT>())
							vk_amSpriteLayoutBindingFlags[u32BindingIndex] |= VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
					}
					const VkDescriptorSetLayoutBindingFlagsCreateInfo vk_spriteLayoutBindingCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
						.pNext = nullptr,
						.bindingCount = 2,
						.pBindingFlags = vk_amSpriteLayoutBindingFlags
					};
					const VkDescriptorSetLayoutBinding vk_aSpriteLayoutBindings[RE_VK_SPRITE_DESC_SET_BINDING_COUNT] = {
						{
							.binding = RE_VK_SPRITE_DESC_SET_SPRITE_LAYOUT_BINDING_INDEX,
							.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
							.descriptorCount = get_max_sprite_layout_count(),
							.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
							.pImmutableSamplers = nullptr
						}, {
							.binding = RE_VK_SPRITE_DESC_SET_TEXTURE_BINDING_INDEX,
							.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
							.descriptorCount = get_max_texture_count(),
							.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
							.pImmutableSamplers = nullptr
						}
					};
					VkDescriptorSetLayoutCreateInfo vk_spriteLayoutCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
						.pNext = &vk_spriteLayoutBindingCreateInfo,
						.flags = 0,
						.bindingCount = RE_VK_SPRITE_DESC_SET_BINDING_COUNT,
						.pBindings = vk_aSpriteLayoutBindings
					};
					if (are_vulkan_features_enabled<ENABLED_FEATURE_UPDATE_DESCRIPTOR_SAMPLED_IMAGE_AFTER_BIND_BIT>())
						vk_spriteLayoutCreateInfo.flags |= VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
					vkGetDescriptorSetLayoutSupport(vk_hDevice, &vk_spriteLayoutCreateInfo, &vk_setLayoutSupported);
					if (vk_rbLayoutSupported && vkCreateDescriptorSetLayout(vk_hDevice, &vk_spriteLayoutCreateInfo, nullptr, &vk_hSpriteDescSetLayout) == VK_SUCCESS)
						return true;
					else
						RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout for textures and sprite layouts");
					PRINT_DEBUG("Destroying Vulkan descriptor set layout for camera due to failure creating all layouts");
					vkDestroyDescriptorSetLayout(vk_hDevice, vk_hCameraDescSetLayout, nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout for camera");
				PRINT_DEBUG("Destroying Vulkan descriptor set layout for sortable depth due to failure creating all layouts");
				vkDestroyDescriptorSetLayout(vk_hDevice, vk_hSortableDepthDescSetLayout, nullptr);
			} else
				RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout for sortable depth");
			PRINT_DEBUG("Destroying Vulkan descriptor set layout for game objects due to failure creating all layouts");
			vkDestroyDescriptorSetLayout(vk_hDevice, vk_hGameObjectsDescSetLayout, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout for game objects");
		return false;
	}

	void destroy_descriptor_set_layouts() {
		PRINT_DEBUG("Destroying all Vulkan descriptor set layouts");
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hSpriteDescSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hCameraDescSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hSortableDepthDescSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hGameObjectsDescSetLayout, nullptr);
	}

}
