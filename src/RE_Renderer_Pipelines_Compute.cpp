#include "RE_Renderer_Pipelines_Compute.hpp"
#include "RE_Vulkan_Wrappers.hpp"

namespace RE {

	VkPipeline vk_ahComputePipelines[COMPUTE_PIPELINE_COUNT];
	
	bool create_compute_pipelines() {
		PRINT_DEBUG("Creating Vulkan shader modules for creating compute pipelines");
		Vulkan_Shader preprocessingShader("shaders/Compute_Processing.glsl.spv"),
			depthSortingShader("shaders/Compute_DepthSorting.glsl.spv");
		if (preprocessingShader.valid() && depthSortingShader.valid()) {
			PRINT_DEBUG("Creating Vulkan compute pipelines");
			const VkComputePipelineCreateInfo vk_aCreateInfos[COMPUTE_PIPELINE_COUNT] = {
				{
					.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.stage = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.stage = VK_SHADER_STAGE_COMPUTE_BIT,
						.module = preprocessingShader(),
						.pName = "main",
						.pSpecializationInfo = nullptr
					},
					.layout = vk_hProcessingPipelineLayout,
					.basePipelineHandle = VK_NULL_HANDLE,
					.basePipelineIndex = -1
				}, {
					.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.stage = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.stage = VK_SHADER_STAGE_COMPUTE_BIT,
						.module = depthSortingShader(),
						.pName = "main",
						.pSpecializationInfo = nullptr
					},
					.layout = vk_hSortDepthPipelineLayout,
					.basePipelineHandle = VK_NULL_HANDLE,
					.basePipelineIndex = -1
				}
			};
			if (vkCreateComputePipelines(vk_hDevice, VK_NULL_HANDLE, COMPUTE_PIPELINE_COUNT, vk_aCreateInfos, nullptr, vk_ahComputePipelines) == VK_SUCCESS) {
				return true;
			} else
				RE_FATAL_ERROR("Failed to create Vulkan compute pipelines");
		} else
			RE_FATAL_ERROR("Failed loading compute shaders");
		return false;
	}

	void destroy_compute_pipelines() {
		PRINT_DEBUG("Destroying Vulkan compute pipelines");
		for (VkPipeline vk_hComputePipeline : vk_ahComputePipelines)
			vkDestroyPipeline(vk_hDevice, vk_hComputePipeline, nullptr);
		std::fill(std::begin(vk_ahComputePipelines), std::end(vk_ahComputePipelines), VK_NULL_HANDLE);
	}

}
