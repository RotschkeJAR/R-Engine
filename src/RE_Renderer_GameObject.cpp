#include "RE_Renderer.hpp"
#include "RE_Manager.hpp"
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

#define RE_VK_VIEW_MATRIX_SIZE (4U * 4U)
#define RE_VK_VIEW_MATRIX_OFFSET 0U
#define RE_VK_PROJECTION_MATRIX_SIZE (4U * 4U)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE

#define RE_VK_SHADER_STAGE_COUNT 2U
#define RE_VK_VERTEX_INPUT_BINDING_DESCRIPTION_COUNT 1U
#define RE_VK_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT 2U
#define RE_VK_COLOR_BLEND_ATTACHMENT_COUNT 1U
#define RE_VK_DYNAMIC_STATE_COUNT 2U

	VkShaderModule vk_hGameObjectVertexShader = VK_NULL_HANDLE, vk_hGameObjectFragmentShader = VK_NULL_HANDLE;
	VkPipelineLayout vk_hGameObjectPipelineLayout = VK_NULL_HANDLE;
	VkPipeline vk_hGameObjectGraphicsPipeline = VK_NULL_HANDLE;

	VkBuffer vk_hGameObjectStagingVertexBuffer = VK_NULL_HANDLE, vk_ahGameObjectVertexBuffers[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkDeviceMemory vk_hGameObjectStagingVertexBufferMemory = VK_NULL_HANDLE, vk_ahGameObjectVertexBufferMemories[RE_VK_FRAMES_IN_FLIGHT] = {};

	VkCommandBuffer vk_ahGameObjectVertexTransferCommandBuffers[RE_VK_FRAMES_IN_FLIGHT] = {}, vk_ahGameObjectSecondaryCommandBuffers[RE_VK_FRAMES_IN_FLIGHT] = {};

	float *pfGameObjectVertices = nullptr;

	bool init_gameobject_renderer() {
		if (CATCH_SIGNAL_AND_RETURN(create_vulkan_shader_from_file("shaders/vertex.spv", &vk_hGameObjectVertexShader), bool)) {
			if (CATCH_SIGNAL_AND_RETURN(create_vulkan_shader_from_file("shaders/fragment.spv", &vk_hGameObjectFragmentShader), bool)) {
				const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
					.flags = 0,
					.setLayoutCount = 0U,
					.pSetLayouts = nullptr,
					.pushConstantRangeCount = 0U,
					.pPushConstantRanges = nullptr
				};
				if (vkCreatePipelineLayout(vk_hDevice, &vk_pipelineLayoutCreateInfo, nullptr, &vk_hGameObjectPipelineLayout) == VK_SUCCESS) {
					const VkPipelineShaderStageCreateInfo vk_shaderStages[RE_VK_SHADER_STAGE_COUNT] = {
						{
							.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
							.flags = 0,
							.stage = VK_SHADER_STAGE_VERTEX_BIT,
							.module = vk_hGameObjectVertexShader,
							.pName = "main",
							.pSpecializationInfo = nullptr
						}, {
							.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
							.flags = 0,
							.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
							.module = vk_hGameObjectFragmentShader,
							.pName = "main",
							.pSpecializationInfo = nullptr
						},
					};
					const VkVertexInputBindingDescription vk_vertexInputBindingDescriptions[RE_VK_VERTEX_INPUT_BINDING_DESCRIPTION_COUNT] = {
						{
							.binding = 0U,
							.stride = RE_VK_VERTEX_TOTAL_SIZE_BYTES,
							.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
						}
					};
					const VkVertexInputAttributeDescription vk_vertexInputAttributeDescriptions[RE_VK_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT] = {
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
						.vertexBindingDescriptionCount = RE_VK_VERTEX_INPUT_BINDING_DESCRIPTION_COUNT,
						.pVertexBindingDescriptions = vk_vertexInputBindingDescriptions,
						.vertexAttributeDescriptionCount = RE_VK_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT,
						.pVertexAttributeDescriptions = vk_vertexInputAttributeDescriptions
					};
					const VkPipelineInputAssemblyStateCreateInfo vk_inputAssemblyCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
						.flags = 0,
						.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
						.primitiveRestartEnable = VK_FALSE
					};
					const VkViewport vk_viewport = {
						.x = 0.0f,
						.y = 0.0f,
						.width = static_cast<float>(vk_worldRenderImageExtent.width),
						.height = static_cast<float>(vk_worldRenderImageExtent.height),
						.minDepth = 0.0f,
						.maxDepth = 1.0f
					};
					const VkRect2D vk_scissor = {
						.offset = {
							.x = 0,
							.y = 0
						},
						.extent = {
							.width = vk_worldRenderImageExtent.width,
							.height = vk_worldRenderImageExtent.height
						}
					};
					const VkPipelineViewportStateCreateInfo vk_viewportCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
						.flags = 0,
						.viewportCount = 1U,
						.pViewports = &vk_viewport,
						.scissorCount = 1U,
						.pScissors = &vk_scissor
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
					const VkPipelineMultisampleStateCreateInfo vk_multisamplingCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
						.flags = 0,
						.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
						.sampleShadingEnable = VK_FALSE,
						.minSampleShading = 1.0f,
						.pSampleMask = nullptr,
						.alphaToCoverageEnable = VK_FALSE,
						.alphaToOneEnable = VK_FALSE
					};
					const VkPipelineColorBlendAttachmentState vk_colorBlendAttachments[RE_VK_COLOR_BLEND_ATTACHMENT_COUNT] = {
						{
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
					const VkPipelineColorBlendStateCreateInfo vk_colorBlendCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
						.flags = 0,
						.logicOpEnable = VK_FALSE,
						.logicOp = VK_LOGIC_OP_COPY,
						.attachmentCount = RE_VK_COLOR_BLEND_ATTACHMENT_COUNT,
						.pAttachments = vk_colorBlendAttachments,
						.blendConstants = {
							0.0f,
							0.0f,
							0.0f,
							0.0f
						}
					};
					const VkDynamicState vk_eDynamicStates[RE_VK_DYNAMIC_STATE_COUNT] = {
						VK_DYNAMIC_STATE_VIEWPORT,
						VK_DYNAMIC_STATE_SCISSOR
					};
					const VkPipelineDynamicStateCreateInfo vk_dynamicStateCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
						.flags = 0,
						.dynamicStateCount = RE_VK_DYNAMIC_STATE_COUNT,
						.pDynamicStates = vk_eDynamicStates
					};
					const VkGraphicsPipelineCreateInfo vk_graphicsPipelineCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
						.flags = 0,
						.stageCount = RE_VK_SHADER_STAGE_COUNT,
						.pStages = vk_shaderStages,
						.pVertexInputState = &vk_vertexInputCreateInfo,
						.pInputAssemblyState = &vk_inputAssemblyCreateInfo,
						.pTessellationState = nullptr,
						.pViewportState = &vk_viewportCreateInfo,
						.pRasterizationState = &vk_rasterizationCreateInfo,
						.pMultisampleState = &vk_multisamplingCreateInfo,
						.pDepthStencilState = nullptr,
						.pColorBlendState = &vk_colorBlendCreateInfo,
						.pDynamicState = &vk_dynamicStateCreateInfo,
						.layout = vk_hGameObjectPipelineLayout,
						.renderPass = vk_hWorldRenderPass,
						.subpass = RE_VK_GAME_OBJECT_SUPBASS
					};
					if (vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1U, &vk_graphicsPipelineCreateInfo, nullptr, &vk_hGameObjectGraphicsPipeline) == VK_SUCCESS) {
						constexpr uint32_t u32StagingVertexBufferQueueCount = 1U, u32StagingVertexBufferQueues[u32StagingVertexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX};
						if (CATCH_SIGNAL_AND_RETURN(create_vulkan_buffer(RE_VK_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingVertexBufferQueueCount, u32StagingVertexBufferQueues, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk_hGameObjectStagingVertexBuffer, &vk_hGameObjectStagingVertexBufferMemory), bool)) {
							vkMapMemory(vk_hDevice, vk_hGameObjectStagingVertexBufferMemory, 0UL, RE_VK_VERTEX_BUFFER_SIZE_BYTES, 0, (void**) &pfGameObjectVertices);
							constexpr uint32_t u32VertexBufferQueueCount = 2U, u32VertexBufferQueues[u32VertexBufferQueueCount] = {RE_VK_QUEUE_GRAPHICS_INDEX, RE_VK_QUEUE_TRANSFER_INDEX};
							uint16_t u16VertexBufferCreateIndex = 0U;
							while (u16VertexBufferCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
								if (!CATCH_SIGNAL_AND_RETURN(create_vulkan_buffer(RE_VK_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, u32VertexBufferQueueCount, u32VertexBufferQueues, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahGameObjectVertexBuffers[u16VertexBufferCreateIndex], &vk_ahGameObjectVertexBufferMemories[u16VertexBufferCreateIndex]), bool)) {
									RE_FATAL_ERROR(append_to_string("Failed creating Vulkan vertex buffer or allocating buffer memory for rendering game objects at index ", u16VertexBufferCreateIndex));
									break;
								}
								u16VertexBufferCreateIndex++;
							}
							if (u16VertexBufferCreateIndex == RE_VK_FRAMES_IN_FLIGHT) {
								if (CATCH_SIGNAL_AND_RETURN(alloc_vulkan_command_buffers(vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_SECONDARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectSecondaryCommandBuffers), bool)) {
									if (CATCH_SIGNAL_AND_RETURN(alloc_vulkan_command_buffers(vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectVertexTransferCommandBuffers), bool))
										return true;
									else
										RE_FATAL_ERROR("Failed allocating Vulkan command buffers for transfering vertex buffer data for rendering game objects to GPU");
									vkFreeCommandBuffers(vk_hDevice, vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectSecondaryCommandBuffers);
								} else
									RE_FATAL_ERROR("Failed allocating Vulkan secondary command buffers for rendering game objects");
							}
							for (uint16_t u16VertexBufferDeleteIndex = 0U; u16VertexBufferDeleteIndex < u16VertexBufferCreateIndex; u16VertexBufferDeleteIndex++) {
								vkFreeMemory(vk_hDevice, vk_ahGameObjectVertexBufferMemories[u16VertexBufferDeleteIndex], nullptr);
								vk_ahGameObjectVertexBufferMemories[u16VertexBufferDeleteIndex] = VK_NULL_HANDLE;
								vkDestroyBuffer(vk_hDevice, vk_ahGameObjectVertexBuffers[u16VertexBufferDeleteIndex], nullptr);
								vk_ahGameObjectVertexBuffers[u16VertexBufferDeleteIndex] = VK_NULL_HANDLE;
							}
							vkUnmapMemory(vk_hDevice, vk_hGameObjectStagingVertexBufferMemory);
							pfGameObjectVertices = nullptr;
							vkFreeMemory(vk_hDevice, vk_hGameObjectStagingVertexBufferMemory, nullptr);
							vk_hGameObjectStagingVertexBufferMemory = VK_NULL_HANDLE;
							vkDestroyBuffer(vk_hDevice, vk_hGameObjectStagingVertexBuffer, nullptr);
							vk_hGameObjectStagingVertexBuffer = VK_NULL_HANDLE;
						} else
							RE_FATAL_ERROR("Failed creating Vulkan staging vertex buffer for rendering game objects");
						vkDestroyPipeline(vk_hDevice, vk_hGameObjectGraphicsPipeline, nullptr);
						vk_hGameObjectGraphicsPipeline = VK_NULL_HANDLE;
					} else
						RE_FATAL_ERROR("Failed creating Vulkan graphics pipeline for rendering game objects");
					vkDestroyPipelineLayout(vk_hDevice, vk_hGameObjectPipelineLayout, nullptr);
					vk_hGameObjectPipelineLayout = VK_NULL_HANDLE;
				} else
					RE_FATAL_ERROR("Failed creating Vulkan pipeline layout for rendering game objects");
				vkDestroyShaderModule(vk_hDevice, vk_hGameObjectFragmentShader, nullptr);
				vk_hGameObjectFragmentShader = VK_NULL_HANDLE;
			} else
				RE_FATAL_ERROR("Failed creating Vulkan fragment shader for rendering game objects");
			vkDestroyShaderModule(vk_hDevice, vk_hGameObjectVertexShader, nullptr);
			vk_hGameObjectVertexShader = VK_NULL_HANDLE;
		} else
			RE_FATAL_ERROR("Failed creating Vulkan vertex shader for rendering game objects");
		return false;
	}

	void destroy_gameobject_renderer() {
		vkUnmapMemory(vk_hDevice, vk_hGameObjectStagingVertexBufferMemory);
		pfGameObjectVertices = nullptr;
		vkFreeCommandBuffers(vk_hDevice, vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahGameObjectSecondaryCommandBuffers);
		for (uint16_t u16VertexBufferDeleteIndex = 0U; u16VertexBufferDeleteIndex < RE_VK_FRAMES_IN_FLIGHT; u16VertexBufferDeleteIndex++) {
			vkFreeMemory(vk_hDevice, vk_ahGameObjectVertexBufferMemories[u16VertexBufferDeleteIndex], nullptr);
			vk_ahGameObjectVertexBufferMemories[u16VertexBufferDeleteIndex] = VK_NULL_HANDLE;
			vkDestroyBuffer(vk_hDevice, vk_ahGameObjectVertexBuffers[u16VertexBufferDeleteIndex], nullptr);
			vk_ahGameObjectVertexBuffers[u16VertexBufferDeleteIndex] = VK_NULL_HANDLE;
		}
		vkFreeMemory(vk_hDevice, vk_hGameObjectStagingVertexBufferMemory, nullptr);
		vk_hGameObjectStagingVertexBufferMemory = VK_NULL_HANDLE;
		vkDestroyBuffer(vk_hDevice, vk_hGameObjectStagingVertexBuffer, nullptr);
		vk_hGameObjectStagingVertexBuffer = VK_NULL_HANDLE;
		vkDestroyPipeline(vk_hDevice, vk_hGameObjectGraphicsPipeline, nullptr);
		vk_hGameObjectGraphicsPipeline = VK_NULL_HANDLE;
		vkDestroyPipelineLayout(vk_hDevice, vk_hGameObjectPipelineLayout, nullptr);
		vk_hGameObjectPipelineLayout = VK_NULL_HANDLE;
		vkDestroyShaderModule(vk_hDevice, vk_hGameObjectVertexShader, nullptr);
		vk_hGameObjectVertexShader = VK_NULL_HANDLE;
		vkDestroyShaderModule(vk_hDevice, vk_hGameObjectFragmentShader, nullptr);
		vk_hGameObjectFragmentShader = VK_NULL_HANDLE;
	}

	bool render_gameobjects() {
		uint16_t u16GameObjectIndex = 0U;
		for (GameObject *pObject : gameObjects) {
			if (pObject->u32SceneParentId != get_current_scene_id())
				continue;
			for (uint16_t u16VertexIndex = 0U; u16VertexIndex < 4U; u16VertexIndex++) {
				const uint16_t u16VertexOffset = u16GameObjectIndex * RE_VK_VERTEX_TOTAL_SIZE * 4U + u16VertexIndex * RE_VK_VERTEX_TOTAL_SIZE;
				switch (u16VertexIndex) {
					case 0U:
						pfGameObjectVertices[u16VertexOffset + 0U] = pObject->transform.position[0] - pObject->transform.scale[0];
						pfGameObjectVertices[u16VertexOffset + 1U] = pObject->transform.position[1] + pObject->transform.scale[1];
						break;
					case 1U:
						pfGameObjectVertices[u16VertexOffset + 0U] = pObject->transform.position[0] + pObject->transform.scale[0];
						pfGameObjectVertices[u16VertexOffset + 1U] = pObject->transform.position[1] + pObject->transform.scale[1];
						break;
					case 2U:
						pfGameObjectVertices[u16VertexOffset + 0U] = pObject->transform.position[0] + pObject->transform.scale[0];
						pfGameObjectVertices[u16VertexOffset + 1U] = pObject->transform.position[1] - pObject->transform.scale[1];
						break;
					case 3U:
						pfGameObjectVertices[u16VertexOffset + 0U] = pObject->transform.position[0] - pObject->transform.scale[0];
						pfGameObjectVertices[u16VertexOffset + 1U] = pObject->transform.position[1] - pObject->transform.scale[1];
						break;
				}
				pfGameObjectVertices[u16VertexOffset + 2U] = pObject->transform.position[2];
				pfGameObjectVertices[u16VertexOffset + 3U] = pObject->spriteRenderer.color[0];
				pfGameObjectVertices[u16VertexOffset + 4U] = pObject->spriteRenderer.color[1];
				pfGameObjectVertices[u16VertexOffset + 5U] = pObject->spriteRenderer.color[2];
				pfGameObjectVertices[u16VertexOffset + 6U] = pObject->spriteRenderer.color[3];
			}
			u16GameObjectIndex++;
		}
		if (u16GameObjectIndex) {
			if (!CATCH_SIGNAL_AND_RETURN(begin_recording_vulkan_command_buffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr), bool)) {
				RE_FATAL_ERROR("Failed beginning to record Vulkan command buffer for transferring vertex buffer data to the GPU for rendering game objects");
				return false;
			}
			const VkBufferCopy vk_vertexBufferCopy = {
				.size = RE_VK_VERTEX_TOTAL_SIZE_BYTES * 4U * u16GameObjectIndex
			};
			vkCmdCopyBuffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], vk_hGameObjectStagingVertexBuffer, vk_ahGameObjectVertexBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_vertexBufferCopy);
			if (vkEndCommandBuffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex]) != VK_SUCCESS) {
				RE_FATAL_ERROR("Failed finishing to record Vulkan command buffer for transferring vertex buffer data to the GPU for rendering game objects");
				return false;
			}
			if (!CATCH_SIGNAL_AND_RETURN(submit_to_vulkan_queue(vk_hDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 0U, nullptr, nullptr, 1U, &vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT], VK_NULL_HANDLE), bool)) {
				RE_FATAL_ERROR("Failed submitting the task for transferring Vulkan vertex buffer data for rendering game objects to the GPU");
				return false;
			}
		} else if (!signal_vulkan_semaphores(1U, &vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT])) {
			RE_FATAL_ERROR("Failed signaling render semaphore synchronizing transferring game object vertices to GPU");
			return false;
		}
		const VkCommandBufferInheritanceInfo vk_inheritanceInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			.renderPass = vk_hWorldRenderPass,
			.subpass = RE_VK_GAME_OBJECT_SUPBASS,
			.framebuffer = vk_ahWorldFramebuffers[u8CurrentFrameInFlightIndex],
			.occlusionQueryEnable = VK_FALSE,
			.queryFlags = 0,
			.pipelineStatistics = 0
		};
		if (!CATCH_SIGNAL_AND_RETURN(begin_recording_vulkan_command_buffer(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, &vk_inheritanceInfo), bool)) {
			RE_FATAL_ERROR("Failed beginning to record Vulkan secondary command buffer for rendering game objects");
			return false;
		}
		vkCmdBindPipeline(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGameObjectGraphicsPipeline);
		const VkViewport vk_viewport = {
			.width = static_cast<float>(vk_worldRenderImageExtent.width),
			.height = static_cast<float>(vk_worldRenderImageExtent.height),
			.maxDepth = 1.0f
		};
		vkCmdSetViewport(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_viewport);
		const VkRect2D vk_scissor = {
			.extent = vk_worldRenderImageExtent
		};
		vkCmdSetScissor(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_scissor);
		vkCmdBindIndexBuffer(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], vk_hRectIndexBuffer, 0UL, VK_INDEX_TYPE_UINT16);
		const VkDeviceSize vk_vertexBufferOffsets[] = {0UL};
		vkCmdBindVertexBuffers(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_ahGameObjectVertexBuffers[u8CurrentFrameInFlightIndex], vk_vertexBufferOffsets);
		vkCmdDrawIndexed(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], u16GameObjectIndex * 6U, 1U, 0U, 0U, 0U);
		if (vkEndCommandBuffer(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex]) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed finishing to record Vulkan secondary command buffer for rendering game objects");
			return false;
		}
		return true;
	}

}
