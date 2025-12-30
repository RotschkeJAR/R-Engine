#include "RE_Renderer_Internal.hpp"
#include "RE_RenderElement.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {

#define RE_VK_COMPUTE_PIPELINE_COUNT 2
#define RE_VK_COMPUTE_PIPELINE_INDEX_PREPROCESSING 0
#define RE_VK_COMPUTE_PIPELINE_INDEX_DEPTH_SORTING 1

	static VkPipelineLayout vk_hComputePipelineLayout;
	static VkShaderModule vk_aComputeShaders[RE_VK_COMPUTE_PIPELINE_COUNT];
	static VkPipeline vk_ahComputePipelines[RE_VK_COMPUTE_PIPELINE_COUNT];
	static Vulkan_Buffer aProcessedRenderBuffers[RE_VK_FRAMES_IN_FLIGHT], aSortableDepthBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_ordering_pipelines() {
		const VkPipelineLayoutCreateInfo vk_computePipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.flags = 0,
			.setLayoutCount = 1,
			.pSetLayouts = &vk_hComputePipelineDescLayout
		};
		if (vkCreatePipelineLayout(vk_hDevice, &vk_computePipelineLayoutCreateInfo, nullptr, &vk_hComputePipelineLayout) == VK_SUCCESS) {
			if (create_vulkan_shader_from_file("", &vk_aComputeShaders[RE_VK_COMPUTE_PIPELINE_INDEX_PREPROCESSING])) {
				if (create_vulkan_shader_from_file("", &vk_aComputeShaders[RE_VK_COMPUTE_PIPELINE_INDEX_DEPTH_SORTING])) {
					const VkComputePipelineCreateInfo vk_aPipelineCreateInfos[RE_VK_COMPUTE_PIPELINE_COUNT] = {
						{
							.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
							.stage = {
								.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
								.stage = VK_SHADER_STAGE_COMPUTE_BIT,
								.module = vk_aComputeShaders[RE_VK_COMPUTE_PIPELINE_INDEX_PREPROCESSING],
								.pName = "main",
								.pSpecializationInfo = nullptr
							},
							.layout = vk_hComputePipelineLayout,
							.basePipelineHandle = VK_NULL_HANDLE,
							.basePipelineIndex = -1
						}, {
							.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
							.stage = {
								.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
								.stage = VK_SHADER_STAGE_COMPUTE_BIT,
								.module = vk_aComputeShaders[RE_VK_COMPUTE_PIPELINE_INDEX_DEPTH_SORTING],
								.pName = "main",
								.pSpecializationInfo = nullptr
							},
							.layout = vk_hComputePipelineLayout,
							.basePipelineHandle = VK_NULL_HANDLE,
							.basePipelineIndex = -1
						}
					};
					if (vkCreateComputePipelines(vk_hDevice, VK_NULL_HANDLE, RE_VK_COMPUTE_PIPELINE_COUNT, vk_aPipelineCreateInfos, nullptr, vk_ahComputePipelines) == VK_SUCCESS)
						return true;
					vkDestroyShaderModule(vk_hDevice, vk_aComputeShaders[RE_VK_COMPUTE_PIPELINE_INDEX_DEPTH_SORTING], nullptr);
				}
				vkDestroyShaderModule(vk_hDevice, vk_aComputeShaders[RE_VK_COMPUTE_PIPELINE_INDEX_PREPROCESSING], nullptr);
			}
			vkDestroyPipelineLayout(vk_hDevice, vk_hComputePipelineLayout, nullptr);
		}
		return false;
	}

	void destroy_ordering_pipelines() {
		vkDestroyPipeline(vk_hDevice, vk_ahComputePipelines[RE_VK_COMPUTE_PIPELINE_INDEX_DEPTH_SORTING], nullptr);
		vkDestroyPipeline(vk_hDevice, vk_ahComputePipelines[RE_VK_COMPUTE_PIPELINE_INDEX_PREPROCESSING], nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_aComputeShaders[RE_VK_COMPUTE_PIPELINE_INDEX_DEPTH_SORTING], nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_aComputeShaders[RE_VK_COMPUTE_PIPELINE_INDEX_PREPROCESSING], nullptr);
		vkDestroyPipelineLayout(vk_hDevice, vk_hComputePipelineLayout, nullptr);
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
