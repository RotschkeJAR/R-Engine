#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

	static std::vector<uint32_t> get_queue_indices(const uint32_t u32QueueTypeCount, const uint32_t *pu32QueueTypes) {
		std::vector<uint32_t> queueFamilyIndices;
		if (u32QueueTypeCount > 1U)
			for (uint32_t u32QueueTypeIndex = 0U; u32QueueTypeIndex < u32QueueTypeCount; u32QueueTypeIndex++) {
				const uint32_t u32QueueType = pu32QueueTypes[u32QueueTypeIndex];
				if (std::find(queueFamilyIndices.begin(), queueFamilyIndices.end(), u32DeviceQueueFamilyIndices[u32QueueType]) == queueFamilyIndices.end())
					queueFamilyIndices.push_back(u32DeviceQueueFamilyIndices[u32QueueType]);
			}
		return queueFamilyIndices;
	}
	
	bool __vk_create_buffer(const VkBufferCreateFlags vk_eCreateFlags, const VkDeviceSize vk_bufferSizeInBytes, const VkBufferUsageFlags vk_eBufferUsage, const uint32_t u32QueueTypeCount, const uint32_t *pu32BufferQueueTypeIndices, const VkMemoryPropertyFlags vk_eMemoryProperties, VkBuffer *vk_phBuffer, VkDeviceMemory *vk_phBufferMemory, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		const std::vector<uint32_t> queueFamilies = get_queue_indices(u32QueueTypeCount, pu32BufferQueueTypeIndices);
		VkBufferCreateInfo vk_bufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.size = vk_bufferSizeInBytes,
			.usage = vk_eBufferUsage
		};
		if (queueFamilies.size() > 1U) {
			vk_bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_bufferCreateInfo.queueFamilyIndexCount = queueFamilies.size();
			vk_bufferCreateInfo.pQueueFamilyIndices = queueFamilies.data();
		} else
			vk_bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (!CHECK_VK_RESULT(vkCreateBuffer(vk_hDevice, &vk_bufferCreateInfo, nullptr, vk_phBuffer))) {
			error(pcFile, pcFunc, u32Line, "Failed creating Vulkan buffer", true);
			return false;
		}
		VkMemoryRequirements vk_bufferMemoryRequirements;
		CATCH_SIGNAL(vkGetBufferMemoryRequirements(vk_hDevice, *vk_phBuffer, &vk_bufferMemoryRequirements));
		std::optional<uint32_t> physicalMemoryTypeSelected;
		for (uint32_t u32PhysicalMemoryTypeIndex = 0U; u32PhysicalMemoryTypeIndex < vk_physicalDeviceMemoryProperties.memoryTypeCount; u32PhysicalMemoryTypeIndex++) {
			if ((vk_bufferMemoryRequirements.memoryTypeBits & (1U << u32PhysicalMemoryTypeIndex)) && (vk_physicalDeviceMemoryProperties.memoryTypes[u32PhysicalMemoryTypeIndex].propertyFlags & vk_eMemoryProperties)) {
				physicalMemoryTypeSelected = u32PhysicalMemoryTypeIndex;
				break;
			}
		}
		if (!physicalMemoryTypeSelected.has_value()) {
			error(pcFile, pcFunc, u32Line, "The physical Vulkan device doesn't support the required memory type for the recently created buffer", true);
			CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, *vk_phBuffer, nullptr));
			return false;
		}
		VkMemoryAllocateInfo vk_bufferMemoryAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = vk_bufferMemoryRequirements.size,
			.memoryTypeIndex = physicalMemoryTypeSelected.value()
		};
		if (!CHECK_VK_RESULT(vkAllocateMemory(vk_hDevice, &vk_bufferMemoryAllocInfo, nullptr, vk_phBufferMemory))) {
			error(pcFile, pcFunc, u32Line, "Failed to allocate memory for the recently created Vulkan buffer", true);
			CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, *vk_phBuffer, nullptr));
			return false;
		}
		if (!CHECK_VK_RESULT(vkBindBufferMemory(vk_hDevice, *vk_phBuffer, *vk_phBufferMemory, 0UL))) {
			error(pcFile, pcFunc, u32Line, "Failed to bind recently allocated memory to recently created Vulkan buffer", true);
			CATCH_SIGNAL(vkFreeMemory(vk_hDevice, *vk_phBufferMemory, nullptr));
			CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, *vk_phBuffer, nullptr));
			return false;
		}
		return true;
	}

	bool __vk_create_framebuffer(const VkFramebufferCreateFlags vk_eCreateFlags, const VkRenderPass vk_hRenderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *vk_phImageViewAttachments, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Layers, VkFramebuffer *vk_phFramebuffer, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkFramebufferCreateInfo vk_framebufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.renderPass = vk_hRenderPass,
			.attachmentCount = u32ImageViewAttachmentCount,
			.pAttachments = vk_phImageViewAttachments,
			.width = u32Width,
			.height = u32Height,
			.layers = u32Layers
		};
		if (!CHECK_VK_RESULT(vkCreateFramebuffer(vk_hDevice, &vk_framebufferCreateInfo, nullptr, vk_phFramebuffer))) {
			error(pcFile, pcFunc, u32Line, "Failed creating Vulkan framebuffer", true);
			return false;
		}
		return true;
	}

	bool __vk_create_semaphore(const VkSemaphoreCreateFlags vk_eCreateFlags, VkSemaphore *vk_phSemaphore, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.flags = vk_eCreateFlags
		};
		if (!CHECK_VK_RESULT(vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, vk_phSemaphore))) {
			error(pcFile, pcFunc, u32Line, "Failed creating Vulkan semaphore", true);
			return false;
		}
		return true;
	}

	bool __vk_create_fence(const VkFenceCreateFlags vk_eCreateFlags, VkFence *vk_phFence, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkFenceCreateInfo vk_fenceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = vk_eCreateFlags
		};
		if (!CHECK_VK_RESULT(vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, vk_phFence))) {
			error(pcFile, pcFunc, u32Line, "Failed creating Vulkan fence", true);
			return false;
		}
		return true;
	}

	bool __vk_create_shader_from_file(const char *pcPathToFile, const VkShaderModuleCreateFlags vk_eCreateFlags, VkShaderModule *vk_phShaderModule, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		std::ifstream shaderBinaryFile(pcPathToFile, std::ios::ate | std::ios::binary);
		if (!shaderBinaryFile.is_open()) {
			error(pcFile, pcFunc, u32Line, append_to_string("Failed opening the SPIR-V shader binary file \"", pcPathToFile, "\"").c_str(), true);
			return false;
		}
		const size_t shaderBinaryFileSize = shaderBinaryFile.tellg();
		char *const pcShaderBinaries = new char[shaderBinaryFileSize];
		shaderBinaryFile.seekg(0);
		CATCH_SIGNAL(shaderBinaryFile.read(pcShaderBinaries, shaderBinaryFileSize));
		shaderBinaryFile.close();
		VkShaderModuleCreateInfo vk_shaderCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.codeSize = static_cast<uint32_t>(shaderBinaryFileSize),
			.pCode = reinterpret_cast<const uint32_t*>(pcShaderBinaries)
		};

		const bool bShaderCreatedSuccessfully = CHECK_VK_RESULT(vkCreateShaderModule(vk_hDevice, &vk_shaderCreateInfo, nullptr, vk_phShaderModule));
		delete[] pcShaderBinaries;
		if (!bShaderCreatedSuccessfully) {
			error(pcFile, pcFunc, u32Line, append_to_string("Failed creating Vulkan shader module for the SPIR-V shader binaries loaded from \"", pcPathToFile, "\"").c_str(), true);
			return false;
		}
		return true;
	}

	bool __vk_create_render_pass(const uint32_t u32AttachmentDescriptionCount, const VkAttachmentDescription *vk_pAttachmentDescriptions, const uint32_t u32SubpassDescriptionCount, const VkSubpassDescription *vk_pSubpassDescriptions, const uint32_t u32SubpassDependencyCount, const VkSubpassDependency *vk_pSubpassDependencies, VkRenderPass *vk_phRenderPass, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkRenderPassCreateInfo vk_renderPassCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = u32AttachmentDescriptionCount,
			.pAttachments = vk_pAttachmentDescriptions,
			.subpassCount = u32SubpassDescriptionCount,
			.pSubpasses = vk_pSubpassDescriptions,
			.dependencyCount = u32SubpassDependencyCount,
			.pDependencies = vk_pSubpassDependencies
		};
		if (!CHECK_VK_RESULT(vkCreateRenderPass(vk_hDevice, &vk_renderPassCreateInfo, nullptr, vk_phRenderPass))) {
			error(pcFile, pcFunc, u32Line, "Failed creating Vulkan render pass", true);
			return false;
		}
		return true;
	}

	bool __vk_create_descriptor_set_layout(const VkDescriptorSetLayoutCreateFlags vk_eCreateFlags, const uint32_t u32BindingCount, const VkDescriptorSetLayoutBinding *vk_pBindings, VkDescriptorSetLayout *vk_pDescriptorSetLayout, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkDescriptorSetLayoutCreateInfo vk_descriptorSetLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.bindingCount = u32BindingCount,
			.pBindings = vk_pBindings
		};
		if (!CHECK_VK_RESULT(vkCreateDescriptorSetLayout(vk_hDevice, &vk_descriptorSetLayoutCreateInfo, nullptr, vk_pDescriptorSetLayout))) {
			error(pcFile, pcFunc, u32Line, "Failed to create Vulkan descriptor set layout", true);
			return false;
		}
		return true;
	}

	bool __vk_create_pipeline_layout(const VkPipelineLayoutCreateFlags vk_eCreateFlags, const uint32_t u32DescriptorSetLayoutCount, const VkDescriptorSetLayout *vk_phDescriptorSetLayouts, const uint32_t u32PushConstantRangeCount, const VkPushConstantRange *vk_pPushConstantRanges, VkPipelineLayout *vk_phPipelineLayout, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.setLayoutCount = u32DescriptorSetLayoutCount,
			.pSetLayouts = vk_phDescriptorSetLayouts,
			.pushConstantRangeCount = u32PushConstantRangeCount,
			.pPushConstantRanges = vk_pPushConstantRanges
		};
		if (!CHECK_VK_RESULT(vkCreatePipelineLayout(vk_hDevice, &vk_pipelineLayoutCreateInfo, nullptr, vk_phPipelineLayout))) {
			error(pcFile, pcFunc, u32Line, "Failed to create Vulkan pipeline layout", true);
			return false;
		}
		return true;
	}

	bool __vk_create_descriptor_pool(const VkDescriptorPoolCreateFlags vk_eCreateFlags, const uint32_t u32MaxSets, const uint32_t u32PoolSizeCount, const VkDescriptorPoolSize *vk_pPoolSizes, VkDescriptorPool *vk_phDescriptorPool, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkDescriptorPoolCreateInfo vk_descriptorPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.maxSets = u32MaxSets,
			.poolSizeCount = u32PoolSizeCount,
			.pPoolSizes = vk_pPoolSizes
		};
		if (!CHECK_VK_RESULT(vkCreateDescriptorPool(vk_hDevice, &vk_descriptorPoolCreateInfo, nullptr, vk_phDescriptorPool))) {
			error(pcFile, pcFunc, u32Line, "Failed to create Vulkan descriptor pool", true);
			return false;
		}
		return true;
	}

	bool __vk_alloc_command_buffers(const VkCommandPool vk_hCommandPool, const VkCommandBufferLevel vk_eCommandBufferLevel, const uint32_t u32CommandBufferCount, VkCommandBuffer *vk_phCommandBuffers, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkCommandBufferAllocateInfo vk_commandBufferAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = vk_hCommandPool,
			.level = vk_eCommandBufferLevel,
			.commandBufferCount = u32CommandBufferCount
		};
		if (!CHECK_VK_RESULT(vkAllocateCommandBuffers(vk_hDevice, &vk_commandBufferAllocInfo, vk_phCommandBuffers))) {
			error(pcFile, pcFunc, u32Line, "Failed to allocate Vulkan command buffers", true);
			return false;
		}
		return true;
	}

	bool __vk_alloc_descriptor_sets(const VkDescriptorPool vk_hDescriptorPool, const uint32_t u32DescriptorSetCount, const VkDescriptorSetLayout *vk_phDescriptorSetLayouts, VkDescriptorSet *vk_phDescriptorSets, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkDescriptorSetAllocateInfo vk_descriptorSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hDescriptorPool,
			.descriptorSetCount = u32DescriptorSetCount,
			.pSetLayouts = vk_phDescriptorSetLayouts
		};
		if (!CHECK_VK_RESULT(vkAllocateDescriptorSets(vk_hDevice, &vk_descriptorSetAllocInfo, vk_phDescriptorSets))) {
			error(pcFile, pcFunc, u32Line, "Failed to allocate Vulkan descriptor sets", true);
			return false;
		}
		return true;
	}


	void __vk_wait_for_fence(const VkFence *vk_phFence) {
		CATCH_SIGNAL(vkWaitForFences(vk_hDevice, 1U, vk_phFence, VK_TRUE, std::numeric_limits<uint32_t>::max()));
	}

	void __vk_reset_fence(const VkFence *vk_phFence) {
		CATCH_SIGNAL(vkResetFences(vk_hDevice, 1U, vk_phFence));
	}

	void __vk_wait_for_and_reset_fence(const VkFence *vk_phFence) {
		__vk_wait_for_fence(vk_phFence);
		__vk_reset_fence(vk_phFence);
	}

	bool __vk_signal_semaphores(const uint32_t u32SignalSemaphoreCount, const VkSemaphore *vk_phSignalSemaphores, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkSubmitInfo vk_queueSubmissionInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1U,
			.pCommandBuffers = &vk_hDummyTransferCommandBuffer,
			.signalSemaphoreCount = u32SignalSemaphoreCount,
			.pSignalSemaphores = vk_phSignalSemaphores
		};
		if (!CHECK_VK_RESULT(vkQueueSubmit(vk_hDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 1U, &vk_queueSubmissionInfo, VK_NULL_HANDLE))) {
			error(pcFile, pcFunc, u32Line, "Failed to submit dummy task to the transfer queue to signal semaphores", true);
			return false;
		}
		return true;
	}

	bool __vk_signal_fence(const VkFence vk_hSignalFence, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkSubmitInfo vk_queueSubmissionInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1U,
			.pCommandBuffers = &vk_hDummyTransferCommandBuffer
		};
		if (!CHECK_VK_RESULT(vkQueueSubmit(vk_hDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 1U, &vk_queueSubmissionInfo, vk_hSignalFence))) {
			error(pcFile, pcFunc, u32Line, "Failed to submit dummy task to the transfer queue to signal a fence", true);
			return false;
		}
		return true;
	}


	static bool submit_to_vulkan_queue(const VkQueue vk_hQueue, const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_phWaitSemaphores, const VkPipelineStageFlags *vk_peWaitPipelineStages, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_phCommandBuffers, const uint32_t u32SignalSemaphoreCount, const VkSemaphore *vk_phSignalSemaphores, const VkFence vk_hFenceToSignal, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkSubmitInfo vk_queueSubmissionInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = u32WaitSemaphoreCount,
			.pWaitSemaphores = vk_phWaitSemaphores,
			.pWaitDstStageMask = vk_peWaitPipelineStages,
			.commandBufferCount = u32CommandBufferCount,
			.pCommandBuffers = vk_phCommandBuffers,
			.signalSemaphoreCount = u32SignalSemaphoreCount,
			.pSignalSemaphores = vk_phSignalSemaphores
		};
		return CHECK_VK_RESULT(vkQueueSubmit(vk_hQueue, 1U, &vk_queueSubmissionInfo, vk_hFenceToSignal));
	}

	bool __vk_submit_to_graphics_queue(const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_phWaitSemaphores, const VkPipelineStageFlags *vk_peWaitPipelineStages, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_phCommandBuffers, const uint32_t u32SignalSemaphoreCount, const VkSemaphore *vk_phSignalSemaphores, const VkFence vk_hFenceToSignal, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		if (!submit_to_vulkan_queue(vk_hDeviceQueueFamilies[RE_VK_QUEUE_GRAPHICS_INDEX], u32WaitSemaphoreCount, vk_phWaitSemaphores, vk_peWaitPipelineStages, u32CommandBufferCount, vk_phCommandBuffers, u32SignalSemaphoreCount, vk_phSignalSemaphores, vk_hFenceToSignal, pcFile, pcFunc, u32Line)) {
			error(pcFile, pcFunc, u32Line, "Failed to submit task(s) to the graphics queue", true);
			return false;
		}
		return true;
	}

	bool __vk_submit_to_transfer_queue(const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_phWaitSemaphores, const VkPipelineStageFlags *vk_peWaitPipelineStages, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_phCommandBuffers, const uint32_t u32SignalSemaphoreCount, const VkSemaphore *vk_phSignalSemaphores, const VkFence vk_hFenceToSignal, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		if (!submit_to_vulkan_queue(vk_hDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], u32WaitSemaphoreCount, vk_phWaitSemaphores, vk_peWaitPipelineStages, u32CommandBufferCount, vk_phCommandBuffers, u32SignalSemaphoreCount, vk_phSignalSemaphores, vk_hFenceToSignal, pcFile, pcFunc, u32Line)) {
			error(pcFile, pcFunc, u32Line, "Failed to submit task(s) to the transfer queue", true);
			return false;
		}
		return true;
	}

	bool __vk_submit_to_present_queue(const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_phWaitSemaphores, const uint32_t *pu32SwapchainImageIndex, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkPresentInfoKHR vk_presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = u32WaitSemaphoreCount,
			.pWaitSemaphores = vk_phWaitSemaphores,
			.swapchainCount = 1U,
			.pSwapchains = &vk_hSwapchain,
			.pImageIndices = pu32SwapchainImageIndex,
		};
		if (!CHECK_VK_RESULT(vkQueuePresentKHR(vk_hDeviceQueueFamilies[RE_VK_QUEUE_PRESENT_INDEX], &vk_presentInfo))) {
			error(pcFile, pcFunc, u32Line, "Failed to submit task(s) to the present queue", true);
			return false;
		}
		return true;
	}

	bool __vk_begin_recording_command_buffer(const VkCommandBuffer vk_hCommandBuffer, const VkCommandBufferUsageFlags vk_eUsageFlags, const VkCommandBufferInheritanceInfo *vk_pInheritanceInfo, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkCommandBufferBeginInfo vk_beginRecordInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = vk_eUsageFlags,
			.pInheritanceInfo = vk_pInheritanceInfo
		};
		if (!CHECK_VK_RESULT(vkBeginCommandBuffer(vk_hCommandBuffer, &vk_beginRecordInfo))) {
			error(pcFile, pcFunc, u32Line, "Failed to begin recording a Vulkan command buffer", true);
			return false;
		}
		return true;
	}

}
