#include "RE_Renderer_Internal.hpp"
#include "RE_RenderElement.hpp"
#include "RE_Vulkan_Wrappers.hpp"

namespace RE {

#define RE_VK_SHADER_COUNT 2

	VkPipelineLayout vk_hGraphicsPipelineLayout;
	static VkShaderModule vk_ahShaders[RE_VK_SHADER_COUNT];
	VkPipeline vk_hGraphicsPipeline = VK_NULL_HANDLE;

	static bool create_render_pipeline_internal() {
		PRINT_DEBUG("Copying old Vulkan graphics pipelines used for game object rendering");
		const VkPipeline vk_hPreviousPipeline = vk_hGraphicsPipeline;
		const VkPipelineShaderStageCreateInfo vk_aShaderStages[RE_VK_SHADER_COUNT] = {
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = vk_ahShaders[0],
				.pName = "main"
			}, {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = vk_ahShaders[1],
				.pName = "main"
			}
		};
		constexpr VkVertexInputBindingDescription vk_aVertexBindings[] = {
			{
				.binding = 0,
				.stride = sizeof(float) * 5,
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
				.format = VK_FORMAT_R32G32B32_SFLOAT,
				.offset = 0
			}, {
				.location = 1,
				.binding = 0,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = sizeof(float) * 3
			}, {
				.location = 2,
				.binding = 1,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(GameObjectInstanceData, a16fModelMatrix)
			}, {
				.location = 3,
				.binding = 1,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(GameObjectInstanceData, a16fModelMatrix) + sizeof(float) * 4
			}, {
				.location = 4,
				.binding = 1,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(GameObjectInstanceData, a16fModelMatrix) + sizeof(float) * 4 * 2
			}, {
				.location = 5,
				.binding = 1,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(GameObjectInstanceData, a16fModelMatrix) + sizeof(float) * 4 * 3
			}, {
				.location = 6,
				.binding = 1,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(GameObjectInstanceData, a4fColor)
			}, {
				.location = 7,
				.binding = 1,
				.format = VK_FORMAT_R32_UINT,
				.offset = offsetof(GameObjectInstanceData, u32TextureId)
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
			.sampleShadingEnable = is_sample_shading_enabled() ? VK_TRUE : VK_FALSE,
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
		VkPipelineRenderingCreateInfo vk_dynamicRenderingInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
			.pNext = nullptr,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &vk_eSwapchainImageFormat,
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
		const VkGraphicsPipelineCreateInfo vk_pipelineCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = &vk_dynamicRenderingInfo,
			.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | static_cast<VkPipelineCreateFlags>(vk_hPreviousPipeline ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0),
			.stageCount = RE_VK_SHADER_COUNT,
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
			.basePipelineIndex = 0
		};
		PRINT_DEBUG("Creating Vulkan graphics pipeline for game object rendering");
		if (vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1, &vk_pipelineCreateInfo, nullptr, &vk_hGraphicsPipeline) == VK_SUCCESS) {
			PRINT_DEBUG("Destroying old Vulkan graphics pipeline ", vk_hPreviousPipeline);
			vkDestroyPipeline(vk_hDevice, vk_hPreviousPipeline, nullptr);
			return true;
		}
		return false;
	}

	bool does_gpu_support_vertex_buffers(const VkPhysicalDevice vk_hPhysicalDevice, std::queue<std::string> &rMissingFeatures) {
		VkFormatProperties2 vk_vertexBufferFormatFeatures;
		vk_vertexBufferFormatFeatures.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_vertexBufferFormatFeatures.pNext = nullptr;
		constexpr VkFormatFeatureFlags vk_eRequiredFeatures = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
		constexpr VkFormat vk_aeVertexBufferFormats[] {
			VK_FORMAT_R32_UINT,
			VK_FORMAT_R32_SINT,
			VK_FORMAT_R32_SFLOAT,
			VK_FORMAT_R32G32_UINT,
			VK_FORMAT_R32G32_SINT,
			VK_FORMAT_R32G32_SFLOAT,
			VK_FORMAT_R32G32B32_UINT,
			VK_FORMAT_R32G32B32_SINT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32A32_UINT,
			VK_FORMAT_R32G32B32A32_SINT,
			VK_FORMAT_R32G32B32A32_SFLOAT
		};
		bool bFormatsProperlySupported = true;
		for (const VkFormat vk_eVertexBufferFormat : vk_aeVertexBufferFormats) {
			vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eVertexBufferFormat, &vk_vertexBufferFormatFeatures);
			if ((vk_vertexBufferFormatFeatures.formatProperties.bufferFeatures & vk_eRequiredFeatures) != vk_eRequiredFeatures) {
				bFormatsProperlySupported = false;
				rMissingFeatures.push(append_to_string("Vulkan format ", std::hex, vk_eVertexBufferFormat, " is not supported for vertex buffers on this device"));
			}
		}
		return bFormatsProperlySupported;
	}

	bool create_render_pipelines() {
		PRINT_DEBUG("Creating Vulkan pipeline layout");
		const VkDescriptorSetLayout vk_ahWorldPipelineLayoutDescriptors[] = {
			vk_hCameraDescLayout,
			vk_hTextureDescLayout
		};
		const VkPipelineLayoutCreateInfo vk_worldPipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = sizeof(vk_ahWorldPipelineLayoutDescriptors) / sizeof(vk_ahWorldPipelineLayoutDescriptors[0]),
			.pSetLayouts = vk_ahWorldPipelineLayoutDescriptors
		};
		if (vkCreatePipelineLayout(vk_hDevice, &vk_worldPipelineLayoutCreateInfo, nullptr, &vk_hGraphicsPipelineLayout) == VK_SUCCESS) {
			PRINT_DEBUG("Creating vertex shader for game object rendering");
			if (create_vulkan_shader_from_file("shaders/gameobject_vertex.glsl.spv", 0, &vk_ahShaders[0])) {
				PRINT_DEBUG("Creating fragment shader for game object rendering");
				if (create_vulkan_shader_from_file("shaders/gameobject_fragment.glsl.spv", 0, &vk_ahShaders[1])) {
					PRINT_DEBUG("Creating render pipelines for rendering game objects");
					if (create_render_pipeline_internal())
						return true;
					PRINT_DEBUG("Destroying fragment shader for rendering game objects due to failure creating its render pipelines");
					vkDestroyShaderModule(vk_hDevice, vk_ahShaders[1], nullptr);
				}
				PRINT_DEBUG("Destroying vertex shader for rendering game objects due to failure creating its essential resources");
				vkDestroyShaderModule(vk_hDevice, vk_ahShaders[0], nullptr);
			}
			PRINT_DEBUG("Destroying Vulkan pipeline layout due to failure initializing the whole renderer");
			vkDestroyPipelineLayout(vk_hDevice, vk_hGraphicsPipelineLayout, nullptr);
		} else
			RE_FATAL_ERROR("Failed creating a Vulkan graphics pipeline for rendering objects in world");
		return false;
	}

	bool recreate_render_pipelines() {
		PRINT_DEBUG("Recreating Vulkan graphics pipeline");
		return create_render_pipeline_internal();
	}

	void destroy_render_pipelines() {
		PRINT_DEBUG("Destroying Vulkan pipeline ", vk_hGraphicsPipeline, " for rendering game objects");
		vkDestroyPipeline(vk_hDevice, vk_hGraphicsPipeline, nullptr);
		vk_hGraphicsPipeline = VK_NULL_HANDLE;
		for (const VkShaderModule &vk_rhShader : vk_ahShaders) {
			PRINT_DEBUG("Destroying Vulkan shader ", vk_rhShader, " for rendering game objects");
			vkDestroyShaderModule(vk_hDevice, vk_rhShader, nullptr);
		}
		vkDestroyPipelineLayout(vk_hDevice, vk_hGraphicsPipelineLayout, nullptr);
	}

}
