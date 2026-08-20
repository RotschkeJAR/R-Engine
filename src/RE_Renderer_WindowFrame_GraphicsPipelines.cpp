#include "RE_Renderer_WindowFrame_GraphicsPipelines.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_WINDOW_FRAME

	VkPipelineLayout vk_hWindowFramePipelineLayout;
	VkPipeline vk_hWindowFrameGraphicsPipeline;

	bool create_window_frame_graphics_pipelines() {
		PRINT_DEBUG("Creating Vulkan pipeline layout for window frame rendering");
		const VkDescriptorSetLayout vk_ahDescSetLayouts[] = {
			vk_hWindowFrameDescSetLayout,
			vk_hCharacterDescSetLayout
		};
		const VkPushConstantRange vk_aPushConstants[] = {
			{
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				.offset = 0,
				.size = sizeof(WindowShaderData)
			}
		};
		const VkPipelineLayoutCreateInfo vk_windowFramePipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = sizeof(vk_ahDescSetLayouts) / sizeof(vk_ahDescSetLayouts[0]),
			.pSetLayouts = vk_ahDescSetLayouts,
			.pushConstantRangeCount = sizeof(vk_aPushConstants) / sizeof(vk_aPushConstants[0]),
			.pPushConstantRanges = vk_aPushConstants
		};
		if (vkCreatePipelineLayout(vk_hDevice, &vk_windowFramePipelineLayoutCreateInfo, nullptr, &vk_hWindowFramePipelineLayout) == VK_SUCCESS) {
			const Vulkan_Shader vertexShader("shaders/Vertex_WindowFrame.glsl.spv"),
				fragmentShader("shaders/Fragment_WindowFrame.glsl.spv");
			if (vertexShader.valid() && fragmentShader.valid()) {
				PRINT_DEBUG("Creating Vulkan graphics pipeline for window frame rendering");
				constexpr int32_t au32SpecialConstants[] = {
					WINDOW_SHADOW_SIZE,
					WINDOW_EDGE_SIZE,
					WINDOW_BAR_SIZE,
					WINDOW_BUTTON_WIDTH,
					WINDOW_BUTTON_TEXTURE_COUNT,
					WINDOW_BUTTON_TEXTURE_SIZE,
					CHAR_TEXTURE_SIZE,
					WINDOW_CHAR_GAP_SIZE,
					std::max(WINDOW_BAR_SIZE / CHAR_TEXTURE_SIZE - 1, 1),
					WINDOW_AREA_BUTTON_CLOSE,
					MAX_CHARS_IN_WINDOW_TITLE,
					~0, /* DONT_RENDER_BUTTON */
					~0  /* DONT_RENDER_CHARACTER */
				};
				constexpr VkSpecializationMapEntry vk_aSpecializationConstants[] = {
					{
						.constantID = 0,
						.offset = 0,
						.size = sizeof(int32_t)
					}, {
						.constantID = 1,
						.offset = sizeof(int32_t),
						.size = sizeof(int32_t)
					}, {
						.constantID = 2,
						.offset = sizeof(int32_t) * 2,
						.size = sizeof(int32_t)
					}, {
						.constantID = 3,
						.offset = sizeof(int32_t) * 3,
						.size = sizeof(int32_t)
					}, {
						.constantID = 4,
						.offset = sizeof(int32_t) * 4,
						.size = sizeof(int32_t)
					}, {
						.constantID = 5,
						.offset = sizeof(int32_t) * 5,
						.size = sizeof(int32_t)
					}, {
						.constantID = 6,
						.offset = sizeof(int32_t) * 6,
						.size = sizeof(int32_t)
					}, {
						.constantID = 7,
						.offset = sizeof(int32_t) * 7,
						.size = sizeof(int32_t)
					}, {
						.constantID = 8,
						.offset = sizeof(int32_t) * 8,
						.size = sizeof(int32_t)
					}, {
						.constantID = 9,
						.offset = sizeof(int32_t) * 9,
						.size = sizeof(int32_t)
					}, {
						.constantID = 10,
						.offset = sizeof(int32_t) * 10,
						.size = sizeof(int32_t)
					}, {
						.constantID = 11,
						.offset = sizeof(int32_t) * 11,
						.size = sizeof(int32_t)
					}, {
						.constantID = 12,
						.offset = sizeof(int32_t) * 12,
						.size = sizeof(int32_t)
					}
				};
				const VkSpecializationInfo vk_specializationInfo = {
					.mapEntryCount = sizeof(vk_aSpecializationConstants) / sizeof(vk_aSpecializationConstants[0]),
					.pMapEntries = vk_aSpecializationConstants,
					.dataSize = sizeof(au32SpecialConstants),
					.pData = reinterpret_cast<const void*>(au32SpecialConstants)
				};
				const VkPipelineShaderStageCreateInfo vk_aShaderStages[] = {
					{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_VERTEX_BIT,
						.module = vertexShader(),
						.pName = "main",
						.pSpecializationInfo = &vk_specializationInfo
					}, {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
						.module = fragmentShader(),
						.pName = "main",
						.pSpecializationInfo = &vk_specializationInfo
					}
				};
				constexpr VkPipelineVertexInputStateCreateInfo vk_vertexInput = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
					.vertexBindingDescriptionCount = 0,
					.pVertexBindingDescriptions = nullptr,
					.vertexAttributeDescriptionCount = 0,
					.pVertexAttributeDescriptions = nullptr
				};
				constexpr VkPipelineInputAssemblyStateCreateInfo vk_inputAssembly = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
					.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
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
					.depthBiasEnable = VK_FALSE,
					.lineWidth = 1.0f
				};
				constexpr VkPipelineMultisampleStateCreateInfo vk_multisampling = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
					.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
					.sampleShadingEnable = VK_FALSE,
					.pSampleMask = nullptr,
					.alphaToCoverageEnable = VK_FALSE,
					.alphaToOneEnable = VK_FALSE
				};
				constexpr VkPipelineDepthStencilStateCreateInfo vk_depthStencils = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
					.depthTestEnable = VK_FALSE,
					.depthWriteEnable = VK_FALSE,
					.depthBoundsTestEnable = VK_FALSE,
					.stencilTestEnable = VK_FALSE
				};
				constexpr VkPipelineColorBlendAttachmentState vk_colorBlendAttachment = {
					.blendEnable = VK_FALSE,
					.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
				};
				const VkPipelineColorBlendStateCreateInfo vk_colorBlend = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
					.logicOpEnable = VK_FALSE,
					.attachmentCount = 1,
					.pAttachments = &vk_colorBlendAttachment,
					.blendConstants = {1.0f, 1.0f, 1.0f, 1.0f}
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
				VkGraphicsPipelineCreateInfo vk_createInfo = {
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
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
					.layout = vk_hWindowFramePipelineLayout,
					.renderPass = vk_hSwapchainRenderPass,
					.subpass = u32SwapchainSubpassWindowFrame,
					.basePipelineHandle = VK_NULL_HANDLE,
					.basePipelineIndex = -1
				};
				VkPipelineRenderingCreateInfo vk_renderingCreateInfo;
				setup_swapchain_pipelines_render_pass(vk_createInfo, vk_renderingCreateInfo);
				if (vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1, &vk_createInfo, nullptr, &vk_hWindowFrameGraphicsPipeline) == VK_SUCCESS) {
					return true;
				} else
					RE_FATAL_ERROR("Failed to create Vulkan graphics pipeline used for window frame rendering");
			} else
				RE_FATAL_ERROR("Failed to create Vulkan shader modules for the graphics pipeline for window frame rendering");
		} else
			RE_FATAL_ERROR("Failed to create Vulkan pipeline layout dedicated for compute pipelines processing game objects");
		return false;
	}

	void destroy_window_frame_graphics_pipelines() {
		PRINT_DEBUG("Destroying Vulkan graphics pipeline ", vk_hWindowFrameGraphicsPipeline, " used for window frame rendering");
		vkDestroyPipeline(vk_hDevice, vk_hWindowFrameGraphicsPipeline, nullptr);
		PRINT_DEBUG("Destroying Vulkan pipeline layout ", vk_hWindowFramePipelineLayout, " used for window frame rendering");
		vkDestroyPipelineLayout(vk_hDevice, vk_hWindowFramePipelineLayout, nullptr);
	}

#endif

}
