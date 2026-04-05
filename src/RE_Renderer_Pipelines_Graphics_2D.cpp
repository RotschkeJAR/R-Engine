#include "RE_Renderer_Pipelines_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"

namespace RE {

	VkPipeline vk_hGraphicsPipeline2D = VK_NULL_HANDLE;

	bool create_graphics_pipeline_2D(const VkShaderModule vk_hVertexShader, const VkShaderModule vk_hFragmentShader) {
		PRINT_DEBUG("Creating Vulkan graphics pipeline for 2D rendering");
		const VkPipeline vk_hPreviousPipeline = vk_hGraphicsPipeline2D;
		const VkPipelineShaderStageCreateInfo vk_aShaderStages[] = {
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = vk_hVertexShader,
				.pName = "main"
			}, {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = vk_hFragmentShader,
				.pName = "main"
			}
		};
		constexpr VkVertexInputBindingDescription vk_aVertexBindings[] = {
			{
				.binding = 0,
				.stride = sizeof(float) * 5,
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			}
		};
		constexpr VkVertexInputAttributeDescription vk_aVertexAttributes[] = {
			{
				.location = 0,
				.binding = 0,
				.format = VK_FORMAT_R32G32B32_SFLOAT,
				.offset = 0
			}, {
				.location = 1,
				.binding = 0,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = sizeof(float) * 3
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
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
			.primitiveRestartEnable = VK_TRUE
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
			.depthBiasEnable = VK_FALSE,
			.lineWidth = 1.0f
		};
		const VkPipelineMultisampleStateCreateInfo vk_multisampling = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = vk_eMsaaCount,
			.sampleShadingEnable = is_sample_shading_enabled() ? VK_TRUE : VK_FALSE,
			.minSampleShading = f32SampleShadingRate,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};
		constexpr VkPipelineDepthStencilStateCreateInfo vk_depthStencils = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE
		};
		constexpr VkPipelineColorBlendAttachmentState vk_colorBlendAttachment = {
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};
		const VkPipelineColorBlendStateCreateInfo vk_colorBlend = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.attachmentCount = 1,
			.pAttachments = &vk_colorBlendAttachment,
			.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
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
		const VkPipelineRenderingCreateInfo vk_dynamicRenderingInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
			.pNext = nullptr,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &vk_eSwapchainImageFormat,
			.depthAttachmentFormat = vk_eSelectedDepthFormat,
			.stencilAttachmentFormat = vk_eStelectedStencilFormat
		};
		const VkGraphicsPipelineCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = &vk_dynamicRenderingInfo,
			.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | static_cast<VkPipelineCreateFlags>(vk_hPreviousPipeline ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0),
			.stageCount = sizeof(vk_aShaderStages) / sizeof(vk_aShaderStages[0]),
			.pStages = vk_aShaderStages,
			.pVertexInputState = &vk_vertexInput,
			.pInputAssemblyState = &vk_inputAssembly,
			.pTessellationState = nullptr,
			.pViewportState = &vk_viewport,
			.pRasterizationState = &vk_rasterization,
			.pMultisampleState = &vk_multisampling,
			.pDepthStencilState = &vk_depthStencils,
			.pColorBlendState = &vk_colorBlend,
			.pDynamicState = &vk_dynamicStates,
			.layout = vk_hGraphicsPipelineLayout,
			.renderPass = VK_NULL_HANDLE,
			.subpass = 0,
			.basePipelineHandle = vk_hPreviousPipeline,
			.basePipelineIndex = -1
		};
		if (vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1, &vk_createInfo, nullptr, &vk_hGraphicsPipeline2D) == VK_SUCCESS) {
			PRINT_DEBUG("Destroying old Vulkan graphics pipeline ", vk_hPreviousPipeline, " used for 2D rendering");
			vkDestroyPipeline(vk_hDevice, vk_hPreviousPipeline, nullptr);
			return true;
		} else
			RE_ERROR("Failed to create Vulkan graphics pipeline used for 2D rendering");
		return false;
	}

	void destroy_graphics_pipeline_2D() {
		PRINT_DEBUG("Destroying Vulkan pipeline ", vk_hGraphicsPipeline2D, " used for 2D rendering");
		vkDestroyPipeline(vk_hDevice, vk_hGraphicsPipeline2D, nullptr);
		vk_hGraphicsPipeline2D = VK_NULL_HANDLE;
	}

}
