#include "RE_Renderer_Internal.hpp"
#include "RE_RenderElement.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {

#define RE_VK_PROCESSING_PIPELINE_COUNT 2
#define RE_VK_PROCESSING_PIPELINE_PREPROCESSING_INDEX 0
#define RE_VK_PROCESSING_PIPELINE_DEPTH_SORTING_INDEX 1

	static VkPipelineLayout vk_hProcessingPipelineLayout;
	static VkShaderModule vk_ahProcessingShaders[RE_VK_PROCESSING_PIPELINE_COUNT];
	static VkPipeline vk_ahProcessingPipelines[RE_VK_PROCESSING_PIPELINE_COUNT];

	bool create_processing_pipelines() {
		const VkPipelineLayoutCreateInfo vk_computePipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.flags = 0,
			.setLayoutCount = 1,
			.pSetLayouts = &vk_hProcessingDescLayout
		};
		if (vkCreatePipelineLayout(vk_hDevice, &vk_computePipelineLayoutCreateInfo, nullptr, &vk_hProcessingPipelineLayout) == VK_SUCCESS) {
			VkComputePipelineCreateInfo vk_aPipelineCreateInfos[RE_VK_PROCESSING_PIPELINE_COUNT];
			uint8_t u8ProcessingPipelineCreateIndex = 0;
			while (u8ProcessingPipelineCreateIndex < RE_VK_PROCESSING_PIPELINE_COUNT) {
				const char *pacShaderFile;
				switch (u8ProcessingPipelineCreateIndex) {
					case RE_VK_PROCESSING_PIPELINE_PREPROCESSING_INDEX:
						pacShaderFile = "preprocessing.glsl.spv";
						break;
					case RE_VK_PROCESSING_PIPELINE_DEPTH_SORTING_INDEX:
						pacShaderFile = "";
						break;
					default:
						RE_ABORT("Unknown index of processing pipeline, which has to be created in Vulkan: ", u8ProcessingPipelineCreateIndex);
				}
				if (!create_vulkan_shader_from_file(pacShaderFile, 0, &vk_ahProcessingShaders[u8ProcessingPipelineCreateIndex]))
					break;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].pNext = nullptr;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].flags = 0;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].stage.pNext = nullptr;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].stage.flags = 0;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].stage.module = vk_ahProcessingShaders[u8ProcessingPipelineCreateIndex];
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].stage.pName = "main";
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].stage.pSpecializationInfo = nullptr;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].layout = vk_hProcessingPipelineLayout;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].basePipelineHandle = VK_NULL_HANDLE;
				vk_aPipelineCreateInfos[u8ProcessingPipelineCreateIndex].basePipelineIndex = -1;
				u8ProcessingPipelineCreateIndex++;
			}
			if (u8ProcessingPipelineCreateIndex == RE_VK_PROCESSING_PIPELINE_COUNT && 
					vkCreateComputePipelines(vk_hDevice, VK_NULL_HANDLE, RE_VK_PROCESSING_PIPELINE_COUNT, vk_aPipelineCreateInfos, nullptr, vk_ahProcessingPipelines) == VK_SUCCESS)
				return true;
			for (uint8_t u8ProcessingShaderDestroyIndex = 0; u8ProcessingShaderDestroyIndex < u8ProcessingPipelineCreateIndex; u8ProcessingShaderDestroyIndex++)
				vkDestroyShaderModule(vk_hDevice, vk_ahProcessingShaders[u8ProcessingShaderDestroyIndex], nullptr);
			vkDestroyPipelineLayout(vk_hDevice, vk_hProcessingPipelineLayout, nullptr);
		}
		return false;
	}

	void destroy_processing_pipelines() {
		for (uint8_t u8ProcessingPipelineCreateIndex = 0; u8ProcessingPipelineCreateIndex < RE_VK_PROCESSING_PIPELINE_COUNT; u8ProcessingPipelineCreateIndex++) {
			vkDestroyPipeline(vk_hDevice, vk_ahProcessingPipelines[u8ProcessingPipelineCreateIndex], nullptr);
			vkDestroyShaderModule(vk_hDevice, vk_ahProcessingShaders[u8ProcessingPipelineCreateIndex], nullptr);
		}
		vkDestroyPipelineLayout(vk_hDevice, vk_hProcessingPipelineLayout, nullptr);
	}
	
	void order_rendering(const VkCommandBuffer vk_hCommandBuffer) {
		const VkViewport vk_viewport = {
			.x = static_cast<float>(vk_cameraProjectionOnscreen.offset.x),
			.y = static_cast<float>(vk_cameraProjectionOnscreen.offset.y),
			.width = static_cast<float>(vk_cameraProjectionOnscreen.extent.width),
			.height = static_cast<float>(vk_cameraProjectionOnscreen.extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		record_cmd_begin_renderpass(vk_hCommandBuffer);
		PRINT_DEBUG("Binding graphics pipeline ", vk_hGraphicsPipeline3D);
		vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipeline3D);
		vkCmdSetViewport(vk_hCommandBuffer, 0, 1, &vk_viewport);
		vkCmdSetScissor(vk_hCommandBuffer, 0, 1, &vk_cameraProjectionOnscreen);
		PRINT_DEBUG("Binding Vulkan descriptor sets");
		const VkDescriptorSet vk_ahDescSets[] = {cameraDescSets[u8CurrentFrameInFlightIndex], vk_hTextureDescSet};
		vkCmdBindDescriptorSets(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipelineLayout, 0, sizeof(vk_ahDescSets) / sizeof(vk_ahDescSets[0]), vk_ahDescSets, 0, nullptr);
		PRINT_DEBUG("Binding vertex, instance and index buffers");
		const VkBuffer vk_aVertexBuffers[] = {vk_hRectBuffer, aRenderBuffers[u8CurrentFrameInFlightIndex].get()};
		const VkDeviceSize vk_aBufferOffsets[] = {RE_VK_RECT_BUFFER_VERTICES_OFFSET, sizeof(VkDrawIndexedIndirectCommand)},
			vk_aBufferSizes[] = {RE_VK_RECT_BUFFER_VERTICES_SIZE, gameObjectsToRenderCount * sizeof(GameObjectInstanceData)};
		vkCmdBindVertexBuffers2(vk_hCommandBuffer, 0, 2, vk_aVertexBuffers, vk_aBufferOffsets, vk_aBufferSizes, nullptr);
		vkCmdBindIndexBuffer(vk_hCommandBuffer, vk_hRectBuffer, RE_VK_RECT_BUFFER_INDICES_OFFSET, VK_INDEX_TYPE_UINT16);
		PRINT_DEBUG("Drawing indirectly");
		vkCmdDrawIndexedIndirect(vk_hCommandBuffer, aRenderBuffers[u8CurrentFrameInFlightIndex].get(), 0, 1, sizeof(VkDrawIndexedIndirectCommand));
		record_cmd_end_renderpass(vk_hCommandBuffer);
	}

}
