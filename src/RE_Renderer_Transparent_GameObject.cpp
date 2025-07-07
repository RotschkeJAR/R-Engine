#include "RE_Renderer_Transparent_GameObject.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

#define RE_VK_VERTEX_COUNT (RE_VK_RENDERABLE_RECTANGLES_COUNT * 4U)
#define RE_VK_VERTEX_POSITION_SIZE 3U
#define RE_VK_VERTEX_POSITION_SIZE_BYTES (RE_VK_VERTEX_POSITION_SIZE * sizeof(float))
#define RE_VK_VERTEX_POSITION_OFFSET 0U
#define RE_VK_VERTEX_POSITION_OFFSET_BYTES (RE_VK_VERTEX_POSITION_OFFSET * sizeof(float))
#define RE_VK_VERTEX_COLOR_SIZE 4U
#define RE_VK_VERTEX_COLOR_SIZE_BYTES (RE_VK_VERTEX_COLOR_SIZE * sizeof(float))
#define RE_VK_VERTEX_COLOR_OFFSET RE_VK_VERTEX_POSITION_SIZE
#define RE_VK_VERTEX_COLOR_OFFSET_BYTES (RE_VK_VERTEX_COLOR_OFFSET * sizeof(float))
#define RE_VK_VERTEX_TOTAL_SIZE (RE_VK_VERTEX_POSITION_SIZE + RE_VK_VERTEX_COLOR_SIZE)
#define RE_VK_VERTEX_TOTAL_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE * sizeof(float))
#define RE_VK_VERTEX_BUFFER_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE_BYTES * 4U * RE_VK_RENDERABLE_RECTANGLES_COUNT)

	std::queue<GameObject*> queuedTransparentGameObjects;

	VkShaderModule vk_hTransparentGameObjectVertexShader = VK_NULL_HANDLE, vk_hTransparentGameObjectFragmentShader = VK_NULL_HANDLE;
	VkPipeline vk_hTransparentGameObjectPipeline = VK_NULL_HANDLE;

	VkBuffer vk_hTransparentGameObjectStagingVertexBuffer = VK_NULL_HANDLE, vk_ahTransparentGameObjectVertexBuffers[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkDeviceMemory vk_hTransparentGameObjectStagingVertexBufferMemory = VK_NULL_HANDLE, vk_ahTransparentGameObjectVertexBufferMemories[RE_VK_FRAMES_IN_FLIGHT] = {};
	
	VkCommandBuffer vk_ahTransparentGameObjectVertexTransferCommandBuffers[RE_VK_FRAMES_IN_FLIGHT] = {}, vk_ahTransparentGameObjectSecondaryCommandBuffers[RE_VK_FRAMES_IN_FLIGHT] = {};

	float *pafTransparentGameObjectVertices = nullptr;

	bool init_transparent_game_object_renderer() {
		if (CATCH_SIGNAL_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_transparent_vertex.glsl.spv", &vk_hTransparentGameObjectVertexShader), bool)) {
			if (CATCH_SIGNAL_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_transparent_fragment.glsl.spv", &vk_hTransparentGameObjectFragmentShader), bool)) {
				constexpr uint32_t u32ShaderStageCount = 2U;
				const VkPipelineShaderStageCreateInfo vk_aShaderStages[u32ShaderStageCount] = {
					{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.flags = 0,
						.stage = VK_SHADER_STAGE_VERTEX_BIT,
						.module = vk_hTransparentGameObjectVertexShader,
						.pName = "main",
						.pSpecializationInfo = nullptr
					}, {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.flags = 0,
						.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
						.module = vk_hTransparentGameObjectFragmentShader,
						.pName = "main",
						.pSpecializationInfo = nullptr
					}
				};
				const VkVertexInputBindingDescription vk_vertexInputBindingDesc = {
					.binding = 0U,
					.stride = RE_VK_VERTEX_TOTAL_SIZE_BYTES,
					.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
				};
				constexpr uint32_t u32VertexInputAttributeDescCount = 2U;
				const VkVertexInputAttributeDescription vk_aVertexInputAttributeDescs[u32VertexInputAttributeDescCount] = {
					{
						.location = 0U,
						.binding = 0U,
						.format = VK_FORMAT_R32G32B32_SFLOAT,
						.offset = RE_VK_VERTEX_POSITION_OFFSET_BYTES
					}, {
						.location = 1U,
						.binding = 0U,
						.format = VK_FORMAT_R32G32B32A32_SFLOAT,
						.offset = RE_VK_VERTEX_COLOR_OFFSET_BYTES
					}
				};
				const VkPipelineVertexInputStateCreateInfo vk_vertexInputCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
					.flags = 0,
					.vertexBindingDescriptionCount = 1U,
					.pVertexBindingDescriptions = &vk_vertexInputBindingDesc,
					.vertexAttributeDescriptionCount = u32VertexInputAttributeDescCount,
					.pVertexAttributeDescriptions = vk_aVertexInputAttributeDescs
				};
				const VkPipelineInputAssemblyStateCreateInfo vk_inputAssemblyCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
					.flags = 0,
					.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
					.primitiveRestartEnable = VK_FALSE,
				};
				const VkPipelineViewportStateCreateInfo vk_viewportCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
					.flags = 0,
					.viewportCount = 1U,
					.pViewports = &vk_cameraViewport,
					.scissorCount = 1U,
					.pScissors = &vk_cameraScissor
				};
				const VkPipelineRasterizationStateCreateInfo vk_rasterizationCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
					.flags = 0,
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
				const VkPipelineMultisampleStateCreateInfo vk_multisampleCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
					.flags = 0,
					.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
					.sampleShadingEnable = VK_FALSE,
					.minSampleShading = 0.0f,
					.pSampleMask = nullptr,
					.alphaToCoverageEnable = VK_FALSE,
					.alphaToOneEnable = VK_FALSE
				};
				const VkPipelineDepthStencilStateCreateInfo vk_depthStencilCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
					.flags = 0,
					.depthTestEnable = VK_TRUE,
					.depthWriteEnable = VK_FALSE,
					.depthCompareOp = VK_COMPARE_OP_GREATER,
					.depthBoundsTestEnable = VK_FALSE,
					.stencilTestEnable = VK_FALSE,
					.front = {},
					.back = {},
					.minDepthBounds = 0.0f,
					.maxDepthBounds = 1.0f
				};
				const VkPipelineColorBlendAttachmentState vk_colorBlendAttachment = {
					.blendEnable = VK_TRUE,
					.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
					.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
					.colorBlendOp = VK_BLEND_OP_ADD,
					.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
					.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
					.alphaBlendOp = VK_BLEND_OP_ADD,
					.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
				};
				const VkPipelineColorBlendStateCreateInfo vk_colorBlendCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
					.flags = 0,
					.logicOpEnable = VK_FALSE,
					.logicOp = VK_LOGIC_OP_COPY,
					.attachmentCount = 1U,
					.pAttachments = &vk_colorBlendAttachment,
					.blendConstants = {
						0.0f,
						0.0f,
						0.0f,
						0.0f
					}
				};
				constexpr uint32_t u32DynamicStateCount = 2U;
				constexpr VkDynamicState vk_aeDynamicStates[u32DynamicStateCount] = {
					VK_DYNAMIC_STATE_VIEWPORT,
					VK_DYNAMIC_STATE_SCISSOR
				};
				const VkPipelineDynamicStateCreateInfo vk_dynamicCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
					.flags = 0,
					.dynamicStateCount = u32DynamicStateCount,
					.pDynamicStates = vk_aeDynamicStates
				};
				const VkGraphicsPipelineCreateInfo vk_graphicsPipelineCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.flags = 0,
					.stageCount = u32ShaderStageCount,
					.pStages = vk_aShaderStages,
					.pVertexInputState = &vk_vertexInputCreateInfo,
					.pInputAssemblyState = &vk_inputAssemblyCreateInfo,
					.pTessellationState = nullptr,
					.pViewportState = &vk_viewportCreateInfo,
					.pRasterizationState = &vk_rasterizationCreateInfo,
					.pMultisampleState = &vk_multisampleCreateInfo,
					.pDepthStencilState = &vk_depthStencilCreateInfo,
					.pColorBlendState = &vk_colorBlendCreateInfo,
					.pDynamicState = &vk_dynamicCreateInfo,
					.layout = vk_hWorldBasicPipelineLayout,
					.renderPass = vk_hWorldRenderPass,
					.subpass = RE_VK_TRANSPARENT_GAME_OBJECT_SUPBASS,
					.basePipelineHandle = VK_NULL_HANDLE,
					.basePipelineIndex = 0
				};
				if (vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1U, &vk_graphicsPipelineCreateInfo, nullptr, &vk_hTransparentGameObjectPipeline) == VK_SUCCESS) {
					constexpr uint32_t u32StagingVertexBufferQueueCount = 1U, au32StagingVertexBufferQueues[u32StagingVertexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX};
					if (CATCH_SIGNAL_AND_RETURN(create_vulkan_buffer(RE_VK_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingVertexBufferQueueCount, au32StagingVertexBufferQueues, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk_hTransparentGameObjectStagingVertexBuffer, &vk_hTransparentGameObjectStagingVertexBufferMemory), bool)) {
						vkMapMemory(vk_hDevice, vk_hTransparentGameObjectStagingVertexBufferMemory, 0UL, RE_VK_VERTEX_BUFFER_SIZE_BYTES, 0, (void**) &pafTransparentGameObjectVertices);
						constexpr uint32_t u32VertexBufferQueueCount = 2U, au32VertexBufferQueues[u32VertexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX, RE_VK_QUEUE_GRAPHICS_INDEX};
						uint16_t u16VertexBufferCreateIndex = 0U;
						while (u16VertexBufferCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
							if (!CATCH_SIGNAL_AND_RETURN(create_vulkan_buffer(RE_VK_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, u32VertexBufferQueueCount, au32VertexBufferQueues, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahTransparentGameObjectVertexBuffers[u16VertexBufferCreateIndex], &vk_ahTransparentGameObjectVertexBufferMemories[u16VertexBufferCreateIndex]), bool)) {
								RE_FATAL_ERROR(append_to_string("Failed to create Vulkan vertex buffer at index ", u16VertexBufferCreateIndex, " for rendering transparent game objects"));
								break;
							}
							u16VertexBufferCreateIndex++;
						}
						if (u16VertexBufferCreateIndex == RE_VK_FRAMES_IN_FLIGHT) {
							if (CATCH_SIGNAL_AND_RETURN(alloc_vulkan_command_buffers(vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_SECONDARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahTransparentGameObjectSecondaryCommandBuffers), bool)) {
								if (CATCH_SIGNAL_AND_RETURN(alloc_vulkan_command_buffers(vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahTransparentGameObjectVertexTransferCommandBuffers), bool))
									return true;
								else
									RE_FATAL_ERROR("Failed allocating Vulkan command buffers for transfering vertices to the GPU for rendering transparent game objects");
								vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahTransparentGameObjectSecondaryCommandBuffers);
							} else
								RE_FATAL_ERROR("Failed allocating Vulkan command buffers for rendering transparent game objects");
						}
						for (uint16_t u16VertexBufferDeleteIndex = 0U; u16VertexBufferDeleteIndex < u16VertexBufferCreateIndex; u16VertexBufferDeleteIndex++) {
							vkFreeMemory(vk_hDevice, vk_ahTransparentGameObjectVertexBufferMemories[u16VertexBufferDeleteIndex], nullptr);
							vkDestroyBuffer(vk_hDevice, vk_ahTransparentGameObjectVertexBuffers[u16VertexBufferDeleteIndex], nullptr);
							vk_ahTransparentGameObjectVertexBufferMemories[u16VertexBufferDeleteIndex] = VK_NULL_HANDLE;
							vk_ahTransparentGameObjectVertexBuffers[u16VertexBufferDeleteIndex] = VK_NULL_HANDLE;
						}
						vkUnmapMemory(vk_hDevice, vk_hTransparentGameObjectStagingVertexBufferMemory);
						vkFreeMemory(vk_hDevice, vk_hTransparentGameObjectStagingVertexBufferMemory, nullptr);
						vkDestroyBuffer(vk_hDevice, vk_hTransparentGameObjectStagingVertexBuffer, nullptr);
					} else
						RE_FATAL_ERROR("Failed creating Vulkan staging vertex buffer for rendering transparent game objects");
					vkDestroyPipeline(vk_hDevice, vk_hTransparentGameObjectPipeline, nullptr);
				} else
					RE_FATAL_ERROR("Failed creating Vulkan graphics pipeline for rendering transparent game objects");
				vkDestroyShaderModule(vk_hDevice, vk_hTransparentGameObjectFragmentShader, nullptr);
			} else
				RE_FATAL_ERROR("Failed creating Vulkan fragment shader for rendering transparent game objects");
			vkDestroyShaderModule(vk_hDevice, vk_hTransparentGameObjectVertexShader, nullptr);
		} else
			RE_FATAL_ERROR("Failed creating Vulkan vertex shader for rendering transparent game objects");
		pafTransparentGameObjectVertices = nullptr;
		std::fill(std::begin(vk_ahTransparentGameObjectSecondaryCommandBuffers), std::end(vk_ahTransparentGameObjectSecondaryCommandBuffers), VK_NULL_HANDLE);
		vk_hTransparentGameObjectStagingVertexBufferMemory = VK_NULL_HANDLE;
		vk_hTransparentGameObjectStagingVertexBuffer = VK_NULL_HANDLE;
		vk_hTransparentGameObjectPipeline = VK_NULL_HANDLE;
		vk_hTransparentGameObjectFragmentShader = VK_NULL_HANDLE;
		vk_hTransparentGameObjectVertexShader = VK_NULL_HANDLE;
		return false;
	}
	
	void destroy_transparent_game_object_renderer() {
		vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahTransparentGameObjectVertexTransferCommandBuffers);
		vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahTransparentGameObjectSecondaryCommandBuffers);
		for (uint16_t u16VertexBufferDeleteIndex = 0U; u16VertexBufferDeleteIndex < RE_VK_FRAMES_IN_FLIGHT; u16VertexBufferDeleteIndex++) {
			vkFreeMemory(vk_hDevice, vk_ahTransparentGameObjectVertexBufferMemories[u16VertexBufferDeleteIndex], nullptr);
			vkDestroyBuffer(vk_hDevice, vk_ahTransparentGameObjectVertexBuffers[u16VertexBufferDeleteIndex], nullptr);
			vk_ahTransparentGameObjectVertexBufferMemories[u16VertexBufferDeleteIndex] = VK_NULL_HANDLE;
			vk_ahTransparentGameObjectVertexBuffers[u16VertexBufferDeleteIndex] = VK_NULL_HANDLE;
		}
		vkUnmapMemory(vk_hDevice, vk_hTransparentGameObjectStagingVertexBufferMemory);
		vkFreeMemory(vk_hDevice, vk_hTransparentGameObjectStagingVertexBufferMemory, nullptr);
		vkDestroyBuffer(vk_hDevice, vk_hTransparentGameObjectStagingVertexBuffer, nullptr);
		vkDestroyPipeline(vk_hDevice, vk_hTransparentGameObjectPipeline, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hTransparentGameObjectFragmentShader, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hTransparentGameObjectVertexShader, nullptr);
		pafTransparentGameObjectVertices = nullptr;
		std::fill(std::begin(vk_ahTransparentGameObjectVertexTransferCommandBuffers), std::end(vk_ahTransparentGameObjectVertexTransferCommandBuffers), VK_NULL_HANDLE);
		std::fill(std::begin(vk_ahTransparentGameObjectSecondaryCommandBuffers), std::end(vk_ahTransparentGameObjectSecondaryCommandBuffers), VK_NULL_HANDLE);
		vk_hTransparentGameObjectStagingVertexBufferMemory = VK_NULL_HANDLE;
		vk_hTransparentGameObjectStagingVertexBuffer = VK_NULL_HANDLE;
		vk_hTransparentGameObjectPipeline = VK_NULL_HANDLE;
		vk_hTransparentGameObjectFragmentShader = VK_NULL_HANDLE;
		vk_hTransparentGameObjectVertexShader = VK_NULL_HANDLE;
	}

	bool render_transparent_game_object() {
		uint16_t u16GameObjectIndex = 0U;
		while (!queuedTransparentGameObjects.empty()) {
			const GameObject *pObject = queuedTransparentGameObjects.front();
			queuedTransparentGameObjects.pop();
			const float a2fObjectScale[2] = {
				pObject->transform.scale[0] / 2.0f,
				pObject->transform.scale[1] / 2.0f
			};
			for (uint16_t u16VertexIndex = 0U; u16VertexIndex < 4U; u16VertexIndex++) {
				const uint16_t u16VertexOffset = u16GameObjectIndex * RE_VK_VERTEX_TOTAL_SIZE * 4U + u16VertexIndex * RE_VK_VERTEX_TOTAL_SIZE;
				switch (u16VertexIndex) {
					case 0U:
						pafTransparentGameObjectVertices[u16VertexOffset + 0U] = pObject->transform.position[0] - a2fObjectScale[0];
						pafTransparentGameObjectVertices[u16VertexOffset + 1U] = pObject->transform.position[1] + a2fObjectScale[1];
						break;
					case 1U:
						pafTransparentGameObjectVertices[u16VertexOffset + 0U] = pObject->transform.position[0] + a2fObjectScale[0];
						pafTransparentGameObjectVertices[u16VertexOffset + 1U] = pObject->transform.position[1] + a2fObjectScale[1];
						break;
					case 2U:
						pafTransparentGameObjectVertices[u16VertexOffset + 0U] = pObject->transform.position[0] + a2fObjectScale[0];
						pafTransparentGameObjectVertices[u16VertexOffset + 1U] = pObject->transform.position[1] - a2fObjectScale[1];
						break;
					case 3U:
						pafTransparentGameObjectVertices[u16VertexOffset + 0U] = pObject->transform.position[0] - a2fObjectScale[0];
						pafTransparentGameObjectVertices[u16VertexOffset + 1U] = pObject->transform.position[1] - a2fObjectScale[1];
						break;
				}
				pafTransparentGameObjectVertices[u16VertexOffset + 2U] = pObject->transform.position[2];
				pafTransparentGameObjectVertices[u16VertexOffset + 3U] = pObject->spriteRenderer.color[0];
				pafTransparentGameObjectVertices[u16VertexOffset + 4U] = pObject->spriteRenderer.color[1];
				pafTransparentGameObjectVertices[u16VertexOffset + 5U] = pObject->spriteRenderer.color[2];
				pafTransparentGameObjectVertices[u16VertexOffset + 6U] = pObject->spriteRenderer.color[3];
			}
			u16GameObjectIndex++;
		}

		if (u16GameObjectIndex) {
			if (!CATCH_SIGNAL_AND_RETURN(begin_recording_vulkan_command_buffer(vk_ahTransparentGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr), bool)) {
				RE_FATAL_ERROR("Failed beginning to record Vulkan command buffer for transferring vertex buffer data to the GPU for rendering transparent game objects");
				return false;
			}
			const VkBufferCopy vk_vertexBufferCopy = {
				.size = RE_VK_VERTEX_TOTAL_SIZE_BYTES * 4U * u16GameObjectIndex
			};
			vkCmdCopyBuffer(vk_ahTransparentGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], vk_hTransparentGameObjectStagingVertexBuffer, vk_ahTransparentGameObjectVertexBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_vertexBufferCopy);
			if (vkEndCommandBuffer(vk_ahTransparentGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex]) != VK_SUCCESS) {
				RE_FATAL_ERROR("Failed finishing to record Vulkan command buffer for transferring vertex buffer data to the GPU for rendering transparent game objects");
				return false;
			}
			if (!CATCH_SIGNAL_AND_RETURN(submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 0U, nullptr, nullptr, 1U, &vk_ahTransparentGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT + RE_VK_TRANSPARENT_GAME_OBJECT_SEMAPHORE_INDEX], VK_NULL_HANDLE), bool)) {
				RE_FATAL_ERROR("Failed submitting the task for transferring Vulkan vertex buffer data for rendering opaque game objects to the GPU");
				return false;
			}
		} else if (!CATCH_SIGNAL_AND_RETURN(signal_vulkan_semaphores(1U, &vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT + RE_VK_TRANSPARENT_GAME_OBJECT_SEMAPHORE_INDEX]), bool)) {
			RE_FATAL_ERROR("Failed signaling render semaphore for synchronizing the transfer of transparent game object vertices to GPU");
			return false;
		}
		const VkCommandBufferInheritanceInfo vk_inheritanceInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			.renderPass = vk_hWorldRenderPass,
			.subpass = RE_VK_TRANSPARENT_GAME_OBJECT_SUPBASS,
			.framebuffer = vk_ahWorldFramebuffers[u8CurrentFrameInFlightIndex],
			.occlusionQueryEnable = VK_FALSE,
			.queryFlags = 0,
			.pipelineStatistics = 0
		};
		if (!CATCH_SIGNAL_AND_RETURN(begin_recording_vulkan_command_buffer(vk_ahTransparentGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, &vk_inheritanceInfo), bool)) {
			RE_FATAL_ERROR("Failed beginning to record Vulkan secondary command buffer for rendering transparent game objects");
			return false;
		}
		if (u16GameObjectIndex) {
			vkCmdBindPipeline(vk_ahTransparentGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hTransparentGameObjectPipeline);
			vkCmdSetViewport(vk_ahTransparentGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_cameraViewport);
			vkCmdSetScissor(vk_ahTransparentGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_cameraScissor);
			vkCmdBindIndexBuffer(vk_ahTransparentGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], vk_hRectIndexBuffer, 0UL, VK_INDEX_TYPE_UINT16);
			vkCmdBindDescriptorSets(vk_ahTransparentGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hWorldBasicPipelineLayout, 0U, 1U, &vk_ahWorldCameraDescriptorSets[u8CurrentFrameInFlightIndex], 0U, nullptr);
			constexpr uint32_t u32VertexBufferCount = 1U;
			constexpr VkDeviceSize vk_vertexBufferOffsets[u32VertexBufferCount] = {0UL};
			vkCmdBindVertexBuffers(vk_ahTransparentGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, u32VertexBufferCount, &vk_ahTransparentGameObjectVertexBuffers[u8CurrentFrameInFlightIndex], vk_vertexBufferOffsets);
			vkCmdDrawIndexed(vk_ahTransparentGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], u16GameObjectIndex * 6U, 1U, 0U, 0U, 0U);
		}
		if (vkEndCommandBuffer(vk_ahTransparentGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex]) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed finishing recording Vulkan command buffer for rendering transparent game objects");
			return false;
		}
		return true;
	}

}
