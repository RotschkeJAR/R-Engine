#include "RE_RenderPipelines_Internal.hpp"
#include "RE_Renderer.hpp"
#include "RE_RenderElement.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

#define RE_VK_SHADER_COUNT 2

	VkShaderModule vk_ahGameObjectShaders[RE_VK_SHADER_COUNT];
	VkPipeline vk_ahGameObjectPipelines[RE_VK_RENDERPIPELINE_COUNT] = {};

	static bool create_game_object_pipelines() {
		const VkPipeline vk_ahPreviousPipelines[RE_VK_RENDERPIPELINE_COUNT] = {vk_ahGameObjectPipelines[0], vk_ahGameObjectPipelines[1]};
		const VkPipelineShaderStageCreateInfo vk_aShaderStages[RE_VK_SHADER_COUNT] = {
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = vk_ahGameObjectShaders[0],
				.pName = "main"
			}, {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = vk_ahGameObjectShaders[1],
				.pName = "main"
			}
		};
		constexpr VkVertexInputBindingDescription vk_aVertexBindings[] = {
			{
				.binding = 0,
				.stride = RE_VK_RECT_BUFFER_VERTEX_STRIDE_SIZE,
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			}, {
				.binding = 1,
				.stride = sizeof(GameObjectInstanceData),
				.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE
			}
		};
		constexpr VkVertexInputAttributeDescription vk_aVertexAttributes[] = {
			{
				.location = 0,
				.binding = 0,
				.format = RE_VK_RECT_BUFFER_VERTEX_FORMAT,
				.offset = 0
			}, {
				.location = 1,
				.binding = 1,
				.format = VK_FORMAT_R32G32B32_SFLOAT,
				.offset = offsetof(GameObjectInstanceData, afPosition)
			}, {
				.location = 2,
				.binding = 1,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(GameObjectInstanceData, afColor)
			}, {
				.location = 3,
				.binding = 1,
				.format = VK_FORMAT_R32_UINT,
				.offset = offsetof(GameObjectInstanceData, u32TexId)
			}, {
				.location = 4,
				.binding = 1,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = offsetof(GameObjectInstanceData, afTexCoords)
			}
		};
		const VkPipelineVertexInputStateCreateInfo vk_vertexInput = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = sizeof(vk_aVertexBindings) / sizeof(vk_aVertexBindings[0]),
			.pVertexBindingDescriptions = vk_aVertexBindings,
			.vertexAttributeDescriptionCount = sizeof(vk_aVertexAttributes) / sizeof(vk_aVertexAttributes[0]),
			.pVertexAttributeDescriptions = vk_aVertexAttributes
		};
		constexpr VkPipelineInputAssemblyStateCreateInfo vk_inputAssembly = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};
		constexpr VkPipelineViewportStateCreateInfo vk_viewport = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.scissorCount = 1
		};
		constexpr VkPipelineRasterizationStateCreateInfo vk_rasterization = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.lineWidth = 1.0f
		};
		const VkPipelineMultisampleStateCreateInfo vk_multisampling = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = vk_eMsaaCount,
			.sampleShadingEnable = vk_bSampleShadingEnabled,
			.minSampleShading = fSampleShadingRate,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};
		constexpr VkPipelineDepthStencilStateCreateInfo vk_depthStencils = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_GREATER,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE
		};
		constexpr VkPipelineColorBlendAttachmentState vk_aColorBlendAttachments[RE_VK_RENDERPIPELINE_COUNT] = {
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
		const VkPipelineColorBlendStateCreateInfo vk_aColorBlend[RE_VK_RENDERPIPELINE_COUNT] = {
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE,
				.attachmentCount = 1,
				.pAttachments = &vk_aColorBlendAttachments[0],
				.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
			}, {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE,
				.attachmentCount = 1,
				.pAttachments = &vk_aColorBlendAttachments[1],
				.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
			}
		};
		constexpr VkDynamicState vk_aeDynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		const VkPipelineDynamicStateCreateInfo vk_dynamicStates = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = sizeof(vk_aeDynamicStates) / sizeof(vk_aeDynamicStates[0]),
			.pDynamicStates = vk_aeDynamicStates
		};
		VkPipelineRenderingCreateInfo vk_dynamicRenderingInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
			.pNext = nullptr,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &vk_paSurfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].format,
			.depthAttachmentFormat = availableDepthStencilFormats[u8IndexToSelectedDepthStencilFormat]
		};
		switch (vk_dynamicRenderingInfo.depthAttachmentFormat) {
			case VK_FORMAT_D16_UNORM:
			case VK_FORMAT_X8_D24_UNORM_PACK32:
			case VK_FORMAT_D32_SFLOAT:
				vk_dynamicRenderingInfo.stencilAttachmentFormat = bStencilsEnabled ? VK_FORMAT_S8_UINT : VK_FORMAT_UNDEFINED;
				break;
			default:
				vk_dynamicRenderingInfo.stencilAttachmentFormat = vk_dynamicRenderingInfo.depthAttachmentFormat;
				break;
		}
		const VkGraphicsPipelineCreateInfo vk_aPipelineCreateInfo[RE_VK_RENDERPIPELINE_COUNT] = {
			{
				.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				.pNext = &vk_dynamicRenderingInfo,
				.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | static_cast<VkPipelineCreateFlags>(vk_ahPreviousPipelines[0] ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0),
				.stageCount = RE_VK_SHADER_COUNT,
				.pStages = vk_aShaderStages,
				.pVertexInputState = &vk_vertexInput,
				.pInputAssemblyState = &vk_inputAssembly,
				.pTessellationState = nullptr,
				.pViewportState = &vk_viewport,
				.pRasterizationState = &vk_rasterization,
				.pMultisampleState = &vk_multisampling,
				.pDepthStencilState = &vk_depthStencils,
				.pColorBlendState = &vk_aColorBlend[0],
				.pDynamicState = &vk_dynamicStates,
				.layout = vk_hWorldPipelineLayout,
				.renderPass = VK_NULL_HANDLE,
				.subpass = 0,
				.basePipelineHandle = vk_ahPreviousPipelines[0],
				.basePipelineIndex = 0
			}, {
				.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				.pNext = &vk_dynamicRenderingInfo,
				.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | static_cast<VkPipelineCreateFlags>(vk_ahPreviousPipelines[1] ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0),
				.stageCount = RE_VK_SHADER_COUNT,
				.pStages = vk_aShaderStages,
				.pVertexInputState = &vk_vertexInput,
				.pInputAssemblyState = &vk_inputAssembly,
				.pTessellationState = nullptr,
				.pViewportState = &vk_viewport,
				.pRasterizationState = &vk_rasterization,
				.pMultisampleState = &vk_multisampling,
				.pDepthStencilState = &vk_depthStencils,
				.pColorBlendState = &vk_aColorBlend[1],
				.pDynamicState = &vk_dynamicStates,
				.layout = vk_hWorldPipelineLayout,
				.renderPass = VK_NULL_HANDLE,
				.subpass = 0,
				.basePipelineHandle = vk_ahPreviousPipelines[1],
				.basePipelineIndex = 0
			}
		};
		if (vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, RE_VK_RENDERPIPELINE_COUNT, vk_aPipelineCreateInfo, nullptr, vk_ahGameObjectPipelines) == VK_SUCCESS) {
			if (vk_ahPreviousPipelines[0] && vk_ahPreviousPipelines[1]) {
				vkDestroyPipeline(vk_hDevice, vk_ahPreviousPipelines[0], nullptr);
				vkDestroyPipeline(vk_hDevice, vk_ahPreviousPipelines[1], nullptr);
			}
			return true;
		}
		return false;
	}
	
	bool create_render_pipeline_game_objects() {
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_vertex.glsl.spv", &vk_ahGameObjectShaders[0]), bool)) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_shader_from_file("shaders/gameobject_fragment.glsl.spv", &vk_ahGameObjectShaders[1]), bool)) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_game_object_pipelines(), bool))
					return true;
				vkDestroyShaderModule(vk_hDevice, vk_ahGameObjectShaders[1], nullptr);
			}
			vkDestroyShaderModule(vk_hDevice, vk_ahGameObjectShaders[0], nullptr);
		}
		return false;
	}

	bool recreate_render_pipeline_game_objects() {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_game_object_pipelines(), bool);
	}

	void destroy_render_pipeline_game_objects() {
		for (VkPipeline &vk_rhGameObjectPipeline : vk_ahGameObjectPipelines) {
			vkDestroyPipeline(vk_hDevice, vk_rhGameObjectPipeline, nullptr);
			vk_rhGameObjectPipeline = VK_NULL_HANDLE;
		}
		for (const VkShaderModule &vk_rhGameObjectShader : vk_ahGameObjectShaders)
			vkDestroyShaderModule(vk_hDevice, vk_rhGameObjectShader, nullptr);
	}

}
