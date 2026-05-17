#include "RE_Renderer_Pipelines_Compute_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"

namespace RE {

	VkPipeline vk_hComputePipelineProcessing;

	bool create_compute_pipeline_preprocessing() {
		PRINT_DEBUG("Creating temporary Vulkan shader module for processing game objects");
		VkShaderModule vk_hShader;
		if (create_vulkan_shader_from_file("shaders/compute_processing.glsl.spv", 0, &vk_hShader)) {
			PRINT_DEBUG("Creating Vulkan compute pipeline for processing game objects");
			const VkComputePipelineCreateInfo vk_createInfo = {
				.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.stage = VK_SHADER_STAGE_COMPUTE_BIT,
					.module = vk_hShader,
					.pName = "main",
					.pSpecializationInfo = nullptr
				},
				.layout = vk_hProcessingPipelineLayout,
				.basePipelineHandle = VK_NULL_HANDLE,
				.basePipelineIndex = -1
			};
			const bool bSuccess = vkCreateComputePipelines(vk_hDevice, VK_NULL_HANDLE, 1, &vk_createInfo, nullptr, &vk_hComputePipelineProcessing) == VK_SUCCESS;
			if (!bSuccess)
				RE_FATAL_ERROR("Failed to create Vulkan compute pipeline for processing game objects");
			PRINT_DEBUG("Destroying temporary Vulkan shader module for processing game objects");
			vkDestroyShaderModule(vk_hDevice, vk_hShader, nullptr);
			return bSuccess;
		} else
			RE_FATAL_ERROR("Failed creating Vulkan shader module for creating the compute pipeline for processing game objects");
		return false;
	}

	void destroy_compute_pipeline_preprocessing() {
		PRINT_DEBUG("Destroying Vulkan compute pipeline for processing game objects");
		vkDestroyPipeline(vk_hDevice, vk_hComputePipelineProcessing, nullptr);
	}

}
