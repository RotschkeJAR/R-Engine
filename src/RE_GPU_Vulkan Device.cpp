#include "RE_GPU_Internal.hpp"

#include <queue>

namespace RE {

	VkDevice vk_hDevice = VK_NULL_HANDLE;

	PFN_vkGetDeviceQueue vkGetDeviceQueue = nullptr;
	PFN_vkQueueSubmit vkQueueSubmit = nullptr;
	PFN_vkQueueWaitIdle vkQueueWaitIdle = nullptr;
	PFN_vkDeviceWaitIdle vkDeviceWaitIdle = nullptr;
	PFN_vkAllocateMemory vkAllocateMemory = nullptr;
	PFN_vkFreeMemory vkFreeMemory = nullptr;
	PFN_vkMapMemory vkMapMemory = nullptr;
	PFN_vkUnmapMemory vkUnmapMemory = nullptr;
	PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges = nullptr;
	PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges = nullptr;
	PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment = nullptr;
	PFN_vkBindBufferMemory vkBindBufferMemory = nullptr;
	PFN_vkBindImageMemory vkBindImageMemory = nullptr;
	PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements = nullptr;
	PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements = nullptr;
	PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements = nullptr;
	PFN_vkQueueBindSparse vkQueueBindSparse = nullptr;
	PFN_vkCreateFence vkCreateFence = nullptr;
	PFN_vkDestroyFence vkDestroyFence = nullptr;
	PFN_vkResetFences vkResetFences = nullptr;
	PFN_vkGetFenceStatus vkGetFenceStatus = nullptr;
	PFN_vkWaitForFences vkWaitForFences = nullptr;
	PFN_vkCreateSemaphore vkCreateSemaphore = nullptr;
	PFN_vkDestroySemaphore vkDestroySemaphore = nullptr;
	PFN_vkCreateEvent vkCreateEvent = nullptr;
	PFN_vkDestroyEvent vkDestroyEvent = nullptr;
	PFN_vkGetEventStatus vkGetEventStatus = nullptr;
	PFN_vkSetEvent vkSetEvent = nullptr;
	PFN_vkResetEvent vkResetEvent = nullptr;
	PFN_vkCreateQueryPool vkCreateQueryPool = nullptr;
	PFN_vkDestroyQueryPool vkDestroyQueryPool = nullptr;
	PFN_vkGetQueryPoolResults vkGetQueryPoolResults = nullptr;
	PFN_vkCreateBuffer vkCreateBuffer = nullptr;
	PFN_vkDestroyBuffer vkDestroyBuffer = nullptr;
	PFN_vkCreateBufferView vkCreateBufferView = nullptr;
	PFN_vkDestroyBufferView vkDestroyBufferView = nullptr;	
	PFN_vkCreateImage vkCreateImage = nullptr;
	PFN_vkDestroyImage vkDestroyImage = nullptr;
	PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout = nullptr;
	PFN_vkCreateImageView vkCreateImageView = nullptr;
	PFN_vkDestroyImageView vkDestroyImageView = nullptr;
	PFN_vkCreateShaderModule vkCreateShaderModule = nullptr;
	PFN_vkDestroyShaderModule vkDestroyShaderModule = nullptr;
	PFN_vkCreatePipelineCache vkCreatePipelineCache = nullptr;
	PFN_vkDestroyPipelineCache vkDestroyPipelineCache = nullptr;
	PFN_vkGetPipelineCacheData vkGetPipelineCacheData = nullptr;
	PFN_vkMergePipelineCaches vkMergePipelineCaches = nullptr;
	PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = nullptr;
	PFN_vkCreateComputePipelines vkCreateComputePipelines = nullptr;
	PFN_vkDestroyPipeline vkDestroyPipeline = nullptr;
	PFN_vkCreatePipelineLayout vkCreatePipelineLayout = nullptr;
	PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout = nullptr;
	PFN_vkCreateSampler vkCreateSampler = nullptr;
	PFN_vkDestroySampler vkDestroySampler = nullptr;
	PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout = nullptr;
	PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout = nullptr;
	PFN_vkCreateDescriptorPool vkCreateDescriptorPool = nullptr;
	PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool = nullptr;
	PFN_vkResetDescriptorPool vkResetDescriptorPool = nullptr;
	PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets = nullptr;
	PFN_vkFreeDescriptorSets vkFreeDescriptorSets = nullptr;
	PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets = nullptr;
	PFN_vkCreateFramebuffer vkCreateFramebuffer = nullptr;
	PFN_vkDestroyFramebuffer vkDestroyFramebuffer = nullptr;
	PFN_vkCreateRenderPass vkCreateRenderPass = nullptr;
	PFN_vkDestroyRenderPass vkDestroyRenderPass = nullptr;
	PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity = nullptr;
	PFN_vkCreateCommandPool vkCreateCommandPool = nullptr;
	PFN_vkDestroyCommandPool vkDestroyCommandPool = nullptr;
	PFN_vkResetCommandPool vkResetCommandPool = nullptr;
	PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = nullptr;
	PFN_vkFreeCommandBuffers vkFreeCommandBuffers = nullptr;
	PFN_vkBeginCommandBuffer vkBeginCommandBuffer = nullptr;
	PFN_vkEndCommandBuffer vkEndCommandBuffer = nullptr;
	PFN_vkResetCommandBuffer vkResetCommandBuffer = nullptr;
	PFN_vkCmdBindPipeline vkCmdBindPipeline = nullptr;
	PFN_vkCmdSetViewport vkCmdSetViewport = nullptr;
	PFN_vkCmdSetScissor vkCmdSetScissor = nullptr;
	PFN_vkCmdSetLineWidth vkCmdSetLineWidth = nullptr;
	PFN_vkCmdSetDepthBias vkCmdSetDepthBias = nullptr;
	PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants = nullptr;
	PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds = nullptr;
	PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask = nullptr;
	PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask = nullptr;
	PFN_vkCmdSetStencilReference vkCmdSetStencilReference = nullptr;
	PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = nullptr;
	PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer = nullptr;
	PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = nullptr;
	PFN_vkCmdDraw vkCmdDraw = nullptr;
	PFN_vkCmdDrawIndexed vkCmdDrawIndexed = nullptr;
	PFN_vkCmdDrawIndirect vkCmdDrawIndirect = nullptr;
	PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect = nullptr;
	PFN_vkCmdDispatch vkCmdDispatch = nullptr;
	PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect = nullptr;
	PFN_vkCmdCopyBuffer vkCmdCopyBuffer = nullptr;
	PFN_vkCmdCopyImage vkCmdCopyImage = nullptr;
	PFN_vkCmdBlitImage vkCmdBlitImage = nullptr;
	PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage = nullptr;
	PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer = nullptr;
	PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer = nullptr;
	PFN_vkCmdFillBuffer vkCmdFillBuffer = nullptr;
	PFN_vkCmdClearColorImage vkCmdClearColorImage = nullptr;
	PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage = nullptr;
	PFN_vkCmdClearAttachments vkCmdClearAttachments = nullptr;
	PFN_vkCmdResolveImage vkCmdResolveImage = nullptr;
	PFN_vkCmdSetEvent vkCmdSetEvent = nullptr;
	PFN_vkCmdResetEvent vkCmdResetEvent = nullptr;
	PFN_vkCmdWaitEvents vkCmdWaitEvents = nullptr;
	PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = nullptr;
	PFN_vkCmdBeginQuery vkCmdBeginQuery = nullptr;
	PFN_vkCmdEndQuery vkCmdEndQuery = nullptr;
	PFN_vkCmdResetQueryPool vkCmdResetQueryPool = nullptr;
	PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp = nullptr;
	PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults = nullptr;
	PFN_vkCmdPushConstants vkCmdPushConstants = nullptr;
	PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = nullptr;
	PFN_vkCmdNextSubpass vkCmdNextSubpass = nullptr;
	PFN_vkCmdEndRenderPass vkCmdEndRenderPass = nullptr;
	PFN_vkCmdExecuteCommands vkCmdExecuteCommands = nullptr;

	// Vulkan 1.1
	PFN_vkBindBufferMemory2 vkBindBufferMemory2 = nullptr;
	PFN_vkBindImageMemory2 vkBindImageMemory2 = nullptr;
	PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures = nullptr;
	PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask = nullptr;
	PFN_vkCmdDispatchBase vkCmdDispatchBase = nullptr;
	PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2 = nullptr;
	PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2 = nullptr;
	PFN_vkGetImageSparseMemoryRequirements2 vkGetImageSparseMemoryRequirements2 = nullptr;
	PFN_vkTrimCommandPool vkTrimCommandPool = nullptr;
	PFN_vkGetDeviceQueue2 vkGetDeviceQueue2 = nullptr;
	PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion = nullptr;
	PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion = nullptr;
	PFN_vkCreateDescriptorUpdateTemplate vkCreateDescriptorUpdateTemplate = nullptr;
	PFN_vkDestroyDescriptorUpdateTemplate vkDestroyDescriptorUpdateTemplate = nullptr;
	PFN_vkUpdateDescriptorSetWithTemplate vkUpdateDescriptorSetWithTemplate = nullptr;
	PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport = nullptr;

	// Vulkan 1.2
	PFN_vkCmdDrawIndirectCount vkCmdDrawIndirectCount = nullptr;
	PFN_vkCmdDrawIndexedIndirectCount vkCmdDrawIndexedIndirectCount = nullptr;
	PFN_vkCreateRenderPass2 vkCreateRenderPass2 = nullptr;
	PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2 = nullptr;
	PFN_vkCmdNextSubpass2 vkCmdNextSubpass2 = nullptr;
	PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2 = nullptr;
	PFN_vkResetQueryPool vkResetQueryPool = nullptr;
	PFN_vkGetSemaphoreCounterValue vkGetSemaphoreCounterValue = nullptr;
	PFN_vkWaitSemaphores vkWaitSemaphores = nullptr;
	PFN_vkSignalSemaphore vkSignalSemaphore = nullptr;
	PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress = nullptr;
	PFN_vkGetBufferOpaqueCaptureAddress vkGetBufferOpaqueCaptureAddress = nullptr;
	PFN_vkGetDeviceMemoryOpaqueCaptureAddress vkGetDeviceMemoryOpaqueCaptureAddress = nullptr;

	// Vulkan 1.3
	PFN_vkCreatePrivateDataSlot vkCreatePrivateDataSlot = nullptr;
	PFN_vkDestroyPrivateDataSlot vkDestroyPrivateDataSlot = nullptr;
	PFN_vkSetPrivateData vkSetPrivateData = nullptr;
	PFN_vkGetPrivateData vkGetPrivateData = nullptr;
	PFN_vkCmdCopyBuffer2 vkCmdCopyBuffer2 = nullptr;
	PFN_vkCmdCopyImage2 vkCmdCopyImage2 = nullptr;
	PFN_vkCmdCopyBufferToImage2 vkCmdCopyBufferToImage2 = nullptr;
	PFN_vkCmdCopyImageToBuffer2 vkCmdCopyImageToBuffer2 = nullptr;
	PFN_vkCmdBlitImage2 vkCmdBlitImage2 = nullptr;
	PFN_vkCmdResolveImage2 vkCmdResolveImage2 = nullptr;
	PFN_vkCmdBeginRendering vkCmdBeginRendering = nullptr;
	PFN_vkCmdEndRendering vkCmdEndRendering = nullptr;
	PFN_vkCmdSetCullMode vkCmdSetCullMode = nullptr;
	PFN_vkCmdSetFrontFace vkCmdSetFrontFace = nullptr;
	PFN_vkCmdSetPrimitiveTopology vkCmdSetPrimitiveTopology = nullptr;
	PFN_vkCmdSetViewportWithCount vkCmdSetViewportWithCount = nullptr;
	PFN_vkCmdSetScissorWithCount vkCmdSetScissorWithCount = nullptr;
	PFN_vkCmdBindVertexBuffers2 vkCmdBindVertexBuffers2 = nullptr;
	PFN_vkCmdSetDepthTestEnable vkCmdSetDepthTestEnable = nullptr;
	PFN_vkCmdSetDepthWriteEnable vkCmdSetDepthWriteEnable = nullptr;
	PFN_vkCmdSetDepthCompareOp vkCmdSetDepthCompareOp = nullptr;
	PFN_vkCmdSetDepthBoundsTestEnable vkCmdSetDepthBoundsTestEnable = nullptr;
	PFN_vkCmdSetStencilTestEnable vkCmdSetStencilTestEnable = nullptr;
	PFN_vkCmdSetStencilOp vkCmdSetStencilOp = nullptr;
	PFN_vkCmdSetRasterizerDiscardEnable vkCmdSetRasterizerDiscardEnable = nullptr;
	PFN_vkCmdSetDepthBiasEnable vkCmdSetDepthBiasEnable = nullptr;
	PFN_vkCmdSetPrimitiveRestartEnable vkCmdSetPrimitiveRestartEnable = nullptr;
	PFN_vkGetDeviceBufferMemoryRequirements vkGetDeviceBufferMemoryRequirements = nullptr;
	PFN_vkGetDeviceImageMemoryRequirements vkGetDeviceImageMemoryRequirements = nullptr;
	PFN_vkGetDeviceImageSparseMemoryRequirements vkGetDeviceImageSparseMemoryRequirements = nullptr;

	// Vulkan 1.4
	/* PFN_vkCmdSetLineStipple vkCmdSetLineStipple = nullptr;
	PFN_vkMapMemory2 vkMapMemory2 = nullptr;
	PFN_vkUnmapMemory2 vkUnmapMemory2 = nullptr;
	PFN_vkCmdBindIndexBuffer2 vkCmdBindIndexBuffer2 = nullptr;
	PFN_vkGetRenderingAreaGranularity vkGetRenderingAreaGranularity = nullptr;
	PFN_vkGetDeviceImageSubresourceLayout vkGetDeviceImageSubresourceLayout = nullptr;
	PFN_vkGetImageSubresourceLayout2 vkGetImageSubresourceLayout2 = nullptr;
	PFN_vkCmdPushDescriptorSet vkCmdPushDescriptorSet = nullptr;
	PFN_vkCmdPushDescriptorSetWithTemplate vkCmdPushDescriptorSetWithTemplate = nullptr;
	PFN_vkCmdSetRenderingAttachmentLocations vkCmdSetRenderingAttachmentLocations = nullptr;
	PFN_vkCmdSetRenderingInputAttachmentIndices vkCmdSetRenderingInputAttachmentIndices = nullptr;
	PFN_vkCmdBindDescriptorSets2 vkCmdBindDescriptorSets2 = nullptr;
	PFN_vkCmdPushConstants2 vkCmdPushConstants2 = nullptr;
	PFN_vkCmdPushDescriptorSet2 vkCmdPushDescriptorSet2 = nullptr;
	PFN_vkCmdPushDescriptorSetWithTemplate2 vkCmdPushDescriptorSetWithTemplate2 = nullptr;
	PFN_vkCopyMemoryToImage vkCopyMemoryToImage = nullptr;
	PFN_vkCopyImageToMemory vkCopyImageToMemory = nullptr;
	PFN_vkCopyImageToImage vkCopyImageToImage = nullptr;
	PFN_vkTransitionImageLayout vkTransitionImageLayout = nullptr; */

	// Debug Messages
	PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT = nullptr;
	PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT = nullptr;
	PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT = nullptr;
	PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = nullptr;
	PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT = nullptr;
	PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT = nullptr;
	// Swapchain
	PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = nullptr;
	PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR = nullptr;
	PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = nullptr;
	PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = nullptr;
	PFN_vkQueuePresentKHR vkQueuePresentKHR = nullptr;
	PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR = nullptr;
	PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR = nullptr;
	PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR = nullptr;

	static bool load_vulkan_1_0_device() {
		PRINT_DEBUG("Loading device-level Vulkan 1.0-functions");
		vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(load_vulkan_func_with_device("vkGetDeviceQueue"));
		if (!vkGetDeviceQueue)
			return false;
		vkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(load_vulkan_func_with_device("vkQueueSubmit"));
		if (!vkQueueSubmit)
			return false;
		vkQueueWaitIdle = reinterpret_cast<PFN_vkQueueWaitIdle>(load_vulkan_func_with_device("vkQueueWaitIdle"));
		if (!vkQueueWaitIdle)
			return false;
		vkDeviceWaitIdle = reinterpret_cast<PFN_vkDeviceWaitIdle>(load_vulkan_func_with_device("vkDeviceWaitIdle"));
		if (!vkDeviceWaitIdle)
			return false;
		vkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(load_vulkan_func_with_device("vkAllocateMemory"));
		if (!vkAllocateMemory)
			return false;
		vkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(load_vulkan_func_with_device("vkFreeMemory"));
		if (!vkFreeMemory)
			return false;
		vkMapMemory = reinterpret_cast<PFN_vkMapMemory>(load_vulkan_func_with_device("vkMapMemory"));
		if (!vkMapMemory)
			return false;
		vkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(load_vulkan_func_with_device("vkUnmapMemory"));
		if (!vkUnmapMemory)
			return false;
		vkFlushMappedMemoryRanges = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(load_vulkan_func_with_device("vkFlushMappedMemoryRanges"));
		if (!vkFlushMappedMemoryRanges)
			return false;
		vkInvalidateMappedMemoryRanges = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(load_vulkan_func_with_device("vkInvalidateMappedMemoryRanges"));
		if (!vkInvalidateMappedMemoryRanges)
			return false;
		vkGetDeviceMemoryCommitment = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(load_vulkan_func_with_device("vkGetDeviceMemoryCommitment"));
		if (!vkGetDeviceMemoryCommitment)
			return false;
		vkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(load_vulkan_func_with_device("vkBindBufferMemory"));
		if (!vkBindBufferMemory)
			return false;
		vkBindImageMemory = reinterpret_cast<PFN_vkBindImageMemory>(load_vulkan_func_with_device("vkBindImageMemory"));
		if (!vkBindImageMemory)
			return false;
		vkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(load_vulkan_func_with_device("vkGetBufferMemoryRequirements"));
		if (!vkGetBufferMemoryRequirements)
			return false;
		vkGetImageMemoryRequirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(load_vulkan_func_with_device("vkGetImageMemoryRequirements"));
		if (!vkGetImageMemoryRequirements)
			return false;
		vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(load_vulkan_func_with_device("vkGetImageSparseMemoryRequirements"));
		if (!vkGetImageSparseMemoryRequirements)
			return false;
		vkQueueBindSparse = reinterpret_cast<PFN_vkQueueBindSparse>(load_vulkan_func_with_device("vkQueueBindSparse"));
		if (!vkQueueBindSparse)
			return false;
		vkCreateFence = reinterpret_cast<PFN_vkCreateFence>(load_vulkan_func_with_device("vkCreateFence"));
		if (!vkCreateFence)
			return false;
		vkDestroyFence = reinterpret_cast<PFN_vkDestroyFence>(load_vulkan_func_with_device("vkDestroyFence"));
		if (!vkDestroyFence)
			return false;
		vkResetFences = reinterpret_cast<PFN_vkResetFences>(load_vulkan_func_with_device("vkResetFences"));
		if (!vkResetFences)
			return false;
		vkGetFenceStatus = reinterpret_cast<PFN_vkGetFenceStatus>(load_vulkan_func_with_device("vkGetFenceStatus"));
		if (!vkGetFenceStatus)
			return false;
		vkWaitForFences = reinterpret_cast<PFN_vkWaitForFences>(load_vulkan_func_with_device("vkWaitForFences"));
		if (!vkWaitForFences)
			return false;
		vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(load_vulkan_func_with_device("vkCreateSemaphore"));
		if (!vkCreateSemaphore)
			return false;
		vkDestroySemaphore = reinterpret_cast<PFN_vkDestroySemaphore>(load_vulkan_func_with_device("vkDestroySemaphore"));
		if (!vkDestroySemaphore)
			return false;
		vkCreateEvent = reinterpret_cast<PFN_vkCreateEvent>(load_vulkan_func_with_device("vkCreateEvent"));
		if (!vkCreateEvent)
			return false;
		vkDestroyEvent = reinterpret_cast<PFN_vkDestroyEvent>(load_vulkan_func_with_device("vkDestroyEvent"));
		if (!vkDestroyEvent)
			return false;
		vkGetEventStatus = reinterpret_cast<PFN_vkGetEventStatus>(load_vulkan_func_with_device("vkGetEventStatus"));
		if (!vkGetEventStatus)
			return false;
		vkSetEvent = reinterpret_cast<PFN_vkSetEvent>(load_vulkan_func_with_device("vkSetEvent"));
		if (!vkSetEvent)
			return false;
		vkResetEvent = reinterpret_cast<PFN_vkResetEvent>(load_vulkan_func_with_device("vkResetEvent"));
		if (!vkResetEvent)
			return false;
		vkCreateQueryPool = reinterpret_cast<PFN_vkCreateQueryPool>(load_vulkan_func_with_device("vkCreateQueryPool"));
		if (!vkCreateQueryPool)
			return false;
		vkDestroyQueryPool = reinterpret_cast<PFN_vkDestroyQueryPool>(load_vulkan_func_with_device("vkDestroyQueryPool"));
		if (!vkDestroyQueryPool)
			return false;
		vkGetQueryPoolResults = reinterpret_cast<PFN_vkGetQueryPoolResults>(load_vulkan_func_with_device("vkGetQueryPoolResults"));
		if (!vkGetQueryPoolResults)
			return false;
		vkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(load_vulkan_func_with_device("vkCreateBuffer"));
		if (!vkCreateBuffer)
			return false;
		vkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(load_vulkan_func_with_device("vkDestroyBuffer"));
		if (!vkDestroyBuffer)
			return false;
		vkCreateBufferView = reinterpret_cast<PFN_vkCreateBufferView>(load_vulkan_func_with_device("vkCreateBufferView"));
		if (!vkCreateBufferView)
			return false;
		vkDestroyBufferView = reinterpret_cast<PFN_vkDestroyBufferView>(load_vulkan_func_with_device("vkDestroyBufferView"));
		if (!vkDestroyBufferView)
			return false;
		vkCreateImage = reinterpret_cast<PFN_vkCreateImage>(load_vulkan_func_with_device("vkCreateImage"));
		if (!vkCreateImage)
			return false;
		vkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(load_vulkan_func_with_device("vkDestroyImage"));
		if (!vkDestroyImage)
			return false;
		vkGetImageSubresourceLayout = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(load_vulkan_func_with_device("vkGetImageSubresourceLayout"));
		if (!vkGetImageSubresourceLayout)
			return false;
		vkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(load_vulkan_func_with_device("vkCreateImageView"));
		if (!vkCreateImageView)
			return false;
		vkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(load_vulkan_func_with_device("vkDestroyImageView"));
		if (!vkDestroyImageView)
			return false;
		vkCreateShaderModule = reinterpret_cast<PFN_vkCreateShaderModule>(load_vulkan_func_with_device("vkCreateShaderModule"));
		if (!vkCreateShaderModule)
			return false;
		vkDestroyShaderModule = reinterpret_cast<PFN_vkDestroyShaderModule>(load_vulkan_func_with_device("vkDestroyShaderModule"));
		if (!vkDestroyShaderModule)
			return false;
		vkCreatePipelineCache = reinterpret_cast<PFN_vkCreatePipelineCache>(load_vulkan_func_with_device("vkCreatePipelineCache"));
		if (!vkCreatePipelineCache)
			return false;
		vkDestroyPipelineCache = reinterpret_cast<PFN_vkDestroyPipelineCache>(load_vulkan_func_with_device("vkDestroyPipelineCache"));
		if (!vkDestroyPipelineCache)
			return false;
		vkGetPipelineCacheData = reinterpret_cast<PFN_vkGetPipelineCacheData>(load_vulkan_func_with_device("vkGetPipelineCacheData"));
		if (!vkGetPipelineCacheData)
			return false;
		vkMergePipelineCaches = reinterpret_cast<PFN_vkMergePipelineCaches>(load_vulkan_func_with_device("vkMergePipelineCaches"));
		if (!vkMergePipelineCaches)
			return false;
		vkCreateGraphicsPipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(load_vulkan_func_with_device("vkCreateGraphicsPipelines"));
		if (!vkCreateGraphicsPipelines)
			return false;
		vkCreateComputePipelines = reinterpret_cast<PFN_vkCreateComputePipelines>(load_vulkan_func_with_device("vkCreateComputePipelines"));
		if (!vkCreateComputePipelines)
			return false;
		vkDestroyPipeline = reinterpret_cast<PFN_vkDestroyPipeline>(load_vulkan_func_with_device("vkDestroyPipeline"));
		if (!vkDestroyPipeline)
			return false;
		vkCreatePipelineLayout = reinterpret_cast<PFN_vkCreatePipelineLayout>(load_vulkan_func_with_device("vkCreatePipelineLayout"));
		if (!vkCreatePipelineLayout)
			return false;
		vkDestroyPipelineLayout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(load_vulkan_func_with_device("vkDestroyPipelineLayout"));
		if (!vkDestroyPipelineLayout)
			return false;
		vkCreateSampler = reinterpret_cast<PFN_vkCreateSampler>(load_vulkan_func_with_device("vkCreateSampler"));
		if (!vkCreateSampler)
			return false;
		vkDestroySampler = reinterpret_cast<PFN_vkDestroySampler>(load_vulkan_func_with_device("vkDestroySampler"));
		if (!vkDestroySampler)
			return false;
		vkCreateDescriptorSetLayout = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(load_vulkan_func_with_device("vkCreateDescriptorSetLayout"));
		if (!vkCreateDescriptorSetLayout)
			return false;
		vkDestroyDescriptorSetLayout = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(load_vulkan_func_with_device("vkDestroyDescriptorSetLayout"));
		if (!vkDestroyDescriptorSetLayout)
			return false;
		vkCreateDescriptorPool = reinterpret_cast<PFN_vkCreateDescriptorPool>(load_vulkan_func_with_device("vkCreateDescriptorPool"));
		if (!vkCreateDescriptorPool)
			return false;
		vkDestroyDescriptorPool = reinterpret_cast<PFN_vkDestroyDescriptorPool>(load_vulkan_func_with_device("vkDestroyDescriptorPool"));
		if (!vkDestroyDescriptorPool)
			return false;
		vkResetDescriptorPool = reinterpret_cast<PFN_vkResetDescriptorPool>(load_vulkan_func_with_device("vkResetDescriptorPool"));
		if (!vkResetDescriptorPool)
			return false;
		vkAllocateDescriptorSets = reinterpret_cast<PFN_vkAllocateDescriptorSets>(load_vulkan_func_with_device("vkAllocateDescriptorSets"));
		if (!vkAllocateDescriptorSets)
			return false;
		vkFreeDescriptorSets = reinterpret_cast<PFN_vkFreeDescriptorSets>(load_vulkan_func_with_device("vkFreeDescriptorSets"));
		if (!vkFreeDescriptorSets)
			return false;
		vkUpdateDescriptorSets = reinterpret_cast<PFN_vkUpdateDescriptorSets>(load_vulkan_func_with_device("vkUpdateDescriptorSets"));
		if (!vkUpdateDescriptorSets)
			return false;
		vkCreateFramebuffer = reinterpret_cast<PFN_vkCreateFramebuffer>(load_vulkan_func_with_device("vkCreateFramebuffer"));
		if (!vkCreateFramebuffer)
			return false;
		vkDestroyFramebuffer = reinterpret_cast<PFN_vkDestroyFramebuffer>(load_vulkan_func_with_device("vkDestroyFramebuffer"));
		if (!vkDestroyFramebuffer)
			return false;
		vkCreateRenderPass = reinterpret_cast<PFN_vkCreateRenderPass>(load_vulkan_func_with_device("vkCreateRenderPass"));
		if (!vkCreateRenderPass)
			return false;
		vkDestroyRenderPass = reinterpret_cast<PFN_vkDestroyRenderPass>(load_vulkan_func_with_device("vkDestroyRenderPass"));
		if (!vkDestroyRenderPass)
			return false;
		vkGetRenderAreaGranularity = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(load_vulkan_func_with_device("vkGetRenderAreaGranularity"));
		if (!vkGetRenderAreaGranularity)
			return false;
		vkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(load_vulkan_func_with_device("vkCreateCommandPool"));
		if (!vkCreateCommandPool)
			return false;
		vkDestroyCommandPool = reinterpret_cast<PFN_vkDestroyCommandPool>(load_vulkan_func_with_device("vkDestroyCommandPool"));
		if (!vkDestroyCommandPool)
			return false;
		vkResetCommandPool = reinterpret_cast<PFN_vkResetCommandPool>(load_vulkan_func_with_device("vkResetCommandPool"));
		if (!vkResetCommandPool)
			return false;
		vkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(load_vulkan_func_with_device("vkAllocateCommandBuffers"));
		if (!vkAllocateCommandBuffers)
			return false;
		vkFreeCommandBuffers = reinterpret_cast<PFN_vkFreeCommandBuffers>(load_vulkan_func_with_device("vkFreeCommandBuffers"));
		if (!vkFreeCommandBuffers)
			return false;
		vkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(load_vulkan_func_with_device("vkBeginCommandBuffer"));
		if (!vkBeginCommandBuffer)
			return false;
		vkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(load_vulkan_func_with_device("vkEndCommandBuffer"));
		if (!vkEndCommandBuffer)
			return false;
		vkResetCommandBuffer = reinterpret_cast<PFN_vkResetCommandBuffer>(load_vulkan_func_with_device("vkResetCommandBuffer"));
		if (!vkResetCommandBuffer)
			return false;
		vkCmdBindPipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(load_vulkan_func_with_device("vkCmdBindPipeline"));
		if (!vkCmdBindPipeline)
			return false;
		vkCmdSetViewport = reinterpret_cast<PFN_vkCmdSetViewport>(load_vulkan_func_with_device("vkCmdSetViewport"));
		if (!vkCmdSetViewport)
			return false;
		vkCmdSetScissor = reinterpret_cast<PFN_vkCmdSetScissor>(load_vulkan_func_with_device("vkCmdSetScissor"));
		if (!vkCmdSetScissor)
			return false;
		vkCmdSetLineWidth = reinterpret_cast<PFN_vkCmdSetLineWidth>(load_vulkan_func_with_device("vkCmdSetLineWidth"));
		if (!vkCmdSetLineWidth)
			return false;
		vkCmdSetDepthBias = reinterpret_cast<PFN_vkCmdSetDepthBias>(load_vulkan_func_with_device("vkCmdSetDepthBias"));
		if (!vkCmdSetDepthBias)
			return false;
		vkCmdSetBlendConstants = reinterpret_cast<PFN_vkCmdSetBlendConstants>(load_vulkan_func_with_device("vkCmdSetBlendConstants"));
		if (!vkCmdSetBlendConstants)
			return false;
		vkCmdSetDepthBounds = reinterpret_cast<PFN_vkCmdSetDepthBounds>(load_vulkan_func_with_device("vkCmdSetDepthBounds"));
		if (!vkCmdSetDepthBounds)
			return false;
		vkCmdSetStencilCompareMask = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(load_vulkan_func_with_device("vkCmdSetStencilCompareMask"));
		if (!vkCmdSetStencilCompareMask)
			return false;
		vkCmdSetStencilWriteMask = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(load_vulkan_func_with_device("vkCmdSetStencilWriteMask"));
		if (!vkCmdSetStencilWriteMask)
			return false;
		vkCmdSetStencilReference = reinterpret_cast<PFN_vkCmdSetStencilReference>(load_vulkan_func_with_device("vkCmdSetStencilReference"));
		if (!vkCmdSetStencilReference)
			return false;
		vkCmdBindDescriptorSets = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(load_vulkan_func_with_device("vkCmdBindDescriptorSets"));
		if (!vkCmdBindDescriptorSets)
			return false;
		vkCmdBindIndexBuffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(load_vulkan_func_with_device("vkCmdBindIndexBuffer"));
		if (!vkCmdBindIndexBuffer)
			return false;
		vkCmdBindVertexBuffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(load_vulkan_func_with_device("vkCmdBindVertexBuffers"));
		if (!vkCmdBindVertexBuffers)
			return false;
		vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(load_vulkan_func_with_device("vkCmdDraw"));
		if (!vkCmdDraw)
			return false;
		vkCmdDrawIndexed = reinterpret_cast<PFN_vkCmdDrawIndexed>(load_vulkan_func_with_device("vkCmdDrawIndexed"));
		if (!vkCmdDrawIndexed)
			return false;
		vkCmdDrawIndirect = reinterpret_cast<PFN_vkCmdDrawIndirect>(load_vulkan_func_with_device("vkCmdDrawIndirect"));
		if (!vkCmdDrawIndirect)
			return false;
		vkCmdDrawIndexedIndirect = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(load_vulkan_func_with_device("vkCmdDrawIndexedIndirect"));
		if (!vkCmdDrawIndexedIndirect)
			return false;
		vkCmdDispatch = reinterpret_cast<PFN_vkCmdDispatch>(load_vulkan_func_with_device("vkCmdDispatch"));
		if (!vkCmdDispatch)
			return false;
		vkCmdDispatchIndirect = reinterpret_cast<PFN_vkCmdDispatchIndirect>(load_vulkan_func_with_device("vkCmdDispatchIndirect"));
		if (!vkCmdDispatchIndirect)
			return false;
		vkCmdCopyBuffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(load_vulkan_func_with_device("vkCmdCopyBuffer"));
		if (!vkCmdCopyBuffer)
			return false;
		vkCmdCopyImage = reinterpret_cast<PFN_vkCmdCopyImage>(load_vulkan_func_with_device("vkCmdCopyImage"));
		if (!vkCmdCopyImage)
			return false;
		vkCmdBlitImage = reinterpret_cast<PFN_vkCmdBlitImage>(load_vulkan_func_with_device("vkCmdBlitImage"));
		if (!vkCmdBlitImage)
			return false;
		vkCmdCopyBufferToImage = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(load_vulkan_func_with_device("vkCmdCopyBufferToImage"));
		if (!vkCmdCopyBufferToImage)
			return false;
		vkCmdCopyImageToBuffer = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(load_vulkan_func_with_device("vkCmdCopyImageToBuffer"));
		if (!vkCmdCopyImageToBuffer)
			return false;
		vkCmdUpdateBuffer = reinterpret_cast<PFN_vkCmdUpdateBuffer>(load_vulkan_func_with_device("vkCmdUpdateBuffer"));
		if (!vkCmdUpdateBuffer)
			return false;
		vkCmdFillBuffer = reinterpret_cast<PFN_vkCmdFillBuffer>(load_vulkan_func_with_device("vkCmdFillBuffer"));
		if (!vkCmdFillBuffer)
			return false;
		vkCmdClearColorImage = reinterpret_cast<PFN_vkCmdClearColorImage>(load_vulkan_func_with_device("vkCmdClearColorImage"));
		if (!vkCmdClearColorImage)
			return false;
		vkCmdClearDepthStencilImage = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(load_vulkan_func_with_device("vkCmdClearDepthStencilImage"));
		if (!vkCmdClearDepthStencilImage)
			return false;
		vkCmdClearAttachments = reinterpret_cast<PFN_vkCmdClearAttachments>(load_vulkan_func_with_device("vkCmdClearAttachments"));
		if (!vkCmdClearAttachments)
			return false;
		vkCmdResolveImage = reinterpret_cast<PFN_vkCmdResolveImage>(load_vulkan_func_with_device("vkCmdResolveImage"));
		if (!vkCmdResolveImage)
			return false;
		vkCmdSetEvent = reinterpret_cast<PFN_vkCmdSetEvent>(load_vulkan_func_with_device("vkCmdSetEvent"));
		if (!vkCmdSetEvent)
			return false;
		vkCmdResetEvent = reinterpret_cast<PFN_vkCmdResetEvent>(load_vulkan_func_with_device("vkCmdResetEvent"));
		if (!vkCmdResetEvent)
			return false;
		vkCmdWaitEvents = reinterpret_cast<PFN_vkCmdWaitEvents>(load_vulkan_func_with_device("vkCmdWaitEvents"));
		if (!vkCmdWaitEvents)
			return false;
		vkCmdPipelineBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(load_vulkan_func_with_device("vkCmdPipelineBarrier"));
		if (!vkCmdPipelineBarrier)
			return false;
		vkCmdBeginQuery = reinterpret_cast<PFN_vkCmdBeginQuery>(load_vulkan_func_with_device("vkCmdBeginQuery"));
		if (!vkCmdBeginQuery)
			return false;
		vkCmdEndQuery = reinterpret_cast<PFN_vkCmdEndQuery>(load_vulkan_func_with_device("vkCmdEndQuery"));
		if (!vkCmdEndQuery)
			return false;
		vkCmdResetQueryPool = reinterpret_cast<PFN_vkCmdResetQueryPool>(load_vulkan_func_with_device("vkCmdResetQueryPool"));
		if (!vkCmdResetQueryPool)
			return false;
		vkCmdWriteTimestamp = reinterpret_cast<PFN_vkCmdWriteTimestamp>(load_vulkan_func_with_device("vkCmdWriteTimestamp"));
		if (!vkCmdWriteTimestamp)
			return false;
		vkCmdCopyQueryPoolResults = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(load_vulkan_func_with_device("vkCmdCopyQueryPoolResults"));
		if (!vkCmdCopyQueryPoolResults)
			return false;
		vkCmdPushConstants = reinterpret_cast<PFN_vkCmdPushConstants>(load_vulkan_func_with_device("vkCmdPushConstants"));
		if (!vkCmdPushConstants)
			return false;
		vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(load_vulkan_func_with_device("vkCmdBeginRenderPass"));
		if (!vkCmdBeginRenderPass)
			return false;
		vkCmdNextSubpass = reinterpret_cast<PFN_vkCmdNextSubpass>(load_vulkan_func_with_device("vkCmdNextSubpass"));
		if (!vkCmdNextSubpass)
			return false;
		vkCmdEndRenderPass = reinterpret_cast<PFN_vkCmdEndRenderPass>(load_vulkan_func_with_device("vkCmdEndRenderPass"));
		if (!vkCmdEndRenderPass)
			return false;
		vkCmdExecuteCommands = reinterpret_cast<PFN_vkCmdExecuteCommands>(load_vulkan_func_with_device("vkCmdExecuteCommands"));
		if (!vkCmdExecuteCommands)
			return false;
		return true;
	}

	static bool load_vulkan_1_1_device() {
		PRINT_DEBUG("Loading device-level Vulkan 1.1-functions");
		vkBindBufferMemory2 = reinterpret_cast<PFN_vkBindBufferMemory2>(load_vulkan_func_with_device("vkBindBufferMemory2"));
		if (!vkBindBufferMemory2)
			return false;
		vkBindImageMemory2 = reinterpret_cast<PFN_vkBindImageMemory2>(load_vulkan_func_with_device("vkBindImageMemory2"));
		if (!vkBindImageMemory2)
			return false;
		vkGetDeviceGroupPeerMemoryFeatures = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeatures>(load_vulkan_func_with_device("vkGetDeviceGroupPeerMemoryFeatures"));
		if (!vkGetDeviceGroupPeerMemoryFeatures)
			return false;
		vkCmdSetDeviceMask = reinterpret_cast<PFN_vkCmdSetDeviceMask>(load_vulkan_func_with_device("vkCmdSetDeviceMask"));
		if (!vkCmdSetDeviceMask)
			return false;
		vkCmdDispatchBase = reinterpret_cast<PFN_vkCmdDispatchBase>(load_vulkan_func_with_device("vkCmdDispatchBase"));
		if (!vkCmdDispatchBase)
			return false;
		vkGetImageMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageMemoryRequirements2>(load_vulkan_func_with_device("vkGetImageMemoryRequirements2"));
		if (!vkGetImageMemoryRequirements2)
			return false;
		vkGetBufferMemoryRequirements2 = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2>(load_vulkan_func_with_device("vkGetBufferMemoryRequirements2"));
		if (!vkGetBufferMemoryRequirements2)
			return false;
		vkGetImageSparseMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2>(load_vulkan_func_with_device("vkGetImageSparseMemoryRequirements2"));
		if (!vkGetImageSparseMemoryRequirements2)
			return false;
		vkTrimCommandPool = reinterpret_cast<PFN_vkTrimCommandPool>(load_vulkan_func_with_device("vkTrimCommandPool"));
		if (!vkTrimCommandPool)
			return false;
		vkGetDeviceQueue2 = reinterpret_cast<PFN_vkGetDeviceQueue2>(load_vulkan_func_with_device("vkGetDeviceQueue2"));
		if (!vkGetDeviceQueue2)
			return false;
		vkCreateSamplerYcbcrConversion = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversion>(load_vulkan_func_with_device("vkCreateSamplerYcbcrConversion"));
		if (!vkCreateSamplerYcbcrConversion)
			return false;
		vkDestroySamplerYcbcrConversion = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversion>(load_vulkan_func_with_device("vkDestroySamplerYcbcrConversion"));
		if (!vkDestroySamplerYcbcrConversion)
			return false;
		vkCreateDescriptorUpdateTemplate = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplate>(load_vulkan_func_with_device("vkCreateDescriptorUpdateTemplate"));
		if (!vkCreateDescriptorUpdateTemplate)
			return false;
		vkDestroyDescriptorUpdateTemplate = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplate>(load_vulkan_func_with_device("vkDestroyDescriptorUpdateTemplate"));
		if (!vkDestroyDescriptorUpdateTemplate)
			return false;
		vkUpdateDescriptorSetWithTemplate = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplate>(load_vulkan_func_with_device("vkUpdateDescriptorSetWithTemplate"));
		if (!vkUpdateDescriptorSetWithTemplate)
			return false;
		vkGetDescriptorSetLayoutSupport = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupport>(load_vulkan_func_with_device("vkGetDescriptorSetLayoutSupport"));
		if (!vkGetDescriptorSetLayoutSupport)
			return false;
		return true;
	}

	static bool load_vulkan_1_2_device() {
		PRINT_DEBUG("Loading device-level Vulkan 1.2-functions");
		vkCmdDrawIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndirectCount>(load_vulkan_func_with_device("vkCmdDrawIndirectCount"));
		if (!vkCmdDrawIndirectCount)
			return false;
		vkCmdDrawIndexedIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCount>(load_vulkan_func_with_device("vkCmdDrawIndexedIndirectCount"));
		if (!vkCmdDrawIndexedIndirectCount)
			return false;
		vkCreateRenderPass2 = reinterpret_cast<PFN_vkCreateRenderPass2>(load_vulkan_func_with_device("vkCreateRenderPass2"));
		if (!vkCreateRenderPass2)
			return false;
		vkCmdBeginRenderPass2 = reinterpret_cast<PFN_vkCmdBeginRenderPass2>(load_vulkan_func_with_device("vkCmdBeginRenderPass2"));
		if (!vkCmdBeginRenderPass2)
			return false;
		vkCmdNextSubpass2 = reinterpret_cast<PFN_vkCmdNextSubpass2>(load_vulkan_func_with_device("vkCmdNextSubpass2"));
		if (!vkCmdNextSubpass2)
			return false;
		vkCmdEndRenderPass2 = reinterpret_cast<PFN_vkCmdEndRenderPass2>(load_vulkan_func_with_device("vkCmdEndRenderPass2"));
		if (!vkCmdEndRenderPass2)
			return false;
		vkResetQueryPool = reinterpret_cast<PFN_vkResetQueryPool>(load_vulkan_func_with_device("vkResetQueryPool"));
		if (!vkResetQueryPool)
			return false;
		vkGetSemaphoreCounterValue = reinterpret_cast<PFN_vkGetSemaphoreCounterValue>(load_vulkan_func_with_device("vkGetSemaphoreCounterValue"));
		if (!vkGetSemaphoreCounterValue)
			return false;
		vkWaitSemaphores = reinterpret_cast<PFN_vkWaitSemaphores>(load_vulkan_func_with_device("vkWaitSemaphores"));
		if (!vkWaitSemaphores)
			return false;
		vkSignalSemaphore = reinterpret_cast<PFN_vkSignalSemaphore>(load_vulkan_func_with_device("vkSignalSemaphore"));
		if (!vkSignalSemaphore)
			return false;
		vkGetBufferDeviceAddress = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(load_vulkan_func_with_device("vkGetBufferDeviceAddress"));
		if (!vkGetBufferDeviceAddress)
			return false;
		vkGetBufferOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddress>(load_vulkan_func_with_device("vkGetBufferOpaqueCaptureAddress"));
		if (!vkGetBufferOpaqueCaptureAddress)
			return false;
		vkGetDeviceMemoryOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddress>(load_vulkan_func_with_device("vkGetDeviceMemoryOpaqueCaptureAddress"));
		if (!vkGetDeviceMemoryOpaqueCaptureAddress)
			return false;
		return true;
	}

	static bool load_vulkan_1_3_device() {
		PRINT_DEBUG("Loading device-level Vulkan 1.3-functions");
		vkCreatePrivateDataSlot = reinterpret_cast<PFN_vkCreatePrivateDataSlot>(load_vulkan_func_with_device("vkCreatePrivateDataSlot"));
		if (!vkCreatePrivateDataSlot)
			return false;
		vkDestroyPrivateDataSlot = reinterpret_cast<PFN_vkDestroyPrivateDataSlot>(load_vulkan_func_with_device("vkDestroyPrivateDataSlot"));
		if (!vkDestroyPrivateDataSlot)
			return false;
		vkSetPrivateData = reinterpret_cast<PFN_vkSetPrivateData>(load_vulkan_func_with_device("vkSetPrivateData"));
		if (!vkSetPrivateData)
			return false;
		vkGetPrivateData = reinterpret_cast<PFN_vkGetPrivateData>(load_vulkan_func_with_device("vkGetPrivateData"));
		if (!vkGetPrivateData)
			return false;
		if (!load_synchronization_2_funcs())
			return false;
		vkCmdCopyBuffer2 = reinterpret_cast<PFN_vkCmdCopyBuffer2>(load_vulkan_func_with_device("vkCmdCopyBuffer2"));
		if (!vkCmdCopyBuffer2)
			return false;
		vkCmdCopyImage2 = reinterpret_cast<PFN_vkCmdCopyImage2>(load_vulkan_func_with_device("vkCmdCopyImage2"));
		if (!vkCmdCopyImage2)
			return false;
		vkCmdCopyBufferToImage2 = reinterpret_cast<PFN_vkCmdCopyBufferToImage2>(load_vulkan_func_with_device("vkCmdCopyBufferToImage2"));
		if (!vkCmdCopyBufferToImage2)
			return false;
		vkCmdCopyImageToBuffer2 = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2>(load_vulkan_func_with_device("vkCmdCopyImageToBuffer2"));
		if (!vkCmdCopyImageToBuffer2)
			return false;
		vkCmdBlitImage2 = reinterpret_cast<PFN_vkCmdBlitImage2>(load_vulkan_func_with_device("vkCmdBlitImage2"));
		if (!vkCmdBlitImage2)
			return false;
		vkCmdResolveImage2 = reinterpret_cast<PFN_vkCmdResolveImage2>(load_vulkan_func_with_device("vkCmdResolveImage2"));
		if (!vkCmdResolveImage2)
			return false;
		vkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(load_vulkan_func_with_device("vkCmdBeginRendering"));
		if (!vkCmdBeginRendering)
			return false;
		vkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(load_vulkan_func_with_device("vkCmdEndRendering"));
		if (!vkCmdEndRendering)
			return false;
		vkCmdSetCullMode = reinterpret_cast<PFN_vkCmdSetCullMode>(load_vulkan_func_with_device("vkCmdSetCullMode"));
		if (!vkCmdSetCullMode)
			return false;
		vkCmdSetFrontFace = reinterpret_cast<PFN_vkCmdSetFrontFace>(load_vulkan_func_with_device("vkCmdSetFrontFace"));
		if (!vkCmdSetFrontFace)
			return false;
		vkCmdSetPrimitiveTopology = reinterpret_cast<PFN_vkCmdSetPrimitiveTopology>(load_vulkan_func_with_device("vkCmdSetPrimitiveTopology"));
		if (!vkCmdSetPrimitiveTopology)
			return false;
		vkCmdSetViewportWithCount = reinterpret_cast<PFN_vkCmdSetViewportWithCount>(load_vulkan_func_with_device("vkCmdSetViewportWithCount"));
		if (!vkCmdSetViewportWithCount)
			return false;
		vkCmdSetScissorWithCount = reinterpret_cast<PFN_vkCmdSetScissorWithCount>(load_vulkan_func_with_device("vkCmdSetScissorWithCount"));
		if (!vkCmdSetScissorWithCount)
			return false;
		vkCmdBindVertexBuffers2 = reinterpret_cast<PFN_vkCmdBindVertexBuffers2>(load_vulkan_func_with_device("vkCmdBindVertexBuffers2"));
		if (!vkCmdBindVertexBuffers2)
			return false;
		vkCmdSetDepthTestEnable = reinterpret_cast<PFN_vkCmdSetDepthTestEnable>(load_vulkan_func_with_device("vkCmdSetDepthTestEnable"));
		if (!vkCmdSetDepthTestEnable)
			return false;
		vkCmdSetDepthWriteEnable = reinterpret_cast<PFN_vkCmdSetDepthWriteEnable>(load_vulkan_func_with_device("vkCmdSetDepthWriteEnable"));
		if (!vkCmdSetDepthWriteEnable)
			return false;
		vkCmdSetDepthCompareOp = reinterpret_cast<PFN_vkCmdSetDepthCompareOp>(load_vulkan_func_with_device("vkCmdSetDepthCompareOp"));
		if (!vkCmdSetDepthCompareOp)
			return false;
		vkCmdSetDepthBoundsTestEnable = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnable>(load_vulkan_func_with_device("vkCmdSetDepthBoundsTestEnable"));
		if (!vkCmdSetDepthBoundsTestEnable)
			return false;
		vkCmdSetStencilTestEnable = reinterpret_cast<PFN_vkCmdSetStencilTestEnable>(load_vulkan_func_with_device("vkCmdSetStencilTestEnable"));
		if (!vkCmdSetStencilTestEnable)
			return false;
		vkCmdSetStencilOp = reinterpret_cast<PFN_vkCmdSetStencilOp>(load_vulkan_func_with_device("vkCmdSetStencilOp"));
		if (!vkCmdSetStencilOp)
			return false;
		vkCmdSetRasterizerDiscardEnable = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnable>(load_vulkan_func_with_device("vkCmdSetRasterizerDiscardEnable"));
		if (!vkCmdSetRasterizerDiscardEnable)
			return false;
		vkCmdSetDepthBiasEnable = reinterpret_cast<PFN_vkCmdSetDepthBiasEnable>(load_vulkan_func_with_device("vkCmdSetDepthBiasEnable"));
		if (!vkCmdSetDepthBiasEnable)
			return false;
		vkCmdSetPrimitiveRestartEnable = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnable>(load_vulkan_func_with_device("vkCmdSetPrimitiveRestartEnable"));
		if (!vkCmdSetPrimitiveRestartEnable)
			return false;
		vkGetDeviceBufferMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirements>(load_vulkan_func_with_device("vkGetDeviceBufferMemoryRequirements"));
		if (!vkGetDeviceBufferMemoryRequirements)
			return false;
		vkGetDeviceImageMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirements>(load_vulkan_func_with_device("vkGetDeviceImageMemoryRequirements"));
		if (!vkGetDeviceImageMemoryRequirements)
			return false;
		vkGetDeviceImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirements>(load_vulkan_func_with_device("vkGetDeviceImageSparseMemoryRequirements"));
		if (!vkGetDeviceImageSparseMemoryRequirements)
			return false;
		return true;
	}

	/* static bool load_vulkan_1_4_device() {
		PRINT_DEBUG("Loading device-level Vulkan 1.4-functions");
		vkCmdSetLineStipple = reinterpret_cast<PFN_vkCmdSetLineStipple>(load_vulkan_func_with_device("vkCmdSetLineStipple"));
		if (!vkCmdSetLineStipple)
			return false;
		vkMapMemory2 = reinterpret_cast<PFN_vkMapMemory2>(load_vulkan_func_with_device("vkMapMemory2"));
		if (!vkMapMemory2)
			return false;
		vkUnmapMemory2 = reinterpret_cast<PFN_vkUnmapMemory2>(load_vulkan_func_with_device("vkUnmapMemory2"));
		if (!vkUnmapMemory2)
			return false;
		vkCmdBindIndexBuffer2 = reinterpret_cast<PFN_vkCmdBindIndexBuffer2>(load_vulkan_func_with_device("vkCmdBindIndexBuffer2"));
		if (!vkCmdBindIndexBuffer2)
			return false;
		vkGetRenderingAreaGranularity = reinterpret_cast<PFN_vkGetRenderingAreaGranularity>(load_vulkan_func_with_device("vkCmdSetLineStipple"));
		if (!vkGetRenderingAreaGranularity)
			return false;
		vkGetDeviceImageSubresourceLayout = reinterpret_cast<PFN_vkGetDeviceImageSubresourceLayout>(load_vulkan_func_with_device("vkGetDeviceImageSubresourceLayout"));
		if (!vkGetDeviceImageSubresourceLayout)
			return false;
		vkGetImageSubresourceLayout2 = reinterpret_cast<PFN_vkGetImageSubresourceLayout2>(load_vulkan_func_with_device("vkGetImageSubresourceLayout2"));
		if (!vkGetImageSubresourceLayout2)
			return false;
		vkCmdPushDescriptorSet = reinterpret_cast<PFN_vkCmdPushDescriptorSet>(load_vulkan_func_with_device("vkCmdPushDescriptorSet"));
		if (!vkCmdPushDescriptorSet)
			return false;
		vkCmdPushDescriptorSetWithTemplate = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplate>(load_vulkan_func_with_device("vkCmdPushDescriptorSetWithTemplate"));
		if (!vkCmdPushDescriptorSetWithTemplate)
			return false;
		vkCmdSetRenderingAttachmentLocations = reinterpret_cast<PFN_vkCmdSetRenderingAttachmentLocations>(load_vulkan_func_with_device("vkCmdSetRenderingAttachmentLocations"));
		if (!vkCmdSetRenderingAttachmentLocations)
			return false;
		vkCmdSetRenderingInputAttachmentIndices = reinterpret_cast<PFN_vkCmdSetRenderingInputAttachmentIndices>(load_vulkan_func_with_device("vkCmdSetRenderingInputAttachmentIndices"));
		if (!vkCmdSetRenderingInputAttachmentIndices)
			return false;
		vkCmdBindDescriptorSets2 = reinterpret_cast<PFN_vkCmdBindDescriptorSets2>(load_vulkan_func_with_device("vkCmdBindDescriptorSets2"));
		if (!vkCmdBindDescriptorSets2)
			return false;
		vkCmdPushConstants2 = reinterpret_cast<PFN_vkCmdPushConstants2>(load_vulkan_func_with_device("vkCmdPushConstants2"));
		if (!vkCmdPushConstants2)
			return false;
		vkCmdPushDescriptorSet2 = reinterpret_cast<PFN_vkCmdPushDescriptorSet2>(load_vulkan_func_with_device("vkCmdPushDescriptorSet2"));
		if (!vkCmdPushDescriptorSet2)
			return false;
		vkCmdPushDescriptorSetWithTemplate2 = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplate2>(load_vulkan_func_with_device("vkCmdPushDescriptorSetWithTemplate2"));
		if (!vkCmdPushDescriptorSetWithTemplate2)
			return false;
		vkCopyMemoryToImage = reinterpret_cast<PFN_vkCopyMemoryToImage>(load_vulkan_func_with_device("vkCopyMemoryToImage"));
		if (!vkCopyMemoryToImage)
			return false;
		vkCopyImageToMemory = reinterpret_cast<PFN_vkCopyImageToMemory>(load_vulkan_func_with_device("vkCopyImageToMemory"));
		if (!vkCopyImageToMemory)
			return false;
		vkCopyImageToImage = reinterpret_cast<PFN_vkCopyImageToImage>(load_vulkan_func_with_device("vkCopyImageToImage"));
		if (!vkCopyImageToImage)
			return false;
		vkTransitionImageLayout = reinterpret_cast<PFN_vkTransitionImageLayout>(load_vulkan_func_with_device("vkTransitionImageLayout"));
		if (!vkTransitionImageLayout)
			return false;
		return true;
	} */

	static bool load_extension_funcs_with_device() {
		PRINT_DEBUG("Loading device-level Vulkan extension-functions");
		vkQueueBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueBeginDebugUtilsLabelEXT>(load_vulkan_func_with_device("vkQueueBeginDebugUtilsLabelEXT"));
		if (!vkQueueBeginDebugUtilsLabelEXT)
			return false;
		vkQueueEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueEndDebugUtilsLabelEXT>(load_vulkan_func_with_device("vkQueueEndDebugUtilsLabelEXT"));
		if (!vkQueueEndDebugUtilsLabelEXT)
			return false;
		vkQueueInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueInsertDebugUtilsLabelEXT>(load_vulkan_func_with_device("vkQueueInsertDebugUtilsLabelEXT"));
		if (!vkQueueInsertDebugUtilsLabelEXT)
			return false;
		vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(load_vulkan_func_with_device("vkCmdBeginDebugUtilsLabelEXT"));
		if (!vkCmdBeginDebugUtilsLabelEXT)
			return false;
		vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(load_vulkan_func_with_device("vkCmdEndDebugUtilsLabelEXT"));
		if (!vkCmdEndDebugUtilsLabelEXT)
			return false;
		vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(load_vulkan_func_with_device("vkCmdInsertDebugUtilsLabelEXT"));
		if (!vkCmdInsertDebugUtilsLabelEXT)
			return false;
		vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(load_vulkan_func_with_device("vkCreateSwapchainKHR"));
		if (!vkCreateSwapchainKHR)
			return false;
		vkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(load_vulkan_func_with_device("vkDestroySwapchainKHR"));
		if (!vkDestroySwapchainKHR)
			return false;
		vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(load_vulkan_func_with_device("vkGetSwapchainImagesKHR"));
		if (!vkGetSwapchainImagesKHR)
			return false;
		vkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(load_vulkan_func_with_device("vkAcquireNextImageKHR"));
		if (!vkAcquireNextImageKHR)
			return false;
		vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(load_vulkan_func_with_device("vkQueuePresentKHR"));
		if (!vkQueuePresentKHR)
			return false;
		vkGetDeviceGroupPresentCapabilitiesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPresentCapabilitiesKHR>(load_vulkan_func_with_device("vkGetDeviceGroupPresentCapabilitiesKHR"));
		if (!vkGetDeviceGroupPresentCapabilitiesKHR)
			return false;
		vkGetDeviceGroupSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModesKHR>(load_vulkan_func_with_device("vkGetDeviceGroupSurfacePresentModesKHR"));
		if (!vkGetDeviceGroupSurfacePresentModesKHR)
			return false;
		vkAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(load_vulkan_func_with_device("vkAcquireNextImage2KHR"));
		if (!vkAcquireNextImage2KHR)
			return false;
		return true;
	}

	// Avoids dangling pointers (don't remove!)
	static void unload_all_vulkan_functions_of_device() {
		PRINT_DEBUG("Resetting all device-level function pointers to Vulkan");
		// Vulkan 1.0
		vkGetDeviceQueue = nullptr;
		vkQueueSubmit = nullptr;
		vkQueueWaitIdle = nullptr;
		vkDeviceWaitIdle = nullptr;
		vkAllocateMemory = nullptr;
		vkFreeMemory = nullptr;
		vkMapMemory = nullptr;
		vkUnmapMemory = nullptr;
		vkFlushMappedMemoryRanges = nullptr;
		vkInvalidateMappedMemoryRanges = nullptr;
		vkGetDeviceMemoryCommitment = nullptr;
		vkBindBufferMemory = nullptr;
		vkBindImageMemory = nullptr;
		vkGetBufferMemoryRequirements = nullptr;
		vkGetImageMemoryRequirements = nullptr;
		vkGetImageSparseMemoryRequirements = nullptr;
		vkQueueBindSparse = nullptr;
		vkCreateFence = nullptr;
		vkDestroyFence = nullptr;
		vkResetFences = nullptr;
		vkGetFenceStatus = nullptr;
		vkWaitForFences = nullptr;
		vkCreateSemaphore = nullptr;
		vkDestroySemaphore = nullptr;
		vkCreateEvent = nullptr;
		vkDestroyEvent = nullptr;
		vkGetEventStatus = nullptr;
		vkSetEvent = nullptr;
		vkResetEvent = nullptr;
		vkCreateQueryPool = nullptr;
		vkDestroyQueryPool = nullptr;
		vkGetQueryPoolResults = nullptr;
		vkCreateBuffer = nullptr;
		vkDestroyBuffer = nullptr;
		vkCreateBufferView = nullptr;
		vkDestroyBufferView = nullptr;
		vkCreateImage = nullptr;
		vkDestroyImage = nullptr;
		vkGetImageSubresourceLayout = nullptr;
		vkCreateImageView = nullptr;
		vkDestroyImageView = nullptr;
		vkCreateShaderModule = nullptr;
		vkDestroyShaderModule = nullptr;
		vkCreatePipelineCache = nullptr;
		vkDestroyPipelineCache = nullptr;
		vkGetPipelineCacheData = nullptr;
		vkMergePipelineCaches = nullptr;
		vkCreateGraphicsPipelines = nullptr;
		vkCreateComputePipelines = nullptr;
		vkDestroyPipeline = nullptr;
		vkCreatePipelineLayout = nullptr;
		vkDestroyPipelineLayout = nullptr;
		vkCreateSampler = nullptr;
		vkDestroySampler = nullptr;
		vkCreateDescriptorSetLayout = nullptr;
		vkDestroyDescriptorSetLayout = nullptr;
		vkCreateDescriptorPool = nullptr;
		vkDestroyDescriptorPool = nullptr;
		vkResetDescriptorPool = nullptr;
		vkAllocateDescriptorSets = nullptr;
		vkFreeDescriptorSets = nullptr;
		vkUpdateDescriptorSets = nullptr;
		vkCreateFramebuffer = nullptr;
		vkDestroyFramebuffer = nullptr;
		vkCreateRenderPass = nullptr;
		vkDestroyRenderPass = nullptr;
		vkGetRenderAreaGranularity = nullptr;
		vkCreateCommandPool = nullptr;
		vkDestroyCommandPool = nullptr;
		vkResetCommandPool = nullptr;
		vkAllocateCommandBuffers = nullptr;
		vkFreeCommandBuffers = nullptr;
		vkBeginCommandBuffer = nullptr;
		vkEndCommandBuffer = nullptr;
		vkResetCommandBuffer = nullptr;
		vkCmdBindPipeline = nullptr;
		vkCmdSetViewport = nullptr;
		vkCmdSetScissor = nullptr;
		vkCmdSetLineWidth = nullptr;
		vkCmdSetDepthBias = nullptr;
		vkCmdSetBlendConstants = nullptr;
		vkCmdSetDepthBounds = nullptr;
		vkCmdSetStencilCompareMask = nullptr;
		vkCmdSetStencilWriteMask = nullptr;
		vkCmdSetStencilReference = nullptr;
		vkCmdBindDescriptorSets = nullptr;
		vkCmdBindIndexBuffer = nullptr;
		vkCmdBindVertexBuffers = nullptr;
		vkCmdDraw = nullptr;
		vkCmdDrawIndexed = nullptr;
		vkCmdDrawIndirect = nullptr;
		vkCmdDrawIndexedIndirect = nullptr;
		vkCmdDispatch = nullptr;
		vkCmdDispatchIndirect = nullptr;
		vkCmdCopyBuffer = nullptr;
		vkCmdCopyImage = nullptr;
		vkCmdBlitImage = nullptr;
		vkCmdCopyBufferToImage = nullptr;
		vkCmdCopyImageToBuffer = nullptr;
		vkCmdUpdateBuffer = nullptr;
		vkCmdFillBuffer = nullptr;
		vkCmdClearColorImage = nullptr;
		vkCmdClearDepthStencilImage = nullptr;
		vkCmdClearAttachments = nullptr;
		vkCmdResolveImage = nullptr;
		vkCmdSetEvent = nullptr;
		vkCmdResetEvent = nullptr;
		vkCmdWaitEvents = nullptr;
		vkCmdPipelineBarrier = nullptr;
		vkCmdBeginQuery = nullptr;
		vkCmdEndQuery = nullptr;
		vkCmdResetQueryPool = nullptr;
		vkCmdWriteTimestamp = nullptr;
		vkCmdCopyQueryPoolResults = nullptr;
		vkCmdPushConstants = nullptr;
		vkCmdBeginRenderPass = nullptr;
		vkCmdNextSubpass = nullptr;
		vkCmdEndRenderPass = nullptr;
		vkCmdExecuteCommands = nullptr;

		// Vulkan 1.1
		vkBindBufferMemory2 = nullptr;
		vkBindImageMemory2 = nullptr;
		vkGetDeviceGroupPeerMemoryFeatures = nullptr;
		vkCmdSetDeviceMask = nullptr;
		vkCmdDispatchBase = nullptr;
		vkGetImageMemoryRequirements2 = nullptr;
		vkGetBufferMemoryRequirements2 = nullptr;
		vkGetImageSparseMemoryRequirements2 = nullptr;
		vkTrimCommandPool = nullptr;
		vkGetDeviceQueue2 = nullptr;
		vkCreateSamplerYcbcrConversion = nullptr;
		vkDestroySamplerYcbcrConversion = nullptr;
		vkCreateDescriptorUpdateTemplate = nullptr;
		vkDestroyDescriptorUpdateTemplate = nullptr;
		vkUpdateDescriptorSetWithTemplate = nullptr;
		vkGetDescriptorSetLayoutSupport = nullptr;

		// Vulkan 1.2
		vkCmdDrawIndirectCount = nullptr;
		vkCmdDrawIndexedIndirectCount = nullptr;
		vkCreateRenderPass2 = nullptr;
		vkCmdBeginRenderPass2 = nullptr;
		vkCmdNextSubpass2 = nullptr;
		vkCmdEndRenderPass2 = nullptr;
		vkResetQueryPool = nullptr;
		vkGetSemaphoreCounterValue = nullptr;
		vkWaitSemaphores = nullptr;
		vkSignalSemaphore = nullptr;
		vkGetBufferDeviceAddress = nullptr;
		vkGetBufferOpaqueCaptureAddress = nullptr;
		vkGetDeviceMemoryOpaqueCaptureAddress = nullptr;

		// Vulkan 1.3
		vkCreatePrivateDataSlot = nullptr;
		vkDestroyPrivateDataSlot = nullptr;
		vkSetPrivateData = nullptr;
		vkGetPrivateData = nullptr;
		unload_synchronization_2_funcs();
		vkCmdCopyBuffer2 = nullptr;
		vkCmdCopyImage2 = nullptr;
		vkCmdCopyBufferToImage2 = nullptr;
		vkCmdCopyImageToBuffer2 = nullptr;
		vkCmdBlitImage2 = nullptr;
		vkCmdResolveImage2 = nullptr;
		vkCmdBeginRendering = nullptr;
		vkCmdEndRendering = nullptr;
		vkCmdSetCullMode = nullptr;
		vkCmdSetFrontFace = nullptr;
		vkCmdSetPrimitiveTopology = nullptr;
		vkCmdSetViewportWithCount = nullptr;
		vkCmdSetScissorWithCount = nullptr;
		vkCmdBindVertexBuffers2 = nullptr;
		vkCmdSetDepthTestEnable = nullptr;
		vkCmdSetDepthWriteEnable = nullptr;
		vkCmdSetDepthCompareOp = nullptr;
		vkCmdSetDepthBoundsTestEnable = nullptr;
		vkCmdSetStencilTestEnable = nullptr;
		vkCmdSetStencilOp = nullptr;
		vkCmdSetRasterizerDiscardEnable = nullptr;
		vkCmdSetDepthBiasEnable = nullptr;
		vkCmdSetPrimitiveRestartEnable = nullptr;
		vkGetDeviceBufferMemoryRequirements = nullptr;
		vkGetDeviceImageMemoryRequirements = nullptr;
		vkGetDeviceImageSparseMemoryRequirements = nullptr;

		// Vulkan 1.4
		/* vkCmdSetLineStipple = nullptr;
		vkMapMemory2 = nullptr;
		vkUnmapMemory2 = nullptr;
		vkCmdBindIndexBuffer2 = nullptr;
		vkGetRenderingAreaGranularity = nullptr;
		vkGetDeviceImageSubresourceLayout = nullptr;
		vkGetImageSubresourceLayout2 = nullptr;
		vkCmdPushDescriptorSet = nullptr;
		vkCmdPushDescriptorSetWithTemplate = nullptr;
		vkCmdSetRenderingAttachmentLocations = nullptr;
		vkCmdSetRenderingInputAttachmentIndices = nullptr;
		vkCmdBindDescriptorSets2 = nullptr;
		vkCmdPushConstants2 = nullptr;
		vkCmdPushDescriptorSet2 = nullptr;
		vkCmdPushDescriptorSetWithTemplate2 = nullptr;
		vkCopyMemoryToImage = nullptr;
		vkCopyImageToMemory = nullptr;
		vkCopyImageToImage = nullptr;
		vkTransitionImageLayout = nullptr; */

		// Debug extension
		vkQueueBeginDebugUtilsLabelEXT = nullptr;
		vkQueueEndDebugUtilsLabelEXT = nullptr;
		vkQueueInsertDebugUtilsLabelEXT = nullptr;
		vkCmdBeginDebugUtilsLabelEXT = nullptr;
		vkCmdEndDebugUtilsLabelEXT = nullptr;
		vkCmdInsertDebugUtilsLabelEXT = nullptr;
		// Swapchain extension
		vkCreateSwapchainKHR = nullptr;
		vkDestroySwapchainKHR = nullptr;
		vkGetSwapchainImagesKHR = nullptr;
		vkAcquireNextImageKHR = nullptr;
		vkQueuePresentKHR = nullptr;
		vkGetDeviceGroupPresentCapabilitiesKHR = nullptr;
		vkGetDeviceGroupSurfacePresentModesKHR = nullptr;
		vkAcquireNextImage2KHR = nullptr;
	}

	bool init_logical_vulkan_device() {
		constexpr uint32_t u32LogicalDeviceExtensionCount = 1;
		const std::array<const char*, u32LogicalDeviceExtensionCount> a2cLogicalDeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		PRINT_DEBUG("Creating create Vulkan queue-information structs");
		constexpr float fQueuePriority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> vk_paDeviceQueueCreateInfos;
		create_queue_create_infos(&fQueuePriority, vk_paDeviceQueueCreateInfos);

		PRINT_DEBUG("Creating logical Vulkan device");
		VkPhysicalDeviceVulkan13Features vk_enabledFeatures_1_3 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
			.pNext = nullptr,
			.synchronization2 = are_vulkan_features_enabled<ENABLED_FEATURE_SYNCHRONIZATION_2_BIT>(),
			.dynamicRendering = are_vulkan_features_enabled<ENABLED_FEATURE_DYNAMIC_RENDERING_BIT>()
		};
		VkPhysicalDeviceVulkan12Features vk_enabledFeatures_1_2 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
			.pNext = &vk_enabledFeatures_1_3,
			.descriptorBindingUpdateUnusedWhilePending = VK_TRUE,
			.descriptorBindingPartiallyBound = VK_TRUE,
			.timelineSemaphore = VK_TRUE
		};
		const VkPhysicalDeviceVulkan11Features vk_enabledFeatures_1_1 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
			.pNext = &vk_enabledFeatures_1_2,
		};
		const VkPhysicalDeviceFeatures vk_physicalDeviceFeaturesEnabled = {
			.sampleRateShading = are_vulkan_features_enabled<ENABLED_FEATURE_SAMPLE_RATE_SHADING_BIT>(),
			.samplerAnisotropy = are_vulkan_features_enabled<ENABLED_FEATURE_ANISOTROPIC_FILTERING_BIT>(),
			.sparseBinding = are_vulkan_features_enabled<ENABLED_FEATURE_SPARSE_BINDING_BIT>(),
			.sparseResidencyBuffer = are_vulkan_features_enabled<ENABLED_FEATURE_SPARSE_RESIDENCY_BUFFER_BIT>(),
			.sparseResidencyImage2D = are_vulkan_features_enabled<ENABLED_FEATURE_SPARSE_RESIDENCY_IMAGE_2D_BIT>(),
			.sparseResidencyAliased = are_vulkan_features_enabled<ENABLED_FEATURE_SPARSE_ALIASING_BIT>()
		};
		const VkDeviceCreateInfo vk_deviceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = &vk_enabledFeatures_1_1,
			.queueCreateInfoCount = static_cast<uint32_t>(vk_paDeviceQueueCreateInfos.size()),
			.pQueueCreateInfos = vk_paDeviceQueueCreateInfos.data(),
			.enabledExtensionCount = u32LogicalDeviceExtensionCount,
			.ppEnabledExtensionNames = static_cast<const char *const *>(a2cLogicalDeviceExtensions.data()),
			.pEnabledFeatures = &vk_physicalDeviceFeaturesEnabled
		};
		if (vkCreateDevice(SELECTED_PHYSICAL_VULKAN_DEVICE, &vk_deviceCreateInfo, nullptr, &vk_hDevice) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating logical Vulkan device");
			return false;
		}

		PRINT_DEBUG("Loading logical Vulkan device-level functions with device ", vk_hDevice);
		if (!load_vulkan_1_0_device() 
				|| !load_vulkan_1_1_device() 
				|| !load_vulkan_1_2_device() 
				|| !load_vulkan_1_3_device() 
				/* || !load_vulkan_1_4_device() */ 
				|| !load_extension_funcs_with_device()) {
			destroy_logical_vulkan_device();
			return false;
		}
		return true;
	}
	
	void destroy_logical_vulkan_device() {
		PRINT_DEBUG("Destroying logical Vulkan device ", vk_hDevice);
		vkDestroyDevice(vk_hDevice, nullptr);
		unload_all_vulkan_functions_of_device();
		vk_hDevice = VK_NULL_HANDLE;
	}

	PFN_vkVoidFunction load_vulkan_func_with_device(const char *const pacFuncName) {
		PRINT_DEBUG("Loading device-level Vulkan function \"", pacFuncName, "\" from logical device ", vk_hDevice);
		const PFN_vkVoidFunction pFunc = vkGetDeviceProcAddr(vk_hDevice, pacFuncName);
		if (!pFunc)
			RE_ERROR("Failed loading the Vulkan logical device-level function \"", pacFuncName, "\" with device ", vk_hDevice);
		return pFunc;
	}

}