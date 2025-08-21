#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Render System.hpp"

namespace RE {

	static std::vector<uint32_t> get_queue_family_indices(const uint32_t u32QueueCount, const uint32_t *const pau32Queues) {
		std::vector<uint32_t> queueFamilies;
		for (uint32_t u32QueueIndex = 0U; u32QueueIndex < u32QueueCount; u32QueueIndex++)
			if (pau32Queues[u32QueueIndex] != VK_QUEUE_FAMILY_IGNORED && std::find(queueFamilies.begin(), queueFamilies.end(), au32DeviceQueueFamilyIndices[pau32Queues[u32QueueIndex]]) == queueFamilies.end())
				queueFamilies.push_back(au32DeviceQueueFamilyIndices[pau32Queues[u32QueueIndex]]);
		return queueFamilies;
	}

	static bool alloc_required_memory(const VkMemoryRequirements vk_memoryRequirements, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkDeviceMemory *const vk_phMemory) {
		uint32_t u32MemoryTypeIndex = VK_MAX_MEMORY_TYPES;
		for (uint32_t u32PhysicalDeviceMemoryTypeIndex = 0U; u32PhysicalDeviceMemoryTypeIndex < vk_physicalDeviceMemoryProperties.memoryTypeCount; u32PhysicalDeviceMemoryTypeIndex++)
			if ((vk_memoryRequirements.memoryTypeBits & (1U << u32PhysicalDeviceMemoryTypeIndex)) && (vk_physicalDeviceMemoryProperties.memoryTypes[u32PhysicalDeviceMemoryTypeIndex].propertyFlags & vk_eMemoryPropertyFlags) == vk_eMemoryPropertyFlags) {
				u32MemoryTypeIndex = u32PhysicalDeviceMemoryTypeIndex;
				break;
			}
		if (u32MemoryTypeIndex == VK_MAX_MEMORY_TYPES) {
			RE_ERROR("Failed to find required memory type");
			return false;
		}
		const VkMemoryAllocateInfo vk_allocInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = vk_memoryRequirements.size,
			.memoryTypeIndex = u32MemoryTypeIndex
		};
		return vkAllocateMemory(vk_hDevice, &vk_allocInfo, nullptr, vk_phMemory) == VK_SUCCESS;
	}

	bool create_vulkan_shader_from_file(const char *const pacPathToFile, VkShaderModule *const vk_phShader) {
		std::ifstream shaderBinaryFile(pacPathToFile, std::ios::ate | std::ios::binary);
		if (!shaderBinaryFile.is_open()) {
			RE_ERROR("Failed loading SPIR-V shader binaries from file \"", pacPathToFile, "\"");
			return false;
		}
		size_t shaderBinaryFileSize = static_cast<size_t>(shaderBinaryFile.tellg());
		char *const pacShaderBinary = new char[shaderBinaryFileSize];
		PUSH_TO_CALLSTACKTRACE(shaderBinaryFile.seekg(0));
		PUSH_TO_CALLSTACKTRACE(shaderBinaryFile.read(pacShaderBinary, shaderBinaryFileSize));
		PUSH_TO_CALLSTACKTRACE(shaderBinaryFile.close());
		const VkShaderModuleCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = shaderBinaryFileSize,
			.pCode = reinterpret_cast<const uint32_t*>(pacShaderBinary)
		};
		const bool bSuccess = vkCreateShaderModule(vk_hDevice, &vk_createInfo, nullptr, vk_phShader) == VK_SUCCESS;
		delete[] pacShaderBinary;
		if (!bSuccess)
			RE_ERROR("Failed creating Vulkan shader from file \"", pacPathToFile, "\"");
		return bSuccess;
	}
	
	bool create_vulkan_buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *const pau32Queues, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkBuffer *const vk_phBuffer, VkDeviceMemory *const vk_phMemory) {
		VkBufferCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = vk_size,
			.usage = vk_eUsages
		};
		std::vector<uint32_t> queueFamilyIndices = get_queue_family_indices(u32QueueCount, pau32Queues);
		if (queueFamilyIndices.size() == 1U)
			vk_createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else {
			vk_createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
			vk_createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
		}
		if (vkCreateBuffer(vk_hDevice, &vk_createInfo, nullptr, vk_phBuffer) == VK_SUCCESS) {
			VkMemoryRequirements vk_memoryRequirements;
			vkGetBufferMemoryRequirements(vk_hDevice, *vk_phBuffer, &vk_memoryRequirements);
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_required_memory(vk_memoryRequirements, vk_eMemoryPropertyFlags, vk_phMemory), bool)) {
				if (vkBindBufferMemory(vk_hDevice, *vk_phBuffer, *vk_phMemory, 0UL) == VK_SUCCESS)
					return true;
				else
					RE_ERROR("Failed to bind memory to Vulkan buffer");
				vkFreeMemory(vk_hDevice, *vk_phMemory, nullptr);
				*vk_phMemory = VK_NULL_HANDLE;
			} else
				RE_ERROR("Failed to allocate memory for Vulkan buffer");
			vkDestroyBuffer(vk_hDevice, *vk_phBuffer, nullptr);
			*vk_phBuffer = VK_NULL_HANDLE;
		} else
			RE_ERROR("Failed to create Vulkan buffer");
		return false;
	}

	VkFormat find_supported_image_format_on_physical_vulkan_device(const VkPhysicalDevice vk_hPhysicalDevice, const uint32_t u32FormatCandidateCount, const VkFormat *const vk_paeFormatCandidates, const VkImageTiling vk_eImgTiling, const VkFormatFeatureFlags vk_eRequiredFeature) {
		for (uint32_t u32FormatCandidateIndex = 0U; u32FormatCandidateIndex < u32FormatCandidateCount; u32FormatCandidateIndex++) {
			VkFormatProperties vk_formatProperties;
			vkGetPhysicalDeviceFormatProperties(vk_hPhysicalDevice, vk_paeFormatCandidates[u32FormatCandidateIndex], &vk_formatProperties);
			switch (vk_eImgTiling) {
				case VK_IMAGE_TILING_OPTIMAL:
					if ((vk_formatProperties.optimalTilingFeatures & vk_eRequiredFeature) == vk_eRequiredFeature)
						return PUSH_TO_CALLSTACKTRACE_AND_RETURN(vk_paeFormatCandidates[u32FormatCandidateIndex], VkFormat);
					break;
				case VK_IMAGE_TILING_LINEAR:
					if ((vk_formatProperties.linearTilingFeatures & vk_eRequiredFeature) == vk_eRequiredFeature)
						return PUSH_TO_CALLSTACKTRACE_AND_RETURN(vk_paeFormatCandidates[u32FormatCandidateIndex], VkFormat);
					break;
				default:
					break;
			}
		}
		return VK_FORMAT_UNDEFINED;
	}

	VkFormat find_supported_image_format(const uint32_t u32FormatCandidateCount, const VkFormat *const vk_paeFormatCandidates, const VkImageTiling vk_eImgTiling, const VkFormatFeatureFlags vk_eRequiredFeature) {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(find_supported_image_format_on_physical_vulkan_device(vk_hPhysicalDeviceSelected, u32FormatCandidateCount, vk_paeFormatCandidates, vk_eImgTiling, vk_eRequiredFeature), VkFormat);
	}

	bool create_vulkan_image(const VkImageCreateFlags vk_eCreateFlags, const VkImageType vk_eType, const VkFormat vk_eFormat, const VkExtent3D vk_extent, const uint32_t u32MipLevels, const uint32_t u32ArrayLayerCount, const VkSampleCountFlagBits vk_eSamples, const VkImageTiling vk_eTiling, const VkImageUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *const pau32Queues, const VkImageLayout vk_eLayout, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkImage *const vk_phImage, VkDeviceMemory *const vk_phMemory) {
		VkImageCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.imageType = vk_eType,
			.format = vk_eFormat,
			.extent = vk_extent,
			.mipLevels = u32MipLevels,
			.arrayLayers = u32ArrayLayerCount,
			.samples = vk_eSamples,
			.tiling = vk_eTiling,
			.usage = vk_eUsages,
			.initialLayout = vk_eLayout
		};
		std::vector<uint32_t> queueFamilies = get_queue_family_indices(u32QueueCount, pau32Queues);
		if (queueFamilies.size() == 1U)
			vk_createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else {
			vk_createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_createInfo.queueFamilyIndexCount = queueFamilies.size();
			vk_createInfo.pQueueFamilyIndices = queueFamilies.data();
		}
		if (vkCreateImage(vk_hDevice, &vk_createInfo, nullptr, vk_phImage) == VK_SUCCESS) {
			VkMemoryRequirements vk_memoryRequirements;
			vkGetImageMemoryRequirements(vk_hDevice, *vk_phImage, &vk_memoryRequirements);
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_required_memory(vk_memoryRequirements, vk_eMemoryPropertyFlags, vk_phMemory), bool)) {
				if (vkBindImageMemory(vk_hDevice, *vk_phImage, *vk_phMemory, 0UL) == VK_SUCCESS)
					return true;
				else
					RE_ERROR("Failed to bind memory to Vulkan image");
				vkFreeMemory(vk_hDevice, *vk_phMemory, nullptr);
				*vk_phMemory = VK_NULL_HANDLE;
			} else
				RE_ERROR("Failed to allocate memory for Vulkan image");
			vkDestroyImage(vk_hDevice, *vk_phImage, nullptr);
			*vk_phImage = VK_NULL_HANDLE;
		} else
			RE_ERROR("Failed to create Vulkan image");
		return false;
	}

	bool create_vulkan_image_view(const VkImage vk_hImage, const VkImageViewType vk_eType, const VkFormat vk_eFormat, const VkImageAspectFlags vk_eImageAspects, const uint32_t u32BaseMipLevel, const uint32_t u32MipLevelCount, const uint32_t u32BaseArrayLayer, const uint32_t u32ArrayLayerCount, VkImageView *const vk_phImageView) {
		VkImageViewCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = vk_hImage,
			.viewType = vk_eType,
			.format = vk_eFormat,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = {
				.aspectMask = vk_eImageAspects,
				.baseMipLevel = u32BaseMipLevel,
				.levelCount = u32MipLevelCount,
				.baseArrayLayer = u32BaseArrayLayer,
				.layerCount = u32ArrayLayerCount
			}
		};
		const bool bSuccess = vkCreateImageView(vk_hDevice, &vk_createInfo, nullptr, vk_phImageView) == VK_SUCCESS;
		if (!bSuccess)
			RE_ERROR("Failed to create Vulkan image view");
		return bSuccess;
	}

	bool create_vulkan_framebuffer(const VkFramebufferCreateFlags vk_eCreateFlags, const VkRenderPass vk_hRenderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *const vk_pahImageViews, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Layer, VkFramebuffer *const vk_phFramebuffer) {
		const VkFramebufferCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.renderPass = vk_hRenderPass,
			.attachmentCount = u32ImageViewAttachmentCount,
			.pAttachments = vk_pahImageViews,
			.width = u32Width,
			.height = u32Height,
			.layers = u32Layer
		};
		const bool bSuccess = vkCreateFramebuffer(vk_hDevice, &vk_createInfo, nullptr, vk_phFramebuffer) == VK_SUCCESS;
		if (!bSuccess)
			RE_ERROR("Failed to create Vulkan framebuffer");
		return bSuccess;
	}

	bool create_pipeline_layout(const uint32_t u32DescriptorSetLayoutCount, const VkDescriptorSetLayout *const vk_pahDescriptorSetLayouts, const uint32_t u32PushConstRangeCount, const VkPushConstantRange *const vk_pahPushConstRanges, const VkPipelineLayoutCreateFlags vk_eCreateFlags, VkPipelineLayout *const vk_phPipelineLayout) {
		const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.flags = 0,
			.setLayoutCount = u32DescriptorSetLayoutCount,
			.pSetLayouts = vk_pahDescriptorSetLayouts,
			.pushConstantRangeCount = u32PushConstRangeCount,
			.pPushConstantRanges = vk_pahPushConstRanges
		};
		return vkCreatePipelineLayout(vk_hDevice, &vk_pipelineLayoutCreateInfo, nullptr, vk_phPipelineLayout) == VK_SUCCESS;
	}

	bool create_descriptor_set_layout(const uint32_t u32DescriptorSetLayoutBindingCount, const VkDescriptorSetLayoutBinding *const vk_paDescriptorSetLayoutBindings, const VkDescriptorSetLayoutCreateFlags vk_eCreateFlags, VkDescriptorSetLayout *const vk_phDescriptorSetLayout) {
		const VkDescriptorSetLayoutCreateInfo vk_descriptorSetLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.bindingCount = u32DescriptorSetLayoutBindingCount,
			.pBindings = vk_paDescriptorSetLayoutBindings
		};
		return vkCreateDescriptorSetLayout(vk_hDevice, &vk_descriptorSetLayoutCreateInfo, nullptr, vk_phDescriptorSetLayout) == VK_SUCCESS;
	}

	bool create_descriptor_pool(const uint32_t u32PoolSizeCounts, const VkDescriptorPoolSize *const vk_paDescriptorPoolSizes, const uint32_t u32MaxSets, VkDescriptorPool *const vk_phDescriptorPool) {
		const VkDescriptorPoolCreateInfo vk_descriptorPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = u32MaxSets,
			.poolSizeCount = u32PoolSizeCounts,
			.pPoolSizes = vk_paDescriptorPoolSizes
		};
		return vkCreateDescriptorPool(vk_hDevice, &vk_descriptorPoolCreateInfo, nullptr, vk_phDescriptorPool) == VK_SUCCESS;
	}

	bool alloc_descriptor_set(const VkDescriptorPool vk_hDescriptorPool, const uint32_t u32SetCount, const VkDescriptorSetLayout *const vk_pahDescriptorSetLayouts, VkDescriptorSet *const vk_pahDescriptorSet) {
		const VkDescriptorSetAllocateInfo vk_descriptorSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hDescriptorPool,
			.descriptorSetCount = u32SetCount,
			.pSetLayouts = vk_pahDescriptorSetLayouts
		};
		return vkAllocateDescriptorSets(vk_hDevice, &vk_descriptorSetAllocInfo, vk_pahDescriptorSet) == VK_SUCCESS;
	}

	bool alloc_vulkan_command_buffers(const VkCommandPool vk_hCommandPool, const VkCommandBufferLevel vk_eLevel, const uint32_t u32Count, VkCommandBuffer *const vk_pahCommandBuffer) {
		const VkCommandBufferAllocateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = vk_hCommandPool,
			.level = vk_eLevel,
			.commandBufferCount = u32Count
		};
		return vkAllocateCommandBuffers(vk_hDevice, &vk_createInfo, vk_pahCommandBuffer) == VK_SUCCESS;
	}

	bool begin_recording_vulkan_command_buffer(const VkCommandBuffer vk_hCommandBuffer, const VkCommandBufferUsageFlags vk_eUsages, const VkCommandBufferInheritanceInfo *const vk_pInheritanceInfo) {
		const VkCommandBufferBeginInfo vk_beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = vk_eUsages,
			.pInheritanceInfo = vk_pInheritanceInfo
		};
		return vkBeginCommandBuffer(vk_hCommandBuffer, &vk_beginInfo) == VK_SUCCESS;
	}

	bool submit_to_vulkan_queue(const VkQueue vk_hQueue, const uint32_t u32WaitSemaphoreCount, const VkSemaphore *const vk_pahWaitSemaphores, const VkPipelineStageFlags *const vk_pahWaitOnPipelineStages, const uint32_t u32CommandBufferCount, const VkCommandBuffer *const vk_pahCommandBuffers, const uint32_t u32SignalSemaphoreCount, const VkSemaphore *const vk_pahSignalSemaphores, const VkFence vk_hFence) {
		const VkSubmitInfo vk_queueSubmitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = u32WaitSemaphoreCount,
			.pWaitSemaphores = vk_pahWaitSemaphores,
			.pWaitDstStageMask = vk_pahWaitOnPipelineStages,
			.commandBufferCount = u32CommandBufferCount,
			.pCommandBuffers = vk_pahCommandBuffers,
			.signalSemaphoreCount = u32SignalSemaphoreCount,
			.pSignalSemaphores = vk_pahSignalSemaphores
		};
		const bool bSuccess = vkQueueSubmit(vk_hQueue, 1U, &vk_queueSubmitInfo, vk_hFence) == VK_SUCCESS;
		if (!bSuccess)
			RE_ERROR("Failed submitting a task to a Vulkan queue");
		return bSuccess;
	}

	bool signal_vulkan_semaphores(const uint32_t u32SemaphoreCount, const VkSemaphore *const vk_pahSemaphores) {
		const bool bSuccess = submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 0U, nullptr, nullptr, 1U, &vk_hDummyTransferCommandBuffer, u32SemaphoreCount, vk_pahSemaphores, VK_NULL_HANDLE);
		if (!bSuccess)
			RE_ERROR("Failed signaling semaphores");
		return bSuccess;
	}

	bool signal_vulkan_fences(const uint32_t u32FenceCount, const VkFence *const vk_pahFences) {
		for (uint32_t u32FenceIndex = 0U; u32FenceIndex < u32FenceCount; u32FenceIndex++)
			if (!submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 0U, nullptr, nullptr, 1U, &vk_hDummyTransferCommandBuffer, 0U, nullptr, vk_pahFences[u32FenceIndex])) {
				RE_ERROR("Failed signaling fence at index ", u32FenceIndex);
				return false;
			}
		return true;
	}

	void vk_cmd_transit_image(const VkCommandBuffer vk_hCommandBuffer, const VkPipelineStageFlags vk_eSrcStageFlags, const VkPipelineStageFlags vk_eDstStageFlags, const VkDependencyFlags vk_eDependencyFlags, const VkAccessFlags vk_eSrcAccessFlags, const VkAccessFlags vk_eDstAccessFlags, const VkImageLayout vk_eOldLayout, const VkImageLayout vk_eNewLayout, const uint32_t u32SrcQueueIndex, const uint32_t u32DstQueueIndex, const VkImage vk_hImage, const VkImageAspectFlags vk_eAspectFlags, const uint32_t u32BaseMipLevel, const uint32_t u32MipLevelCount, const uint32_t u32BaseArrayLayer, const uint32_t u32ArrayLayerCount) {
		std::vector<uint32_t> srcQueues = get_queue_family_indices(1U, &u32SrcQueueIndex), dstQueues = get_queue_family_indices(1U, &u32DstQueueIndex);
		const VkImageMemoryBarrier vk_imageTransit = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = vk_eSrcAccessFlags,
			.dstAccessMask = vk_eDstAccessFlags,
			.oldLayout = vk_eOldLayout,
			.newLayout = vk_eNewLayout,
			.srcQueueFamilyIndex = u32SrcQueueIndex != VK_QUEUE_FAMILY_IGNORED ? *srcQueues.begin() : VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = u32DstQueueIndex != VK_QUEUE_FAMILY_IGNORED ? *dstQueues.begin() : VK_QUEUE_FAMILY_IGNORED,
			.image = vk_hImage,
			.subresourceRange = {
				.aspectMask = vk_eAspectFlags,
				.baseMipLevel = u32BaseMipLevel,
				.levelCount = u32MipLevelCount,
				.baseArrayLayer = u32BaseArrayLayer,
				.layerCount = u32ArrayLayerCount
			}
		};
		vkCmdPipelineBarrier(vk_hCommandBuffer, vk_eSrcStageFlags, vk_eDstStageFlags, vk_eDependencyFlags, 0U, nullptr, 0U, nullptr, 1U, &vk_imageTransit);
	}

	bool transit_image(const uint32_t u32QueueIndex, const VkPipelineStageFlags vk_eSrcStageFlags, const VkPipelineStageFlags vk_eDstStageFlags, const VkDependencyFlags vk_eDependencyFlags, const VkAccessFlags vk_eSrcAccessFlags, const VkAccessFlags vk_eDstAccessFlags, const VkImageLayout vk_eOldLayout, const VkImageLayout vk_eNewLayout, const uint32_t u32SrcQueueIndex, const uint32_t u32DstQueueIndex, const VkImage vk_hImage, const VkImageAspectFlags vk_eAspectFlags, const uint32_t u32BaseMipLevel, const uint32_t u32MipLevelCount, const uint32_t u32BaseArrayLayer, const uint32_t u32ArrayLayerCount, VkCommandBuffer *vk_phCommandBufferToFree, VkFence *vk_phFence) {
		if (!vk_phCommandBufferToFree) {
			RE_ERROR("No pointer to command buffer attribute has been provided");
			return false;
		}
		if (vk_phFence) {
			const VkFenceCreateInfo vk_fenceCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.flags = 0
			};
			if (vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, vk_phFence) != VK_SUCCESS)
				*vk_phFence = VK_NULL_HANDLE;
		}
		uint32_t u32CommandPoolIndex;
		switch (u32QueueIndex) {
			case RE_VK_QUEUE_GRAPHICS_INDEX:
				u32CommandPoolIndex = RE_VK_COMMAND_POOL_GRAPHICS_TRANSIENT_INDEX;
				break;
			case RE_VK_QUEUE_TRANSFER_INDEX:
				u32CommandPoolIndex = RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX;
				break;
			default:
				RE_ERROR("Failed to find the command pool for the queue. The queue's index is ", u32QueueIndex);
				if (vk_phFence && *vk_phFence) {
					vkDestroyFence(vk_hDevice, *vk_phFence, nullptr);
					*vk_phFence = VK_NULL_HANDLE;
				}
				return false;
		}
		if (alloc_vulkan_command_buffers(vk_ahCommandPools[u32CommandPoolIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1U, vk_phCommandBufferToFree)) {
			if (begin_recording_vulkan_command_buffer(*vk_phCommandBufferToFree, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
				vk_cmd_transit_image(*vk_phCommandBufferToFree, vk_eSrcStageFlags, vk_eDstStageFlags, vk_eDependencyFlags, vk_eSrcAccessFlags, vk_eDstAccessFlags, vk_eOldLayout, vk_eNewLayout, u32SrcQueueIndex, u32DstQueueIndex, vk_hImage, vk_eAspectFlags, u32BaseMipLevel, u32MipLevelCount, u32BaseArrayLayer, u32ArrayLayerCount);
				if (vkEndCommandBuffer(*vk_phCommandBufferToFree) == VK_SUCCESS) {
					if (vk_phFence && *vk_phFence) {
						if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[u32QueueIndex], 0U, nullptr, nullptr, 1U, vk_phCommandBufferToFree, 0U, nullptr, *vk_phFence), bool))
							return true;
						else
							RE_ERROR("Failed to submit Vulkan command buffer to transit image and signal a fence");
					} else {
						if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[u32QueueIndex], 0U, nullptr, nullptr, 1U, vk_phCommandBufferToFree, 0U, nullptr, VK_NULL_HANDLE), bool)) {
							vkQueueWaitIdle(vk_ahDeviceQueueFamilies[u32QueueIndex]);
							vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[u32CommandPoolIndex], 1U, vk_phCommandBufferToFree);
							*vk_phCommandBufferToFree = VK_NULL_HANDLE;
							return true;
						} else
							RE_ERROR("Failed to submit Vulkan command buffer to transit image without signaling a fence");
					}
				} else
					RE_ERROR("Failed to finish recording Vulkan command buffer for transitting an image");
			} else
				RE_ERROR("Failed to begin recording Vulkan command buffer for transitting an image");
			vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[u32CommandPoolIndex], 1U, vk_phCommandBufferToFree);
			*vk_phCommandBufferToFree = VK_NULL_HANDLE;
		}
		return false;
	}

}
