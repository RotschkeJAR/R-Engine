#include "RE_Renderer_Pipelines_Graphics_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Settings.hpp"

namespace RE {

	VkPipeline vk_ahGraphicsPipelines[GRAPHICS_PIPELINE_COUNT] = {};
	
	bool create_graphics_pipelines() {
		PRINT_DEBUG("Creating temporary Vulkan shader modules for vertex- and fragment-stage");
		Vulkan_Shader generalVertexShader("shaders/Vertex_General.glsl.spv"),
			generalFragmentShader("shaders/Fragment_General.glsl.spv");
		if (generalVertexShader.valid() && generalFragmentShader.valid()) {
			PRINT_DEBUG("Creating Vulkan graphics pipelines");
			VkPipeline vk_ahPreviousPipelines[GRAPHICS_PIPELINE_COUNT];
			std::copy(std::begin(vk_ahGraphicsPipelines), std::end(vk_ahGraphicsPipelines), std::begin(vk_ahPreviousPipelines));
			const uint32_t au32Constants[] = {
				DONT_USE_TEXTURE
			};
			const VkSpecializationMapEntry vk_aMapEntries[] = {
				{
					.constantID = 0,
					.offset = 0,
					.size = sizeof(uint32_t)
				}
			};
			const VkSpecializationInfo vk_specializationInfo = {
				.mapEntryCount = sizeof(vk_aMapEntries) / sizeof(vk_aMapEntries[0]),
				.pMapEntries = vk_aMapEntries,
				.dataSize = sizeof(au32Constants),
				.pData = au32Constants
			};
			const VkPipelineShaderStageCreateInfo vk_aShaderStages[] = {
				{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.stage = VK_SHADER_STAGE_VERTEX_BIT,
					.module = generalVertexShader(),
					.pName = "main",
					.pSpecializationInfo = &vk_specializationInfo
				}, {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
					.module = generalFragmentShader(),
					.pName = "main",
					.pSpecializationInfo = &vk_specializationInfo
				}
			};
			constexpr VkVertexInputBindingDescription vk_aVertexBindings[] = {
				{
					.binding = 0,
					.stride = sizeof(VertexData),
					.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
				}, {
					.binding = 1,
					.stride = sizeof(DepthShaderData),
					.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE
				}
			};
			constexpr VkVertexInputAttributeDescription vk_aVertexAttributes[] = {
				{
					.location = 0,
					.binding = 0,
					.format = VK_FORMAT_R32G32B32A32_SFLOAT,
					.offset = offsetof(VertexData, coords)
				}, {
					.location = 1,
					.binding = 0,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = offsetof(VertexData, texCoords)
				}, {
					.location = 2,
					.binding = 1,
					.format = VK_FORMAT_R32_UINT,
					.offset = offsetof(DepthShaderData, objectIndex)
				}
			};
			const VkPipelineVertexInputStateCreateInfo vk_vertexInput = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.vertexBindingDescriptionCount = sizeof(vk_aVertexBindings) / sizeof(vk_aVertexBindings[0]),
				.pVertexBindingDescriptions = vk_aVertexBindings,
				.vertexAttributeDescriptionCount = sizeof(vk_aVertexAttributes) / sizeof(vk_aVertexAttributes[0]),
				.pVertexAttributeDescriptions = vk_aVertexAttributes
			};
			VkPipelineInputAssemblyStateCreateInfo vk_aInputAssemblies[GRAPHICS_PIPELINE_COUNT];
			constexpr VkPipelineViewportStateCreateInfo vk_viewport = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				.viewportCount = 1,
				.scissorCount = 1
			};
			VkPipelineRasterizationStateCreateInfo vk_aRasterizations[GRAPHICS_PIPELINE_COUNT];
			const VkPipelineMultisampleStateCreateInfo vk_multisampling = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				.rasterizationSamples = vk_eMsaaMode,
				.sampleShadingEnable = IS_SAMPLE_SHADING_ENABLED() ? VK_TRUE : VK_FALSE,
				.minSampleShading = fSampleShadingRate,
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
			VkPipelineColorBlendAttachmentState vk_aColorBlendAttachments[GRAPHICS_PIPELINE_COUNT];
			VkPipelineColorBlendStateCreateInfo vk_aColorBlends[GRAPHICS_PIPELINE_COUNT];
			constexpr VkDynamicState vk_aeDynamicStates[] = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};
			const VkPipelineDynamicStateCreateInfo vk_dynamicStates = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				.dynamicStateCount = sizeof(vk_aeDynamicStates) / sizeof(vk_aeDynamicStates[0]),
				.pDynamicStates = vk_aeDynamicStates
			};
			VkGraphicsPipelineCreateInfo vk_aCreateInfos[GRAPHICS_PIPELINE_COUNT] = {
				{
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.pNext = nullptr,
					.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | static_cast<VkPipelineCreateFlags>(vk_ahPreviousPipelines[GRAPHICS_PIPELINE_2D] ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0),
					.stageCount = sizeof(vk_aShaderStages) / sizeof(vk_aShaderStages[0]),
					.pStages = vk_aShaderStages,
					.pVertexInputState = &vk_vertexInput,
					.pInputAssemblyState = &vk_aInputAssemblies[GRAPHICS_PIPELINE_2D],
					.pTessellationState = nullptr,
					.pViewportState = &vk_viewport,
					.pRasterizationState = &vk_aRasterizations[GRAPHICS_PIPELINE_2D],
					.pMultisampleState = &vk_multisampling,
					.pDepthStencilState = &vk_depthStencils,
					.pColorBlendState = &vk_aColorBlends[GRAPHICS_PIPELINE_2D],
					.pDynamicState = &vk_dynamicStates,
					.layout = vk_hGraphicsPipelineLayout,
					.basePipelineHandle = vk_ahPreviousPipelines[GRAPHICS_PIPELINE_2D],
					.basePipelineIndex = -1
				}, {
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.pNext = nullptr,
					.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | static_cast<VkPipelineCreateFlags>(vk_ahPreviousPipelines[GRAPHICS_PIPELINE_2D_OPAQUE] ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0),
					.stageCount = sizeof(vk_aShaderStages) / sizeof(vk_aShaderStages[0]),
					.pStages = vk_aShaderStages,
					.pVertexInputState = &vk_vertexInput,
					.pInputAssemblyState = &vk_aInputAssemblies[GRAPHICS_PIPELINE_2D_OPAQUE],
					.pTessellationState = nullptr,
					.pViewportState = &vk_viewport,
					.pRasterizationState = &vk_aRasterizations[GRAPHICS_PIPELINE_2D_OPAQUE],
					.pMultisampleState = &vk_multisampling,
					.pDepthStencilState = &vk_depthStencils,
					.pColorBlendState = &vk_aColorBlends[GRAPHICS_PIPELINE_2D_OPAQUE],
					.pDynamicState = &vk_dynamicStates,
					.layout = vk_hGraphicsPipelineLayout,
					.basePipelineHandle = vk_ahPreviousPipelines[GRAPHICS_PIPELINE_2D_OPAQUE],
					.basePipelineIndex = -1
				}, {
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.pNext = nullptr,
					.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | static_cast<VkPipelineCreateFlags>(vk_ahPreviousPipelines[GRAPHICS_PIPELINE_3D] ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0),
					.stageCount = sizeof(vk_aShaderStages) / sizeof(vk_aShaderStages[0]),
					.pStages = vk_aShaderStages,
					.pVertexInputState = &vk_vertexInput,
					.pInputAssemblyState = &vk_aInputAssemblies[GRAPHICS_PIPELINE_3D],
					.pTessellationState = nullptr,
					.pViewportState = &vk_viewport,
					.pRasterizationState = &vk_aRasterizations[GRAPHICS_PIPELINE_3D],
					.pMultisampleState = &vk_multisampling,
					.pDepthStencilState = &vk_depthStencils,
					.pColorBlendState = &vk_aColorBlends[GRAPHICS_PIPELINE_3D],
					.pDynamicState = &vk_dynamicStates,
					.layout = vk_hGraphicsPipelineLayout,
					.basePipelineHandle = vk_ahPreviousPipelines[GRAPHICS_PIPELINE_3D],
					.basePipelineIndex = -1
				}, {
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.pNext = nullptr,
					.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | static_cast<VkPipelineCreateFlags>(vk_ahPreviousPipelines[GRAPHICS_PIPELINE_3D_OPAQUE] ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0),
					.stageCount = sizeof(vk_aShaderStages) / sizeof(vk_aShaderStages[0]),
					.pStages = vk_aShaderStages,
					.pVertexInputState = &vk_vertexInput,
					.pInputAssemblyState = &vk_aInputAssemblies[GRAPHICS_PIPELINE_3D_OPAQUE],
					.pTessellationState = nullptr,
					.pViewportState = &vk_viewport,
					.pRasterizationState = &vk_aRasterizations[GRAPHICS_PIPELINE_3D_OPAQUE],
					.pMultisampleState = &vk_multisampling,
					.pDepthStencilState = &vk_depthStencils,
					.pColorBlendState = &vk_aColorBlends[GRAPHICS_PIPELINE_3D_OPAQUE],
					.pDynamicState = &vk_dynamicStates,
					.layout = vk_hGraphicsPipelineLayout,
					.basePipelineHandle = vk_ahPreviousPipelines[GRAPHICS_PIPELINE_3D_OPAQUE],
					.basePipelineIndex = -1
				}
			};
			VkPipelineRenderingCreateInfo vk_dynamicRenderPassInfo;
			setup_graphics_pipeline_2D(
					vk_aInputAssemblies[GRAPHICS_PIPELINE_2D],
					vk_aRasterizations[GRAPHICS_PIPELINE_2D],
					vk_aColorBlendAttachments[GRAPHICS_PIPELINE_2D],
					vk_aColorBlends[GRAPHICS_PIPELINE_2D]);
			setup_graphics_pipeline_2D_opaque_only(
					vk_aInputAssemblies[GRAPHICS_PIPELINE_2D_OPAQUE],
					vk_aRasterizations[GRAPHICS_PIPELINE_2D_OPAQUE],
					vk_aColorBlendAttachments[GRAPHICS_PIPELINE_2D_OPAQUE],
					vk_aColorBlends[GRAPHICS_PIPELINE_2D_OPAQUE]);
			setup_graphics_pipeline_3D(
					vk_aInputAssemblies[GRAPHICS_PIPELINE_3D],
					vk_aRasterizations[GRAPHICS_PIPELINE_3D],
					vk_aColorBlendAttachments[GRAPHICS_PIPELINE_3D],
					vk_aColorBlends[GRAPHICS_PIPELINE_3D]);
			setup_graphics_pipeline_3D_opaque_only(
					vk_aInputAssemblies[GRAPHICS_PIPELINE_3D_OPAQUE],
					vk_aRasterizations[GRAPHICS_PIPELINE_3D_OPAQUE],
					vk_aColorBlendAttachments[GRAPHICS_PIPELINE_3D_OPAQUE],
					vk_aColorBlends[GRAPHICS_PIPELINE_3D_OPAQUE]);
			setup_graphics_pipelines_render_pass(vk_aCreateInfos, vk_dynamicRenderPassInfo);
			const VkResult vk_eResult = vkCreateGraphicsPipelines(
					vk_hDevice,
					VK_NULL_HANDLE,
					GRAPHICS_PIPELINE_COUNT,
					vk_aCreateInfos,
					nullptr,
					vk_ahGraphicsPipelines);
			if (vk_eResult == VK_SUCCESS) {
				PRINT_DEBUG("Destroying previous Vulkan graphics pipelines");
				for (VkPipeline vk_hPreviousPipeline : vk_ahPreviousPipelines)
					vkDestroyPipeline(vk_hDevice, vk_hPreviousPipeline, nullptr);
				return true;
			} else
				RE_FATAL_ERROR("Failed to create Vulkan graphics pipelines (code ", vk_eResult, ")");
		} else
			RE_FATAL_ERROR("Failed to create temporary Vulkan shader modules for vertex- and fragment-stage");
		return false;
	}

	void destroy_graphics_pipelines() {
		PRINT_DEBUG("Destroying Vulkan graphics pipelines");
		for (VkPipeline vk_hGraphicsPipeline : vk_ahGraphicsPipelines)
			vkDestroyPipeline(vk_hDevice, vk_hGraphicsPipeline, nullptr);
		std::fill(std::begin(vk_ahGraphicsPipelines), std::end(vk_ahGraphicsPipelines), VK_NULL_HANDLE);
	}

}
