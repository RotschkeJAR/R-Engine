#include "RE_Renderer_Pipelines_Graphics_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Window.hpp"

namespace RE {

#ifdef RE_OS_LINUX

	VkPipeline vk_hGraphicsPipelineWindowFrame;

	bool create_graphics_pipeline_window_frame() {
		const Vulkan_Shader vertexShader("shaders/vertex_window frame.glsl.spv"),
			fragmentShader("shaders/fragment_window frame.glsl.spv");
		if (!vertexShader.valid() || !fragmentShader.valid()) {
			RE_ERROR("Failed to create Vulkan shader modules for the graphics pipeline for window frame rendering");
			return false;
		}
		PRINT_DEBUG("Creating Vulkan graphics pipeline for window frame rendering");
		constexpr uint32_t au32SpecialConstants[] = {
			WINDOW_WAYLAND_SHADOW_SIZE,
			WINDOW_WAYLAND_EDGE_SIZE,
			WINDOW_WAYLAND_BAR_SIZE,
			WINDOW_WAYLAND_BUTTON_WIDTH,
			WINDOW_WAYLAND_BUTTON_COUNT,
			WINDOW_WAYLAND_BUTTON_TEXTURE_SIZE
		};
		constexpr VkSpecializationMapEntry vk_aSpecializationConstants[] = {
			{
				.constantID = 0,
				.offset = 0,
				.size = sizeof(uint32_t)
			}, {
				.constantID = 1,
				.offset = sizeof(uint32_t),
				.size = sizeof(uint32_t)
			}, {
				.constantID = 2,
				.offset = sizeof(uint32_t) * 2,
				.size = sizeof(uint32_t)
			}, {
				.constantID = 3,
				.offset = sizeof(uint32_t) * 3,
				.size = sizeof(uint32_t)
			}, {
				.constantID = 4,
				.offset = sizeof(uint32_t) * 4,
				.size = sizeof(uint32_t)
			}, {
				.constantID = 5,
				.offset = sizeof(uint32_t) * 5,
				.size = sizeof(uint32_t)
			}
		};
		const VkSpecializationInfo vk_specializationInfo = {
			.mapEntryCount = sizeof(vk_aSpecializationConstants) / sizeof(vk_aSpecializationConstants[0]),
			.pMapEntries = vk_aSpecializationConstants,
			.dataSize = sizeof(au32SpecialConstants),
			.pData = static_cast<const void*>(au32SpecialConstants)
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
				.pSpecializationInfo = nullptr
			}
		};
		const VkPipelineVertexInputStateCreateInfo vk_vertexInput = {
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
		const VkPipelineMultisampleStateCreateInfo vk_multisampling = {
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
		const VkPipelineRenderingCreateInfo vk_dynamicRenderingInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
			.pNext = nullptr,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &vk_eSwapchainImageFormat,
			.depthAttachmentFormat = VK_FORMAT_UNDEFINED,
			.stencilAttachmentFormat = VK_FORMAT_UNDEFINED
		};
		const VkGraphicsPipelineCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = &vk_dynamicRenderingInfo,
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
			.renderPass = VK_NULL_HANDLE,
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1
		};
		if (vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1, &vk_createInfo, nullptr, &vk_hGraphicsPipelineWindowFrame) == VK_SUCCESS)
			return true;
		else
			RE_ERROR("Failed to create Vulkan graphics pipeline used for window frame rendering");
		return false;
	}

	void destroy_graphics_pipeline_window_frame() {
		PRINT_DEBUG("Destroying Vulkan graphics pipeline ", vk_hGraphicsPipelineWindowFrame, " used for window frame rendering");
		vkDestroyPipeline(vk_hDevice, vk_hGraphicsPipelineWindowFrame, nullptr);
	}

#else

	bool create_graphics_pipeline_window_frame() {
		return true;
	}

	void destroy_graphics_pipeline_window_frame() {}

#endif

}
