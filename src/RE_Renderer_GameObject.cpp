#include "RE_Renderer_GameObject.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_Renderer.hpp"

namespace RE {

	VkShaderModule vk_hOpaqueGameObjectVertexShader = VK_NULL_HANDLE, vk_hOpaqueGameObjectFragmentShader = VK_NULL_HANDLE, vk_hTransparentGameObjectVertexShader = VK_NULL_HANDLE, vk_hTransparentGameObjectFragmentShader = VK_NULL_HANDLE;
	VkPipeline vk_hOpaqueGameObjectGraphicsPipeline = VK_NULL_HANDLE, vk_hTransparentGameObjectGraphicsPipeline = VK_NULL_HANDLE;

	VkCommandBuffer vk_ahGameObjectVertexTransferCommandBuffers[RE_VK_FRAMES_IN_FLIGHT] = {}, vk_ahGameObjectSecondaryCommandBuffers[RE_VK_FRAMES_IN_FLIGHT] = {};

	static bool create_game_object_pipelines() {
		/*constexpr uint32_t u32GraphicsPipelineCount = 2;
		constexpr uint32_t u32ShaderStageCount = 2;
		const VkPipelineShaderStageCreateInfo vk_aaShaderStages[u32GraphicsPipelineCount][u32ShaderStageCount] = {
			{
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
				}
			}, {
				{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_VERTEX_BIT,
					.module = vk_hTransparentGameObjectVertexShader,
					.pName = "main",
					.pSpecializationInfo = nullptr
				}, {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
					.module = vk_hTransparentGameObjectFragmentShader,
					.pName = "main",
					.pSpecializationInfo = nullptr
				}
			}
		};
		constexpr VkVertexInputBindingDescription vk_vertexInputBindingDesc = {
			.binding = 0,
			.stride = RE_VK_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES,
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};
		constexpr uint32_t u32VertexInputAttributeDescCount = 4;
		constexpr VkVertexInputAttributeDescription vk_aaVertexInputAttributeDescs[u32GraphicsPipelineCount][u32VertexInputAttributeDescCount] = {
			{
				{
					.location = 0,
					.binding = 0,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = RE_VK_GAME_OBJECT_VERTEX_POSITION_OFFSET_BYTES
				}, {
					.location = 1,
					.binding = 0,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = RE_VK_GAME_OBJECT_VERTEX_COLOR_OFFSET_BYTES
				}, {
					.location = 2,
					.binding = 0,
					.format = VK_FORMAT_R32_SINT,
					.offset = RE_VK_GAME_OBJECT_VERTEX_TEXTURE_ID_OFFSET_BYTES
				}, {
					.location = 3,
					.binding = 0,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = RE_VK_GAME_OBJECT_VERTEX_TEXTURE_COORDS_OFFSET_BYTES
				}
			}, {
				{
					.location = 0,
					.binding = 0,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = RE_VK_GAME_OBJECT_VERTEX_POSITION_OFFSET_BYTES
				}, {
					.location = 1,
					.binding = 0,
					.format = VK_FORMAT_R32G32B32A32_SFLOAT,
					.offset = RE_VK_GAME_OBJECT_VERTEX_COLOR_OFFSET_BYTES
				}, {
					.location = 2,
					.binding = 0,
					.format = VK_FORMAT_R32_SINT,
					.offset = RE_VK_GAME_OBJECT_VERTEX_TEXTURE_ID_OFFSET_BYTES
				}, {
					.location = 3,
					.binding = 0,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = RE_VK_GAME_OBJECT_VERTEX_TEXTURE_COORDS_OFFSET_BYTES
				}
			}
		};
		const VkPipelineVertexInputStateCreateInfo vk_aVertexInputCreateInfos[u32GraphicsPipelineCount] = {
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.vertexBindingDescriptionCount = 1,
				.pVertexBindingDescriptions = &vk_vertexInputBindingDesc,
				.vertexAttributeDescriptionCount = u32VertexInputAttributeDescCount,
				.pVertexAttributeDescriptions = vk_aaVertexInputAttributeDescs[0]
			}, {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.vertexBindingDescriptionCount = 1,
				.pVertexBindingDescriptions = &vk_vertexInputBindingDesc,
				.vertexAttributeDescriptionCount = u32VertexInputAttributeDescCount,
				.pVertexAttributeDescriptions = vk_aaVertexInputAttributeDescs[1]
			}
		};
		constexpr VkPipelineInputAssemblyStateCreateInfo vk_inputAssemblyCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};
		const VkPipelineViewportStateCreateInfo vk_viewportCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = &vk_cameraViewport,
			.scissorCount = 1,
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
		const VkPipelineMultisampleStateCreateInfo vk_multisamplingCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = vk_eMsaaCount,
			.sampleShadingEnable = vk_bSampleShadingEnabled,
			.minSampleShading = fSampleShadingRate,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};
		constexpr VkPipelineDepthStencilStateCreateInfo vk_aDepthStencilCreateInfos[u32GraphicsPipelineCount] = {
			{
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
			}, {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
				.depthTestEnable = VK_TRUE,
				.depthWriteEnable = VK_FALSE,
				.depthCompareOp = VK_COMPARE_OP_GREATER,
				.depthBoundsTestEnable = VK_FALSE,
				.stencilTestEnable = VK_FALSE,
				.front = {},
				.back = {},
				.minDepthBounds = 0.0f,
				.maxDepthBounds = 1.0f
			}
		};
		constexpr VkPipelineColorBlendAttachmentState vk_aColorBlendAttachments[u32GraphicsPipelineCount] = {
			{
				.blendEnable = VK_FALSE,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
			}, {
				.blendEnable = VK_TRUE,
				.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
				.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				.colorBlendOp = VK_BLEND_OP_ADD,
				.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
				.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
				.alphaBlendOp = VK_BLEND_OP_ADD,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			}
		};
		const VkPipelineColorBlendStateCreateInfo vk_aColorBlendCreateInfo[u32GraphicsPipelineCount] = {
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE,
				.logicOp = VK_LOGIC_OP_COPY,
				.attachmentCount = 1,
				.pAttachments = &vk_aColorBlendAttachments[0],
				.blendConstants = {
					0.0f,
					0.0f,
					0.0f,
					1.0f
				}
			}, {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE,
				.logicOp = VK_LOGIC_OP_COPY,
				.attachmentCount = 1,
				.pAttachments = &vk_aColorBlendAttachments[1],
				.blendConstants = {
					0.0f,
					0.0f,
					0.0f,
					1.0f
				}
			}
		};
		constexpr uint32_t u32DynamicStateCount = 2;
		constexpr VkDynamicState vk_aeDynamicStates[u32DynamicStateCount] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		const VkPipelineDynamicStateCreateInfo vk_dynamicStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = u32DynamicStateCount,
			.pDynamicStates = vk_aeDynamicStates
		};
		const VkGraphicsPipelineCreateInfo vk_a2GraphicsPipelineCreateInfos[u32GraphicsPipelineCount] = {
			{
				.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				.flags = static_cast<VkPipelineCreateFlags>(VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | (vk_hOpaqueGameObjectGraphicsPipeline ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0)),
				.stageCount = u32ShaderStageCount,
				.pStages = vk_aaShaderStages[0],
				.pVertexInputState = &vk_aVertexInputCreateInfos[0],
				.pInputAssemblyState = &vk_inputAssemblyCreateInfo,
				.pTessellationState = nullptr,
				.pViewportState = &vk_viewportCreateInfo,
				.pRasterizationState = &vk_rasterizationCreateInfo,
				.pMultisampleState = &vk_multisamplingCreateInfo,
				.pDepthStencilState = &vk_aDepthStencilCreateInfos[0],
				.pColorBlendState = &vk_aColorBlendCreateInfo[0],
				.pDynamicState = &vk_dynamicStateCreateInfo,
				.layout = vk_hWorldBasicPipelineLayout,
				.renderPass = vk_hWorldRenderPass,
				.subpass = RE_VK_GAME_OBJECT_SUPBASS,
				.basePipelineHandle = vk_hOpaqueGameObjectGraphicsPipeline,
				.basePipelineIndex = -1
			}, {
				.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				.flags = static_cast<VkPipelineCreateFlags>(VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | (vk_hTransparentGameObjectGraphicsPipeline ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0)),
				.stageCount = u32ShaderStageCount,
				.pStages = vk_aaShaderStages[1],
				.pVertexInputState = &vk_aVertexInputCreateInfos[1],
				.pInputAssemblyState = &vk_inputAssemblyCreateInfo,
				.pTessellationState = nullptr,
				.pViewportState = &vk_viewportCreateInfo,
				.pRasterizationState = &vk_rasterizationCreateInfo,
				.pMultisampleState = &vk_multisamplingCreateInfo,
				.pDepthStencilState = &vk_aDepthStencilCreateInfos[1],
				.pColorBlendState = &vk_aColorBlendCreateInfo[1],
				.pDynamicState = &vk_dynamicStateCreateInfo,
				.layout = vk_hWorldBasicPipelineLayout,
				.renderPass = vk_hWorldRenderPass,
				.subpass = RE_VK_GAME_OBJECT_SUPBASS,
				.basePipelineHandle = vk_hTransparentGameObjectGraphicsPipeline,
				.basePipelineIndex = -1
			}
		};
		VkPipeline vk_ahPipelinesCreated[u32GraphicsPipelineCount];
		const bool bSuccess = vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, u32GraphicsPipelineCount, vk_a2GraphicsPipelineCreateInfos, nullptr, vk_ahPipelinesCreated) == VK_SUCCESS;
		if (vk_hOpaqueGameObjectGraphicsPipeline) {
			vkDestroyPipeline(vk_hDevice, vk_hOpaqueGameObjectGraphicsPipeline, nullptr);
			vkDestroyPipeline(vk_hDevice, vk_hTransparentGameObjectGraphicsPipeline, nullptr);
		}
		if (bSuccess) {
			vk_hOpaqueGameObjectGraphicsPipeline = vk_ahPipelinesCreated[0];
			vk_hTransparentGameObjectGraphicsPipeline = vk_ahPipelinesCreated[1];
			return true;
		} else {
			RE_FATAL_ERROR("Failed to create Vulkan graphics pipelines for rendering game objects");
			return false;
		}*/
		return true;
	}

	bool init_game_object_renderer() {
		/*if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_opaque_vertex.glsl.spv", &vk_hOpaqueGameObjectVertexShader), bool)) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_opaque_fragment.glsl.spv", &vk_hOpaqueGameObjectFragmentShader), bool)) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_transparent_vertex.glsl.spv", &vk_hTransparentGameObjectVertexShader), bool)) {
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_transparent_fragment.glsl.spv", &vk_hTransparentGameObjectFragmentShader), bool)) {
						if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_game_object_pipelines(), bool)) {
							if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_vulkan_command_buffers(vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_SECONDARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectSecondaryCommandBuffers), bool)) {
								if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_vulkan_command_buffers(vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectVertexTransferCommandBuffers), bool)) {
									return PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_game_object_render_batches(), bool);
								} else
									RE_FATAL_ERROR("Failed allocating Vulkan command buffers for transferring vertex buffer data for rendering opaque game objects to GPU");
								vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectSecondaryCommandBuffers);
							} else
								RE_FATAL_ERROR("Failed allocating Vulkan secondary command buffers for rendering opaque game objects");
							vkDestroyPipeline(vk_hDevice, vk_hTransparentGameObjectGraphicsPipeline, nullptr);
							vkDestroyPipeline(vk_hDevice, vk_hOpaqueGameObjectGraphicsPipeline, nullptr);
						}
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
		vk_hTransparentGameObjectGraphicsPipeline = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectGraphicsPipeline = VK_NULL_HANDLE;
		vk_hTransparentGameObjectFragmentShader = VK_NULL_HANDLE;
		vk_hTransparentGameObjectVertexShader = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectFragmentShader = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectVertexShader = VK_NULL_HANDLE;
		return false;*/
		return true;
	}

	void destroy_game_object_renderer() {
		/*PUSH_TO_CALLSTACKTRACE(destroy_game_object_render_batches());
		vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectVertexTransferCommandBuffers);
		vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectSecondaryCommandBuffers);
		vkDestroyPipeline(vk_hDevice, vk_hTransparentGameObjectGraphicsPipeline, nullptr);
		vkDestroyPipeline(vk_hDevice, vk_hOpaqueGameObjectGraphicsPipeline, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hTransparentGameObjectFragmentShader, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hTransparentGameObjectVertexShader, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hOpaqueGameObjectFragmentShader, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hOpaqueGameObjectVertexShader, nullptr);
		std::fill(std::begin(vk_ahGameObjectVertexTransferCommandBuffers), std::end(vk_ahGameObjectVertexTransferCommandBuffers), VK_NULL_HANDLE);
		std::fill(std::begin(vk_ahGameObjectSecondaryCommandBuffers), std::end(vk_ahGameObjectSecondaryCommandBuffers), VK_NULL_HANDLE);
		vk_hTransparentGameObjectGraphicsPipeline = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectGraphicsPipeline = VK_NULL_HANDLE;
		vk_hTransparentGameObjectFragmentShader = VK_NULL_HANDLE;
		vk_hTransparentGameObjectVertexShader = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectFragmentShader = VK_NULL_HANDLE;
		vk_hOpaqueGameObjectVertexShader = VK_NULL_HANDLE;*/
	}

	bool load_game_object_vertices_and_transfer(bool &rbNeedsRender) {
		/*bool bSuccess = false;
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(begin_recording_vulkan_command_buffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr), bool)) {
			PUSH_TO_CALLSTACKTRACE(load_game_object_vertices(rbNeedsRender));
			if (vkEndCommandBuffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex]) == VK_SUCCESS)
				bSuccess = true;
			else
				RE_FATAL_ERROR("Failed to finish recording a Vulkan command buffer for transferring vertices for rendering game objects");
		} else
			RE_FATAL_ERROR("Failed to start recording a Vulkan command buffer for transferring vertices to the GPU for rendering game objects");

		if (!bSuccess)
			return false;
		else if (rbNeedsRender) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 0, nullptr, nullptr, 1, &vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], 1, &vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT + RE_VK_TRANSFER_GAME_OBJECT_VERTICES_SEMAPHORE_INDEX], VK_NULL_HANDLE), bool))
				return true;
			else
				RE_FATAL_ERROR("Failed submitting the Vulkan command buffer for transferring vertices to the GPU for rendering game objects");
		} else
			return true;
		return false;*/
		return true;
	}

	bool render_game_objects() {
		/*const VkCommandBufferInheritanceInfo vk_inheritanceInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			.renderPass = vk_hWorldRenderPass,
			.subpass = RE_VK_GAME_OBJECT_SUPBASS,
			.framebuffer = vk_ahWorldFramebuffers[u8CurrentFrameInFlightIndex],
			.occlusionQueryEnable = VK_FALSE
		};
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(begin_recording_vulkan_command_buffer(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, &vk_inheritanceInfo), bool)) {
			vkCmdSetViewport(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0, 1, &vk_cameraViewport);
			vkCmdSetScissor(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0, 1, &vk_cameraScissor);
			vkCmdBindDescriptorSets(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hWorldBasicPipelineLayout, 0, 1, &vk_ahDescriptorSets[u8CurrentFrameInFlightIndex], 0, nullptr);
			vkCmdBindIndexBuffer(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], vk_hRectIndexBuffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindPipeline(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hOpaqueGameObjectGraphicsPipeline);
			PUSH_TO_CALLSTACKTRACE(render_opaque_game_objects());
			vkCmdBindPipeline(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hTransparentGameObjectGraphicsPipeline);
			PUSH_TO_CALLSTACKTRACE(render_transparent_game_objects());
				
			if (vkEndCommandBuffer(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex]) == VK_SUCCESS)
				return true;
			else
				RE_FATAL_ERROR("Failed to finish recording a Vulkan secondary command buffer for rendering game objects");
		} else
			RE_FATAL_ERROR("Failed to begin recording a Vulkan secondary command buffer for rendering game objects");
		return false;*/
		return true;
	}

	bool recreate_game_object_render_pipelines() {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_game_object_pipelines(), bool);
	}

}
