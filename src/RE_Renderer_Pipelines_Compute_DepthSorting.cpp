#include "RE_Renderer_Pipelines_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"

namespace RE {

	VkPipeline vk_hComputePipelineDepthSorting;

	bool create_compute_pipeline_depth_sorting() {
		VkShaderModule vk_hShader;
		if (create_vulkan_shader_from_file("shaders/gameobject_depth sorting_compute.glsl.spv", 0, &vk_hShader)) {
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
				.layout = vk_hComputePipelineLayoutProcessing,
				.basePipelineHandle = VK_NULL_HANDLE,
				.basePipelineIndex = -1
			};
			const bool bSuccess = vkCreateComputePipelines(vk_hDevice, VK_NULL_HANDLE, 1, &vk_createInfo, nullptr, &vk_hComputePipelineDepthSorting) == VK_SUCCESS;
			if (!bSuccess)
				RE_FATAL_ERROR("Failed to create the compute pipeline for depth sorting");
			vkDestroyShaderModule(vk_hDevice, vk_hShader, nullptr);
			return bSuccess;
		} else
			RE_FATAL_ERROR("Failed loading shader for the compute pipeline for depth sorting");
		return false;
	}

	void destroy_compute_pipeline_depth_sorting() {
		vkDestroyPipeline(vk_hDevice, vk_hComputePipelineDepthSorting, nullptr);
	}

}
