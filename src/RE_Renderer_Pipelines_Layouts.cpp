#include "RE_Renderer_Pipelines_Layouts.hpp"

namespace RE {
	
	VkPipelineLayout vk_hGraphicsPipelineLayout,
		vk_hSortDepthPipelineLayout,
		vk_hProcessingPipelineLayout;
#ifdef RE_OS_LINUX
	VkPipelineLayout vk_hWindowFramePipelineLayout;
#endif /* RE_OS_LINUX */

	bool create_pipeline_layouts() {
		PRINT_DEBUG("Creating Vulkan pipeline layout dedicated for graphics pipelines");
		const VkDescriptorSetLayout vk_ahDescSetLayouts[] = {
			vk_hGameObjectsDescSetLayout,
			vk_hCameraDescSetLayout,
			vk_hSpriteDescSetLayout
		};
		const VkPipelineLayoutCreateInfo vk_graphicsPipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = sizeof(vk_ahDescSetLayouts) / sizeof(vk_ahDescSetLayouts[0]),
			.pSetLayouts = vk_ahDescSetLayouts,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};
		if (vkCreatePipelineLayout(vk_hDevice, &vk_graphicsPipelineLayoutCreateInfo, nullptr, &vk_hGraphicsPipelineLayout) == VK_SUCCESS) {
			PRINT_DEBUG("Creating Vulkan pipeline layout dedicated for compute pipelines sorting by depth");
			const VkDescriptorSetLayout vk_ahDescSetLayouts[] = {
				vk_hSortableDepthDescSetLayout
			};
			const VkPipelineLayoutCreateInfo vk_sortDepthPipelineLayoutCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.setLayoutCount = sizeof(vk_ahDescSetLayouts) / sizeof(vk_ahDescSetLayouts[0]),
				.pSetLayouts = vk_ahDescSetLayouts,
				.pushConstantRangeCount = 0,
				.pPushConstantRanges = nullptr
			};
			if (vkCreatePipelineLayout(vk_hDevice, &vk_sortDepthPipelineLayoutCreateInfo, nullptr, &vk_hSortDepthPipelineLayout) == VK_SUCCESS) {
				PRINT_DEBUG("Creating Vulkan pipeline layout dedicated for compute pipelines processing game objects");
				const VkDescriptorSetLayout vk_ahDescSetLayouts[] = {
					vk_hGameObjectsDescSetLayout,
					vk_hCameraDescSetLayout,
					vk_hSortableDepthDescSetLayout
				};
				const VkPipelineLayoutCreateInfo vk_processingPipelineLayoutCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.setLayoutCount = sizeof(vk_ahDescSetLayouts) / sizeof(vk_ahDescSetLayouts[0]),
					.pSetLayouts = vk_ahDescSetLayouts,
					.pushConstantRangeCount = 0,
					.pPushConstantRanges = nullptr
				};
				if (vkCreatePipelineLayout(vk_hDevice, &vk_processingPipelineLayoutCreateInfo, nullptr, &vk_hProcessingPipelineLayout) == VK_SUCCESS) {
#ifdef RE_OS_LINUX
					PRINT_DEBUG("Creating Vulkan pipeline layout dedicated for compute pipelines processing game objects");
					const VkPushConstantRange vk_aPushConstants[] = {
						{
							.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
							.offset = 0,
							.size = sizeof(WindowShaderData)
						}
					};
					const VkPipelineLayoutCreateInfo vk_windowFramePipelineLayoutCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.setLayoutCount = 1,
						.pSetLayouts = &vk_hCursorDescSetLayout,
						.pushConstantRangeCount = sizeof(vk_aPushConstants) / sizeof(vk_aPushConstants[0]),
						.pPushConstantRanges = vk_aPushConstants
					};
					if (vkCreatePipelineLayout(vk_hDevice, &vk_windowFramePipelineLayoutCreateInfo, nullptr, &vk_hWindowFramePipelineLayout) == VK_SUCCESS)
						return true;
					else
						RE_FATAL_ERROR("Failed to create Vulkan pipeline layout dedicated for compute pipelines processing game objects");
#else
					return true;
#endif
					PRINT_DEBUG("Destroying Vulkan pipeline layout dedicated for compute pipeline sorting by depth for failing to create all");
					vkDestroyPipelineLayout(vk_hDevice, vk_hProcessingPipelineLayout, nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan pipeline layout dedicated for compute pipelines processing game objects");
				PRINT_DEBUG("Destroying Vulkan pipeline layout dedicated for compute pipeline sorting by depth for failing to create all");
				vkDestroyPipelineLayout(vk_hDevice, vk_hSortDepthPipelineLayout, nullptr);
			} else
				RE_FATAL_ERROR("Failed to create Vulkan pipeline layout dedicated for compute pipelines sorting by depth");
			PRINT_DEBUG("Destroying Vulkan pipeline layout dedicated for graphics pipelines for failing to create all");
			vkDestroyPipelineLayout(vk_hDevice, vk_hGraphicsPipelineLayout, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create Vulkan pipeline layout dedicated for graphics pipelines");
		return true;
	}

	void destroy_pipeline_layouts() {
		PRINT_DEBUG("Destroying all Vulkan pipeline layouts");
#ifdef RE_OS_LINUX
		vkDestroyPipelineLayout(vk_hDevice, vk_hWindowFramePipelineLayout, nullptr);
#endif /* RE_OS_LINUX */
		vkDestroyPipelineLayout(vk_hDevice, vk_hProcessingPipelineLayout, nullptr);
		vkDestroyPipelineLayout(vk_hDevice, vk_hSortDepthPipelineLayout, nullptr);
		vkDestroyPipelineLayout(vk_hDevice, vk_hGraphicsPipelineLayout, nullptr);
	}

}
