#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_GPU.hpp"

namespace RE {

	static bool alloc_required_memory(VkMemoryRequirements *const vk_pMemoryRequirements, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkDeviceMemory *const vk_phMemory) {
		uint32_t u32MemoryTypeIndex = VK_MAX_MEMORY_TYPES + 1;
		for (uint32_t u32PhysicalDeviceMemoryTypeIndex = 0; u32PhysicalDeviceMemoryTypeIndex < vk_physicalDeviceMemoryProperties.memoryTypeCount; u32PhysicalDeviceMemoryTypeIndex++) {
			PRINT_DEBUG("Checking Vulkan memory type at index ", u32PhysicalDeviceMemoryTypeIndex, " whether it fits the requirements");
			if ((vk_pMemoryRequirements->memoryTypeBits & (1U << u32PhysicalDeviceMemoryTypeIndex)) && (vk_physicalDeviceMemoryProperties.memoryTypes[u32PhysicalDeviceMemoryTypeIndex].propertyFlags & vk_eMemoryPropertyFlags) == vk_eMemoryPropertyFlags) {
				u32MemoryTypeIndex = u32PhysicalDeviceMemoryTypeIndex;
				PRINT_DEBUG("Vulkan memory type at index ", u32MemoryTypeIndex, " fullfills requirements");
				break;
			}
		}
		if (u32MemoryTypeIndex == VK_MAX_MEMORY_TYPES + 1) {
			RE_ERROR("Failed to find required memory type");
			return false;
		}
		PRINT_DEBUG("Allocating the required memory");
		const VkMemoryAllocateInfo vk_allocInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = vk_pMemoryRequirements->size,
			.memoryTypeIndex = u32MemoryTypeIndex
		};
		return vkAllocateMemory(vk_hDevice, &vk_allocInfo, nullptr, vk_phMemory) == VK_SUCCESS;
	}

	bool create_vulkan_shader_from_file(const char *const pacPathToFile, VkShaderModule *const vk_phShader) {
		PRINT_DEBUG("Loading SPIR-V shader binary file from \"", pacPathToFile, "\"");
		std::ifstream shaderBinaryFile(pacPathToFile, std::ios::ate | std::ios::binary);
		if (!shaderBinaryFile.is_open()) {
			RE_ERROR("Failed loading SPIR-V shader binaries from file \"", pacPathToFile, "\"");
			return false;
		}
		const size_t shaderBinaryFileSize = static_cast<size_t>(shaderBinaryFile.tellg());
		std::vector<char> shaderBinary(shaderBinaryFileSize);
		PRINT_DEBUG("Copying ", shaderBinaryFileSize, " bytes from loaded shader file");
		shaderBinaryFile.seekg(0);
		shaderBinaryFile.read(shaderBinary.data(), shaderBinaryFileSize);
		shaderBinaryFile.close();
		PRINT_DEBUG("Creating Vulkan shader module");
		const VkShaderModuleCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = shaderBinaryFileSize,
			.pCode = reinterpret_cast<const uint32_t*>(shaderBinary.data())
		};
		PRINT(pacPathToFile, " (", shaderBinaryFileSize / 4, "): ", vk_createInfo.pCode[0]);
		for (size_t i = 1; i < shaderBinaryFileSize / 4; i++)
			print(", ", vk_createInfo.pCode[i]);
		println();
		return vkCreateShaderModule(vk_hDevice, &vk_createInfo, nullptr, vk_phShader) == VK_SUCCESS;
	}
	
	bool create_vulkan_buffer(const VkBufferCreateFlags vk_eCreateFlags, const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueFamilyCount, const uint32_t *const pau32QueueFamilies, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkBuffer *const vk_phBuffer, VkDeviceMemory *const vk_phMemory) {
		PRINT_DEBUG("Creating Vulkan buffer");
		VkBufferCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.size = vk_size,
			.usage = vk_eUsages
		};
		if (u32QueueFamilyCount == 1)
			vk_createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else {
			vk_createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_createInfo.queueFamilyIndexCount = u32QueueFamilyCount;
			vk_createInfo.pQueueFamilyIndices = pau32QueueFamilies;
		}
		if (vkCreateBuffer(vk_hDevice, &vk_createInfo, nullptr, vk_phBuffer) == VK_SUCCESS) {
			PRINT_DEBUG("Fetching requirements for allocating memory for recently created Vulkan buffer ", vk_phBuffer);
			const VkBufferMemoryRequirementsInfo2 vk_bufferInfo = {
				.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
				.buffer = *vk_phBuffer
			};
			VkMemoryRequirements2 vk_memoryRequirements;
			vk_memoryRequirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
			vk_memoryRequirements.pNext = nullptr;
			vkGetBufferMemoryRequirements2(vk_hDevice, &vk_bufferInfo, &vk_memoryRequirements);
			if (alloc_required_memory(&vk_memoryRequirements.memoryRequirements, vk_eMemoryPropertyFlags, vk_phMemory)) {
				PRINT_DEBUG("Binding allocated memory ", vk_phMemory, " to Vulkan buffer ", vk_phBuffer);
				const VkBindBufferMemoryInfo vk_bindMemoryInfo = {
					.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
					.buffer = *vk_phBuffer,
					.memory = *vk_phMemory
				};
				if (vkBindBufferMemory2(vk_hDevice, 1, &vk_bindMemoryInfo) == VK_SUCCESS)
					return true;
				else
					RE_ERROR("Failed to bind memory to Vulkan buffer");
				PRINT_DEBUG("Freeing memory ", vk_phMemory);
				vkFreeMemory(vk_hDevice, *vk_phMemory, nullptr);
				*vk_phMemory = VK_NULL_HANDLE;
			} else
				RE_ERROR("Failed to allocate memory for Vulkan buffer");
			PRINT_DEBUG("Destroying Vulkan buffer ", vk_phBuffer);
			vkDestroyBuffer(vk_hDevice, *vk_phBuffer, nullptr);
			*vk_phBuffer = VK_NULL_HANDLE;
		} else
			RE_ERROR("Failed to create Vulkan buffer");
		return false;
	}

	bool create_vulkan_image(const VkImageCreateFlags vk_eCreateFlags, const VkImageType vk_eType, const VkFormat vk_eFormat, const VkExtent3D vk_extent, const uint32_t u32MipLevels, const uint32_t u32ArrayLayerCount, const VkSampleCountFlagBits vk_eSamples, const VkImageTiling vk_eTiling, const VkImageUsageFlags vk_eUsages, const uint32_t u32QueueFamilyCount, const uint32_t *const pau32QueueFamilies, const VkImageLayout vk_eLayout, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkImage *const vk_phImage, VkDeviceMemory *const vk_phMemory) {
		PRINT_DEBUG("Creating Vulkan image");
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
		if (u32QueueFamilyCount == 1)
			vk_createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else {
			vk_createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_createInfo.queueFamilyIndexCount = u32QueueFamilyCount;
			vk_createInfo.pQueueFamilyIndices = pau32QueueFamilies;
		}
		if (vkCreateImage(vk_hDevice, &vk_createInfo, nullptr, vk_phImage) == VK_SUCCESS) {
			PRINT_DEBUG("Fetching memory requirements for recently created Vulkan image ", vk_phImage);
			const VkImageMemoryRequirementsInfo2 vk_imageInfo = {
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
				.image = *vk_phImage
			};
			VkMemoryRequirements2 vk_memoryRequirements;
			vk_memoryRequirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
			vk_memoryRequirements.pNext = nullptr;
			vkGetImageMemoryRequirements2(vk_hDevice, &vk_imageInfo, &vk_memoryRequirements);
			if (alloc_required_memory(&vk_memoryRequirements.memoryRequirements, vk_eMemoryPropertyFlags, vk_phMemory)) {
				PRINT_DEBUG("Binding allocated memory ", vk_phMemory, " to Vulkan image ", vk_phImage);
				const VkBindImageMemoryInfo vk_bindMemoryInfo = {
					.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
					.image = *vk_phImage,
					.memory = *vk_phMemory
				};
				if (vkBindImageMemory2(vk_hDevice, 1, &vk_bindMemoryInfo) == VK_SUCCESS)
					return true;
				else
					RE_ERROR("Failed to bind memory to Vulkan image");
				PRINT_DEBUG("Freeing memory ", vk_phMemory);
				vkFreeMemory(vk_hDevice, *vk_phMemory, nullptr);
				*vk_phMemory = VK_NULL_HANDLE;
			} else
				RE_ERROR("Failed to allocate memory for Vulkan image");
			PRINT_DEBUG("Destroying Vulkan image ", vk_phImage);
			vkDestroyImage(vk_hDevice, *vk_phImage, nullptr);
			*vk_phImage = VK_NULL_HANDLE;
		} else
			RE_ERROR("Failed to create Vulkan image");
		return false;
	}

	bool create_vulkan_image_view(const VkImage vk_hImage, const VkImageViewType vk_eType, const VkFormat vk_eFormat, const VkImageAspectFlags vk_eImageAspects, const uint32_t u32BaseMipLevel, const uint32_t u32MipLevelCount, const uint32_t u32BaseArrayLayer, const uint32_t u32ArrayLayerCount, VkImageView *const vk_phImageView) {
		PRINT_DEBUG("Creating Vulkan image view for image ", vk_hImage);
		const VkImageViewCreateInfo vk_createInfo = {
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
		return vkCreateImageView(vk_hDevice, &vk_createInfo, nullptr, vk_phImageView) == VK_SUCCESS;
	}

	bool create_vulkan_framebuffer(const VkFramebufferCreateFlags vk_eCreateFlags, const VkRenderPass vk_hRenderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *const vk_pahImageViews, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Layer, VkFramebuffer *const vk_phFramebuffer) {
		PRINT_DEBUG("Creating Vulkan framebuffer sized ", u32Width, "x", u32Height, " with ", u32Layer, " layers aiming render pass ", vk_hRenderPass, " for ", u32ImageViewAttachmentCount, " image views");
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
		return vkCreateFramebuffer(vk_hDevice, &vk_createInfo, nullptr, vk_phFramebuffer) == VK_SUCCESS;
	}

	bool create_pipeline_layout(const uint32_t u32DescriptorSetLayoutCount, const VkDescriptorSetLayout *const vk_pahDescriptorSetLayouts, const uint32_t u32PushConstRangeCount, const VkPushConstantRange *const vk_pahPushConstRanges, const VkPipelineLayoutCreateFlags vk_eCreateFlags, VkPipelineLayout *const vk_phPipelineLayout) {
		PRINT_DEBUG("Creating Vulkan pipeline layout with ", u32DescriptorSetLayoutCount, " descriptor set layouts and ", u32PushConstRangeCount, " push constants");
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
		PRINT_DEBUG("Creating Vulkan descriptor set layout with ", u32DescriptorSetLayoutBindingCount, " bindings");
		const VkDescriptorSetLayoutCreateInfo vk_descriptorSetLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.bindingCount = u32DescriptorSetLayoutBindingCount,
			.pBindings = vk_paDescriptorSetLayoutBindings
		};
		return vkCreateDescriptorSetLayout(vk_hDevice, &vk_descriptorSetLayoutCreateInfo, nullptr, vk_phDescriptorSetLayout) == VK_SUCCESS;
	}

	bool create_descriptor_pool(const uint32_t u32PoolSizeCounts, const VkDescriptorPoolSize *const vk_paDescriptorPoolSizes, const uint32_t u32MaxSets, VkDescriptorPool *const vk_phDescriptorPool) {
		PRINT_DEBUG("Creating Vulkan descriptor set pool with ", u32PoolSizeCounts, " pool sizes containing a maximum of ", u32MaxSets, " sets simultaneously");
		const VkDescriptorPoolCreateInfo vk_descriptorPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = u32MaxSets,
			.poolSizeCount = u32PoolSizeCounts,
			.pPoolSizes = vk_paDescriptorPoolSizes
		};
		return vkCreateDescriptorPool(vk_hDevice, &vk_descriptorPoolCreateInfo, nullptr, vk_phDescriptorPool) == VK_SUCCESS;
	}

	bool alloc_descriptor_set(const VkDescriptorPool vk_hDescriptorPool, const uint32_t u32SetCount, const VkDescriptorSetLayout *const vk_pahDescriptorSetLayouts, VkDescriptorSet *const vk_pahDescriptorSet) {
		PRINT_DEBUG("Allocating ", u32SetCount, " descriptor set/-s in pool ", vk_hDescriptorPool);
		const VkDescriptorSetAllocateInfo vk_descriptorSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hDescriptorPool,
			.descriptorSetCount = u32SetCount,
			.pSetLayouts = vk_pahDescriptorSetLayouts
		};
		return vkAllocateDescriptorSets(vk_hDevice, &vk_descriptorSetAllocInfo, vk_pahDescriptorSet) == VK_SUCCESS;
	}

	bool alloc_vulkan_command_buffers(const VkCommandPool vk_hCommandPool, const VkCommandBufferLevel vk_eLevel, const uint32_t u32Count, VkCommandBuffer *const vk_pahCommandBuffer) {
		PRINT_DEBUG("Allocating ", u32Count, " Vulkan command buffer/-s on level ", std::hex, vk_eLevel, std::dec, " in pool ", vk_hCommandPool);
		const VkCommandBufferAllocateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = vk_hCommandPool,
			.level = vk_eLevel,
			.commandBufferCount = u32Count
		};
		return vkAllocateCommandBuffers(vk_hDevice, &vk_createInfo, vk_pahCommandBuffer) == VK_SUCCESS;
	}

	bool begin_recording_vulkan_command_buffer(const VkCommandBuffer vk_hCommandBuffer, const VkCommandBufferUsageFlags vk_eUsages, const VkCommandBufferInheritanceInfo *const vk_pInheritanceInfo) {
		PRINT_DEBUG("Beginning to record Vulkan command buffer ", vk_hCommandBuffer, " with combination of usage flags ", std::hex, vk_eUsages, std::dec, " and inheritance info ", vk_pInheritanceInfo);
		const VkCommandBufferBeginInfo vk_beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = vk_eUsages,
			.pInheritanceInfo = vk_pInheritanceInfo
		};
		return vkBeginCommandBuffer(vk_hCommandBuffer, &vk_beginInfo) == VK_SUCCESS;
	}

}
