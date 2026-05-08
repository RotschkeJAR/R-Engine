#include "RE_Renderer_Pipelines_Layouts.hpp"

namespace RE {
	
	VkPipelineLayout vk_hGraphicsPipelineLayout,
		vk_hComputePipelineLayoutProcessing;

	bool create_pipeline_layouts() {
		PRINT_DEBUG("Creating Vulkan pipeline layout dedicated for graphics pipelines");
		const VkDescriptorSetLayout vk_ahDescSetLayouts[] = {
			vk_hRenderContentDescSetLayout,
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
			PRINT_DEBUG("Creating Vulkan pipeline layout dedicated for compute pipelines");
			const VkDescriptorSetLayout vk_ahDescSetLayouts[] = {
				vk_hRawGameObjectsDescSetLayout,
				vk_hRenderContentDescSetLayout,
				vk_hCameraDescSetLayout
			};
			const VkPipelineLayoutCreateInfo vk_computePipelineLayoutCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.setLayoutCount = sizeof(vk_ahDescSetLayouts) / sizeof(vk_ahDescSetLayouts[0]),
				.pSetLayouts = vk_ahDescSetLayouts,
				.pushConstantRangeCount = 0,
				.pPushConstantRanges = nullptr
			};
			if (vkCreatePipelineLayout(vk_hDevice, &vk_computePipelineLayoutCreateInfo, nullptr, &vk_hComputePipelineLayoutProcessing) == VK_SUCCESS)
				return true;
			else
				RE_FATAL_ERROR("Failed to create Vulkan pipeline layout dedicated for compute pipelines");
			PRINT_DEBUG("Destroying Vulkan pipeline layout dedicated for graphics pipelines");
			vkDestroyPipelineLayout(vk_hDevice, vk_hGraphicsPipelineLayout, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create Vulkan pipeline layout dedicated for graphics pipelines");
		return true;
	}

	void destroy_pipeline_layouts() {
		PRINT_DEBUG("Destroying all Vulkan pipeline layouts");
		vkDestroyPipelineLayout(vk_hDevice, vk_hComputePipelineLayoutProcessing, nullptr);
		vkDestroyPipelineLayout(vk_hDevice, vk_hGraphicsPipelineLayout, nullptr);
	}

}
