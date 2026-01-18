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
		PRINT_DEBUG("Creating Vulkan pipeline layout for processing");
		const VkDescriptorSetLayout vk_ahDescSetLayouts[] = {
			vk_hSortableBuffersDescSetLayout, 
			vk_hStagingBufferDescSetLayout
		};
		const VkPipelineLayoutCreateInfo vk_computePipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.flags = 0,
			.setLayoutCount = 1,
			.pSetLayouts = vk_ahDescSetLayouts
		};
		if (vkCreatePipelineLayout(vk_hDevice, &vk_computePipelineLayoutCreateInfo, nullptr, &vk_hProcessingPipelineLayout) == VK_SUCCESS) {
			VkComputePipelineCreateInfo vk_aPipelineCreateInfos[RE_VK_PROCESSING_PIPELINE_COUNT];
			uint8_t u8ProcessingPipelineCreateIndex = 0;
			while (u8ProcessingPipelineCreateIndex < RE_VK_PROCESSING_PIPELINE_COUNT) {
				const char *pacShaderFile;
				switch (u8ProcessingPipelineCreateIndex) {
					case RE_VK_PROCESSING_PIPELINE_PREPROCESSING_INDEX:
						pacShaderFile = "shaders/gameobject_processing_compute.glsl.spv";
						break;
					case RE_VK_PROCESSING_PIPELINE_DEPTH_SORTING_INDEX:
						pacShaderFile = "shaders/gameobject_processing_compute.glsl.spv";
						break;
					default:
						RE_ABORT("Unknown index of processing pipeline, which has to be created in Vulkan: ", u8ProcessingPipelineCreateIndex);
				}
				PRINT_DEBUG("Creating Vulkan shader module from the file \"", pacShaderFile, "\"");
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
			if (u8ProcessingPipelineCreateIndex == RE_VK_PROCESSING_PIPELINE_COUNT) {
				PRINT_DEBUG("Creating Vulkan compute pipeline for processing");
				if (vkCreateComputePipelines(vk_hDevice, VK_NULL_HANDLE, RE_VK_PROCESSING_PIPELINE_COUNT, vk_aPipelineCreateInfos, nullptr, vk_ahProcessingPipelines) == VK_SUCCESS) {
					for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++)
						renderTasks[u8FrameInFlightIndex].record(
								RENDER_TASK_SUBINDEX_PROCESSING, 
								0, 
								[](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {});
					return true;
				}
			}
			for (uint8_t u8ProcessingShaderDestroyIndex = 0; u8ProcessingShaderDestroyIndex < u8ProcessingPipelineCreateIndex; u8ProcessingShaderDestroyIndex++) {
				PRINT_DEBUG("Destroying Vulkan shader module used for processing for failure creating all shaders or pipelines");
				vkDestroyShaderModule(vk_hDevice, vk_ahProcessingShaders[u8ProcessingShaderDestroyIndex], nullptr);
			}
			PRINT_DEBUG("Destroying Vulkan pipeline layout used for processing for failure creating compute shaders or pipelines");
			vkDestroyPipelineLayout(vk_hDevice, vk_hProcessingPipelineLayout, nullptr);
		}
		return false;
	}

	void destroy_processing_pipelines() {
		for (uint8_t u8ProcessingPipelineDestroyIndex = 0; u8ProcessingPipelineDestroyIndex < RE_VK_PROCESSING_PIPELINE_COUNT; u8ProcessingPipelineDestroyIndex++) {
			PRINT_DEBUG("Destroying Vulkan shader module and compute pipeline for processing at index ", u8ProcessingPipelineDestroyIndex);
			vkDestroyPipeline(vk_hDevice, vk_ahProcessingPipelines[u8ProcessingPipelineDestroyIndex], nullptr);
			vkDestroyShaderModule(vk_hDevice, vk_ahProcessingShaders[u8ProcessingPipelineDestroyIndex], nullptr);
		}
		PRINT_DEBUG("Destroying Vulkan pipeline layout used for processing");
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
		const VkDescriptorSet vk_ahDescSets[] = {vk_ahCameraDescSets[u8CurrentFrameInFlightIndex], vk_hTextureDescSet};
		vkCmdBindDescriptorSets(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipelineLayout, 0, sizeof(vk_ahDescSets) / sizeof(vk_ahDescSets[0]), vk_ahDescSets, 0, nullptr);
		PRINT_DEBUG("Binding vertex, instance and index buffers");
		const VkBuffer vk_aVertexBuffers[] = {vk_hRectBuffer, aRawGameObjectBuffers[u8CurrentFrameInFlightIndex].get()};
		const VkDeviceSize vk_aBufferOffsets[] = {RE_VK_RECT_BUFFER_VERTICES_OFFSET, sizeof(VkDrawIndexedIndirectCommand)},
			vk_aBufferSizes[] = {RE_VK_RECT_BUFFER_VERTICES_SIZE, gameObjectsToRenderCount * sizeof(GameObjectInstanceData)};
		vkCmdBindVertexBuffers2(vk_hCommandBuffer, 0, 2, vk_aVertexBuffers, vk_aBufferOffsets, vk_aBufferSizes, nullptr);
		vkCmdBindIndexBuffer(vk_hCommandBuffer, vk_hRectBuffer, RE_VK_RECT_BUFFER_INDICES_OFFSET, VK_INDEX_TYPE_UINT16);
		PRINT_DEBUG("Drawing indirectly");
		vkCmdDrawIndexedIndirect(vk_hCommandBuffer, aRawGameObjectBuffers[u8CurrentFrameInFlightIndex].get(), 0, 1, sizeof(VkDrawIndexedIndirectCommand));
		record_cmd_end_renderpass(vk_hCommandBuffer);
	}

}
