#include "RE_Vulkan_Device.hpp"
#include "RE_Render System.hpp"
#include "RE_Render System_Scheduler.hpp"

#include <queue>

namespace RE {

	VkDevice vk_hDevice = VK_NULL_HANDLE;

	PFN_vkGetDeviceQueue pfn_vkGetDeviceQueue = nullptr;
	PFN_vkQueueSubmit pfn_vkQueueSubmit = nullptr;
	PFN_vkQueueWaitIdle pfn_vkQueueWaitIdle = nullptr;
	PFN_vkDeviceWaitIdle pfn_vkDeviceWaitIdle = nullptr;
	PFN_vkAllocateMemory pfn_vkAllocateMemory = nullptr;
	PFN_vkFreeMemory pfn_vkFreeMemory = nullptr;
	PFN_vkMapMemory pfn_vkMapMemory = nullptr;
	PFN_vkUnmapMemory pfn_vkUnmapMemory = nullptr;
	PFN_vkFlushMappedMemoryRanges pfn_vkFlushMappedMemoryRanges = nullptr;
	PFN_vkInvalidateMappedMemoryRanges pfn_vkInvalidateMappedMemoryRanges = nullptr;
	PFN_vkGetDeviceMemoryCommitment pfn_vkGetDeviceMemoryCommitment = nullptr;
	PFN_vkBindBufferMemory pfn_vkBindBufferMemory = nullptr;
	PFN_vkBindImageMemory pfn_vkBindImageMemory = nullptr;
	PFN_vkGetBufferMemoryRequirements pfn_vkGetBufferMemoryRequirements = nullptr;
	PFN_vkGetImageMemoryRequirements pfn_vkGetImageMemoryRequirements = nullptr;
	PFN_vkGetImageSparseMemoryRequirements pfn_vkGetImageSparseMemoryRequirements = nullptr;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties pfn_vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;
	PFN_vkQueueBindSparse pfn_vkQueueBindSparse = nullptr;
	PFN_vkCreateFence pfn_vkCreateFence = nullptr;
	PFN_vkDestroyFence pfn_vkDestroyFence = nullptr;
	PFN_vkResetFences pfn_vkResetFences = nullptr;
	PFN_vkGetFenceStatus pfn_vkGetFenceStatus = nullptr;
	PFN_vkWaitForFences pfn_vkWaitForFences = nullptr;
	PFN_vkCreateSemaphore pfn_vkCreateSemaphore = nullptr;
	PFN_vkDestroySemaphore pfn_vkDestroySemaphore = nullptr;
	PFN_vkCreateEvent pfn_vkCreateEvent = nullptr;
	PFN_vkDestroyEvent pfn_vkDestroyEvent = nullptr;
	PFN_vkGetEventStatus pfn_vkGetEventStatus = nullptr;
	PFN_vkSetEvent pfn_vkSetEvent = nullptr;
	PFN_vkResetEvent pfn_vkResetEvent = nullptr;
	PFN_vkCreateQueryPool pfn_vkCreateQueryPool = nullptr;
	PFN_vkDestroyQueryPool pfn_vkDestroyQueryPool = nullptr;
	PFN_vkGetQueryPoolResults pfn_vkGetQueryPoolResults = nullptr;
	PFN_vkCreateBuffer pfn_vkCreateBuffer = nullptr;
	PFN_vkDestroyBuffer pfn_vkDestroyBuffer = nullptr;
	PFN_vkCreateBufferView pfn_vkCreateBufferView = nullptr;
	PFN_vkDestroyBufferView pfn_vkDestroyBufferView = nullptr;	
	PFN_vkCreateImage pfn_vkCreateImage = nullptr;
	PFN_vkDestroyImage pfn_vkDestroyImage = nullptr;
	PFN_vkGetImageSubresourceLayout pfn_vkGetImageSubresourceLayout = nullptr;
	PFN_vkCreateImageView pfn_vkCreateImageView = nullptr;
	PFN_vkDestroyImageView pfn_vkDestroyImageView = nullptr;
	PFN_vkCreateShaderModule pfn_vkCreateShaderModule = nullptr;
	PFN_vkDestroyShaderModule pfn_vkDestroyShaderModule = nullptr;
	PFN_vkCreatePipelineCache pfn_vkCreatePipelineCache = nullptr;
	PFN_vkDestroyPipelineCache pfn_vkDestroyPipelineCache = nullptr;
	PFN_vkGetPipelineCacheData pfn_vkGetPipelineCacheData = nullptr;
	PFN_vkMergePipelineCaches pfn_vkMergePipelineCaches = nullptr;
	PFN_vkCreateGraphicsPipelines pfn_vkCreateGraphicsPipelines = nullptr;
	PFN_vkCreateComputePipelines pfn_vkCreateComputePipelines = nullptr;
	PFN_vkDestroyPipeline pfn_vkDestroyPipeline = nullptr;
	PFN_vkCreatePipelineLayout pfn_vkCreatePipelineLayout = nullptr;
	PFN_vkDestroyPipelineLayout pfn_vkDestroyPipelineLayout = nullptr;
	PFN_vkCreateSampler pfn_vkCreateSampler = nullptr;
	PFN_vkDestroySampler pfn_vkDestroySampler = nullptr;
	PFN_vkCreateDescriptorSetLayout pfn_vkCreateDescriptorSetLayout = nullptr;
	PFN_vkDestroyDescriptorSetLayout pfn_vkDestroyDescriptorSetLayout = nullptr;
	PFN_vkCreateDescriptorPool pfn_vkCreateDescriptorPool = nullptr;
	PFN_vkDestroyDescriptorPool pfn_vkDestroyDescriptorPool = nullptr;
	PFN_vkResetDescriptorPool pfn_vkResetDescriptorPool = nullptr;
	PFN_vkAllocateDescriptorSets pfn_vkAllocateDescriptorSets = nullptr;
	PFN_vkFreeDescriptorSets pfn_vkFreeDescriptorSets = nullptr;
	PFN_vkUpdateDescriptorSets pfn_vkUpdateDescriptorSets = nullptr;
	PFN_vkCreateFramebuffer pfn_vkCreateFramebuffer = nullptr;
	PFN_vkDestroyFramebuffer pfn_vkDestroyFramebuffer = nullptr;
	PFN_vkCreateRenderPass pfn_vkCreateRenderPass = nullptr;
	PFN_vkDestroyRenderPass pfn_vkDestroyRenderPass = nullptr;
	PFN_vkGetRenderAreaGranularity pfn_vkGetRenderAreaGranularity = nullptr;
	PFN_vkCreateCommandPool pfn_vkCreateCommandPool = nullptr;
	PFN_vkDestroyCommandPool pfn_vkDestroyCommandPool = nullptr;
	PFN_vkResetCommandPool pfn_vkResetCommandPool = nullptr;
	PFN_vkAllocateCommandBuffers pfn_vkAllocateCommandBuffers = nullptr;
	PFN_vkFreeCommandBuffers pfn_vkFreeCommandBuffers = nullptr;
	PFN_vkBeginCommandBuffer pfn_vkBeginCommandBuffer = nullptr;
	PFN_vkEndCommandBuffer pfn_vkEndCommandBuffer = nullptr;
	PFN_vkResetCommandBuffer pfn_vkResetCommandBuffer = nullptr;
	PFN_vkCmdBindPipeline pfn_vkCmdBindPipeline = nullptr;
	PFN_vkCmdSetViewport pfn_vkCmdSetViewport = nullptr;
	PFN_vkCmdSetScissor pfn_vkCmdSetScissor = nullptr;
	PFN_vkCmdSetLineWidth pfn_vkCmdSetLineWidth = nullptr;
	PFN_vkCmdSetDepthBias pfn_vkCmdSetDepthBias = nullptr;
	PFN_vkCmdSetBlendConstants pfn_vkCmdSetBlendConstants = nullptr;
	PFN_vkCmdSetDepthBounds pfn_vkCmdSetDepthBounds = nullptr;
	PFN_vkCmdSetStencilCompareMask pfn_vkCmdSetStencilCompareMask = nullptr;
	PFN_vkCmdSetStencilWriteMask pfn_vkCmdSetStencilWriteMask = nullptr;
	PFN_vkCmdSetStencilReference pfn_vkCmdSetStencilReference = nullptr;
	PFN_vkCmdBindDescriptorSets pfn_vkCmdBindDescriptorSets = nullptr;
	PFN_vkCmdBindIndexBuffer pfn_vkCmdBindIndexBuffer = nullptr;
	PFN_vkCmdBindVertexBuffers pfn_vkCmdBindVertexBuffers = nullptr;
	PFN_vkCmdDraw pfn_vkCmdDraw = nullptr;
	PFN_vkCmdDrawIndexed pfn_vkCmdDrawIndexed = nullptr;
	PFN_vkCmdDrawIndirect pfn_vkCmdDrawIndirect = nullptr;
	PFN_vkCmdDrawIndexedIndirect pfn_vkCmdDrawIndexedIndirect = nullptr;
	PFN_vkCmdDispatch pfn_vkCmdDispatch = nullptr;
	PFN_vkCmdDispatchIndirect pfn_vkCmdDispatchIndirect = nullptr;
	PFN_vkCmdCopyBuffer pfn_vkCmdCopyBuffer = nullptr;
	PFN_vkCmdCopyImage pfn_vkCmdCopyImage = nullptr;
	PFN_vkCmdBlitImage pfn_vkCmdBlitImage = nullptr;
	PFN_vkCmdCopyBufferToImage pfn_vkCmdCopyBufferToImage = nullptr;
	PFN_vkCmdCopyImageToBuffer pfn_vkCmdCopyImageToBuffer = nullptr;
	PFN_vkCmdUpdateBuffer pfn_vkCmdUpdateBuffer = nullptr;
	PFN_vkCmdFillBuffer pfn_vkCmdFillBuffer = nullptr;
	PFN_vkCmdClearColorImage pfn_vkCmdClearColorImage = nullptr;
	PFN_vkCmdClearDepthStencilImage pfn_vkCmdClearDepthStencilImage = nullptr;
	PFN_vkCmdClearAttachments pfn_vkCmdClearAttachments = nullptr;
	PFN_vkCmdResolveImage pfn_vkCmdResolveImage = nullptr;
	PFN_vkCmdSetEvent pfn_vkCmdSetEvent = nullptr;
	PFN_vkCmdResetEvent pfn_vkCmdResetEvent = nullptr;
	PFN_vkCmdWaitEvents pfn_vkCmdWaitEvents = nullptr;
	PFN_vkCmdPipelineBarrier pfn_vkCmdPipelineBarrier = nullptr;
	PFN_vkCmdBeginQuery pfn_vkCmdBeginQuery = nullptr;
	PFN_vkCmdEndQuery pfn_vkCmdEndQuery = nullptr;
	PFN_vkCmdResetQueryPool pfn_vkCmdResetQueryPool = nullptr;
	PFN_vkCmdWriteTimestamp pfn_vkCmdWriteTimestamp = nullptr;
	PFN_vkCmdCopyQueryPoolResults pfn_vkCmdCopyQueryPoolResults = nullptr;
	PFN_vkCmdPushConstants pfn_vkCmdPushConstants = nullptr;
	PFN_vkCmdBeginRenderPass pfn_vkCmdBeginRenderPass = nullptr;
	PFN_vkCmdNextSubpass pfn_vkCmdNextSubpass = nullptr;
	PFN_vkCmdEndRenderPass pfn_vkCmdEndRenderPass = nullptr;
	PFN_vkCmdExecuteCommands pfn_vkCmdExecuteCommands = nullptr;

	// Vulkan 1.1
	PFN_vkBindBufferMemory2 pfn_vkBindBufferMemory2 = nullptr;
	PFN_vkBindImageMemory2 pfn_vkBindImageMemory2 = nullptr;
	PFN_vkGetDeviceGroupPeerMemoryFeatures pfn_vkGetDeviceGroupPeerMemoryFeatures = nullptr;
	PFN_vkCmdSetDeviceMask pfn_vkCmdSetDeviceMask = nullptr;
	PFN_vkCmdDispatchBase pfn_vkCmdDispatchBase = nullptr;
	PFN_vkGetImageMemoryRequirements2 pfn_vkGetImageMemoryRequirements2 = nullptr;
	PFN_vkGetBufferMemoryRequirements2 pfn_vkGetBufferMemoryRequirements2 = nullptr;
	PFN_vkGetImageSparseMemoryRequirements2 pfn_vkGetImageSparseMemoryRequirements2 = nullptr;
	PFN_vkTrimCommandPool pfn_vkTrimCommandPool = nullptr;
	PFN_vkGetDeviceQueue2 pfn_vkGetDeviceQueue2 = nullptr;
	PFN_vkCreateSamplerYcbcrConversion pfn_vkCreateSamplerYcbcrConversion = nullptr;
	PFN_vkDestroySamplerYcbcrConversion pfn_vkDestroySamplerYcbcrConversion = nullptr;
	PFN_vkCreateDescriptorUpdateTemplate pfn_vkCreateDescriptorUpdateTemplate = nullptr;
	PFN_vkDestroyDescriptorUpdateTemplate pfn_vkDestroyDescriptorUpdateTemplate = nullptr;
	PFN_vkUpdateDescriptorSetWithTemplate pfn_vkUpdateDescriptorSetWithTemplate = nullptr;
	PFN_vkGetDescriptorSetLayoutSupport pfn_vkGetDescriptorSetLayoutSupport = nullptr;

	// Vulkan 1.2
	PFN_vkCmdDrawIndirectCount pfn_vkCmdDrawIndirectCount = nullptr;
	PFN_vkCmdDrawIndexedIndirectCount pfn_vkCmdDrawIndexedIndirectCount = nullptr;
	PFN_vkCreateRenderPass2 pfn_vkCreateRenderPass2 = nullptr;
	PFN_vkCmdBeginRenderPass2 pfn_vkCmdBeginRenderPass2 = nullptr;
	PFN_vkCmdNextSubpass2 pfn_vkCmdNextSubpass2 = nullptr;
	PFN_vkCmdEndRenderPass2 pfn_vkCmdEndRenderPass2 = nullptr;
	PFN_vkResetQueryPool pfn_vkResetQueryPool = nullptr;
	PFN_vkGetSemaphoreCounterValue pfn_vkGetSemaphoreCounterValue = nullptr;
	PFN_vkWaitSemaphores pfn_vkWaitSemaphores = nullptr;
	PFN_vkSignalSemaphore pfn_vkSignalSemaphore = nullptr;
	PFN_vkGetBufferDeviceAddress pfn_vkGetBufferDeviceAddress = nullptr;
	PFN_vkGetBufferOpaqueCaptureAddress pfn_vkGetBufferOpaqueCaptureAddress = nullptr;
	PFN_vkGetDeviceMemoryOpaqueCaptureAddress pfn_vkGetDeviceMemoryOpaqueCaptureAddress = nullptr;

	// Vulkan 1.3
	PFN_vkCreatePrivateDataSlot pfn_vkCreatePrivateDataSlot = nullptr;
	PFN_vkDestroyPrivateDataSlot pfn_vkDestroyPrivateDataSlot = nullptr;
	PFN_vkSetPrivateData pfn_vkSetPrivateData = nullptr;
	PFN_vkGetPrivateData pfn_vkGetPrivateData = nullptr;
	PFN_vkCmdSetEvent2 pfn_vkCmdSetEvent2 = nullptr;
	PFN_vkCmdResetEvent2 pfn_vkCmdResetEvent2 = nullptr;
	PFN_vkCmdWaitEvents2 pfn_vkCmdWaitEvents2 = nullptr;
	PFN_vkCmdPipelineBarrier2 pfn_vkCmdPipelineBarrier2 = nullptr;
	PFN_vkCmdWriteTimestamp2 pfn_vkCmdWriteTimestamp2 = nullptr;
	PFN_vkQueueSubmit2 pfn_vkQueueSubmit2 = nullptr;
	PFN_vkCmdCopyBuffer2 pfn_vkCmdCopyBuffer2 = nullptr;
	PFN_vkCmdCopyImage2 pfn_vkCmdCopyImage2 = nullptr;
	PFN_vkCmdCopyBufferToImage2 pfn_vkCmdCopyBufferToImage2 = nullptr;
	PFN_vkCmdCopyImageToBuffer2 pfn_vkCmdCopyImageToBuffer2 = nullptr;
	PFN_vkCmdBlitImage2 pfn_vkCmdBlitImage2 = nullptr;
	PFN_vkCmdResolveImage2 pfn_vkCmdResolveImage2 = nullptr;
	PFN_vkCmdBeginRendering pfn_vkCmdBeginRendering = nullptr;
	PFN_vkCmdEndRendering pfn_vkCmdEndRendering = nullptr;
	PFN_vkCmdSetCullMode pfn_vkCmdSetCullMode = nullptr;
	PFN_vkCmdSetFrontFace pfn_vkCmdSetFrontFace = nullptr;
	PFN_vkCmdSetPrimitiveTopology pfn_vkCmdSetPrimitiveTopology = nullptr;
	PFN_vkCmdSetViewportWithCount pfn_vkCmdSetViewportWithCount = nullptr;
	PFN_vkCmdSetScissorWithCount pfn_vkCmdSetScissorWithCount = nullptr;
	PFN_vkCmdBindVertexBuffers2 pfn_vkCmdBindVertexBuffers2 = nullptr;
	PFN_vkCmdSetDepthTestEnable pfn_vkCmdSetDepthTestEnable = nullptr;
	PFN_vkCmdSetDepthWriteEnable pfn_vkCmdSetDepthWriteEnable = nullptr;
	PFN_vkCmdSetDepthCompareOp pfn_vkCmdSetDepthCompareOp = nullptr;
	PFN_vkCmdSetDepthBoundsTestEnable pfn_vkCmdSetDepthBoundsTestEnable = nullptr;
	PFN_vkCmdSetStencilTestEnable pfn_vkCmdSetStencilTestEnable = nullptr;
	PFN_vkCmdSetStencilOp pfn_vkCmdSetStencilOp = nullptr;
	PFN_vkCmdSetRasterizerDiscardEnable pfn_vkCmdSetRasterizerDiscardEnable = nullptr;
	PFN_vkCmdSetDepthBiasEnable pfn_vkCmdSetDepthBiasEnable = nullptr;
	PFN_vkCmdSetPrimitiveRestartEnable pfn_vkCmdSetPrimitiveRestartEnable = nullptr;
	PFN_vkGetDeviceBufferMemoryRequirements pfn_vkGetDeviceBufferMemoryRequirements = nullptr;
	PFN_vkGetDeviceImageMemoryRequirements pfn_vkGetDeviceImageMemoryRequirements = nullptr;
	PFN_vkGetDeviceImageSparseMemoryRequirements pfn_vkGetDeviceImageSparseMemoryRequirements = nullptr;

	// Vulkan 1.4
	/* PFN_vkCmdSetLineStipple pfn_vkCmdSetLineStipple = nullptr;
	PFN_vkMapMemory2 pfn_vkMapMemory2 = nullptr;
	PFN_vkUnmapMemory2 pfn_vkUnmapMemory2 = nullptr;
	PFN_vkCmdBindIndexBuffer2 pfn_vkCmdBindIndexBuffer2 = nullptr;
	PFN_vkGetRenderingAreaGranularity pfn_vkGetRenderingAreaGranularity = nullptr;
	PFN_vkGetDeviceImageSubresourceLayout pfn_vkGetDeviceImageSubresourceLayout = nullptr;
	PFN_vkGetImageSubresourceLayout2 pfn_vkGetImageSubresourceLayout2 = nullptr;
	PFN_vkCmdPushDescriptorSet pfn_vkCmdPushDescriptorSet = nullptr;
	PFN_vkCmdPushDescriptorSetWithTemplate pfn_vkCmdPushDescriptorSetWithTemplate = nullptr;
	PFN_vkCmdSetRenderingAttachmentLocations pfn_vkCmdSetRenderingAttachmentLocations = nullptr;
	PFN_vkCmdSetRenderingInputAttachmentIndices pfn_vkCmdSetRenderingInputAttachmentIndices = nullptr;
	PFN_vkCmdBindDescriptorSets2 pfn_vkCmdBindDescriptorSets2 = nullptr;
	PFN_vkCmdPushConstants2 pfn_vkCmdPushConstants2 = nullptr;
	PFN_vkCmdPushDescriptorSet2 pfn_vkCmdPushDescriptorSet2 = nullptr;
	PFN_vkCmdPushDescriptorSetWithTemplate2 pfn_vkCmdPushDescriptorSetWithTemplate2 = nullptr;
	PFN_vkCopyMemoryToImage pfn_vkCopyMemoryToImage = nullptr;
	PFN_vkCopyImageToMemory pfn_vkCopyImageToMemory = nullptr;
	PFN_vkCopyImageToImage pfn_vkCopyImageToImage = nullptr;
	PFN_vkTransitionImageLayout pfn_vkTransitionImageLayout = nullptr; */

	// Debug Messages
	PFN_vkQueueBeginDebugUtilsLabelEXT pfn_vkQueueBeginDebugUtilsLabelEXT = nullptr;
	PFN_vkQueueEndDebugUtilsLabelEXT pfn_vkQueueEndDebugUtilsLabelEXT = nullptr;
	PFN_vkQueueInsertDebugUtilsLabelEXT pfn_vkQueueInsertDebugUtilsLabelEXT = nullptr;
	PFN_vkCmdBeginDebugUtilsLabelEXT pfn_vkCmdBeginDebugUtilsLabelEXT = nullptr;
	PFN_vkCmdEndDebugUtilsLabelEXT pfn_vkCmdEndDebugUtilsLabelEXT = nullptr;
	PFN_vkCmdInsertDebugUtilsLabelEXT pfn_vkCmdInsertDebugUtilsLabelEXT = nullptr;
	// Swapchain
	PFN_vkCreateSwapchainKHR pfn_vkCreateSwapchainKHR = nullptr;
	PFN_vkDestroySwapchainKHR pfn_vkDestroySwapchainKHR = nullptr;
	PFN_vkGetSwapchainImagesKHR pfn_vkGetSwapchainImagesKHR = nullptr;
	PFN_vkAcquireNextImageKHR pfn_vkAcquireNextImageKHR = nullptr;
	PFN_vkQueuePresentKHR pfn_vkQueuePresentKHR = nullptr;
	PFN_vkGetDeviceGroupPresentCapabilitiesKHR pfn_vkGetDeviceGroupPresentCapabilitiesKHR = nullptr;
	PFN_vkGetDeviceGroupSurfacePresentModesKHR pfn_vkGetDeviceGroupSurfacePresentModesKHR = nullptr;
	PFN_vkAcquireNextImage2KHR pfn_vkAcquireNextImage2KHR = nullptr;

	static PFN_vkVoidFunction load_func_with_device(const char* pacFuncName) {
		PFN_vkVoidFunction pFunc;
		PUSH_TO_CALLSTACKTRACE(pFunc = vkGetDeviceProcAddr(vk_hDevice, pacFuncName));
		if (!pFunc)
			RE_FATAL_ERROR("Failed loading the Vulkan logical device-level function \"", pacFuncName, "\"");
		return pFunc;
	}

	static bool load_vulkan_1_0_device() {
		pfn_vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(load_func_with_device("vkGetDeviceQueue"));
		if (!pfn_vkGetDeviceQueue)
			return false;
		pfn_vkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(load_func_with_device("vkQueueSubmit"));
		if (!pfn_vkQueueSubmit)
			return false;
		pfn_vkQueueWaitIdle = reinterpret_cast<PFN_vkQueueWaitIdle>(load_func_with_device("vkQueueWaitIdle"));
		if (!pfn_vkQueueWaitIdle)
			return false;
		pfn_vkDeviceWaitIdle = reinterpret_cast<PFN_vkDeviceWaitIdle>(load_func_with_device("vkDeviceWaitIdle"));
		if (!pfn_vkDeviceWaitIdle)
			return false;
		pfn_vkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(load_func_with_device("vkAllocateMemory"));
		if (!pfn_vkAllocateMemory)
			return false;
		pfn_vkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(load_func_with_device("vkFreeMemory"));
		if (!pfn_vkFreeMemory)
			return false;
		pfn_vkMapMemory = reinterpret_cast<PFN_vkMapMemory>(load_func_with_device("vkMapMemory"));
		if (!pfn_vkMapMemory)
			return false;
		pfn_vkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(load_func_with_device("vkUnmapMemory"));
		if (!pfn_vkUnmapMemory)
			return false;
		pfn_vkFlushMappedMemoryRanges = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(load_func_with_device("vkFlushMappedMemoryRanges"));
		if (!pfn_vkFlushMappedMemoryRanges)
			return false;
		pfn_vkInvalidateMappedMemoryRanges = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(load_func_with_device("vkInvalidateMappedMemoryRanges"));
		if (!pfn_vkInvalidateMappedMemoryRanges)
			return false;
		pfn_vkGetDeviceMemoryCommitment = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(load_func_with_device("vkGetDeviceMemoryCommitment"));
		if (!pfn_vkGetDeviceMemoryCommitment)
			return false;
		pfn_vkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(load_func_with_device("vkBindBufferMemory"));
		if (!pfn_vkBindBufferMemory)
			return false;
		pfn_vkBindImageMemory = reinterpret_cast<PFN_vkBindImageMemory>(load_func_with_device("vkBindImageMemory"));
		if (!pfn_vkBindImageMemory)
			return false;
		pfn_vkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(load_func_with_device("vkGetBufferMemoryRequirements"));
		if (!pfn_vkGetBufferMemoryRequirements)
			return false;
		pfn_vkGetImageMemoryRequirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(load_func_with_device("vkGetImageMemoryRequirements"));
		if (!pfn_vkGetImageMemoryRequirements)
			return false;
		pfn_vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(load_func_with_device("vkGetImageSparseMemoryRequirements"));
		if (!pfn_vkGetImageSparseMemoryRequirements)
			return false;
		pfn_vkQueueBindSparse = reinterpret_cast<PFN_vkQueueBindSparse>(load_func_with_device("vkQueueBindSparse"));
		if (!pfn_vkQueueBindSparse)
			return false;
		pfn_vkCreateFence = reinterpret_cast<PFN_vkCreateFence>(load_func_with_device("vkCreateFence"));
		if (!pfn_vkCreateFence)
			return false;
		pfn_vkDestroyFence = reinterpret_cast<PFN_vkDestroyFence>(load_func_with_device("vkDestroyFence"));
		if (!pfn_vkDestroyFence)
			return false;
		pfn_vkResetFences = reinterpret_cast<PFN_vkResetFences>(load_func_with_device("vkResetFences"));
		if (!pfn_vkResetFences)
			return false;
		pfn_vkGetFenceStatus = reinterpret_cast<PFN_vkGetFenceStatus>(load_func_with_device("vkGetFenceStatus"));
		if (!pfn_vkGetFenceStatus)
			return false;
		pfn_vkWaitForFences = reinterpret_cast<PFN_vkWaitForFences>(load_func_with_device("vkWaitForFences"));
		if (!pfn_vkWaitForFences)
			return false;
		pfn_vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(load_func_with_device("vkCreateSemaphore"));
		if (!pfn_vkCreateSemaphore)
			return false;
		pfn_vkDestroySemaphore = reinterpret_cast<PFN_vkDestroySemaphore>(load_func_with_device("vkDestroySemaphore"));
		if (!pfn_vkDestroySemaphore)
			return false;
		pfn_vkCreateEvent = reinterpret_cast<PFN_vkCreateEvent>(load_func_with_device("vkCreateEvent"));
		if (!pfn_vkCreateEvent)
			return false;
		pfn_vkDestroyEvent = reinterpret_cast<PFN_vkDestroyEvent>(load_func_with_device("vkDestroyEvent"));
		if (!pfn_vkDestroyEvent)
			return false;
		pfn_vkGetEventStatus = reinterpret_cast<PFN_vkGetEventStatus>(load_func_with_device("vkGetEventStatus"));
		if (!pfn_vkGetEventStatus)
			return false;
		pfn_vkSetEvent = reinterpret_cast<PFN_vkSetEvent>(load_func_with_device("vkSetEvent"));
		if (!pfn_vkSetEvent)
			return false;
		pfn_vkResetEvent = reinterpret_cast<PFN_vkResetEvent>(load_func_with_device("vkResetEvent"));
		if (!pfn_vkResetEvent)
			return false;
		pfn_vkCreateQueryPool = reinterpret_cast<PFN_vkCreateQueryPool>(load_func_with_device("vkCreateQueryPool"));
		if (!pfn_vkCreateQueryPool)
			return false;
		pfn_vkDestroyQueryPool = reinterpret_cast<PFN_vkDestroyQueryPool>(load_func_with_device("vkDestroyQueryPool"));
		if (!pfn_vkDestroyQueryPool)
			return false;
		pfn_vkGetQueryPoolResults = reinterpret_cast<PFN_vkGetQueryPoolResults>(load_func_with_device("vkGetQueryPoolResults"));
		if (!pfn_vkGetQueryPoolResults)
			return false;
		pfn_vkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(load_func_with_device("vkCreateBuffer"));
		if (!pfn_vkCreateBuffer)
			return false;
		pfn_vkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(load_func_with_device("vkDestroyBuffer"));
		if (!pfn_vkDestroyBuffer)
			return false;
		pfn_vkCreateBufferView = reinterpret_cast<PFN_vkCreateBufferView>(load_func_with_device("vkCreateBufferView"));
		if (!pfn_vkCreateBufferView)
			return false;
		pfn_vkDestroyBufferView = reinterpret_cast<PFN_vkDestroyBufferView>(load_func_with_device("vkDestroyBufferView"));
		if (!pfn_vkDestroyBufferView)
			return false;
		pfn_vkCreateImage = reinterpret_cast<PFN_vkCreateImage>(load_func_with_device("vkCreateImage"));
		if (!pfn_vkCreateImage)
			return false;
		pfn_vkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(load_func_with_device("vkDestroyImage"));
		if (!pfn_vkDestroyImage)
			return false;
		pfn_vkGetImageSubresourceLayout = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(load_func_with_device("vkGetImageSubresourceLayout"));
		if (!pfn_vkGetImageSubresourceLayout)
			return false;
		pfn_vkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(load_func_with_device("vkCreateImageView"));
		if (!pfn_vkCreateImageView)
			return false;
		pfn_vkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(load_func_with_device("vkDestroyImageView"));
		if (!pfn_vkDestroyImageView)
			return false;
		pfn_vkCreateShaderModule = reinterpret_cast<PFN_vkCreateShaderModule>(load_func_with_device("vkCreateShaderModule"));
		if (!pfn_vkCreateShaderModule)
			return false;
		pfn_vkDestroyShaderModule = reinterpret_cast<PFN_vkDestroyShaderModule>(load_func_with_device("vkDestroyShaderModule"));
		if (!pfn_vkDestroyShaderModule)
			return false;
		pfn_vkCreatePipelineCache = reinterpret_cast<PFN_vkCreatePipelineCache>(load_func_with_device("vkCreatePipelineCache"));
		if (!pfn_vkCreatePipelineCache)
			return false;
		pfn_vkDestroyPipelineCache = reinterpret_cast<PFN_vkDestroyPipelineCache>(load_func_with_device("vkDestroyPipelineCache"));
		if (!pfn_vkDestroyPipelineCache)
			return false;
		pfn_vkGetPipelineCacheData = reinterpret_cast<PFN_vkGetPipelineCacheData>(load_func_with_device("vkGetPipelineCacheData"));
		if (!pfn_vkGetPipelineCacheData)
			return false;
		pfn_vkMergePipelineCaches = reinterpret_cast<PFN_vkMergePipelineCaches>(load_func_with_device("vkMergePipelineCaches"));
		if (!pfn_vkMergePipelineCaches)
			return false;
		pfn_vkCreateGraphicsPipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(load_func_with_device("vkCreateGraphicsPipelines"));
		if (!pfn_vkCreateGraphicsPipelines)
			return false;
		pfn_vkCreateComputePipelines = reinterpret_cast<PFN_vkCreateComputePipelines>(load_func_with_device("vkCreateComputePipelines"));
		if (!pfn_vkCreateComputePipelines)
			return false;
		pfn_vkDestroyPipeline = reinterpret_cast<PFN_vkDestroyPipeline>(load_func_with_device("vkDestroyPipeline"));
		if (!pfn_vkDestroyPipeline)
			return false;
		pfn_vkCreatePipelineLayout = reinterpret_cast<PFN_vkCreatePipelineLayout>(load_func_with_device("vkCreatePipelineLayout"));
		if (!pfn_vkCreatePipelineLayout)
			return false;
		pfn_vkDestroyPipelineLayout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(load_func_with_device("vkDestroyPipelineLayout"));
		if (!pfn_vkDestroyPipelineLayout)
			return false;
		pfn_vkCreateSampler = reinterpret_cast<PFN_vkCreateSampler>(load_func_with_device("vkCreateSampler"));
		if (!pfn_vkCreateSampler)
			return false;
		pfn_vkDestroySampler = reinterpret_cast<PFN_vkDestroySampler>(load_func_with_device("vkDestroySampler"));
		if (!pfn_vkDestroySampler)
			return false;
		pfn_vkCreateDescriptorSetLayout = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(load_func_with_device("vkCreateDescriptorSetLayout"));
		if (!pfn_vkCreateDescriptorSetLayout)
			return false;
		pfn_vkDestroyDescriptorSetLayout = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(load_func_with_device("vkDestroyDescriptorSetLayout"));
		if (!pfn_vkDestroyDescriptorSetLayout)
			return false;
		pfn_vkCreateDescriptorPool = reinterpret_cast<PFN_vkCreateDescriptorPool>(load_func_with_device("vkCreateDescriptorPool"));
		if (!pfn_vkCreateDescriptorPool)
			return false;
		pfn_vkDestroyDescriptorPool = reinterpret_cast<PFN_vkDestroyDescriptorPool>(load_func_with_device("vkDestroyDescriptorPool"));
		if (!pfn_vkDestroyDescriptorPool)
			return false;
		pfn_vkResetDescriptorPool = reinterpret_cast<PFN_vkResetDescriptorPool>(load_func_with_device("vkResetDescriptorPool"));
		if (!pfn_vkResetDescriptorPool)
			return false;
		pfn_vkAllocateDescriptorSets = reinterpret_cast<PFN_vkAllocateDescriptorSets>(load_func_with_device("vkAllocateDescriptorSets"));
		if (!pfn_vkAllocateDescriptorSets)
			return false;
		pfn_vkFreeDescriptorSets = reinterpret_cast<PFN_vkFreeDescriptorSets>(load_func_with_device("vkFreeDescriptorSets"));
		if (!pfn_vkFreeDescriptorSets)
			return false;
		pfn_vkUpdateDescriptorSets = reinterpret_cast<PFN_vkUpdateDescriptorSets>(load_func_with_device("vkUpdateDescriptorSets"));
		if (!pfn_vkUpdateDescriptorSets)
			return false;
		pfn_vkCreateFramebuffer = reinterpret_cast<PFN_vkCreateFramebuffer>(load_func_with_device("vkCreateFramebuffer"));
		if (!pfn_vkCreateFramebuffer)
			return false;
		pfn_vkDestroyFramebuffer = reinterpret_cast<PFN_vkDestroyFramebuffer>(load_func_with_device("vkDestroyFramebuffer"));
		if (!pfn_vkDestroyFramebuffer)
			return false;
		pfn_vkCreateRenderPass = reinterpret_cast<PFN_vkCreateRenderPass>(load_func_with_device("vkCreateRenderPass"));
		if (!pfn_vkCreateRenderPass)
			return false;
		pfn_vkDestroyRenderPass = reinterpret_cast<PFN_vkDestroyRenderPass>(load_func_with_device("vkDestroyRenderPass"));
		if (!pfn_vkDestroyRenderPass)
			return false;
		pfn_vkGetRenderAreaGranularity = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(load_func_with_device("vkGetRenderAreaGranularity"));
		if (!pfn_vkGetRenderAreaGranularity)
			return false;
		pfn_vkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(load_func_with_device("vkCreateCommandPool"));
		if (!pfn_vkCreateCommandPool)
			return false;
		pfn_vkDestroyCommandPool = reinterpret_cast<PFN_vkDestroyCommandPool>(load_func_with_device("vkDestroyCommandPool"));
		if (!pfn_vkDestroyCommandPool)
			return false;
		pfn_vkResetCommandPool = reinterpret_cast<PFN_vkResetCommandPool>(load_func_with_device("vkResetCommandPool"));
		if (!pfn_vkResetCommandPool)
			return false;
		pfn_vkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(load_func_with_device("vkAllocateCommandBuffers"));
		if (!pfn_vkAllocateCommandBuffers)
			return false;
		pfn_vkFreeCommandBuffers = reinterpret_cast<PFN_vkFreeCommandBuffers>(load_func_with_device("vkFreeCommandBuffers"));
		if (!pfn_vkFreeCommandBuffers)
			return false;
		pfn_vkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(load_func_with_device("vkBeginCommandBuffer"));
		if (!pfn_vkBeginCommandBuffer)
			return false;
		pfn_vkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(load_func_with_device("vkEndCommandBuffer"));
		if (!pfn_vkEndCommandBuffer)
			return false;
		pfn_vkResetCommandBuffer = reinterpret_cast<PFN_vkResetCommandBuffer>(load_func_with_device("vkResetCommandBuffer"));
		if (!pfn_vkResetCommandBuffer)
			return false;
		pfn_vkCmdBindPipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(load_func_with_device("vkCmdBindPipeline"));
		if (!pfn_vkCmdBindPipeline)
			return false;
		pfn_vkCmdSetViewport = reinterpret_cast<PFN_vkCmdSetViewport>(load_func_with_device("vkCmdSetViewport"));
		if (!pfn_vkCmdSetViewport)
			return false;
		pfn_vkCmdSetScissor = reinterpret_cast<PFN_vkCmdSetScissor>(load_func_with_device("vkCmdSetScissor"));
		if (!pfn_vkCmdSetScissor)
			return false;
		pfn_vkCmdSetLineWidth = reinterpret_cast<PFN_vkCmdSetLineWidth>(load_func_with_device("vkCmdSetLineWidth"));
		if (!pfn_vkCmdSetLineWidth)
			return false;
		pfn_vkCmdSetDepthBias = reinterpret_cast<PFN_vkCmdSetDepthBias>(load_func_with_device("vkCmdSetDepthBias"));
		if (!pfn_vkCmdSetDepthBias)
			return false;
		pfn_vkCmdSetBlendConstants = reinterpret_cast<PFN_vkCmdSetBlendConstants>(load_func_with_device("vkCmdSetBlendConstants"));
		if (!pfn_vkCmdSetBlendConstants)
			return false;
		pfn_vkCmdSetDepthBounds = reinterpret_cast<PFN_vkCmdSetDepthBounds>(load_func_with_device("vkCmdSetDepthBounds"));
		if (!pfn_vkCmdSetDepthBounds)
			return false;
		pfn_vkCmdSetStencilCompareMask = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(load_func_with_device("vkCmdSetStencilCompareMask"));
		if (!pfn_vkCmdSetStencilCompareMask)
			return false;
		pfn_vkCmdSetStencilWriteMask = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(load_func_with_device("vkCmdSetStencilWriteMask"));
		if (!pfn_vkCmdSetStencilWriteMask)
			return false;
		pfn_vkCmdSetStencilReference = reinterpret_cast<PFN_vkCmdSetStencilReference>(load_func_with_device("vkCmdSetStencilReference"));
		if (!pfn_vkCmdSetStencilReference)
			return false;
		pfn_vkCmdBindDescriptorSets = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(load_func_with_device("vkCmdBindDescriptorSets"));
		if (!pfn_vkCmdBindDescriptorSets)
			return false;
		pfn_vkCmdBindIndexBuffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(load_func_with_device("vkCmdBindIndexBuffer"));
		if (!pfn_vkCmdBindIndexBuffer)
			return false;
		pfn_vkCmdBindVertexBuffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(load_func_with_device("vkCmdBindVertexBuffers"));
		if (!pfn_vkCmdBindVertexBuffers)
			return false;
		pfn_vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(load_func_with_device("vkCmdDraw"));
		if (!pfn_vkCmdDraw)
			return false;
		pfn_vkCmdDrawIndexed = reinterpret_cast<PFN_vkCmdDrawIndexed>(load_func_with_device("vkCmdDrawIndexed"));
		if (!pfn_vkCmdDrawIndexed)
			return false;
		pfn_vkCmdDrawIndirect = reinterpret_cast<PFN_vkCmdDrawIndirect>(load_func_with_device("vkCmdDrawIndirect"));
		if (!pfn_vkCmdDrawIndirect)
			return false;
		pfn_vkCmdDrawIndexedIndirect = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(load_func_with_device("vkCmdDrawIndexedIndirect"));
		if (!pfn_vkCmdDrawIndexedIndirect)
			return false;
		pfn_vkCmdDispatch = reinterpret_cast<PFN_vkCmdDispatch>(load_func_with_device("vkCmdDispatch"));
		if (!pfn_vkCmdDispatch)
			return false;
		pfn_vkCmdDispatchIndirect = reinterpret_cast<PFN_vkCmdDispatchIndirect>(load_func_with_device("vkCmdDispatchIndirect"));
		if (!pfn_vkCmdDispatchIndirect)
			return false;
		pfn_vkCmdCopyBuffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(load_func_with_device("vkCmdCopyBuffer"));
		if (!pfn_vkCmdCopyBuffer)
			return false;
		pfn_vkCmdCopyImage = reinterpret_cast<PFN_vkCmdCopyImage>(load_func_with_device("vkCmdCopyImage"));
		if (!pfn_vkCmdCopyImage)
			return false;
		pfn_vkCmdBlitImage = reinterpret_cast<PFN_vkCmdBlitImage>(load_func_with_device("vkCmdBlitImage"));
		if (!pfn_vkCmdBlitImage)
			return false;
		pfn_vkCmdCopyBufferToImage = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(load_func_with_device("vkCmdCopyBufferToImage"));
		if (!pfn_vkCmdCopyBufferToImage)
			return false;
		pfn_vkCmdCopyImageToBuffer = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(load_func_with_device("vkCmdCopyImageToBuffer"));
		if (!pfn_vkCmdCopyImageToBuffer)
			return false;
		pfn_vkCmdUpdateBuffer = reinterpret_cast<PFN_vkCmdUpdateBuffer>(load_func_with_device("vkCmdUpdateBuffer"));
		if (!pfn_vkCmdUpdateBuffer)
			return false;
		pfn_vkCmdFillBuffer = reinterpret_cast<PFN_vkCmdFillBuffer>(load_func_with_device("vkCmdFillBuffer"));
		if (!pfn_vkCmdFillBuffer)
			return false;
		pfn_vkCmdClearColorImage = reinterpret_cast<PFN_vkCmdClearColorImage>(load_func_with_device("vkCmdClearColorImage"));
		if (!pfn_vkCmdClearColorImage)
			return false;
		pfn_vkCmdClearDepthStencilImage = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(load_func_with_device("vkCmdClearDepthStencilImage"));
		if (!pfn_vkCmdClearDepthStencilImage)
			return false;
		pfn_vkCmdClearAttachments = reinterpret_cast<PFN_vkCmdClearAttachments>(load_func_with_device("vkCmdClearAttachments"));
		if (!pfn_vkCmdClearAttachments)
			return false;
		pfn_vkCmdResolveImage = reinterpret_cast<PFN_vkCmdResolveImage>(load_func_with_device("vkCmdResolveImage"));
		if (!pfn_vkCmdResolveImage)
			return false;
		pfn_vkCmdSetEvent = reinterpret_cast<PFN_vkCmdSetEvent>(load_func_with_device("vkCmdSetEvent"));
		if (!pfn_vkCmdSetEvent)
			return false;
		pfn_vkCmdResetEvent = reinterpret_cast<PFN_vkCmdResetEvent>(load_func_with_device("vkCmdResetEvent"));
		if (!pfn_vkCmdResetEvent)
			return false;
		pfn_vkCmdWaitEvents = reinterpret_cast<PFN_vkCmdWaitEvents>(load_func_with_device("vkCmdWaitEvents"));
		if (!pfn_vkCmdWaitEvents)
			return false;
		pfn_vkCmdPipelineBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(load_func_with_device("vkCmdPipelineBarrier"));
		if (!pfn_vkCmdPipelineBarrier)
			return false;
		pfn_vkCmdBeginQuery = reinterpret_cast<PFN_vkCmdBeginQuery>(load_func_with_device("vkCmdBeginQuery"));
		if (!pfn_vkCmdBeginQuery)
			return false;
		pfn_vkCmdEndQuery = reinterpret_cast<PFN_vkCmdEndQuery>(load_func_with_device("vkCmdEndQuery"));
		if (!pfn_vkCmdEndQuery)
			return false;
		pfn_vkCmdResetQueryPool = reinterpret_cast<PFN_vkCmdResetQueryPool>(load_func_with_device("vkCmdResetQueryPool"));
		if (!pfn_vkCmdResetQueryPool)
			return false;
		pfn_vkCmdWriteTimestamp = reinterpret_cast<PFN_vkCmdWriteTimestamp>(load_func_with_device("vkCmdWriteTimestamp"));
		if (!pfn_vkCmdWriteTimestamp)
			return false;
		pfn_vkCmdCopyQueryPoolResults = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(load_func_with_device("vkCmdCopyQueryPoolResults"));
		if (!pfn_vkCmdCopyQueryPoolResults)
			return false;
		pfn_vkCmdPushConstants = reinterpret_cast<PFN_vkCmdPushConstants>(load_func_with_device("vkCmdPushConstants"));
		if (!pfn_vkCmdPushConstants)
			return false;
		pfn_vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(load_func_with_device("vkCmdBeginRenderPass"));
		if (!pfn_vkCmdBeginRenderPass)
			return false;
		pfn_vkCmdNextSubpass = reinterpret_cast<PFN_vkCmdNextSubpass>(load_func_with_device("vkCmdNextSubpass"));
		if (!pfn_vkCmdNextSubpass)
			return false;
		pfn_vkCmdEndRenderPass = reinterpret_cast<PFN_vkCmdEndRenderPass>(load_func_with_device("vkCmdEndRenderPass"));
		if (!pfn_vkCmdEndRenderPass)
			return false;
		pfn_vkCmdExecuteCommands = reinterpret_cast<PFN_vkCmdExecuteCommands>(load_func_with_device("vkCmdExecuteCommands"));
		if (!pfn_vkCmdExecuteCommands)
			return false;
		return true;
	}

	static bool load_vulkan_1_1_device() {
		pfn_vkBindBufferMemory2 = reinterpret_cast<PFN_vkBindBufferMemory2>(load_func_with_device("vkBindBufferMemory2"));
		if (!pfn_vkBindBufferMemory2)
			return false;
		pfn_vkBindImageMemory2 = reinterpret_cast<PFN_vkBindImageMemory2>(load_func_with_device("vkBindImageMemory2"));
		if (!pfn_vkBindImageMemory2)
			return false;
		pfn_vkGetDeviceGroupPeerMemoryFeatures = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeatures>(load_func_with_device("vkGetDeviceGroupPeerMemoryFeatures"));
		if (!pfn_vkGetDeviceGroupPeerMemoryFeatures)
			return false;
		pfn_vkCmdSetDeviceMask = reinterpret_cast<PFN_vkCmdSetDeviceMask>(load_func_with_device("vkCmdSetDeviceMask"));
		if (!pfn_vkCmdSetDeviceMask)
			return false;
		pfn_vkCmdDispatchBase = reinterpret_cast<PFN_vkCmdDispatchBase>(load_func_with_device("vkCmdDispatchBase"));
		if (!pfn_vkCmdDispatchBase)
			return false;
		pfn_vkGetImageMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageMemoryRequirements2>(load_func_with_device("vkGetImageMemoryRequirements2"));
		if (!pfn_vkGetImageMemoryRequirements2)
			return false;
		pfn_vkGetBufferMemoryRequirements2 = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2>(load_func_with_device("vkGetBufferMemoryRequirements2"));
		if (!pfn_vkGetBufferMemoryRequirements2)
			return false;
		pfn_vkGetImageSparseMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2>(load_func_with_device("vkGetImageSparseMemoryRequirements2"));
		if (!pfn_vkGetImageSparseMemoryRequirements2)
			return false;
		pfn_vkTrimCommandPool = reinterpret_cast<PFN_vkTrimCommandPool>(load_func_with_device("vkTrimCommandPool"));
		if (!pfn_vkTrimCommandPool)
			return false;
		pfn_vkGetDeviceQueue2 = reinterpret_cast<PFN_vkGetDeviceQueue2>(load_func_with_device("vkGetDeviceQueue2"));
		if (!pfn_vkGetDeviceQueue2)
			return false;
		pfn_vkCreateSamplerYcbcrConversion = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversion>(load_func_with_device("vkCreateSamplerYcbcrConversion"));
		if (!pfn_vkCreateSamplerYcbcrConversion)
			return false;
		pfn_vkDestroySamplerYcbcrConversion = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversion>(load_func_with_device("vkDestroySamplerYcbcrConversion"));
		if (!pfn_vkDestroySamplerYcbcrConversion)
			return false;
		pfn_vkCreateDescriptorUpdateTemplate = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplate>(load_func_with_device("vkCreateDescriptorUpdateTemplate"));
		if (!pfn_vkCreateDescriptorUpdateTemplate)
			return false;
		pfn_vkDestroyDescriptorUpdateTemplate = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplate>(load_func_with_device("vkDestroyDescriptorUpdateTemplate"));
		if (!pfn_vkDestroyDescriptorUpdateTemplate)
			return false;
		pfn_vkUpdateDescriptorSetWithTemplate = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplate>(load_func_with_device("vkUpdateDescriptorSetWithTemplate"));
		if (!pfn_vkUpdateDescriptorSetWithTemplate)
			return false;
		pfn_vkGetDescriptorSetLayoutSupport = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupport>(load_func_with_device("vkGetDescriptorSetLayoutSupport"));
		if (!pfn_vkGetDescriptorSetLayoutSupport)
			return false;
		return true;
	}

	static bool load_vulkan_1_2_device() {
		pfn_vkCmdDrawIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndirectCount>(load_func_with_device("vkCmdDrawIndirectCount"));
		if (!pfn_vkCmdDrawIndirectCount)
			return false;
		pfn_vkCmdDrawIndexedIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCount>(load_func_with_device("vkCmdDrawIndexedIndirectCount"));
		if (!pfn_vkCmdDrawIndexedIndirectCount)
			return false;
		pfn_vkCreateRenderPass2 = reinterpret_cast<PFN_vkCreateRenderPass2>(load_func_with_device("vkCreateRenderPass2"));
		if (!pfn_vkCreateRenderPass2)
			return false;
		pfn_vkCmdBeginRenderPass2 = reinterpret_cast<PFN_vkCmdBeginRenderPass2>(load_func_with_device("vkCmdBeginRenderPass2"));
		if (!pfn_vkCmdBeginRenderPass2)
			return false;
		pfn_vkCmdNextSubpass2 = reinterpret_cast<PFN_vkCmdNextSubpass2>(load_func_with_device("vkCmdNextSubpass2"));
		if (!pfn_vkCmdNextSubpass2)
			return false;
		pfn_vkCmdEndRenderPass2 = reinterpret_cast<PFN_vkCmdEndRenderPass2>(load_func_with_device("vkCmdEndRenderPass2"));
		if (!pfn_vkCmdEndRenderPass2)
			return false;
		pfn_vkResetQueryPool = reinterpret_cast<PFN_vkResetQueryPool>(load_func_with_device("vkResetQueryPool"));
		if (!pfn_vkResetQueryPool)
			return false;
		pfn_vkGetSemaphoreCounterValue = reinterpret_cast<PFN_vkGetSemaphoreCounterValue>(load_func_with_device("vkGetSemaphoreCounterValue"));
		if (!pfn_vkGetSemaphoreCounterValue)
			return false;
		pfn_vkWaitSemaphores = reinterpret_cast<PFN_vkWaitSemaphores>(load_func_with_device("vkWaitSemaphores"));
		if (!pfn_vkWaitSemaphores)
			return false;
		pfn_vkSignalSemaphore = reinterpret_cast<PFN_vkSignalSemaphore>(load_func_with_device("vkSignalSemaphore"));
		if (!pfn_vkSignalSemaphore)
			return false;
		pfn_vkGetBufferDeviceAddress = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(load_func_with_device("vkGetBufferDeviceAddress"));
		if (!pfn_vkGetBufferDeviceAddress)
			return false;
		pfn_vkGetBufferOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddress>(load_func_with_device("vkGetBufferOpaqueCaptureAddress"));
		if (!pfn_vkGetBufferOpaqueCaptureAddress)
			return false;
		pfn_vkGetDeviceMemoryOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddress>(load_func_with_device("vkGetDeviceMemoryOpaqueCaptureAddress"));
		if (!pfn_vkGetDeviceMemoryOpaqueCaptureAddress)
			return false;
		return true;
	}

	static bool load_vulkan_1_3_device() {
		pfn_vkCreatePrivateDataSlot = reinterpret_cast<PFN_vkCreatePrivateDataSlot>(load_func_with_device("vkCreatePrivateDataSlot"));
		if (!pfn_vkCreatePrivateDataSlot)
			return false;
		pfn_vkDestroyPrivateDataSlot = reinterpret_cast<PFN_vkDestroyPrivateDataSlot>(load_func_with_device("vkDestroyPrivateDataSlot"));
		if (!pfn_vkDestroyPrivateDataSlot)
			return false;
		pfn_vkSetPrivateData = reinterpret_cast<PFN_vkSetPrivateData>(load_func_with_device("vkSetPrivateData"));
		if (!pfn_vkSetPrivateData)
			return false;
		pfn_vkGetPrivateData = reinterpret_cast<PFN_vkGetPrivateData>(load_func_with_device("vkGetPrivateData"));
		if (!pfn_vkGetPrivateData)
			return false;
		pfn_vkCmdSetEvent2 = reinterpret_cast<PFN_vkCmdSetEvent2>(load_func_with_device("vkCmdSetEvent2"));
		if (!pfn_vkCmdSetEvent2)
			return false;
		pfn_vkCmdResetEvent2 = reinterpret_cast<PFN_vkCmdResetEvent2>(load_func_with_device("vkCmdResetEvent2"));
		if (!pfn_vkCmdResetEvent2)
			return false;
		pfn_vkCmdWaitEvents2 = reinterpret_cast<PFN_vkCmdWaitEvents2>(load_func_with_device("vkCmdWaitEvents2"));
		if (!pfn_vkCmdWaitEvents2)
			return false;
		pfn_vkCmdPipelineBarrier2 = reinterpret_cast<PFN_vkCmdPipelineBarrier2>(load_func_with_device("vkCmdPipelineBarrier2"));
		if (!pfn_vkCmdPipelineBarrier2)
			return false;
		pfn_vkCmdWriteTimestamp2 = reinterpret_cast<PFN_vkCmdWriteTimestamp2>(load_func_with_device("vkCmdWriteTimestamp2"));
		if (!pfn_vkCmdWriteTimestamp2)
			return false;
		pfn_vkQueueSubmit2 = reinterpret_cast<PFN_vkQueueSubmit2>(load_func_with_device("vkQueueSubmit2"));
		if (!pfn_vkQueueSubmit2)
			return false;
		pfn_vkCmdCopyBuffer2 = reinterpret_cast<PFN_vkCmdCopyBuffer2>(load_func_with_device("vkCmdCopyBuffer2"));
		if (!pfn_vkCmdCopyBuffer2)
			return false;
		pfn_vkCmdCopyImage2 = reinterpret_cast<PFN_vkCmdCopyImage2>(load_func_with_device("vkCmdCopyImage2"));
		if (!pfn_vkCmdCopyImage2)
			return false;
		pfn_vkCmdCopyBufferToImage2 = reinterpret_cast<PFN_vkCmdCopyBufferToImage2>(load_func_with_device("vkCmdCopyBufferToImage2"));
		if (!pfn_vkCmdCopyBufferToImage2)
			return false;
		pfn_vkCmdCopyImageToBuffer2 = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2>(load_func_with_device("vkCmdCopyImageToBuffer2"));
		if (!pfn_vkCmdCopyImageToBuffer2)
			return false;
		pfn_vkCmdBlitImage2 = reinterpret_cast<PFN_vkCmdBlitImage2>(load_func_with_device("vkCmdBlitImage2"));
		if (!pfn_vkCmdBlitImage2)
			return false;
		pfn_vkCmdResolveImage2 = reinterpret_cast<PFN_vkCmdResolveImage2>(load_func_with_device("vkCmdResolveImage2"));
		if (!pfn_vkCmdResolveImage2)
			return false;
		pfn_vkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(load_func_with_device("vkCmdBeginRendering"));
		if (!pfn_vkCmdBeginRendering)
			return false;
		pfn_vkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(load_func_with_device("vkCmdEndRendering"));
		if (!pfn_vkCmdEndRendering)
			return false;
		pfn_vkCmdSetCullMode = reinterpret_cast<PFN_vkCmdSetCullMode>(load_func_with_device("vkCmdSetCullMode"));
		if (!pfn_vkCmdSetCullMode)
			return false;
		pfn_vkCmdSetFrontFace = reinterpret_cast<PFN_vkCmdSetFrontFace>(load_func_with_device("vkCmdSetFrontFace"));
		if (!pfn_vkCmdSetFrontFace)
			return false;
		pfn_vkCmdSetPrimitiveTopology = reinterpret_cast<PFN_vkCmdSetPrimitiveTopology>(load_func_with_device("vkCmdSetPrimitiveTopology"));
		if (!pfn_vkCmdSetPrimitiveTopology)
			return false;
		pfn_vkCmdSetViewportWithCount = reinterpret_cast<PFN_vkCmdSetViewportWithCount>(load_func_with_device("vkCmdSetViewportWithCount"));
		if (!pfn_vkCmdSetViewportWithCount)
			return false;
		pfn_vkCmdSetScissorWithCount = reinterpret_cast<PFN_vkCmdSetScissorWithCount>(load_func_with_device("vkCmdSetScissorWithCount"));
		if (!pfn_vkCmdSetScissorWithCount)
			return false;
		pfn_vkCmdBindVertexBuffers2 = reinterpret_cast<PFN_vkCmdBindVertexBuffers2>(load_func_with_device("vkCmdBindVertexBuffers2"));
		if (!pfn_vkCmdBindVertexBuffers2)
			return false;
		pfn_vkCmdSetDepthTestEnable = reinterpret_cast<PFN_vkCmdSetDepthTestEnable>(load_func_with_device("vkCmdSetDepthTestEnable"));
		if (!pfn_vkCmdSetDepthTestEnable)
			return false;
		pfn_vkCmdSetDepthWriteEnable = reinterpret_cast<PFN_vkCmdSetDepthWriteEnable>(load_func_with_device("vkCmdSetDepthWriteEnable"));
		if (!pfn_vkCmdSetDepthWriteEnable)
			return false;
		pfn_vkCmdSetDepthCompareOp = reinterpret_cast<PFN_vkCmdSetDepthCompareOp>(load_func_with_device("vkCmdSetDepthCompareOp"));
		if (!pfn_vkCmdSetDepthCompareOp)
			return false;
		pfn_vkCmdSetDepthBoundsTestEnable = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnable>(load_func_with_device("vkCmdSetDepthBoundsTestEnable"));
		if (!pfn_vkCmdSetDepthBoundsTestEnable)
			return false;
		pfn_vkCmdSetStencilTestEnable = reinterpret_cast<PFN_vkCmdSetStencilTestEnable>(load_func_with_device("vkCmdSetStencilTestEnable"));
		if (!pfn_vkCmdSetStencilTestEnable)
			return false;
		pfn_vkCmdSetStencilOp = reinterpret_cast<PFN_vkCmdSetStencilOp>(load_func_with_device("vkCmdSetStencilOp"));
		if (!pfn_vkCmdSetStencilOp)
			return false;
		pfn_vkCmdSetRasterizerDiscardEnable = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnable>(load_func_with_device("vkCmdSetRasterizerDiscardEnable"));
		if (!pfn_vkCmdSetRasterizerDiscardEnable)
			return false;
		pfn_vkCmdSetDepthBiasEnable = reinterpret_cast<PFN_vkCmdSetDepthBiasEnable>(load_func_with_device("vkCmdSetDepthBiasEnable"));
		if (!pfn_vkCmdSetDepthBiasEnable)
			return false;
		pfn_vkCmdSetPrimitiveRestartEnable = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnable>(load_func_with_device("vkCmdSetPrimitiveRestartEnable"));
		if (!pfn_vkCmdSetPrimitiveRestartEnable)
			return false;
		pfn_vkGetDeviceBufferMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirements>(load_func_with_device("vkGetDeviceBufferMemoryRequirements"));
		if (!pfn_vkGetDeviceBufferMemoryRequirements)
			return false;
		pfn_vkGetDeviceImageMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirements>(load_func_with_device("vkGetDeviceImageMemoryRequirements"));
		if (!pfn_vkGetDeviceImageMemoryRequirements)
			return false;
		pfn_vkGetDeviceImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirements>(load_func_with_device("vkGetDeviceImageSparseMemoryRequirements"));
		if (!pfn_vkGetDeviceImageSparseMemoryRequirements)
			return false;
		return true;
	}

	/* static bool load_vulkan_1_4_device() {
		pfn_vkCmdSetLineStipple = reinterpret_cast<PFN_vkCmdSetLineStipple>(load_func_with_device("vkCmdSetLineStipple"));
		if (!pfn_vkCmdSetLineStipple)
			return false;
		pfn_vkMapMemory2 = reinterpret_cast<PFN_vkMapMemory2>(load_func_with_device("vkMapMemory2"));
		if (!pfn_vkMapMemory2)
			return false;
		pfn_vkUnmapMemory2 = reinterpret_cast<PFN_vkUnmapMemory2>(load_func_with_device("vkUnmapMemory2"));
		if (!pfn_vkUnmapMemory2)
			return false;
		pfn_vkCmdBindIndexBuffer2 = reinterpret_cast<PFN_vkCmdBindIndexBuffer2>(load_func_with_device("vkCmdBindIndexBuffer2"));
		if (!pfn_vkCmdBindIndexBuffer2)
			return false;
		pfn_vkGetRenderingAreaGranularity = reinterpret_cast<PFN_vkGetRenderingAreaGranularity>(load_func_with_device("vkCmdSetLineStipple"));
		if (!pfn_vkGetRenderingAreaGranularity)
			return false;
		pfn_vkGetDeviceImageSubresourceLayout = reinterpret_cast<PFN_vkGetDeviceImageSubresourceLayout>(load_func_with_device("vkGetDeviceImageSubresourceLayout"));
		if (!pfn_vkGetDeviceImageSubresourceLayout)
			return false;
		pfn_vkGetImageSubresourceLayout2 = reinterpret_cast<PFN_vkGetImageSubresourceLayout2>(load_func_with_device("vkGetImageSubresourceLayout2"));
		if (!pfn_vkGetImageSubresourceLayout2)
			return false;
		pfn_vkCmdPushDescriptorSet = reinterpret_cast<PFN_vkCmdPushDescriptorSet>(load_func_with_device("vkCmdPushDescriptorSet"));
		if (!pfn_vkCmdPushDescriptorSet)
			return false;
		pfn_vkCmdPushDescriptorSetWithTemplate = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplate>(load_func_with_device("vkCmdPushDescriptorSetWithTemplate"));
		if (!pfn_vkCmdPushDescriptorSetWithTemplate)
			return false;
		pfn_vkCmdSetRenderingAttachmentLocations = reinterpret_cast<PFN_vkCmdSetRenderingAttachmentLocations>(load_func_with_device("vkCmdSetRenderingAttachmentLocations"));
		if (!pfn_vkCmdSetRenderingAttachmentLocations)
			return false;
		pfn_vkCmdSetRenderingInputAttachmentIndices = reinterpret_cast<PFN_vkCmdSetRenderingInputAttachmentIndices>(load_func_with_device("vkCmdSetRenderingInputAttachmentIndices"));
		if (!pfn_vkCmdSetRenderingInputAttachmentIndices)
			return false;
		pfn_vkCmdBindDescriptorSets2 = reinterpret_cast<PFN_vkCmdBindDescriptorSets2>(load_func_with_device("vkCmdBindDescriptorSets2"));
		if (!pfn_vkCmdBindDescriptorSets2)
			return false;
		pfn_vkCmdPushConstants2 = reinterpret_cast<PFN_vkCmdPushConstants2>(load_func_with_device("vkCmdPushConstants2"));
		if (!pfn_vkCmdPushConstants2)
			return false;
		pfn_vkCmdPushDescriptorSet2 = reinterpret_cast<PFN_vkCmdPushDescriptorSet2>(load_func_with_device("vkCmdPushDescriptorSet2"));
		if (!pfn_vkCmdPushDescriptorSet2)
			return false;
		pfn_vkCmdPushDescriptorSetWithTemplate2 = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplate2>(load_func_with_device("vkCmdPushDescriptorSetWithTemplate2"));
		if (!pfn_vkCmdPushDescriptorSetWithTemplate2)
			return false;
		pfn_vkCopyMemoryToImage = reinterpret_cast<PFN_vkCopyMemoryToImage>(load_func_with_device("vkCopyMemoryToImage"));
		if (!pfn_vkCopyMemoryToImage)
			return false;
		pfn_vkCopyImageToMemory = reinterpret_cast<PFN_vkCopyImageToMemory>(load_func_with_device("vkCopyImageToMemory"));
		if (!pfn_vkCopyImageToMemory)
			return false;
		pfn_vkCopyImageToImage = reinterpret_cast<PFN_vkCopyImageToImage>(load_func_with_device("vkCopyImageToImage"));
		if (!pfn_vkCopyImageToImage)
			return false;
		pfn_vkTransitionImageLayout = reinterpret_cast<PFN_vkTransitionImageLayout>(load_func_with_device("vkTransitionImageLayout"));
		if (!pfn_vkTransitionImageLayout)
			return false;
		return true;
	} */

	static bool load_extension_funcs_with_device() {
		pfn_vkQueueBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueBeginDebugUtilsLabelEXT>(load_func_with_device("vkQueueBeginDebugUtilsLabelEXT"));
		if (!pfn_vkQueueBeginDebugUtilsLabelEXT)
			return false;
		pfn_vkQueueEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueEndDebugUtilsLabelEXT>(load_func_with_device("vkQueueEndDebugUtilsLabelEXT"));
		if (!pfn_vkQueueEndDebugUtilsLabelEXT)
			return false;
		pfn_vkQueueInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueInsertDebugUtilsLabelEXT>(load_func_with_device("vkQueueInsertDebugUtilsLabelEXT"));
		if (!pfn_vkQueueInsertDebugUtilsLabelEXT)
			return false;
		pfn_vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(load_func_with_device("vkCmdBeginDebugUtilsLabelEXT"));
		if (!pfn_vkCmdBeginDebugUtilsLabelEXT)
			return false;
		pfn_vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(load_func_with_device("vkCmdEndDebugUtilsLabelEXT"));
		if (!pfn_vkCmdEndDebugUtilsLabelEXT)
			return false;
		pfn_vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(load_func_with_device("vkCmdInsertDebugUtilsLabelEXT"));
		if (!pfn_vkCmdInsertDebugUtilsLabelEXT)
			return false;
		pfn_vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(load_func_with_device("vkCreateSwapchainKHR"));
		if (!pfn_vkCreateSwapchainKHR)
			return false;
		pfn_vkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(load_func_with_device("vkDestroySwapchainKHR"));
		if (!pfn_vkDestroySwapchainKHR)
			return false;
		pfn_vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(load_func_with_device("vkGetSwapchainImagesKHR"));
		if (!pfn_vkGetSwapchainImagesKHR)
			return false;
		pfn_vkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(load_func_with_device("vkAcquireNextImageKHR"));
		if (!pfn_vkAcquireNextImageKHR)
			return false;
		pfn_vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(load_func_with_device("vkQueuePresentKHR"));
		if (!pfn_vkQueuePresentKHR)
			return false;
		pfn_vkGetDeviceGroupPresentCapabilitiesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPresentCapabilitiesKHR>(load_func_with_device("vkGetDeviceGroupPresentCapabilitiesKHR"));
		if (!pfn_vkGetDeviceGroupPresentCapabilitiesKHR)
			return false;
		pfn_vkGetDeviceGroupSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModesKHR>(load_func_with_device("vkGetDeviceGroupSurfacePresentModesKHR"));
		if (!pfn_vkGetDeviceGroupSurfacePresentModesKHR)
			return false;
		pfn_vkAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(load_func_with_device("vkAcquireNextImage2KHR"));
		if (!pfn_vkAcquireNextImage2KHR)
			return false;
		return true;
	}

	// Avoids dangling pointers (don't remove!)
	static void unload_all_vulkan_functions_of_device() {
		// Vulkan 1.0
		pfn_vkGetDeviceQueue = nullptr;
		pfn_vkQueueSubmit = nullptr;
		pfn_vkQueueWaitIdle = nullptr;
		pfn_vkDeviceWaitIdle = nullptr;
		pfn_vkAllocateMemory = nullptr;
		pfn_vkFreeMemory = nullptr;
		pfn_vkMapMemory = nullptr;
		pfn_vkUnmapMemory = nullptr;
		pfn_vkFlushMappedMemoryRanges = nullptr;
		pfn_vkInvalidateMappedMemoryRanges = nullptr;
		pfn_vkGetDeviceMemoryCommitment = nullptr;
		pfn_vkBindBufferMemory = nullptr;
		pfn_vkBindImageMemory = nullptr;
		pfn_vkGetBufferMemoryRequirements = nullptr;
		pfn_vkGetImageMemoryRequirements = nullptr;
		pfn_vkGetImageSparseMemoryRequirements = nullptr;
		pfn_vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;
		pfn_vkQueueBindSparse = nullptr;
		pfn_vkCreateFence = nullptr;
		pfn_vkDestroyFence = nullptr;
		pfn_vkResetFences = nullptr;
		pfn_vkGetFenceStatus = nullptr;
		pfn_vkWaitForFences = nullptr;
		pfn_vkCreateSemaphore = nullptr;
		pfn_vkDestroySemaphore = nullptr;
		pfn_vkCreateEvent = nullptr;
		pfn_vkDestroyEvent = nullptr;
		pfn_vkGetEventStatus = nullptr;
		pfn_vkSetEvent = nullptr;
		pfn_vkResetEvent = nullptr;
		pfn_vkCreateQueryPool = nullptr;
		pfn_vkDestroyQueryPool = nullptr;
		pfn_vkGetQueryPoolResults = nullptr;
		pfn_vkCreateBuffer = nullptr;
		pfn_vkDestroyBuffer = nullptr;
		pfn_vkCreateBufferView = nullptr;
		pfn_vkDestroyBufferView = nullptr;
		pfn_vkCreateImage = nullptr;
		pfn_vkDestroyImage = nullptr;
		pfn_vkGetImageSubresourceLayout = nullptr;
		pfn_vkCreateImageView = nullptr;
		pfn_vkDestroyImageView = nullptr;
		pfn_vkCreateShaderModule = nullptr;
		pfn_vkDestroyShaderModule = nullptr;
		pfn_vkCreatePipelineCache = nullptr;
		pfn_vkDestroyPipelineCache = nullptr;
		pfn_vkGetPipelineCacheData = nullptr;
		pfn_vkMergePipelineCaches = nullptr;
		pfn_vkCreateGraphicsPipelines = nullptr;
		pfn_vkCreateComputePipelines = nullptr;
		pfn_vkDestroyPipeline = nullptr;
		pfn_vkCreatePipelineLayout = nullptr;
		pfn_vkDestroyPipelineLayout = nullptr;
		pfn_vkCreateSampler = nullptr;
		pfn_vkDestroySampler = nullptr;
		pfn_vkCreateDescriptorSetLayout = nullptr;
		pfn_vkDestroyDescriptorSetLayout = nullptr;
		pfn_vkCreateDescriptorPool = nullptr;
		pfn_vkDestroyDescriptorPool = nullptr;
		pfn_vkResetDescriptorPool = nullptr;
		pfn_vkAllocateDescriptorSets = nullptr;
		pfn_vkFreeDescriptorSets = nullptr;
		pfn_vkUpdateDescriptorSets = nullptr;
		pfn_vkCreateFramebuffer = nullptr;
		pfn_vkDestroyFramebuffer = nullptr;
		pfn_vkCreateRenderPass = nullptr;
		pfn_vkDestroyRenderPass = nullptr;
		pfn_vkGetRenderAreaGranularity = nullptr;
		pfn_vkCreateCommandPool = nullptr;
		pfn_vkDestroyCommandPool = nullptr;
		pfn_vkResetCommandPool = nullptr;
		pfn_vkAllocateCommandBuffers = nullptr;
		pfn_vkFreeCommandBuffers = nullptr;
		pfn_vkBeginCommandBuffer = nullptr;
		pfn_vkEndCommandBuffer = nullptr;
		pfn_vkResetCommandBuffer = nullptr;
		pfn_vkCmdBindPipeline = nullptr;
		pfn_vkCmdSetViewport = nullptr;
		pfn_vkCmdSetScissor = nullptr;
		pfn_vkCmdSetLineWidth = nullptr;
		pfn_vkCmdSetDepthBias = nullptr;
		pfn_vkCmdSetBlendConstants = nullptr;
		pfn_vkCmdSetDepthBounds = nullptr;
		pfn_vkCmdSetStencilCompareMask = nullptr;
		pfn_vkCmdSetStencilWriteMask = nullptr;
		pfn_vkCmdSetStencilReference = nullptr;
		pfn_vkCmdBindDescriptorSets = nullptr;
		pfn_vkCmdBindIndexBuffer = nullptr;
		pfn_vkCmdBindVertexBuffers = nullptr;
		pfn_vkCmdDraw = nullptr;
		pfn_vkCmdDrawIndexed = nullptr;
		pfn_vkCmdDrawIndirect = nullptr;
		pfn_vkCmdDrawIndexedIndirect = nullptr;
		pfn_vkCmdDispatch = nullptr;
		pfn_vkCmdDispatchIndirect = nullptr;
		pfn_vkCmdCopyBuffer = nullptr;
		pfn_vkCmdCopyImage = nullptr;
		pfn_vkCmdBlitImage = nullptr;
		pfn_vkCmdCopyBufferToImage = nullptr;
		pfn_vkCmdCopyImageToBuffer = nullptr;
		pfn_vkCmdUpdateBuffer = nullptr;
		pfn_vkCmdFillBuffer = nullptr;
		pfn_vkCmdClearColorImage = nullptr;
		pfn_vkCmdClearDepthStencilImage = nullptr;
		pfn_vkCmdClearAttachments = nullptr;
		pfn_vkCmdResolveImage = nullptr;
		pfn_vkCmdSetEvent = nullptr;
		pfn_vkCmdResetEvent = nullptr;
		pfn_vkCmdWaitEvents = nullptr;
		pfn_vkCmdPipelineBarrier = nullptr;
		pfn_vkCmdBeginQuery = nullptr;
		pfn_vkCmdEndQuery = nullptr;
		pfn_vkCmdResetQueryPool = nullptr;
		pfn_vkCmdWriteTimestamp = nullptr;
		pfn_vkCmdCopyQueryPoolResults = nullptr;
		pfn_vkCmdPushConstants = nullptr;
		pfn_vkCmdBeginRenderPass = nullptr;
		pfn_vkCmdNextSubpass = nullptr;
		pfn_vkCmdEndRenderPass = nullptr;
		pfn_vkCmdExecuteCommands = nullptr;

		// Vulkan 1.1
		pfn_vkBindBufferMemory2 = nullptr;
		pfn_vkBindImageMemory2 = nullptr;
		pfn_vkGetDeviceGroupPeerMemoryFeatures = nullptr;
		pfn_vkCmdSetDeviceMask = nullptr;
		pfn_vkCmdDispatchBase = nullptr;
		pfn_vkGetImageMemoryRequirements2 = nullptr;
		pfn_vkGetBufferMemoryRequirements2 = nullptr;
		pfn_vkGetImageSparseMemoryRequirements2 = nullptr;
		pfn_vkTrimCommandPool = nullptr;
		pfn_vkGetDeviceQueue2 = nullptr;
		pfn_vkCreateSamplerYcbcrConversion = nullptr;
		pfn_vkDestroySamplerYcbcrConversion = nullptr;
		pfn_vkCreateDescriptorUpdateTemplate = nullptr;
		pfn_vkDestroyDescriptorUpdateTemplate = nullptr;
		pfn_vkUpdateDescriptorSetWithTemplate = nullptr;
		pfn_vkGetDescriptorSetLayoutSupport = nullptr;

		// Vulkan 1.2
		pfn_vkCmdDrawIndirectCount = nullptr;
		pfn_vkCmdDrawIndexedIndirectCount = nullptr;
		pfn_vkCreateRenderPass2 = nullptr;
		pfn_vkCmdBeginRenderPass2 = nullptr;
		pfn_vkCmdNextSubpass2 = nullptr;
		pfn_vkCmdEndRenderPass2 = nullptr;
		pfn_vkResetQueryPool = nullptr;
		pfn_vkGetSemaphoreCounterValue = nullptr;
		pfn_vkWaitSemaphores = nullptr;
		pfn_vkSignalSemaphore = nullptr;
		pfn_vkGetBufferDeviceAddress = nullptr;
		pfn_vkGetBufferOpaqueCaptureAddress = nullptr;
		pfn_vkGetDeviceMemoryOpaqueCaptureAddress = nullptr;

		// Vulkan 1.3
		pfn_vkCreatePrivateDataSlot = nullptr;
		pfn_vkDestroyPrivateDataSlot = nullptr;
		pfn_vkSetPrivateData = nullptr;
		pfn_vkGetPrivateData = nullptr;
		pfn_vkCmdSetEvent2 = nullptr;
		pfn_vkCmdResetEvent2 = nullptr;
		pfn_vkCmdWaitEvents2 = nullptr;
		pfn_vkCmdPipelineBarrier2 = nullptr;
		pfn_vkCmdWriteTimestamp2 = nullptr;
		pfn_vkQueueSubmit2 = nullptr;
		pfn_vkCmdCopyBuffer2 = nullptr;
		pfn_vkCmdCopyImage2 = nullptr;
		pfn_vkCmdCopyBufferToImage2 = nullptr;
		pfn_vkCmdCopyImageToBuffer2 = nullptr;
		pfn_vkCmdBlitImage2 = nullptr;
		pfn_vkCmdResolveImage2 = nullptr;
		pfn_vkCmdBeginRendering = nullptr;
		pfn_vkCmdEndRendering = nullptr;
		pfn_vkCmdSetCullMode = nullptr;
		pfn_vkCmdSetFrontFace = nullptr;
		pfn_vkCmdSetPrimitiveTopology = nullptr;
		pfn_vkCmdSetViewportWithCount = nullptr;
		pfn_vkCmdSetScissorWithCount = nullptr;
		pfn_vkCmdBindVertexBuffers2 = nullptr;
		pfn_vkCmdSetDepthTestEnable = nullptr;
		pfn_vkCmdSetDepthWriteEnable = nullptr;
		pfn_vkCmdSetDepthCompareOp = nullptr;
		pfn_vkCmdSetDepthBoundsTestEnable = nullptr;
		pfn_vkCmdSetStencilTestEnable = nullptr;
		pfn_vkCmdSetStencilOp = nullptr;
		pfn_vkCmdSetRasterizerDiscardEnable = nullptr;
		pfn_vkCmdSetDepthBiasEnable = nullptr;
		pfn_vkCmdSetPrimitiveRestartEnable = nullptr;
		pfn_vkGetDeviceBufferMemoryRequirements = nullptr;
		pfn_vkGetDeviceImageMemoryRequirements = nullptr;
		pfn_vkGetDeviceImageSparseMemoryRequirements = nullptr;

		// Vulkan 1.4
		/* pfn_vkCmdSetLineStipple = nullptr;
		pfn_vkMapMemory2 = nullptr;
		pfn_vkUnmapMemory2 = nullptr;
		pfn_vkCmdBindIndexBuffer2 = nullptr;
		pfn_vkGetRenderingAreaGranularity = nullptr;
		pfn_vkGetDeviceImageSubresourceLayout = nullptr;
		pfn_vkGetImageSubresourceLayout2 = nullptr;
		pfn_vkCmdPushDescriptorSet = nullptr;
		pfn_vkCmdPushDescriptorSetWithTemplate = nullptr;
		pfn_vkCmdSetRenderingAttachmentLocations = nullptr;
		pfn_vkCmdSetRenderingInputAttachmentIndices = nullptr;
		pfn_vkCmdBindDescriptorSets2 = nullptr;
		pfn_vkCmdPushConstants2 = nullptr;
		pfn_vkCmdPushDescriptorSet2 = nullptr;
		pfn_vkCmdPushDescriptorSetWithTemplate2 = nullptr;
		pfn_vkCopyMemoryToImage = nullptr;
		pfn_vkCopyImageToMemory = nullptr;
		pfn_vkCopyImageToImage = nullptr;
		pfn_vkTransitionImageLayout = nullptr; */

		// Debug extension
		pfn_vkQueueBeginDebugUtilsLabelEXT = nullptr;
		pfn_vkQueueEndDebugUtilsLabelEXT = nullptr;
		pfn_vkQueueInsertDebugUtilsLabelEXT = nullptr;
		pfn_vkCmdBeginDebugUtilsLabelEXT = nullptr;
		pfn_vkCmdEndDebugUtilsLabelEXT = nullptr;
		pfn_vkCmdInsertDebugUtilsLabelEXT = nullptr;
		// Swapchain extension
		pfn_vkCreateSwapchainKHR = nullptr;
		pfn_vkDestroySwapchainKHR = nullptr;
		pfn_vkGetSwapchainImagesKHR = nullptr;
		pfn_vkAcquireNextImageKHR = nullptr;
		pfn_vkQueuePresentKHR = nullptr;
		pfn_vkGetDeviceGroupPresentCapabilitiesKHR = nullptr;
		pfn_vkGetDeviceGroupSurfacePresentModesKHR = nullptr;
		pfn_vkAcquireNextImage2KHR = nullptr;
	}

	bool init_logical_vulkan_device() {
		DEFINE_SIGNAL_GUARD(sigGuardCreateDevice);
		constexpr uint32_t u32LogicalDeviceExtensionCount = 1;
		const std::array<const char*, u32LogicalDeviceExtensionCount> a2cLogicalDeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		constexpr float fQueuePriority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> vk_paDeviceQueueCreateInfos;
		PUSH_TO_CALLSTACKTRACE(create_queue_create_infos(fQueuePriority, vk_paDeviceQueueCreateInfos));

		const VkPhysicalDeviceFeatures vk_physicalDeviceFeaturesEnabled = {
			.sampleRateShading = vk_physicalDeviceFeatures.sampleRateShading,
			.samplerAnisotropy = vk_physicalDeviceFeatures.samplerAnisotropy
		};

		const VkDeviceCreateInfo vk_deviceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = vk_paDeviceQueueCreateInfos.size(),
			.pQueueCreateInfos = vk_paDeviceQueueCreateInfos.data(),
			.enabledExtensionCount = u32LogicalDeviceExtensionCount,
			.ppEnabledExtensionNames = static_cast<const char *const *>(a2cLogicalDeviceExtensions.data()),
			.pEnabledFeatures = &vk_physicalDeviceFeaturesEnabled
		};
		const bool bCreatedDeviceSuccessfully = vkCreateDevice(vk_hPhysicalDeviceSelected, &vk_deviceCreateInfo, nullptr, &vk_hDevice) == VK_SUCCESS;
		if (!bCreatedDeviceSuccessfully) {
			RE_FATAL_ERROR("Failed creating logical Vulkan device");
			return false;
		}

		if (!PUSH_TO_CALLSTACKTRACE_AND_RETURN(load_vulkan_1_0_device() && load_vulkan_1_1_device() && load_vulkan_1_2_device() && load_vulkan_1_3_device() /* && load_vulkan_1_4_device() */ && load_extension_funcs_with_device(), bool)) {
			destroy_logical_vulkan_device();
			return false;
		}
		return true;
	}
	
	void destroy_logical_vulkan_device() {
		vkDestroyDevice(vk_hDevice, nullptr);
		unload_all_vulkan_functions_of_device();
		vk_hDevice = VK_NULL_HANDLE;
	}

}