#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

	static std::vector<uint32_t> get_queue_indices(const uint32_t u32QueueTypeCount, const uint32_t *pu32QueueTypes) {
		std::vector<uint32_t> queueFamilyIndices;
		if (u32QueueTypeCount > 1U)
			for (uint32_t u32QueueTypeIndex = 0U; u32QueueTypeIndex < u32QueueTypeCount; u32QueueTypeIndex++)
				if (std::find(queueFamilyIndices.begin(), queueFamilyIndices.end(), pDeviceQueues[pu32QueueTypes[u32QueueTypeIndex]]->u32QueueIndex) == queueFamilyIndices.end())
					queueFamilyIndices.push_back(pDeviceQueues[pu32QueueTypes[u32QueueTypeIndex]]->u32QueueIndex);
		return queueFamilyIndices;
	}
	
	bool __vk_create_buffer(const VkBufferCreateFlags vk_eCreateFlags, const VkDeviceSize vk_bufferSizeInBytes, const VkBufferUsageFlags vk_bufferUsage, const uint32_t u32QueueTypeCount, const uint32_t *pu32BufferQueueTypeIndices, const VkMemoryPropertyFlags vk_memoryProperties, VkBuffer *vk_phBuffer, VkDeviceMemory *vk_phBufferMemory, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		const std::vector<uint32_t> queueFamilies = get_queue_indices(u32QueueTypeCount, pu32BufferQueueTypeIndices);
		VkBufferCreateInfo vk_bufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.flags = vk_eCreateFlags,
			.size = vk_bufferSizeInBytes,
			.usage = vk_bufferUsage
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
		CATCH_SIGNAL(vkGetBufferMemoryRequirements(vk_hDevice, vk_hBuffer, &vk_bufferMemoryRequirements));
		std::optional<uint32_t> physicalMemoryTypeSelected;
		for (uint32_t u32PhysicalMemoryTypeIndex = 0U; u32PhysicalMemoryTypeIndex < vk_physicalDeviceMemoryProperties.memoryTypeCount; u32PhysicalMemoryTypeIndex++) {
			if ((vk_bufferMemoryRequirements.memoryTypeBits & (1U << u32PhysicalMemoryTypeIndex)) && (vk_physicalDeviceMemoryProperties.memoryTypes[u32PhysicalMemoryTypeIndex].propertyFlags & vk_memoryProperties)) {
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
		if (!CHECK_VK_RESULT(vkBindBufferMemory(vk_hDevice, *vk_phBuffer, *vk_phBufferMemory, vk_bufferMemoryRequirements.size))) {
			error(pcFile, pcFunc, u32Line, "Failed to bind recently allocated memory to recently created Vulkan buffer", true);
			CATCH_SIGNAL(vkFreeMemory(vk_hDevice, *vk_phBufferMemory, nullptr));
			CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, *vk_phBuffer, nullptr));
			return false;
		}
		return true;
	}

	bool __vk_create_framebuffer(const VkFramebufferCreateFlags vk_eCreateFlags, const VkRenderPass vk_hRenderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *vk_phImageViewAttachments, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Layers, VkFramebuffer *vk_phFramebuffer, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkFramebufferCreateInfo vk_framebufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			.flags = vk_eCreateFlags,
			.renderPass = vk_hRenderPass;
			.attachmentCount = u32ImageViewAttachmentCount;
			.pAttachments = vk_phImageViewAttachments;
			.width = u32Width;
			.height = u32Height;
			.layers = u32Layers;
		}
		if (!CHECK_VK_RESULT(vkCreateFramebuffer(vk_hDevice, &vk_framebufferCreateInfo, nullptr, vk_phFramebuffer))) {
			error(pcFile, pcFunc, u32Line, "Failed creating Vulkan framebuffer", true);
			return false;
		}
		return true;
	}

	bool __vk_create_semaphore(const VkSemaphoreCreateFlags vk_eCreateFlags, const VkSemaphore *vk_phSemaphore, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.flags = vk_eCreateFlags
		}
		if (!CHECK_VK_RESULT(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, vk_phSemaphore)) {
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
		if (!CHECK_VK_RESULT(vk_hDevice, &vk_fenceCreateInfo, nullptr, vk_phFence)) {
			error(pcFile, pcFunc, u32Line, "Failed creating Vulkan fence", true);
			return false;
		}
		return true;
	}

	bool __vk_create_shader_from_file(const char *pcPathToFile, const VkShaderModuleCreateFlags vk_eCreateFlags, VkShaderModule *vk_phShaderModule, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		std::ifstream shaderBinaryFile(pcShaderBinaryFileSource, std::ios::ate | std::ios::binary);
		if (!shaderBinaryFile.is_open()) {
			error(pcFile, pcFunc, u32Line, append_to_string("Failed opening the SPIR-V shader binary file \"", pcShaderBinaryFileSource, "\"").c_str(), true);
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
			error(pcFile, pcFunc, u32Line, append_to_string("Failed creating Vulkan shader module for the SPIR-V shader binaries loaded from \"", pcShaderBinaryFileSource, "\"").c_str(), true);
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


	void __vk_wait_for_fence(const VkFence vk_hFence) {
		constexpr uint32_t u32FenceCount = 1U;
		const VkFence vk_allFences[u32FenceCount] = {vk_hFence};
		CATCH_SIGNAL(vkWaitForFences(vk_hDevice, u32FenceCount, vk_allFences, VK_TRUE, std::numeric_limits<uint32_t>::max()));
	}

	void __vk_reset_fence(const VkFence vk_hFence) {
		constexpr uint32_t u32FenceCount = 1U;
		const VkFence vk_allFences[u32FenceCount] = {vk_hFence};
		CATCH_SIGNAL(vkResetFences(vk_hDevice, u32FenceCount, vk_allFences));
	}

	void __vk_wait_for_and_reset_fence(const VkFence vk_hFence) {
		constexpr uint32_t u32FenceCount = 1U;
		const VkFence vk_allFences[u32FenceCount] = {vk_hFence};
		CATCH_SIGNAL(vkWaitForFences(vk_hDevice, u32FenceCount, vk_allFences, VK_TRUE, std::numeric_limits<uint32_t>::max()));
		CATCH_SIGNAL(vkResetFences(vk_hDevice, u32FenceCount, vk_allFences));
	}

	bool __vk_signal_semaphores(const uint32_t u32SignalSemaphoreCount, const VkSemaphore *vk_phSignalSemaphores, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkSubmitInfo vk_queueSubmissionInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 0U,
			.pWaitSemaphores = nullptr,
			.pWaitDstStageMask = nullptr,
			.commandBufferCount = 1U,
			.pCommandBuffers = ,
			.signalSemaphoreCount = u32SignalSemaphoreCount,
			.pSignalSemaphores = vk_phSignalSemaphores
		};
		if (!CHECK_VK_RESULT(vkQueueSubmit(vk_deviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 1U, &vk_queueSubmissionInfo, VK_NULL_HANDLE))) {
			error(pcFile, pcFunc, u32Line, "Failed to submit dummy task to the transfer queue to signal semaphores", true);
			return false;
		}
		return true;
	}


	static bool submit_to_vulkan_queue(const VkQueue vk_hQueue, const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_phWaitSemaphores, const VkPipelineStageFlagBits vk_eWaitPipelineStage, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_phCommandBuffers, const uint32_t u32SignalSemaphoreCount, const VkSemaphore vk_phSignalSemaphores, const VkFence vk_hFenceToSignal, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		VkSubmitInfo vk_queueSubmissionInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = u32WaitSemaphoreCount,
			.pWaitSemaphores = vk_phWaitSemaphores,
			.pWaitDstStageMask = vk_eWaitPipelineStage,
			.commandBufferCount = u32CommandBufferCount,
			.pCommandBuffers = vk_phCommandBuffers,
			.signalSemaphoreCount = u32SignalSemaphoreCount,
			.pSignalSemaphores = vk_phSignalSemaphores
		};
		return CHECK_VK_RESULT(vkQueueSubmit(vk_hQueue, 1U, &vk_queueSubmissionInfo, vk_hFence));
	}

	bool __vk_submit_to_graphics_queue(const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_phWaitSemaphores, const VkPipelineStageFlagBits vk_eWaitPipelineStage, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_phCommandBuffers, const uint32_t u32SignalSemaphoreCount, const VkSemaphore vk_phSignalSemaphores, const VkFence vk_hFenceToSignal, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		if (!submit_to_vulkan_queue(vk_deviceQueueFamilies[RE_VK_QUEUE_GRAPHICS_INDEX], vk_phWaitSemaphores, vk_eWaitPipelineStage, u32CommandBufferCount, vk_phCommandBuffers, u32SignalSemaphoreCount, vk_phSignalSemaphores, vk_hFenceToSignal, pcFile, pcFunc, u32Line)) {
			error(pcFile, pcFunc, u32Line, "Failed to submit task(s) to the graphics queue", true);
			return false;
		}
		return true;
	}

	bool __vk_submit_to_transfer_queue(const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_phWaitSemaphores, const VkPipelineStageFlagBits vk_eWaitPipelineStage, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_phCommandBuffers, const uint32_t u32SignalSemaphoreCount, const VkSemaphore vk_phSignalSemaphores, const VkFence vk_hFenceToSignal, const char *pcFile, const char *pcFunc, const uint32_t u32Line) {
		if (!submit_to_vulkan_queue(vk_deviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], vk_phWaitSemaphores, vk_eWaitPipelineStage, u32CommandBufferCount, vk_phCommandBuffers, u32SignalSemaphoreCount, vk_phSignalSemaphores, vk_hFenceToSignal, pcFile, pcFunc, u32Line)) {
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
		if (!CHECK_VK_RESULT(vkQueuePresentKHR(vk_deviceQueueFamilies[RE_VK_QUEUE_PRESENT_INDEX], &vk_presentInfo))) {
			error(pcFile, pcFunc, u32Line, "Failed to submit task(s) to the present queue");
		}
	}

}
