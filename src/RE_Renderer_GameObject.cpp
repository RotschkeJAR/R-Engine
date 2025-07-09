#include "RE_Renderer_GameObject.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Batch_GameObject.hpp"

namespace RE {

	VkShaderModule vk_hOpaqueGameObjectVertexShader = VK_NULL_HANDLE, vk_hOpaqueGameObjectFragmentShader = VK_NULL_HANDLE, vk_hTransparentGameObjectVertexShader = VK_NULL_HANDLE, vk_hTransparentGameObjectFragmentShader = VK_NULL_HANDLE;
	VkPipeline vk_hOpaqueGameObjectGraphicsPipeline = VK_NULL_HANDLE, vk_hTransparentGameObjectPipeline = VK_NULL_HANDLE;

	VkCommandBuffer vk_ahGameObjectVertexTransferCommandBuffers[RE_VK_FRAMES_IN_FLIGHT] = {}, vk_ahGameObjectSecondaryCommandBuffers[RE_VK_FRAMES_IN_FLIGHT] = {};

	bool init_game_object_renderer() {
		if (CATCH_SIGNAL_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_opaque_vertex.glsl.spv", &vk_hOpaqueGameObjectVertexShader), bool)) {
			if (CATCH_SIGNAL_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_opaque_fragment.glsl.spv", &vk_hOpaqueGameObjectFragmentShader), bool)) {
				if (CATCH_SIGNAL_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_transparent_vertex.glsl.spv", &vk_hTransparentGameObjectVertexShader), bool)) {
					if (CATCH_SIGNAL_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_transparent_fragment.glsl.spv", &vk_hTransparentGameObjectFragmentShader), bool)) {
						constexpr uint32_t u32ShaderStageCount = 2U;
						VkPipelineShaderStageCreateInfo vk_aShaderStages[u32ShaderStageCount] = {
							{
								.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
								.stage = VK_SHADER_STAGE_VERTEX_BIT,
								.module = vk_hOpaqueGameObjectVertexShader,
								.pName = "main",
								.pSpecializationInfo = nullptr
							}, {
								.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
								.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
								.module = vk_hOpaqueGameObjectFragmentShader,
								.pName = "main",
								.pSpecializationInfo = nullptr
							},
						};
						VkVertexInputBindingDescription vk_vertexInputBindingDesc = {
							.binding = 0U,
							.stride = RE_VK_OPAQUE_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES,
							.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
						};
						constexpr uint32_t u32VertexInputAttributeDescCount = 2U;
						VkVertexInputAttributeDescription vk_aVertexInputAttributeDescs[u32VertexInputAttributeDescCount] = {
							{
								.location = 0U,
								.binding = 0U,
								.format = VK_FORMAT_R32G32B32_SFLOAT,
								.offset = RE_VK_OPAQUE_GAME_OBJECT_VERTEX_POSITION_OFFSET_BYTES
							}, {
								.location = 1U,
								.binding = 0U,
								.format = VK_FORMAT_R32G32B32_SFLOAT,
								.offset = RE_VK_OPAQUE_GAME_OBJECT_VERTEX_COLOR_OFFSET_BYTES
							}
						};
						const VkPipelineVertexInputStateCreateInfo vk_vertexInputCreateInfo = {
							.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
							.vertexBindingDescriptionCount = 1U,
							.pVertexBindingDescriptions = &vk_vertexInputBindingDesc,
							.vertexAttributeDescriptionCount = u32VertexInputAttributeDescCount,
							.pVertexAttributeDescriptions = vk_aVertexInputAttributeDescs
						};
						constexpr VkPipelineInputAssemblyStateCreateInfo vk_inputAssemblyCreateInfo = {
							.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
							.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
							.primitiveRestartEnable = VK_FALSE
						};
						const VkPipelineViewportStateCreateInfo vk_viewportCreateInfo = {
							.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
							.viewportCount = 1U,
							.pViewports = &vk_cameraViewport,
							.scissorCount = 1U,
							.pScissors = &vk_cameraScissor
						};
						constexpr VkPipelineRasterizationStateCreateInfo vk_rasterizationCreateInfo = {
							.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
							.depthClampEnable = VK_FALSE,
							.rasterizerDiscardEnable = VK_FALSE,
							.polygonMode = VK_POLYGON_MODE_FILL,
							.cullMode = VK_CULL_MODE_NONE,
							.frontFace = VK_FRONT_FACE_CLOCKWISE,
							.depthBiasEnable = VK_FALSE,
							.depthBiasConstantFactor = 0.0f,
							.depthBiasClamp = 0.0f,
							.depthBiasSlopeFactor = 0.0f,
							.lineWidth = 1.0f
						};
						constexpr VkPipelineMultisampleStateCreateInfo vk_multisamplingCreateInfo = {
							.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
							.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
							.sampleShadingEnable = VK_FALSE,
							.minSampleShading = 0.0f,
							.pSampleMask = nullptr,
							.alphaToCoverageEnable = VK_FALSE,
							.alphaToOneEnable = VK_FALSE
						};
						VkPipelineDepthStencilStateCreateInfo vk_depthStencilCreateInfo = {
							.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
							.depthTestEnable = VK_TRUE,
							.depthWriteEnable = VK_TRUE,
							.depthCompareOp = VK_COMPARE_OP_GREATER,
							.depthBoundsTestEnable = VK_FALSE,
							.stencilTestEnable = VK_FALSE,
							.front = {},
							.back = {},
							.minDepthBounds = 0.0f,
							.maxDepthBounds = 1.0f
						};
						VkPipelineColorBlendAttachmentState vk_colorBlendAttachment = {
							.blendEnable = VK_FALSE,
							.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
							.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
							.colorBlendOp = VK_BLEND_OP_ADD,
							.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
							.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
							.alphaBlendOp = VK_BLEND_OP_ADD,
							.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
						};
						const VkPipelineColorBlendStateCreateInfo vk_colorBlendCreateInfo = {
							.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
							.logicOpEnable = VK_FALSE,
							.logicOp = VK_LOGIC_OP_COPY,
							.attachmentCount = 1U,
							.pAttachments = &vk_colorBlendAttachment,
							.blendConstants = {
								0.0f,
								0.0f,
								0.0f,
								1.0f
							}
						};
						constexpr uint32_t u32DynamicStateCount = 2U;
						constexpr VkDynamicState vk_aeDynamicStates[u32DynamicStateCount] = {
							VK_DYNAMIC_STATE_VIEWPORT,
							VK_DYNAMIC_STATE_SCISSOR
						};
						const VkPipelineDynamicStateCreateInfo vk_dynamicStateCreateInfo = {
							.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
							.dynamicStateCount = u32DynamicStateCount,
							.pDynamicStates = vk_aeDynamicStates
						};
						const VkGraphicsPipelineCreateInfo vk_graphicsPipelineCreateInfo = {
							.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
							.stageCount = u32ShaderStageCount,
							.pStages = vk_aShaderStages,
							.pVertexInputState = &vk_vertexInputCreateInfo,
							.pInputAssemblyState = &vk_inputAssemblyCreateInfo,
							.pTessellationState = nullptr,
							.pViewportState = &vk_viewportCreateInfo,
							.pRasterizationState = &vk_rasterizationCreateInfo,
							.pMultisampleState = &vk_multisamplingCreateInfo,
							.pDepthStencilState = &vk_depthStencilCreateInfo,
							.pColorBlendState = &vk_colorBlendCreateInfo,
							.pDynamicState = &vk_dynamicStateCreateInfo,
							.layout = vk_hWorldBasicPipelineLayout,
							.renderPass = vk_hWorldRenderPass,
							.subpass = RE_VK_GAME_OBJECT_SUPBASS,
							.basePipelineHandle = VK_NULL_HANDLE,
							.basePipelineIndex = 0
						};
						if (vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1U, &vk_graphicsPipelineCreateInfo, nullptr, &vk_hOpaqueGameObjectGraphicsPipeline) == VK_SUCCESS) {
							vk_aShaderStages[0].module = vk_hTransparentGameObjectVertexShader;
							vk_aShaderStages[1].module = vk_hTransparentGameObjectFragmentShader;
							vk_vertexInputBindingDesc.stride = RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES;
							vk_aVertexInputAttributeDescs[0].offset = RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_POSITION_OFFSET_BYTES;
							vk_aVertexInputAttributeDescs[1].offset = RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_COLOR_OFFSET_BYTES;
							vk_aVertexInputAttributeDescs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
							vk_depthStencilCreateInfo.depthWriteEnable = VK_FALSE;
							vk_colorBlendAttachment.blendEnable = VK_TRUE;
							vk_colorBlendAttachment.colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
							if (vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1U, &vk_graphicsPipelineCreateInfo, nullptr, &vk_hTransparentGameObjectPipeline) == VK_SUCCESS) {
								if (CATCH_SIGNAL_AND_RETURN(alloc_vulkan_command_buffers(vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_SECONDARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectSecondaryCommandBuffers), bool)) {
									if (CATCH_SIGNAL_AND_RETURN(alloc_vulkan_command_buffers(vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectVertexTransferCommandBuffers), bool)) {
										return CATCH_SIGNAL_AND_RETURN(init_game_object_render_batches(), bool);
									} else
										RE_FATAL_ERROR("Failed allocating Vulkan command buffers for transferring vertex buffer data for rendering opaque game objects to GPU");
									vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectSecondaryCommandBuffers);
								} else
									RE_FATAL_ERROR("Failed allocating Vulkan secondary command buffers for rendering opaque game objects");
								vkDestroyPipeline(vk_hDevice, vk_hTransparentGameObjectPipeline, nullptr);
							} else 
								RE_FATAL_ERROR("Failed creating Vulkan graphics pipeline for rendering transparent game objects");
							vkDestroyPipeline(vk_hDevice, vk_hOpaqueGameObjectGraphicsPipeline, nullptr);
						} else
							RE_FATAL_ERROR("Failed creating Vulkan graphics pipeline for rendering opaque game objects");
						vkDestroyShaderModule(vk_hDevice, vk_hTransparentGameObjectFragmentShader, nullptr);
					} else
						RE_FATAL_ERROR("Failed creating Vulkan fragment shader for rendering transparent game objects");
					vkDestroyShaderModule(vk_hDevice, vk_hTransparentGameObjectVertexShader, nullptr);
				} else
					RE_FATAL_ERROR("Failed creating Vulkan vertex shader for rendering transparent game objects");
				vkDestroyShaderModule(vk_hDevice, vk_hOpaqueGameObjectFragmentShader, nullptr);
			} else
				RE_FATAL_ERROR("Failed creating Vulkan fragment shader for rendering opaque game objects");
			vkDestroyShaderModule(vk_hDevice, vk_hOpaqueGameObjectVertexShader, nullptr);
		} else
			RE_FATAL_ERROR("Failed creating Vulkan vertex shader for rendering opaque game objects");
		std::fill(std::begin(vk_ahGameObjectSecondaryCommandBuffers), std::end(vk_ahGameObjectSecondaryCommandBuffers), VK_NULL_HANDLE);
		vk_hTransparentGameObjectPipeline = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectGraphicsPipeline = VK_NULL_HANDLE;
		vk_hTransparentGameObjectFragmentShader = VK_NULL_HANDLE;
		vk_hTransparentGameObjectVertexShader = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectFragmentShader = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectVertexShader = VK_NULL_HANDLE;
		return false;
	}

	void destroy_game_object_renderer() {
		CATCH_SIGNAL(destroy_game_object_render_batches());
		vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectVertexTransferCommandBuffers);
		vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectSecondaryCommandBuffers);
		vkDestroyPipeline(vk_hDevice, vk_hTransparentGameObjectPipeline, nullptr);
		vkDestroyPipeline(vk_hDevice, vk_hOpaqueGameObjectGraphicsPipeline, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hTransparentGameObjectFragmentShader, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hTransparentGameObjectVertexShader, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hOpaqueGameObjectFragmentShader, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hOpaqueGameObjectVertexShader, nullptr);
		std::fill(std::begin(vk_ahGameObjectVertexTransferCommandBuffers), std::end(vk_ahGameObjectVertexTransferCommandBuffers), VK_NULL_HANDLE);
		std::fill(std::begin(vk_ahGameObjectSecondaryCommandBuffers), std::end(vk_ahGameObjectSecondaryCommandBuffers), VK_NULL_HANDLE);
		vk_hTransparentGameObjectPipeline = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectGraphicsPipeline = VK_NULL_HANDLE;
		vk_hTransparentGameObjectFragmentShader = VK_NULL_HANDLE;
		vk_hTransparentGameObjectVertexShader = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectFragmentShader = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectVertexShader = VK_NULL_HANDLE;
	}

	bool render_game_objects() {
#define SUCCESS_BIT 0U
#define TRANSFER_BIT 1U
		uint8_t u8Progress = 0U;
		if (CATCH_SIGNAL_AND_RETURN(begin_recording_vulkan_command_buffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr), bool)) {
			const VkCommandBufferInheritanceInfo vk_inheritanceInfo = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
				.renderPass = vk_hWorldRenderPass,
				.subpass = RE_VK_GAME_OBJECT_SUPBASS,
				.framebuffer = vk_ahWorldFramebuffers[u8CurrentFrameInFlightIndex],
				.occlusionQueryEnable = VK_FALSE
			};
			if (CATCH_SIGNAL_AND_RETURN(begin_recording_vulkan_command_buffer(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, &vk_inheritanceInfo), bool)) {
				vkCmdBindPipeline(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hOpaqueGameObjectGraphicsPipeline);
				vkCmdSetViewport(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_cameraViewport);
				vkCmdSetScissor(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_cameraScissor);
				vkCmdBindDescriptorSets(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hWorldBasicPipelineLayout, 0U, 1U, &vk_ahWorldCameraDescriptorSets[u8CurrentFrameInFlightIndex], 0U, nullptr);
				vkCmdBindIndexBuffer(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], vk_hRectIndexBuffer, 0UL, VK_INDEX_TYPE_UINT16);
				CATCH_SIGNAL(render_opaque_game_objects());
				vkCmdBindPipeline(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hTransparentGameObjectPipeline);
				vkCmdSetViewport(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_cameraViewport);
				vkCmdSetScissor(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_cameraScissor);
				vkCmdBindDescriptorSets(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hWorldBasicPipelineLayout, 0U, 1U, &vk_ahWorldCameraDescriptorSets[u8CurrentFrameInFlightIndex], 0U, nullptr);
				vkCmdBindIndexBuffer(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], vk_hRectIndexBuffer, 0UL, VK_INDEX_TYPE_UINT16);
				CATCH_SIGNAL(render_transparent_game_objects());
				if (vkEndCommandBuffer(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex]) != VK_SUCCESS)
					RE_FATAL_ERROR("Failed to finish recording a Vulkan secondary command buffer for rendering game objects");
				else
					set_bit<uint8_t>(u8Progress, SUCCESS_BIT, true);
			} else
				RE_FATAL_ERROR("Failed to begin recording a Vulkan secondary command buffer for rendering game objects");
			if (vkEndCommandBuffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex]) != VK_SUCCESS)
				RE_FATAL_ERROR("Failed to finish recording a Vulkan command buffer for transferring vertices for rendering game objects");
		} else
			RE_FATAL_ERROR("Failed to start recording a Vulkan command buffer for transferring vertices to the GPU for rendering game objects");

		if (!is_bit_true<uint8_t>(u8Progress, SUCCESS_BIT))
			return false;
		else if (is_bit_true<uint8_t>(u8Progress, TRANSFER_BIT)) {
			if (!CATCH_SIGNAL_AND_RETURN(submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 0U, nullptr, nullptr, 1U, &vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT + RE_VK_TRANSFER_GAME_OBJECT_VERTICES_SEMAPHORE_INDEX], VK_NULL_HANDLE), bool)) {
				RE_FATAL_ERROR("Failed submitting the Vulkan command buffer for transferring vertices to the GPU for rendering game objects");
				return false;
			}
		} else if (!signal_vulkan_semaphores(1U, &vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT + RE_VK_TRANSFER_GAME_OBJECT_VERTICES_SEMAPHORE_INDEX])) {
			RE_FATAL_ERROR("Failed signaling a Vulkan semaphore used for synchronizing the transfer of game object vertices to the GPU");
			return false;
		}

#undef ERROR_BIT
#undef TRANSFER_BIT
		return true;
	}

}
