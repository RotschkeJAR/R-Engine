#include "RE_Render System.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	RenderSystem* RenderSystem::instance = nullptr;

	RenderSystem::RenderSystem() {
		if (RenderSystem::instance) {
			RE_ERROR("An instance of the render system has already been created. The new one won't be initialized and remains invalid");
			return;
		}
		RenderSystem::instance = this;

		valid = false;
		vsyncActive = false;
		vsyncMode = VK_PRESENT_MODE_FIFO_KHR;
		noVsync = VK_PRESENT_MODE_FIFO_KHR;
		internalSurfaceFormat = {};
		internalSwapchainImageSize = {};
		internalSwapchain = VK_NULL_HANDLE;
		internalSwapchainImageCount = 0;
		internalSwapchainImages = nullptr;
		internalSwapchainImageViews = nullptr;
		internalVertexShader = VK_NULL_HANDLE;
		internalFragmentShader = VK_NULL_HANDLE;
		internalRenderPass = VK_NULL_HANDLE;
		internalPipelineLayout = VK_NULL_HANDLE;
		internalPipeline = VK_NULL_HANDLE;
		internalFramebuffers = nullptr;
		internalCmdPool = VK_NULL_HANDLE;
		internalCmdBuffer = VK_NULL_HANDLE;
		internalImgAvailableSemaphore = VK_NULL_HANDLE;
		internalRenderFinishedSemaphore = VK_NULL_HANDLE;
		internalFence = VK_NULL_HANDLE;

		for (uint32_t i = 0; i < vkPresentModesCount; i++) {
			if (vkPresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
				vsyncMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			else if (vkPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				noVsync = VK_PRESENT_MODE_MAILBOX_KHR;
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
		valid = true;
	}

	RenderSystem::~RenderSystem() {
		if (RenderSystem::instance != this)
			return;
		RenderSystem::instance = nullptr;

		if (internalImgAvailableSemaphore != VK_NULL_HANDLE)
			vkDestroySemaphore(vkDevice, internalImgAvailableSemaphore, nullptr);
		if (internalRenderFinishedSemaphore != VK_NULL_HANDLE)
			vkDestroySemaphore(vkDevice, internalRenderFinishedSemaphore, nullptr);
		if (internalFence != VK_NULL_HANDLE)
			vkDestroyFence(vkDevice, internalFence, nullptr);
		if (internalCmdPool != VK_NULL_HANDLE)
			vkDestroyCommandPool(vkDevice, internalCmdPool, nullptr);
		if (internalFramebuffers) {
			for (uint32_t i = 0; i < internalSwapchainImageCount; i++)
				vkDestroyFramebuffer(vkDevice, internalFramebuffers[i], nullptr);
			delete[] internalFramebuffers;
		}
		if (internalPipeline != VK_NULL_HANDLE)
			vkDestroyPipeline(vkDevice, internalPipeline, nullptr);
		if (internalPipelineLayout != VK_NULL_HANDLE)
			vkDestroyPipelineLayout(vkDevice, internalPipelineLayout, nullptr);
		if (internalRenderPass != VK_NULL_HANDLE)
			vkDestroyRenderPass(vkDevice, internalRenderPass, nullptr);
		if (internalVertexShader != VK_NULL_HANDLE)
			vkDestroyShaderModule(vkDevice, internalVertexShader, nullptr);
		if (internalFragmentShader != VK_NULL_HANDLE)
			vkDestroyShaderModule(vkDevice, internalFragmentShader, nullptr);
		if (internalSwapchainImageViews)
			for (uint32_t i = 0; i < internalSwapchainImageCount; i++)
				if (internalSwapchainImageViews[i] != VK_NULL_HANDLE)
					vkDestroyImageView(vkDevice, internalSwapchainImageViews[i], nullptr);
		if (internalSwapchainImages) {
			delete[] internalSwapchainImages;
			delete[] internalSwapchainImageViews;
		}
		if (internalSwapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(vkDevice, internalSwapchain, nullptr);
	}

	bool RenderSystem::createSwapchain() {
		bool foundFormat = false;
		for (uint32_t i = 0; i < vkSurfaceFormatsCount; i++) {
			if (vkSurfaceFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB && vkSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				internalSurfaceFormat = vkSurfaceFormats[i];
				foundFormat = true;
				break;
			}
		}
		if (!foundFormat)
			internalSurfaceFormat = vkSurfaceFormats[0];
		if (vkSurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() && vkSurfaceCapabilities.currentExtent.height != std::numeric_limits<uint32_t>::max())
			internalSwapchainImageSize = vkSurfaceCapabilities.currentExtent;
		else {
			Vector<REushort, 2> winSize = Window::instance->getSize();
			internalSwapchainImageSize = {static_cast<uint32_t>(winSize[0]), static_cast<uint32_t>(winSize[1])};
			internalSwapchainImageSize.width = std::clamp(internalSwapchainImageSize.width, vkSurfaceCapabilities.minImageExtent.width, vkSurfaceCapabilities.maxImageExtent.width);
			internalSwapchainImageSize.height = std::clamp(internalSwapchainImageSize.height, vkSurfaceCapabilities.minImageExtent.height, vkSurfaceCapabilities.maxImageExtent.height);
		}
		internalSwapchainImageCount = vkSurfaceCapabilities.minImageCount + 1;
		if (vkSurfaceCapabilities.maxImageCount > 0 && internalSwapchainImageCount > vkSurfaceCapabilities.maxImageCount)
			internalSwapchainImageCount = vkSurfaceCapabilities.maxImageCount;

		VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		swapchainCreateInfo.surface = vkSurface;
		swapchainCreateInfo.minImageCount = internalSwapchainImageCount;
		swapchainCreateInfo.imageFormat = internalSurfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = internalSurfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = internalSwapchainImageSize;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		uint32_t queueFamilyIndices[] = {vkQueueIndices.graphicsFamily, vkQueueIndices.presentationFamily};
		if (vkQueueIndices.graphicsFamily != vkQueueIndices.presentationFamily) {
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.preTransform = vkSurfaceCapabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = vsyncActive ? vsyncMode : noVsync;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
		VkResult successResult = vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, nullptr, &internalSwapchain);
		if (!CHECK_VK_RESULT(successResult))
			RE_FATAL_ERROR("Failed creating swapchain in Vulkan");
		return successResult == VK_SUCCESS;
	}

	bool RenderSystem::createImageViews() {
		vkGetSwapchainImagesKHR(vkDevice, internalSwapchain, &internalSwapchainImageCount, nullptr);
		internalSwapchainImages = new VkImage[internalSwapchainImageCount];
		internalSwapchainImageViews = new VkImageView[internalSwapchainImageCount];
		for (uint32_t i = 0; i < internalSwapchainImageCount; i++)
			internalSwapchainImageViews[i] = VK_NULL_HANDLE;
		vkGetSwapchainImagesKHR(vkDevice, internalSwapchain, &internalSwapchainImageCount, internalSwapchainImages);
		bool failure = false;
		for (uint32_t imageIndex = 0; imageIndex < internalSwapchainImageCount; imageIndex++) {
			VkImageViewCreateInfo imgViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			imgViewCreateInfo.image = internalSwapchainImages[imageIndex];
			imgViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imgViewCreateInfo.format = internalSurfaceFormat.format;
			imgViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imgViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imgViewCreateInfo.subresourceRange.levelCount = 1;
			imgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imgViewCreateInfo.subresourceRange.layerCount = 1;
			if (!CHECK_VK_RESULT(vkCreateImageView(vkDevice, &imgViewCreateInfo, nullptr, &internalSwapchainImageViews[imageIndex]))) {
				RE_FATAL_ERROR(appendStrings("Failed creating image view ", imageIndex, " out of ", internalSwapchainImageCount, " in Vulkan"));
				failure = true;
			}
		}
		return !failure;
	}

	bool RenderSystem::createShaders() {
		std::ifstream vertexFile("shaders/vertex.spv", std::ios::ate | std::ios::binary);
		if (!vertexFile.is_open()) {
			RE_FATAL_ERROR("Failed opening vertex binary file");
			return false;
		}
		REuint fileSize = static_cast<REuint>(vertexFile.tellg());
		vertexFile.seekg(0);
		char* data = new char[fileSize];
		vertexFile.read(data, fileSize);
		vertexFile.close();
		VkShaderModuleCreateInfo shaderCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		shaderCreateInfo.codeSize = fileSize;
		shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(data);
		if (!CHECK_VK_RESULT(vkCreateShaderModule(vkDevice, &shaderCreateInfo, nullptr, &internalVertexShader))) {
			RE_FATAL_ERROR("Failed creating vertex shader in Vulkan");
			delete[] data;
			return false;
		}
		delete[] data;

		std::ifstream fragmentFile("shaders/fragment.spv", std::ios::ate | std::ios::binary);
		if (!fragmentFile.is_open()) {
			RE_FATAL_ERROR("Failed opening fragment binary file");
			return false;
		}
		fileSize = static_cast<REuint>(fragmentFile.tellg());
		fragmentFile.seekg(0);
		data = new char[fileSize];
		fragmentFile.read(data, fileSize);
		fragmentFile.close();
		shaderCreateInfo.codeSize = fileSize;
		shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(data);
		VkResult successResult = vkCreateShaderModule(vkDevice, &shaderCreateInfo, nullptr, &internalFragmentShader);
		if (!CHECK_VK_RESULT(successResult))
			RE_FATAL_ERROR("Failed creating fragment shader in Vulkan");
		delete[] data;
		return successResult == VK_SUCCESS;
	}

	bool RenderSystem::createRenderPass() {
		VkAttachmentDescription attachmentDesc = {};
		attachmentDesc.format = internalSurfaceFormat.format;
		attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference attachmentRef = {};
		attachmentRef.attachment = 0;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDesc = {};
		subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDesc.colorAttachmentCount = 1;
		subpassDesc.pColorAttachments = &attachmentRef;

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.srcAccessMask = 0;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo internalRenderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		internalRenderPassCreateInfo.attachmentCount = 1;
		internalRenderPassCreateInfo.pAttachments = &attachmentDesc;
		internalRenderPassCreateInfo.subpassCount = 1;
		internalRenderPassCreateInfo.pSubpasses = &subpassDesc;
		internalRenderPassCreateInfo.dependencyCount = 1;
		internalRenderPassCreateInfo.pDependencies = &subpassDependency;
		VkResult successResult = vkCreateRenderPass(vkDevice, &internalRenderPassCreateInfo, nullptr, &internalRenderPass);
		if (!CHECK_VK_RESULT(successResult))
			RE_FATAL_ERROR("Failed creating a render pass in Vulkan");
		return successResult == VK_SUCCESS;
	}

	bool RenderSystem::createPipeline() {
		VkPipelineLayoutCreateInfo layoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		layoutCreateInfo.setLayoutCount = 0;
		layoutCreateInfo.pSetLayouts = nullptr;
		layoutCreateInfo.pushConstantRangeCount = 0;
		layoutCreateInfo.pPushConstantRanges = nullptr;
		if (!CHECK_VK_RESULT(vkCreatePipelineLayout(vkDevice, &layoutCreateInfo, nullptr, &internalPipelineLayout))) {
			RE_FATAL_ERROR("Failed creating pipeline layout in Vulkan");
			return false;
		}

		VkPipelineShaderStageCreateInfo* shaderStages = new VkPipelineShaderStageCreateInfo[2] {{}, {}};
		for (REuint i = 0; i < 2; i++) {
			shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[i].pName = "main";
		}
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = internalVertexShader;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = internalFragmentShader;

		constexpr uint32_t dynamicStatesCount = 2;
		VkDynamicState* dynamicStates = new VkDynamicState[dynamicStatesCount] {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamicStatesCreateInfo.dynamicStateCount = dynamicStatesCount;
		dynamicStatesCreateInfo.pDynamicStates = dynamicStates;

		VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
		vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
		vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
		vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(internalSwapchainImageSize.width);
		viewport.height = static_cast<float>(internalSwapchainImageSize.height);
		viewport.minDepth = -1.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = internalSwapchainImageSize;

		VkPipelineViewportStateCreateInfo viewportCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportCreateInfo.viewportCount = 1;
		viewportCreateInfo.pViewports = &viewport;
		viewportCreateInfo.scissorCount = 1;
		viewportCreateInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizationCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationCreateInfo.lineWidth = 1.0f;
		rasterizationCreateInfo.cullMode = VK_CULL_MODE_NONE;
		rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationCreateInfo.depthBiasClamp = 0.0f;
		rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleCreateInfo.minSampleShading = 1.0f;
		multisampleCreateInfo.pSampleMask = nullptr;
		multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		colorBlendCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendCreateInfo.attachmentCount = 1;
		colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
		colorBlendCreateInfo.blendConstants[0] = 0.0f;
		colorBlendCreateInfo.blendConstants[1] = 0.0f;
		colorBlendCreateInfo.blendConstants[2] = 0.0f;
		colorBlendCreateInfo.blendConstants[3] = 0.0f;

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStages;
		pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
		pipelineCreateInfo.pViewportState = &viewportCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
		pipelineCreateInfo.pDepthStencilState = nullptr;
		pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStatesCreateInfo;
		pipelineCreateInfo.layout = internalPipelineLayout;
		pipelineCreateInfo.renderPass = internalRenderPass;
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;
		VkResult successResult = vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &internalPipeline); // TODO: Fix sudden crash
		if (!CHECK_VK_RESULT(successResult))
			RE_FATAL_ERROR("Failed creating a graphics render pipeline in Vulkan");
		delete[] shaderStages;
		delete[] dynamicStates;
		return successResult == VK_SUCCESS;
	}

	bool RenderSystem::createFramebuffers() {
		internalFramebuffers = new VkFramebuffer[internalSwapchainImageCount];
		for (uint32_t i = 0; i < internalSwapchainImageCount; i++) {
			VkFramebufferCreateInfo framebufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferCreateInfo.renderPass = internalRenderPass;
			framebufferCreateInfo.attachmentCount = 1;
			framebufferCreateInfo.pAttachments = &internalSwapchainImageViews[i];
			framebufferCreateInfo.width = internalSwapchainImageSize.width;
			framebufferCreateInfo.height = internalSwapchainImageSize.height;
			framebufferCreateInfo.layers = 1;
			if (!CHECK_VK_RESULT(vkCreateFramebuffer(vkDevice, &framebufferCreateInfo, nullptr, &internalFramebuffers[i]))) {
				RE_FATAL_ERROR(appendStrings("Failed creating a framebuffer at index ", i, " in Vulkan"));
				return false;
			}
		}
		return true;
	}

	bool RenderSystem::createCommands() {
		VkCommandPoolCreateInfo cmdPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		cmdPoolCreateInfo.queueFamilyIndex = vkQueueIndices.graphicsFamily;
		if (!CHECK_VK_RESULT(vkCreateCommandPool(vkDevice, &cmdPoolCreateInfo, nullptr, &internalCmdPool))) {
			RE_FATAL_ERROR("Failed creating a command pool in Vulkan");
			return false;
		}

		VkCommandBufferAllocateInfo cmdBufferAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		cmdBufferAllocInfo.commandPool = internalCmdPool;
		cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufferAllocInfo.commandBufferCount = 1;
		if (!CHECK_VK_RESULT(vkAllocateCommandBuffers(vkDevice, &cmdBufferAllocInfo, &internalCmdBuffer))) {
			RE_FATAL_ERROR("Failed allocating a command buffer in Vulkan");
			return false;
		}
		return true;
	}

	bool RenderSystem::createSync() {
		VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		if (!CHECK_VK_RESULT(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &internalImgAvailableSemaphore)) || !CHECK_VK_RESULT(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &internalRenderFinishedSemaphore)) || !CHECK_VK_RESULT(vkCreateFence(vkDevice, &fenceCreateInfo, nullptr, &internalFence))) {
			RE_FATAL_ERROR("Failed creating semaphores/fence for synchronization with rendering in Vulkan");
			return false;
		}
		return true;
	}

	bool RenderSystem::recordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t imgIndex) {
		VkCommandBufferBeginInfo cmdBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		cmdBufferBeginInfo.flags = 0;
		cmdBufferBeginInfo.pInheritanceInfo = nullptr;
		if (!CHECK_VK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo))) {
			RE_FATAL_ERROR("Failed beginning to record the command buffer in Vulkan");
			return false;
		}
		VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassBeginInfo.renderPass = internalRenderPass;
		renderPassBeginInfo.framebuffer = internalFramebuffers[imgIndex];
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = internalSwapchainImageSize;
		VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, internalPipeline);
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(internalSwapchainImageSize.width);
		viewport.height = static_cast<float>(internalSwapchainImageSize.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = internalSwapchainImageSize;
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
		vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
		vkCmdEndRenderPass(cmdBuffer);
		if (!CHECK_VK_RESULT(vkEndCommandBuffer(cmdBuffer))) {
			RE_FATAL_ERROR("Failed stopping to record the command buffer in Vulkan");
			return false;
		}
		return true;
	}

	void RenderSystem::drawFrame() {
		vkWaitForFences(vkDevice, 1, &internalFence, VK_TRUE, UINT64_MAX);
		vkResetFences(vkDevice, 1, &internalFence);
		uint32_t imgIndex = 0;
		vkAcquireNextImageKHR(vkDevice, internalSwapchain, UINT64_MAX, internalImgAvailableSemaphore, VK_NULL_HANDLE, &imgIndex);
		vkResetCommandBuffer(internalCmdBuffer, 0);
		recordCommandBuffer(internalCmdBuffer, imgIndex);

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &internalImgAvailableSemaphore;
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &internalCmdBuffer;
		VkSemaphore semaphores[] = {internalRenderFinishedSemaphore};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = semaphores;
		if (!CHECK_VK_RESULT(vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, internalFence))) {
			RE_FATAL_ERROR("Failed submitting data to graphics queue in Vulkan");
			return;
		}
		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = semaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &internalSwapchain;
		presentInfo.pImageIndices = &imgIndex;
		presentInfo.pResults = nullptr;
		vkQueuePresentKHR(vkPresentQueue, &presentInfo);
		vkDeviceWaitIdle(vkDevice);
	}

	void RenderSystem::windowResize(Vector<REushort, 2> newSize) {

	}

	bool RenderSystem::isValid() {
		return valid;
	}

}