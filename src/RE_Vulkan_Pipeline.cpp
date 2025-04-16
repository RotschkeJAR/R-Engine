#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_GraphicsPipeline::Vulkan_GraphicsPipeline(const Vulkan_Shader *pVertexShader, const Vulkan_Shader *pFragmentShader, const uint32_t u32VertexInputBindingDescritpionCount, const VkVertexInputBindingDescription *vk_pVertexInputBindingDescritpions, const uint32_t u32VertexInputAttributeDescritpionCount, const VkVertexInputAttributeDescription *vk_pVertexInputAttributeDescritpions, const Vulkan_PipelineLayout *pPipelineLayout, const Vulkan_RenderPass *pRenderPass) : vk_hGraphicsPipeline(VK_NULL_HANDLE) {
		VkPipelineShaderStageCreateInfo vk_pipelineVertexShaderStateCreateInfo = {};
		vk_pipelineVertexShaderStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vk_pipelineVertexShaderStateCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vk_pipelineVertexShaderStateCreateInfo.module = *pVertexShader;
		vk_pipelineVertexShaderStateCreateInfo.pName = "main";
		VkPipelineShaderStageCreateInfo vk_pipelineFragmentShaderStateCreateInfo = {};
		vk_pipelineFragmentShaderStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vk_pipelineFragmentShaderStateCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		vk_pipelineFragmentShaderStateCreateInfo.module = *pFragmentShader;
		vk_pipelineFragmentShaderStateCreateInfo.pName = "main";
		VkPipelineShaderStageCreateInfo vk_pipelineShaderStageCreateInfos[2] = {vk_pipelineVertexShaderStateCreateInfo, vk_pipelineFragmentShaderStateCreateInfo};

		VkDynamicState vk_dynamicStates[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo vk_pipelineDynamicStateCreateInfo = {};
		vk_pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		vk_pipelineDynamicStateCreateInfo.dynamicStateCount = sizeof(vk_dynamicStates) / sizeof(VkDynamicState);
		vk_pipelineDynamicStateCreateInfo.pDynamicStates = vk_dynamicStates;

		VkPipelineVertexInputStateCreateInfo vk_pipelineVertexInputStateCreateInfo = {};
		vk_pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vk_pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = u32VertexInputBindingDescritpionCount;
		vk_pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vk_pVertexInputBindingDescritpions;
		vk_pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = u32VertexInputAttributeDescritpionCount;
		vk_pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vk_pVertexInputAttributeDescritpions;

		VkPipelineInputAssemblyStateCreateInfo vk_pipelineInputAssemblyStateCreateInfo = {};
		vk_pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		vk_pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		vk_pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport vk_viewport = {};
		vk_viewport.x = 0.0f;
		vk_viewport.y = 0.0f;
		vk_viewport.width = vk_swapchainResolution.width;
		vk_viewport.height = vk_swapchainResolution.height;
		vk_viewport.minDepth = 0.0f;
		vk_viewport.maxDepth = 1.0f;

		VkRect2D vk_scissor = {};
		vk_scissor.offset.x = 0.0f;
		vk_scissor.offset.y = 0.0f;
		vk_scissor.extent = vk_swapchainResolution;

		VkPipelineViewportStateCreateInfo vk_pipelineViewportStateCreateInfo = {};
		vk_pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vk_pipelineViewportStateCreateInfo.viewportCount = 1U;
		vk_pipelineViewportStateCreateInfo.pViewports = &vk_viewport;
		vk_pipelineViewportStateCreateInfo.scissorCount = 1U;
		vk_pipelineViewportStateCreateInfo.pScissors = &vk_scissor;

		VkPipelineRasterizationStateCreateInfo vk_pipelineRasterizationStateCreateInfo = {};
		vk_pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		vk_pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		vk_pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		vk_pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		vk_pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
		vk_pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		vk_pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		vk_pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		vk_pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		vk_pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		vk_pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo vk_pipelineMultisampleStateCreateInfo = {};
		vk_pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		vk_pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		vk_pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		vk_pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f;
		vk_pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
		vk_pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		vk_pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState vk_pipelineColorBlendAttachmentState = {};
		vk_pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		vk_pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
		vk_pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		vk_pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		vk_pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		vk_pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		vk_pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		vk_pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo vk_pipelineColorBlendStateCreateInfo = {};
		vk_pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		vk_pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		vk_pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		vk_pipelineColorBlendStateCreateInfo.attachmentCount = 1U;
		vk_pipelineColorBlendStateCreateInfo.pAttachments = &vk_pipelineColorBlendAttachmentState;
		vk_pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		vk_pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		vk_pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		vk_pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		VkGraphicsPipelineCreateInfo vk_graphicsPipelineCreateInfo = {};
		vk_graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		vk_graphicsPipelineCreateInfo.stageCount = sizeof(vk_pipelineShaderStageCreateInfos) / sizeof(VkPipelineShaderStageCreateInfo);
		vk_graphicsPipelineCreateInfo.pStages = vk_pipelineShaderStageCreateInfos;
		vk_graphicsPipelineCreateInfo.pVertexInputState = &vk_pipelineVertexInputStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pInputAssemblyState = &vk_pipelineInputAssemblyStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pViewportState = &vk_pipelineViewportStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pRasterizationState = &vk_pipelineRasterizationStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pMultisampleState = &vk_pipelineMultisampleStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
		vk_graphicsPipelineCreateInfo.pColorBlendState = &vk_pipelineColorBlendStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pDynamicState = &vk_pipelineDynamicStateCreateInfo;
		vk_graphicsPipelineCreateInfo.layout = *pPipelineLayout;
		vk_graphicsPipelineCreateInfo.renderPass = *pRenderPass;
		vk_graphicsPipelineCreateInfo.subpass = 0U;
		vk_graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		vk_graphicsPipelineCreateInfo.basePipelineIndex = -1;
		if (!CHECK_VK_RESULT(vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1, &vk_graphicsPipelineCreateInfo, nullptr, &vk_hGraphicsPipeline))) {
			RE_ERROR("Failed to create Vulkan graphics pipeline");
			vk_hGraphicsPipeline = VK_NULL_HANDLE;
		}
	}

	Vulkan_GraphicsPipeline::~Vulkan_GraphicsPipeline() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyPipeline(vk_hDevice, vk_hGraphicsPipeline, nullptr));
	}

	VkPipeline Vulkan_GraphicsPipeline::get_graphics_pipeline() const {
		return vk_hGraphicsPipeline;
	}
	
	bool Vulkan_GraphicsPipeline::is_valid() const {
		return vk_hGraphicsPipeline != VK_NULL_HANDLE;
	}

	Vulkan_GraphicsPipeline::operator VkPipeline() const {
		return this->vk_hGraphicsPipeline;
	}

}
