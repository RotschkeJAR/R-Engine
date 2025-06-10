#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Render System.hpp"

namespace RE {

	static std::vector<uint32_t> get_queue_family_indices(const uint32_t u32QueueCount, const uint32_t *pau32Queues) {
		std::vector<uint32_t> queueFamilies;
		for (uint32_t u32QueueIndex = 0U; u32QueueIndex < u32QueueCount; u32QueueIndex++)
			if (std::find(queueFamilies.begin(), queueFamilies.end(), u32DeviceQueueFamilyIndices[pau32Queues[u32QueueIndex]]) == queueFamilies.end())
				queueFamilies.push_back(u32DeviceQueueFamilyIndices[pau32Queues[u32QueueIndex]]);
		return queueFamilies;
	}

	static bool alloc_required_memory(const VkMemoryRequirements vk_memoryRequirements, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkDeviceMemory *vk_phMemory) {
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

	bool create_vulkan_shader_from_file(const char *pcPathToFile, VkShaderModule *vk_phShader) {
		std::ifstream shaderBinaryFile(pcPathToFile, std::ios::ate | std::ios::binary);
		if (!shaderBinaryFile.is_open()) {
			RE_ERROR(append_to_string("Failed loading SPIR-V shader binaries from file \"", pcPathToFile, "\""));
			return false;
		}
		size_t shaderBinaryFileSize = static_cast<size_t>(shaderBinaryFile.tellg());
		char *const pacShaderBinary = new char[shaderBinaryFileSize];
		CATCH_SIGNAL(shaderBinaryFile.seekg(0));
		CATCH_SIGNAL(shaderBinaryFile.read(pacShaderBinary, shaderBinaryFileSize));
		CATCH_SIGNAL(shaderBinaryFile.close());
		const VkShaderModuleCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = shaderBinaryFileSize,
			.pCode = reinterpret_cast<const uint32_t*>(pacShaderBinary)
		};
		const bool bSuccess = vkCreateShaderModule(vk_hDevice, &vk_createInfo, nullptr, vk_phShader) == VK_SUCCESS;
		delete[] pacShaderBinary;
		if (!bSuccess)
			RE_ERROR(append_to_string("Failed creating Vulkan shader from file \"", pcPathToFile, "\""));
		return bSuccess;
	}
	
	bool create_vulkan_buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *pau32Queues, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkBuffer *vk_phBuffer, VkDeviceMemory *vk_phMemory) {
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
			if (CATCH_SIGNAL_AND_RETURN(alloc_required_memory(vk_memoryRequirements, vk_eMemoryPropertyFlags, vk_phMemory), bool)) {
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

	bool create_vulkan_image(const VkImageCreateFlags vk_eCreateFlags, const VkImageType vk_eType, const VkFormat vk_eFormat, const VkExtent3D vk_extent, const uint32_t u32MipLevels, const uint32_t u32ArrayLayerCount, const VkSampleCountFlagBits vk_eSamples, const VkImageTiling vk_eTiling, const VkImageUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *pau32Queues, const VkImageLayout vk_eLayout, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkImage *vk_phImage, VkDeviceMemory *vk_phMemory) {
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
			if (CATCH_SIGNAL_AND_RETURN(alloc_required_memory(vk_memoryRequirements, vk_eMemoryPropertyFlags, vk_phMemory), bool)) {
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

	bool create_vulkan_image_view(const VkImage vk_hImage, const VkImageViewType vk_eType, const VkFormat vk_eFormat, const VkImageAspectFlags vk_eImageAspects, const uint32_t u32BaseMipLevel, const uint32_t u32MipLevelCount, const uint32_t u32BaseArrayLayer, const uint32_t u32ArrayLayerCount, VkImageView *vk_phImageView) {
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

	bool create_vulkan_framebuffer(const VkFramebufferCreateFlags vk_eCreateFlags, const VkRenderPass vk_hRenderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *vk_pahImageViews, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Layer, VkFramebuffer *vk_phFramebuffer) {
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

	bool alloc_vulkan_command_buffers(const VkCommandPool vk_hCommandPool, const VkCommandBufferLevel vk_eLevel, const uint32_t u32Count, VkCommandBuffer *vk_pahCommandBuffer) {
		const VkCommandBufferAllocateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = vk_hCommandPool,
			.level = vk_eLevel,
			.commandBufferCount = u32Count
		};
		return vkAllocateCommandBuffers(vk_hDevice, &vk_createInfo, vk_pahCommandBuffer) == VK_SUCCESS;
	}

	bool begin_recording_vulkan_command_buffer(const VkCommandBuffer vk_hCommandBuffer, const VkCommandBufferUsageFlags vk_eUsages, const VkCommandBufferInheritanceInfo *vk_pInheritanceInfo) {
		const VkCommandBufferBeginInfo vk_beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = vk_eUsages,
			.pInheritanceInfo = vk_pInheritanceInfo
		};
		return vkBeginCommandBuffer(vk_hCommandBuffer, &vk_beginInfo) == VK_SUCCESS;
	}

	bool submit_to_vulkan_queue(const VkQueue vk_hQueue, const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_pahWaitSemaphores, const VkPipelineStageFlags *vk_pahWaitOnPipelineStages, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_pahCommandBuffers, const uint32_t u32SignalSemaphoreCount, const VkSemaphore *vk_pahSignalSemaphores, const VkFence vk_hFence) {
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

}
