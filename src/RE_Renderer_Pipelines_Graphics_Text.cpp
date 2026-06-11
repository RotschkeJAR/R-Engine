#include "RE_Renderer_Pipelines_Internal.hpp"

namespace RE {

	VkPipeline vk_hGraphicsPipelineText;
	
	bool create_graphics_pipeline_text() {
		PRINT_DEBUG("Creating temporary Vulkan shader modules for graphics pipeline rendering text");
		Vulkan_Shader vertexShader("shaders/vertex_text.glsl.spv"),
			fragmentShader("shaders/fragment_text.glsl.spv");
		if (!vertexShader.valid() || !fragmentShader.valid()) {
			RE_FATAL_ERROR("");
			return false;
		}
		PRINT_DEBUG("Creating Vulkan graphics pipeline rendering text");
		const VkPipelineShaderStageCreateInfo vk_aShaderStages[] = {
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = vertexShader(),
				.pName = "main",
				.pSpecializationInfo = nullptr
			}, {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = fragmentShader(),
				.pName = "main",
				.pSpecializationInfo = nullptr
			}
		};
		constexpr VkVertexInputBindingDescription vk_aVertexBindings[] = {
			{
				.binding = 0,
				.stride = sizeof(uint32_t),
				.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE
			}
		};
		constexpr VkVertexInputAttributeDescription vk_aVertexAttributes[] = {
			{
				.location = 0,
				.binding = 0,
				.format = VK_FORMAT_R32_UINT,
				.offset = 0
			}
		};
		const VkPipelineVertexInputStateCreateInfo vk_vertexInput = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = sizeof(vk_aVertexBindings) / sizeof(vk_aVertexBindings[0]),
			.pVertexBindingDescriptions = vk_aVertexBindings,
			.vertexAttributeDescriptionCount = sizeof(vk_aVertexAttributes) / sizeof(vk_aVertexAttributes[0]),
			.pVertexAttributeDescriptions = vk_aVertexAttributes
		};
		constexpr VkPipelineInputAssemblyStateCreateInfo vk_inputAssembly = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
			.primitiveRestartEnable = VK_FALSE
		};
		const VkPipelineViewportStateCreateInfo vk_viewportInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.viewportCount = 1,
			.pViewports = nullptr,
			.scissorCount = 1,
			.pScissors = nullptr
		};
		const VkPipelineRasterizationStateCreateInfo vk_rasterizationInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.lineWidth = 1.0f
		};
		const VkPipelineMultisampleStateCreateInfo vk_multisample = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 0.0f,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};
		const VkPipelineDepthStencilStateCreateInfo vk_depthStencil = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE,
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f
		};
		const VkPipelineColorBlendAttachmentState vk_aColorBlendAttachments[] = {
			{
				.blendEnable = VK_FALSE,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			}
		};
		const VkPipelineColorBlendStateCreateInfo vk_colorBlends = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.attachmentCount = sizeof(vk_aColorBlendAttachments) / sizeof(vk_aColorBlendAttachments[0]),
			.pAttachments = vk_aColorBlendAttachments,
			.blendConstants = {1.0f, 1.0f, 1.0f, 1.0f}
		};
		constexpr VkDynamicState vk_aeDynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		const VkPipelineDynamicStateCreateInfo vk_dynamicStates = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.dynamicStateCount = sizeof(vk_aeDynamicStates) / sizeof(vk_aeDynamicStates[0]),
			.pDynamicStates = vk_aeDynamicStates
		};
		const VkGraphicsPipelineCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stageCount = sizeof(vk_aShaderStages) / sizeof(vk_aShaderStages[0]),
			.pStages = vk_aShaderStages,
			.pVertexInputState = &vk_vertexInput,
			.pInputAssemblyState = &vk_inputAssembly,
			.pTessellationState = nullptr,
			.pViewportState = &vk_viewportInfo,
			.pRasterizationState = &vk_rasterizationInfo,
			.pMultisampleState = &vk_multisample,
			.pDepthStencilState = &vk_depthStencil,
			.pColorBlendState = &vk_colorBlends,
			.pDynamicState = &vk_dynamicStates,
			.layout = vk_hTextPipelineLayout,
			.renderPass = VK_NULL_HANDLE,
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1
		};
		if (vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1, &vk_createInfo, nullptr, &vk_hGraphicsPipelineText) == VK_SUCCESS)
			return true;
		else
			RE_FATAL_ERROR("Failed to create Vulkan graphics pipeline rendering text");
		return false;
	}

	void destroy_graphics_pipeline_text() {
		PRINT_DEBUG("Destroying Vulkan graphics pipeline ", vk_hGraphicsPipelineText, " used for rendering text");
		vkDestroyPipeline(vk_hDevice, vk_hGraphicsPipelineText, nullptr);
	}

}
