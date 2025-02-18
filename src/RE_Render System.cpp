#include "RE_Render System.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	RenderSystem* RenderSystem::pInstance = nullptr;

	RenderSystem::RenderSystem() {
		if (RenderSystem::pInstance) {
			RE_ERROR("An instance of the render system has already been created. The new one won't be initialized and remains invalid");
			return;
		}
		RenderSystem::pInstance = this;

		bValid = false;
		bVsyncActive = false;
		vk_eVsyncMode = VK_PRESENT_MODE_FIFO_KHR;
		vk_eNoVsync = VK_PRESENT_MODE_FIFO_KHR;
		vk_internalSurfaceFormat = {};
		vk_internalSwapchainImageSize = {};
		vk_hInternalSwapchain = VK_NULL_HANDLE;
		u32InternalSwapchainImageCount = 0;
		vk_pInternalSwapchainImages = nullptr;
		vk_pInternalSwapchainImageViews = nullptr;
		vk_hInternalVertexShader = VK_NULL_HANDLE;
		vk_hInternalFragmentShader = VK_NULL_HANDLE;
		vk_hInternalRenderPass = VK_NULL_HANDLE;
		vk_hInternalPipelineLayout = VK_NULL_HANDLE;
		vk_hInternalPipeline = VK_NULL_HANDLE;
		vk_pInternalFramebuffers = nullptr;
		vk_hInternalCmdPool = VK_NULL_HANDLE;
		vk_hInternalCmdBuffer = VK_NULL_HANDLE;
		vk_hInternalImgAvailableSemaphore = VK_NULL_HANDLE;
		vk_hInternalRenderFinishedSemaphore = VK_NULL_HANDLE;
		vk_hInternalFence = VK_NULL_HANDLE;

		for (uint32_t i = 0; i < RE_VK_UINT_PRESENT_MODES_COUNT; i++) {
			if (RE_VK_PTR_PRESENT_MODES[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
				vk_eVsyncMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			else if (RE_VK_PTR_PRESENT_MODES[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				vk_eNoVsync = VK_PRESENT_MODE_MAILBOX_KHR;
		}
		if (!createSwapchain())
			return;
		if (!createImageViews())
			return;
		if (!createShaders())
			return;
		if (!createRenderPass())
			return;
		if (!createPipeline())
			return;
		if (!createFramebuffers())
			return;
		if (!createCommands())
			return;
		if (!createSync())
			return;
		bValid = true;
	}

	RenderSystem::~RenderSystem() {
		if (RenderSystem::pInstance != this)
			return;
		RenderSystem::pInstance = nullptr;

		if (vk_hInternalImgAvailableSemaphore != VK_NULL_HANDLE)
			vkDestroySemaphore(RE_VK_HANDLE_DEVICE, vk_hInternalImgAvailableSemaphore, nullptr);
		if (vk_hInternalRenderFinishedSemaphore != VK_NULL_HANDLE)
			vkDestroySemaphore(RE_VK_HANDLE_DEVICE, vk_hInternalRenderFinishedSemaphore, nullptr);
		if (vk_hInternalFence != VK_NULL_HANDLE)
			vkDestroyFence(RE_VK_HANDLE_DEVICE, vk_hInternalFence, nullptr);
		if (vk_hInternalCmdPool != VK_NULL_HANDLE)
			vkDestroyCommandPool(RE_VK_HANDLE_DEVICE, vk_hInternalCmdPool, nullptr);
		if (vk_pInternalFramebuffers) {
			for (uint32_t i = 0; i < u32InternalSwapchainImageCount; i++)
				vkDestroyFramebuffer(RE_VK_HANDLE_DEVICE, vk_pInternalFramebuffers[i], nullptr);
			delete[] vk_pInternalFramebuffers;
		}
		if (vk_hInternalPipeline != VK_NULL_HANDLE)
			vkDestroyPipeline(RE_VK_HANDLE_DEVICE, vk_hInternalPipeline, nullptr);
		if (vk_hInternalPipelineLayout != VK_NULL_HANDLE)
			vkDestroyPipelineLayout(RE_VK_HANDLE_DEVICE, vk_hInternalPipelineLayout, nullptr);
		if (vk_hInternalRenderPass != VK_NULL_HANDLE)
			vkDestroyRenderPass(RE_VK_HANDLE_DEVICE, vk_hInternalRenderPass, nullptr);
		if (vk_hInternalVertexShader != VK_NULL_HANDLE)
			vkDestroyShaderModule(RE_VK_HANDLE_DEVICE, vk_hInternalVertexShader, nullptr);
		if (vk_hInternalFragmentShader != VK_NULL_HANDLE)
			vkDestroyShaderModule(RE_VK_HANDLE_DEVICE, vk_hInternalFragmentShader, nullptr);
		if (vk_pInternalSwapchainImageViews)
			for (uint32_t i = 0; i < u32InternalSwapchainImageCount; i++)
				if (vk_pInternalSwapchainImageViews[i] != VK_NULL_HANDLE)
					vkDestroyImageView(RE_VK_HANDLE_DEVICE, vk_pInternalSwapchainImageViews[i], nullptr);
		if (vk_pInternalSwapchainImages) {
			delete[] vk_pInternalSwapchainImages;
			delete[] vk_pInternalSwapchainImageViews;
		}
		if (vk_hInternalSwapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(RE_VK_HANDLE_DEVICE, vk_hInternalSwapchain, nullptr);
	}

	bool RenderSystem::createSwapchain() {
		bool bFoundFormat = false;
		for (uint32_t u32SurfaceFormatIndex = 0; u32SurfaceFormatIndex < RE_VK_UINT_SURFACE_FORMATS_COUNT; u32SurfaceFormatIndex++) {
			if (RE_VK_PTR_SURFACE_FORMATS[u32SurfaceFormatIndex].format == VK_FORMAT_R8G8B8A8_SRGB && RE_VK_PTR_SURFACE_FORMATS[u32SurfaceFormatIndex].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				vk_internalSurfaceFormat = RE_VK_PTR_SURFACE_FORMATS[u32SurfaceFormatIndex];
				bFoundFormat = true;
				break;
			}
		}
		if (!bFoundFormat)
			vk_internalSurfaceFormat = RE_VK_PTR_SURFACE_FORMATS[0];
		if (RE_VK_SURFACE_CAPABILITIES.currentExtent.width != std::numeric_limits<uint32_t>::max() && RE_VK_SURFACE_CAPABILITIES.currentExtent.height != std::numeric_limits<uint32_t>::max())
			vk_internalSwapchainImageSize = RE_VK_SURFACE_CAPABILITIES.currentExtent;
		else {
			Vector<REushort, 2> winSize = Window::pInstance->getSize();
			vk_internalSwapchainImageSize = {static_cast<uint32_t>(winSize[0]), static_cast<uint32_t>(winSize[1])};
			vk_internalSwapchainImageSize.width = std::clamp(vk_internalSwapchainImageSize.width, RE_VK_SURFACE_CAPABILITIES.minImageExtent.width, RE_VK_SURFACE_CAPABILITIES.maxImageExtent.width);
			vk_internalSwapchainImageSize.height = std::clamp(vk_internalSwapchainImageSize.height, RE_VK_SURFACE_CAPABILITIES.minImageExtent.height, RE_VK_SURFACE_CAPABILITIES.maxImageExtent.height);
		}
		u32InternalSwapchainImageCount = RE_VK_SURFACE_CAPABILITIES.minImageCount + 1;
		if (RE_VK_SURFACE_CAPABILITIES.maxImageCount > 0 && u32InternalSwapchainImageCount > RE_VK_SURFACE_CAPABILITIES.maxImageCount)
			u32InternalSwapchainImageCount = RE_VK_SURFACE_CAPABILITIES.maxImageCount;

		VkSwapchainCreateInfoKHR vk_swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		vk_swapchainCreateInfo.surface = RE_VK_HANDLE_SURFACE;
		vk_swapchainCreateInfo.minImageCount = u32InternalSwapchainImageCount;
		vk_swapchainCreateInfo.imageFormat = vk_internalSurfaceFormat.format;
		vk_swapchainCreateInfo.imageColorSpace = vk_internalSurfaceFormat.colorSpace;
		vk_swapchainCreateInfo.imageExtent = vk_internalSwapchainImageSize;
		vk_swapchainCreateInfo.imageArrayLayers = 1;
		vk_swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		uint32_t u32QueueFamilyIndices[] = {RE_VK_QUEUE_INDICES.u32GraphicsFamily, RE_VK_QUEUE_INDICES.u32PresentationFamily};
		if (RE_VK_QUEUE_INDICES.u32GraphicsFamily != RE_VK_QUEUE_INDICES.u32PresentationFamily) {
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_swapchainCreateInfo.queueFamilyIndexCount = 2;
			vk_swapchainCreateInfo.pQueueFamilyIndices = u32QueueFamilyIndices;
		} else
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vk_swapchainCreateInfo.preTransform = RE_VK_SURFACE_CAPABILITIES.currentTransform;
		vk_swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		vk_swapchainCreateInfo.presentMode = bVsyncActive ? vk_eVsyncMode : vk_eNoVsync;
		vk_swapchainCreateInfo.clipped = VK_TRUE;
		vk_swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
		VkResult vk_eSuccessResult = vkCreateSwapchainKHR(RE_VK_HANDLE_DEVICE, &vk_swapchainCreateInfo, nullptr, &vk_hInternalSwapchain);
		if (!CHECK_VK_RESULT(vk_eSuccessResult))
			RE_FATAL_ERROR("Failed creating swapchain in Vulkan");
		return vk_eSuccessResult == VK_SUCCESS;
	}

	bool RenderSystem::createImageViews() {
		vkGetSwapchainImagesKHR(RE_VK_HANDLE_DEVICE, vk_hInternalSwapchain, &u32InternalSwapchainImageCount, nullptr);
		vk_pInternalSwapchainImages = new VkImage[u32InternalSwapchainImageCount];
		vk_pInternalSwapchainImageViews = new VkImageView[u32InternalSwapchainImageCount];
		for (uint32_t uiIndex = 0; uiIndex < u32InternalSwapchainImageCount; uiIndex++)
			vk_pInternalSwapchainImageViews[uiIndex] = VK_NULL_HANDLE;
		vkGetSwapchainImagesKHR(RE_VK_HANDLE_DEVICE, vk_hInternalSwapchain, &u32InternalSwapchainImageCount, vk_pInternalSwapchainImages);
		bool bFailure = false;
		for (uint32_t u32ImageIndex = 0; u32ImageIndex < u32InternalSwapchainImageCount; u32ImageIndex++) {
			VkImageViewCreateInfo vk_imgViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			vk_imgViewCreateInfo.image = vk_pInternalSwapchainImages[u32ImageIndex];
			vk_imgViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			vk_imgViewCreateInfo.format = vk_internalSurfaceFormat.format;
			vk_imgViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			vk_imgViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			vk_imgViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			vk_imgViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			vk_imgViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			vk_imgViewCreateInfo.subresourceRange.baseMipLevel = 0;
			vk_imgViewCreateInfo.subresourceRange.levelCount = 1;
			vk_imgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			vk_imgViewCreateInfo.subresourceRange.layerCount = 1;
			if (!CHECK_VK_RESULT(vkCreateImageView(RE_VK_HANDLE_DEVICE, &vk_imgViewCreateInfo, nullptr, &vk_pInternalSwapchainImageViews[u32ImageIndex]))) {
				RE_FATAL_ERROR(appendStrings("Failed creating image view ", u32ImageIndex, " out of ", u32InternalSwapchainImageCount, " in Vulkan"));
				bFailure = true;
			}
		}
		return !bFailure;
	}

	bool RenderSystem::createShaders() {
		std::ifstream vertexFile("shaders/vertex.spv", std::ios::ate | std::ios::binary);
		if (!vertexFile.is_open()) {
			RE_FATAL_ERROR("Failed opening vertex binary file");
			return false;
		}
		REuint u32FileSize = static_cast<REuint>(vertexFile.tellg());
		vertexFile.seekg(0);
		char* pcData = new char[u32FileSize];
		vertexFile.read(pcData, u32FileSize);
		vertexFile.close();
		VkShaderModuleCreateInfo vk_shaderCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		vk_shaderCreateInfo.codeSize = u32FileSize;
		vk_shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(pcData);
		if (!CHECK_VK_RESULT(vkCreateShaderModule(RE_VK_HANDLE_DEVICE, &vk_shaderCreateInfo, nullptr, &vk_hInternalVertexShader))) {
			RE_FATAL_ERROR("Failed creating vertex shader in Vulkan");
			delete[] pcData;
			return false;
		}
		delete[] pcData;

		std::ifstream fragmentFile("shaders/fragment.spv", std::ios::ate | std::ios::binary);
		if (!fragmentFile.is_open()) {
			RE_FATAL_ERROR("Failed opening fragment binary file");
			return false;
		}
		u32FileSize = static_cast<REuint>(fragmentFile.tellg());
		fragmentFile.seekg(0);
		pcData = new char[u32FileSize];
		fragmentFile.read(pcData, u32FileSize);
		fragmentFile.close();
		vk_shaderCreateInfo.codeSize = u32FileSize;
		vk_shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(pcData);
		VkResult vk_eSuccessResult = vkCreateShaderModule(RE_VK_HANDLE_DEVICE, &vk_shaderCreateInfo, nullptr, &vk_hInternalFragmentShader);
		if (!CHECK_VK_RESULT(vk_eSuccessResult))
			RE_FATAL_ERROR("Failed creating fragment shader in Vulkan");
		delete[] pcData;
		return vk_eSuccessResult == VK_SUCCESS;
	}

	bool RenderSystem::createRenderPass() {
		VkAttachmentDescription vk_attachmentDesc = {};
		vk_attachmentDesc.format = vk_internalSurfaceFormat.format;
		vk_attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		vk_attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		vk_attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		vk_attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		vk_attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vk_attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		vk_attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference vk_attachmentRef = {};
		vk_attachmentRef.attachment = 0;
		vk_attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription vk_subpassDesc = {};
		vk_subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vk_subpassDesc.colorAttachmentCount = 1;
		vk_subpassDesc.pColorAttachments = &vk_attachmentRef;

		VkSubpassDependency vk_subpassDependency = {};
		vk_subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		vk_subpassDependency.dstSubpass = 0;
		vk_subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vk_subpassDependency.srcAccessMask = 0;
		vk_subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vk_subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo vk_hInternalRenderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		vk_hInternalRenderPassCreateInfo.attachmentCount = 1;
		vk_hInternalRenderPassCreateInfo.pAttachments = &vk_attachmentDesc;
		vk_hInternalRenderPassCreateInfo.subpassCount = 1;
		vk_hInternalRenderPassCreateInfo.pSubpasses = &vk_subpassDesc;
		vk_hInternalRenderPassCreateInfo.dependencyCount = 1;
		vk_hInternalRenderPassCreateInfo.pDependencies = &vk_subpassDependency;
		VkResult vk_eSuccessResult = vkCreateRenderPass(RE_VK_HANDLE_DEVICE, &vk_hInternalRenderPassCreateInfo, nullptr, &vk_hInternalRenderPass);
		if (!CHECK_VK_RESULT(vk_eSuccessResult))
			RE_FATAL_ERROR("Failed creating a render pass in Vulkan");
		return vk_eSuccessResult == VK_SUCCESS;
	}

	bool RenderSystem::createPipeline() {
		VkPipelineLayoutCreateInfo vk_layoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		vk_layoutCreateInfo.setLayoutCount = 0;
		vk_layoutCreateInfo.pSetLayouts = nullptr;
		vk_layoutCreateInfo.pushConstantRangeCount = 0;
		vk_layoutCreateInfo.pPushConstantRanges = nullptr;
		if (!CHECK_VK_RESULT(vkCreatePipelineLayout(RE_VK_HANDLE_DEVICE, &vk_layoutCreateInfo, nullptr, &vk_hInternalPipelineLayout))) {
			RE_FATAL_ERROR("Failed creating pipeline layout in Vulkan");
			return false;
		}

		VkPipelineShaderStageCreateInfo* vk_pShaderStages = new VkPipelineShaderStageCreateInfo[2] {{}, {}};
		for (REuint i = 0; i < 2; i++) {
			vk_pShaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vk_pShaderStages[i].pName = "main";
		}
		vk_pShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		vk_pShaderStages[0].module = vk_hInternalVertexShader;
		vk_pShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		vk_pShaderStages[1].module = vk_hInternalFragmentShader;

		constexpr uint32_t u32DynamicStatesCount = 2;
		VkDynamicState* vk_pDynamicStates = new VkDynamicState[u32DynamicStatesCount] {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo vk_dynamicStatesCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		vk_dynamicStatesCreateInfo.dynamicStateCount = u32DynamicStatesCount;
		vk_dynamicStatesCreateInfo.pDynamicStates = vk_pDynamicStates;

		VkPipelineVertexInputStateCreateInfo vk_vertexInputCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		vk_vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
		vk_vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
		vk_vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
		vk_vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo vk_inputAssemblyCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		vk_inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		vk_inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport vk_viewport = {};
		vk_viewport.x = 0.0f;
		vk_viewport.y = 0.0f;
		vk_viewport.width = static_cast<float>(vk_internalSwapchainImageSize.width);
		vk_viewport.height = static_cast<float>(vk_internalSwapchainImageSize.height);
		vk_viewport.minDepth = 0.0f;
		vk_viewport.maxDepth = 1.0f;

		VkRect2D vk_scissor = {};
		vk_scissor.offset = {0, 0};
		vk_scissor.extent = vk_internalSwapchainImageSize;

		VkPipelineViewportStateCreateInfo vk_viewportCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		vk_viewportCreateInfo.viewportCount = 1;
		vk_viewportCreateInfo.pViewports = &vk_viewport;
		vk_viewportCreateInfo.scissorCount = 1;
		vk_viewportCreateInfo.pScissors = &vk_scissor;

		VkPipelineRasterizationStateCreateInfo vk_rasterizationCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		vk_rasterizationCreateInfo.depthClampEnable = VK_FALSE;
		vk_rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		vk_rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		vk_rasterizationCreateInfo.lineWidth = 1.0f;
		vk_rasterizationCreateInfo.cullMode = VK_CULL_MODE_NONE;
		vk_rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		vk_rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
		vk_rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
		vk_rasterizationCreateInfo.depthBiasClamp = 0.0f;
		vk_rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo vk_multisampleCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		vk_multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
		vk_multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		vk_multisampleCreateInfo.minSampleShading = 1.0f;
		vk_multisampleCreateInfo.pSampleMask = nullptr;
		vk_multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
		vk_multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState vk_colorBlendAttachment = {};
		vk_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		vk_colorBlendAttachment.blendEnable = VK_TRUE;
		vk_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		vk_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		vk_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		vk_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		vk_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		vk_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo vk_colorBlendCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		vk_colorBlendCreateInfo.logicOpEnable = VK_FALSE;
		vk_colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		vk_colorBlendCreateInfo.attachmentCount = 1;
		vk_colorBlendCreateInfo.pAttachments = &vk_colorBlendAttachment;
		vk_colorBlendCreateInfo.blendConstants[0] = 0.0f;
		vk_colorBlendCreateInfo.blendConstants[1] = 0.0f;
		vk_colorBlendCreateInfo.blendConstants[2] = 0.0f;
		vk_colorBlendCreateInfo.blendConstants[3] = 0.0f;

		VkGraphicsPipelineCreateInfo vk_pipelineCreateInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		vk_pipelineCreateInfo.stageCount = 2;
		vk_pipelineCreateInfo.pStages = vk_pShaderStages;
		vk_pipelineCreateInfo.pVertexInputState = &vk_vertexInputCreateInfo;
		vk_pipelineCreateInfo.pInputAssemblyState = &vk_inputAssemblyCreateInfo;
		vk_pipelineCreateInfo.pViewportState = &vk_viewportCreateInfo;
		vk_pipelineCreateInfo.pRasterizationState = &vk_rasterizationCreateInfo;
		vk_pipelineCreateInfo.pMultisampleState = &vk_multisampleCreateInfo;
		vk_pipelineCreateInfo.pDepthStencilState = nullptr;
		vk_pipelineCreateInfo.pColorBlendState = &vk_colorBlendCreateInfo;
		vk_pipelineCreateInfo.pDynamicState = &vk_dynamicStatesCreateInfo;
		vk_pipelineCreateInfo.layout = vk_hInternalPipelineLayout;
		vk_pipelineCreateInfo.renderPass = vk_hInternalRenderPass;
		vk_pipelineCreateInfo.subpass = 0;
		vk_pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		vk_pipelineCreateInfo.basePipelineIndex = -1;
		VkResult vk_eSuccessResult = vkCreateGraphicsPipelines(RE_VK_HANDLE_DEVICE, VK_NULL_HANDLE, 1, &vk_pipelineCreateInfo, nullptr, &vk_hInternalPipeline);
		if (!CHECK_VK_RESULT(vk_eSuccessResult))
			RE_FATAL_ERROR("Failed creating a graphics render pipeline in Vulkan");
		delete[] vk_pShaderStages;
		delete[] vk_pDynamicStates;
		return vk_eSuccessResult == VK_SUCCESS;
	}

	bool RenderSystem::createFramebuffers() {
		vk_pInternalFramebuffers = new VkFramebuffer[u32InternalSwapchainImageCount];
		bool bFailure = false;
		for (uint32_t u32FramebufferIndex = 0; u32FramebufferIndex < u32InternalSwapchainImageCount; u32FramebufferIndex++) {
			VkFramebufferCreateInfo vk_framebufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			vk_framebufferCreateInfo.renderPass = vk_hInternalRenderPass;
			vk_framebufferCreateInfo.attachmentCount = 1;
			vk_framebufferCreateInfo.pAttachments = &vk_pInternalSwapchainImageViews[u32FramebufferIndex];
			vk_framebufferCreateInfo.width = vk_internalSwapchainImageSize.width;
			vk_framebufferCreateInfo.height = vk_internalSwapchainImageSize.height;
			vk_framebufferCreateInfo.layers = 1;
			if (!CHECK_VK_RESULT(vkCreateFramebuffer(RE_VK_HANDLE_DEVICE, &vk_framebufferCreateInfo, nullptr, &vk_pInternalFramebuffers[u32FramebufferIndex]))) {
				RE_FATAL_ERROR(appendStrings("Failed creating a framebuffer at index ", u32FramebufferIndex, " in Vulkan"));
				bFailure = true;
			}
		}
		return !bFailure;
	}

	bool RenderSystem::createCommands() {
		VkCommandPoolCreateInfo vk_cmdPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		vk_cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		vk_cmdPoolCreateInfo.queueFamilyIndex = RE_VK_QUEUE_INDICES.u32GraphicsFamily;
		if (!CHECK_VK_RESULT(vkCreateCommandPool(RE_VK_HANDLE_DEVICE, &vk_cmdPoolCreateInfo, nullptr, &vk_hInternalCmdPool))) {
			RE_FATAL_ERROR("Failed creating a command pool in Vulkan");
			return false;
		}

		VkCommandBufferAllocateInfo vk_cmdBufferAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		vk_cmdBufferAllocInfo.commandPool = vk_hInternalCmdPool;
		vk_cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		vk_cmdBufferAllocInfo.commandBufferCount = 1;
		if (!CHECK_VK_RESULT(vkAllocateCommandBuffers(RE_VK_HANDLE_DEVICE, &vk_cmdBufferAllocInfo, &vk_hInternalCmdBuffer))) {
			RE_FATAL_ERROR("Failed allocating a command buffer in Vulkan");
			return false;
		}
		return true;
	}

	bool RenderSystem::createSync() {
		VkSemaphoreCreateInfo vk_semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		if (!CHECK_VK_RESULT(vkCreateSemaphore(RE_VK_HANDLE_DEVICE, &vk_semaphoreCreateInfo, nullptr, &vk_hInternalImgAvailableSemaphore)) || !CHECK_VK_RESULT(vkCreateSemaphore(RE_VK_HANDLE_DEVICE, &vk_semaphoreCreateInfo, nullptr, &vk_hInternalRenderFinishedSemaphore)) || !CHECK_VK_RESULT(vkCreateFence(RE_VK_HANDLE_DEVICE, &fenceCreateInfo, nullptr, &vk_hInternalFence))) {
			RE_FATAL_ERROR("Failed creating semaphores/fence for synchronization with rendering in Vulkan");
			return false;
		}
		return true;
	}

	bool RenderSystem::recordCommandBuffer(VkCommandBuffer vk_cmdBuffer, uint32_t u32ImgIndex) {
		VkCommandBufferBeginInfo vk_cmdBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		vk_cmdBufferBeginInfo.flags = 0;
		vk_cmdBufferBeginInfo.pInheritanceInfo = nullptr;
		if (!CHECK_VK_RESULT(vkBeginCommandBuffer(vk_cmdBuffer, &vk_cmdBufferBeginInfo))) {
			RE_FATAL_ERROR("Failed beginning to record the command buffer in Vulkan");
			return false;
		}
		VkRenderPassBeginInfo vk_renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		vk_renderPassBeginInfo.renderPass = vk_hInternalRenderPass;
		vk_renderPassBeginInfo.framebuffer = vk_pInternalFramebuffers[u32ImgIndex];
		vk_renderPassBeginInfo.renderArea.offset = {0, 0};
		vk_renderPassBeginInfo.renderArea.extent = vk_internalSwapchainImageSize;
		VkClearValue vk_clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
		vk_renderPassBeginInfo.clearValueCount = 1;
		vk_renderPassBeginInfo.pClearValues = &vk_clearColor;
		vkCmdBeginRenderPass(vk_cmdBuffer, &vk_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(vk_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hInternalPipeline);
		VkViewport vk_viewport = {};
		vk_viewport.x = 0.0f;
		vk_viewport.y = 0.0f;
		vk_viewport.width = static_cast<float>(vk_internalSwapchainImageSize.width);
		vk_viewport.height = static_cast<float>(vk_internalSwapchainImageSize.height);
		vk_viewport.minDepth = 0.0f;
		vk_viewport.maxDepth = 1.0f;
		vkCmdSetViewport(vk_cmdBuffer, 0, 1, &vk_viewport);
		VkRect2D vk_scissor = {};
		vk_scissor.offset = {0, 0};
		vk_scissor.extent = vk_internalSwapchainImageSize;
		vkCmdSetScissor(vk_cmdBuffer, 0, 1, &vk_scissor);
		vkCmdDraw(vk_cmdBuffer, 3, 1, 0, 0);
		vkCmdEndRenderPass(vk_cmdBuffer);
		if (!CHECK_VK_RESULT(vkEndCommandBuffer(vk_cmdBuffer))) {
			RE_FATAL_ERROR("Failed stopping to record the command buffer in Vulkan");
			return false;
		}
		return true;
	}

	void RenderSystem::drawFrame() {
		vkWaitForFences(RE_VK_HANDLE_DEVICE, 1, &vk_hInternalFence, VK_TRUE, UINT64_MAX);
		vkResetFences(RE_VK_HANDLE_DEVICE, 1, &vk_hInternalFence);
		uint32_t u32ImgIndex = 0;
		vkAcquireNextImageKHR(RE_VK_HANDLE_DEVICE, vk_hInternalSwapchain, UINT64_MAX, vk_hInternalImgAvailableSemaphore, VK_NULL_HANDLE, &u32ImgIndex);
		vkResetCommandBuffer(vk_hInternalCmdBuffer, 0);
		recordCommandBuffer(vk_hInternalCmdBuffer, u32ImgIndex);

		VkSubmitInfo vk_submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		vk_submitInfo.waitSemaphoreCount = 1;
		vk_submitInfo.pWaitSemaphores = &vk_hInternalImgAvailableSemaphore;
		VkPipelineStageFlags vk_waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		vk_submitInfo.pWaitDstStageMask = vk_waitStages;
		vk_submitInfo.commandBufferCount = 1;
		vk_submitInfo.pCommandBuffers = &vk_hInternalCmdBuffer;
		vk_submitInfo.signalSemaphoreCount = 1;
		vk_submitInfo.pSignalSemaphores = &vk_hInternalRenderFinishedSemaphore;
		if (!CHECK_VK_RESULT(vkQueueSubmit(RE_VK_HANDLE_GRAPHICS_QUEUE, 1, &vk_submitInfo, vk_hInternalFence))) {
			RE_FATAL_ERROR("Failed submitting data to graphics queue in Vulkan");
			return;
		}
		VkPresentInfoKHR vk_presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		vk_presentInfo.waitSemaphoreCount = 1;
		vk_presentInfo.pWaitSemaphores = &vk_hInternalRenderFinishedSemaphore;
		vk_presentInfo.swapchainCount = 1;
		vk_presentInfo.pSwapchains = &vk_hInternalSwapchain;
		vk_presentInfo.pImageIndices = &u32ImgIndex;
		vk_presentInfo.pResults = nullptr;
		vkQueuePresentKHR(RE_VK_HANDLE_PRESENT_QUEUE, &vk_presentInfo);
	}

	void RenderSystem::windowResize(Vector<REushort, 2> newSize) {

	}

	bool RenderSystem::isValid() {
		return bValid;
	}

}