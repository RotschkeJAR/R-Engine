#include "RE_Vulkan.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"

#include <queue>

namespace RE {

#ifdef RE_OS_WINDOWS
	HMODULE hLibVulkan = nullptr;
#elif defined RE_OS_LINUX
	void *hLibVulkan = nullptr;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	VkInstance vk_hInstance = VK_NULL_HANDLE;

	const char *pcVulkanDebugFocusOnFile = nullptr, *pcVulkanDebugFocusOnFunc = nullptr;
	uint32_t u32VulkanDebugFocusOnLine = 0U, u32VulkanDebugFocusCount = 0U;
	VkDebugUtilsMessengerEXT vk_hDebugMessenger = VK_NULL_HANDLE;

	PFN_vkCreateInstance pfn_vkCreateInstance = nullptr;
	PFN_vkDestroyInstance pfn_vkDestroyInstance = nullptr;
	PFN_vkEnumeratePhysicalDevices pfn_vkEnumeratePhysicalDevices = nullptr;
	PFN_vkGetPhysicalDeviceFeatures pfn_vkGetPhysicalDeviceFeatures = nullptr;
	PFN_vkGetPhysicalDeviceFormatProperties pfn_vkGetPhysicalDeviceFormatProperties = nullptr;
	PFN_vkGetPhysicalDeviceImageFormatProperties pfn_vkGetPhysicalDeviceImageFormatProperties = nullptr;
	PFN_vkGetPhysicalDeviceProperties pfn_vkGetPhysicalDeviceProperties = nullptr;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties pfn_vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
	PFN_vkGetPhysicalDeviceMemoryProperties pfn_vkGetPhysicalDeviceMemoryProperties = nullptr;
	PFN_vkGetInstanceProcAddr pfn_vkGetInstanceProcAddr = nullptr;
	PFN_vkGetDeviceProcAddr pfn_vkGetDeviceProcAddr = nullptr;
	PFN_vkCreateDevice pfn_vkCreateDevice = nullptr;
	PFN_vkDestroyDevice pfn_vkDestroyDevice = nullptr;
	PFN_vkEnumerateInstanceExtensionProperties pfn_vkEnumerateInstanceExtensionProperties = nullptr;
	PFN_vkEnumerateDeviceExtensionProperties pfn_vkEnumerateDeviceExtensionProperties = nullptr;
	PFN_vkEnumerateInstanceLayerProperties pfn_vkEnumerateInstanceLayerProperties = nullptr;
	PFN_vkEnumerateDeviceLayerProperties pfn_vkEnumerateDeviceLayerProperties = nullptr;
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
	PFN_vkEnumerateInstanceVersion pfn_vkEnumerateInstanceVersion = nullptr;
	PFN_vkBindBufferMemory2 pfn_vkBindBufferMemory2 = nullptr;
	PFN_vkBindImageMemory2 pfn_vkBindImageMemory2 = nullptr;
	PFN_vkGetDeviceGroupPeerMemoryFeatures pfn_vkGetDeviceGroupPeerMemoryFeatures = nullptr;
	PFN_vkCmdSetDeviceMask pfn_vkCmdSetDeviceMask = nullptr;
	PFN_vkCmdDispatchBase pfn_vkCmdDispatchBase = nullptr;
	PFN_vkEnumeratePhysicalDeviceGroups pfn_vkEnumeratePhysicalDeviceGroups = nullptr;
	PFN_vkGetImageMemoryRequirements2 pfn_vkGetImageMemoryRequirements2 = nullptr;
	PFN_vkGetBufferMemoryRequirements2 pfn_vkGetBufferMemoryRequirements2 = nullptr;
	PFN_vkGetImageSparseMemoryRequirements2 pfn_vkGetImageSparseMemoryRequirements2 = nullptr;
	PFN_vkGetPhysicalDeviceFeatures2 pfn_vkGetPhysicalDeviceFeatures2 = nullptr;
	PFN_vkGetPhysicalDeviceProperties2 pfn_vkGetPhysicalDeviceProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceFormatProperties2 pfn_vkGetPhysicalDeviceFormatProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceImageFormatProperties2 pfn_vkGetPhysicalDeviceImageFormatProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties2 pfn_vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceMemoryProperties2 pfn_vkGetPhysicalDeviceMemoryProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 pfn_vkGetPhysicalDeviceSparseImageFormatProperties2 = nullptr;
	PFN_vkTrimCommandPool pfn_vkTrimCommandPool = nullptr;
	PFN_vkGetDeviceQueue2 pfn_vkGetDeviceQueue2 = nullptr;
	PFN_vkCreateSamplerYcbcrConversion pfn_vkCreateSamplerYcbcrConversion = nullptr;
	PFN_vkDestroySamplerYcbcrConversion pfn_vkDestroySamplerYcbcrConversion = nullptr;
	PFN_vkCreateDescriptorUpdateTemplate pfn_vkCreateDescriptorUpdateTemplate = nullptr;
	PFN_vkDestroyDescriptorUpdateTemplate pfn_vkDestroyDescriptorUpdateTemplate = nullptr;
	PFN_vkUpdateDescriptorSetWithTemplate pfn_vkUpdateDescriptorSetWithTemplate = nullptr;
	PFN_vkGetPhysicalDeviceExternalBufferProperties pfn_vkGetPhysicalDeviceExternalBufferProperties = nullptr;
	PFN_vkGetPhysicalDeviceExternalFenceProperties pfn_vkGetPhysicalDeviceExternalFenceProperties = nullptr;
	PFN_vkGetPhysicalDeviceExternalSemaphoreProperties pfn_vkGetPhysicalDeviceExternalSemaphoreProperties = nullptr;
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
	PFN_vkGetPhysicalDeviceToolProperties pfn_vkGetPhysicalDeviceToolProperties = nullptr;
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
	PFN_vkSetDebugUtilsObjectNameEXT pfn_vkSetDebugUtilsObjectNameEXT = nullptr;
	PFN_vkSetDebugUtilsObjectTagEXT pfn_vkSetDebugUtilsObjectTagEXT = nullptr;
	PFN_vkQueueBeginDebugUtilsLabelEXT pfn_vkQueueBeginDebugUtilsLabelEXT = nullptr;
	PFN_vkQueueEndDebugUtilsLabelEXT pfn_vkQueueEndDebugUtilsLabelEXT = nullptr;
	PFN_vkQueueInsertDebugUtilsLabelEXT pfn_vkQueueInsertDebugUtilsLabelEXT = nullptr;
	PFN_vkCmdBeginDebugUtilsLabelEXT pfn_vkCmdBeginDebugUtilsLabelEXT = nullptr;
	PFN_vkCmdEndDebugUtilsLabelEXT pfn_vkCmdEndDebugUtilsLabelEXT = nullptr;
	PFN_vkCmdInsertDebugUtilsLabelEXT pfn_vkCmdInsertDebugUtilsLabelEXT = nullptr;
	PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT = nullptr;
	PFN_vkSubmitDebugUtilsMessageEXT pfn_vkSubmitDebugUtilsMessageEXT = nullptr;

	// Surface
	PFN_vkDestroySurfaceKHR pfn_vkDestroySurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR pfn_vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR pfn_vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR pfn_vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;

	// Swapchain
	PFN_vkCreateSwapchainKHR pfn_vkCreateSwapchainKHR = nullptr;
	PFN_vkDestroySwapchainKHR pfn_vkDestroySwapchainKHR = nullptr;
	PFN_vkGetSwapchainImagesKHR pfn_vkGetSwapchainImagesKHR = nullptr;
	PFN_vkAcquireNextImageKHR pfn_vkAcquireNextImageKHR = nullptr;
	PFN_vkQueuePresentKHR pfn_vkQueuePresentKHR = nullptr;
	PFN_vkGetDeviceGroupPresentCapabilitiesKHR pfn_vkGetDeviceGroupPresentCapabilitiesKHR = nullptr;
	PFN_vkGetDeviceGroupSurfacePresentModesKHR pfn_vkGetDeviceGroupSurfacePresentModesKHR = nullptr;
	PFN_vkGetPhysicalDevicePresentRectanglesKHR pfn_vkGetPhysicalDevicePresentRectanglesKHR = nullptr;
	PFN_vkAcquireNextImage2KHR pfn_vkAcquireNextImage2KHR = nullptr;

#ifdef RE_OS_WINDOWS
	// Win32-Surface
	PFN_vkCreateWin32SurfaceKHR pfn_vkCreateWin32SurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR = nullptr;
#elif defined RE_OS_LINUX
	// Wayland-Surface
	PFN_vkCreateWaylandSurfaceKHR pfn_vkCreateWaylandSurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
	// X11-Surface
	PFN_vkCreateXlibSurfaceKHR pfn_vkCreateXlibSurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

	static void* load_func_with_instance(VkInstance vk_hInstance, const char* pFuncName) {
		void* pFuncPtr;
		CATCH_SIGNAL(pFuncPtr = reinterpret_cast<void*>(pfn_vkGetInstanceProcAddr(vk_hInstance, pFuncName)));
		if (!pFuncPtr)
			RE_FATAL_ERROR(append_to_string("Failed loading the Vulkan function \"", pFuncName, "\""));
		return pFuncPtr;
	}

	static void* load_func(const char* pFuncName) {
		return CATCH_SIGNAL_AND_RETURN(load_func_with_instance(vk_hInstance, pFuncName), void*);
	}

#define RE_VK_REQUIRED_EXTENSIONS_COUNT 3U
	const char *pcRequiredExtensions[RE_VK_REQUIRED_EXTENSIONS_COUNT] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, "\0"};
#define RE_VK_REQUIRED_LAYERS_COUNT 1U
	constexpr const char *pcRequiredLayers[RE_VK_REQUIRED_LAYERS_COUNT] = {VK_KHR_VALIDATION_LAYER_NAME};

	static bool create_vulkan_instance() {
		CATCH_SIGNAL(pfn_vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(load_func_with_instance(VK_NULL_HANDLE, "vkCreateInstance")));
		CATCH_SIGNAL(pfn_vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(load_func_with_instance(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties")));
		CATCH_SIGNAL(pfn_vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(load_func_with_instance(VK_NULL_HANDLE, "vkEnumerateInstanceLayerProperties")));
		if (!pfn_vkCreateInstance || !pfn_vkEnumerateInstanceExtensionProperties || !pfn_vkEnumerateInstanceLayerProperties)
			return false;

		bool bFailure = false;
		pcRequiredExtensions[RE_VK_REQUIRED_EXTENSIONS_COUNT - 1U] = Window::pInstance->get_vulkan_required_surface_extension_name();
		uint32_t u32AvailableExtensionsCount = 0U;
		CATCH_SIGNAL(pfn_vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, nullptr));
		VkExtensionProperties* vk_pAvailableExtensions = new VkExtensionProperties[u32AvailableExtensionsCount];
		CATCH_SIGNAL(pfn_vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, vk_pAvailableExtensions));
		bool bExtensionsPresent[RE_VK_REQUIRED_EXTENSIONS_COUNT] = {};
		std::queue<const char*> missingExtensions;
		PRINT_LN("Available Vulkan instance extensions:");
		for (uint32_t u32ExtensionIndex = 0U; u32ExtensionIndex < u32AvailableExtensionsCount; u32ExtensionIndex++) {
			println(append_to_string("\t", vk_pAvailableExtensions[u32ExtensionIndex].extensionName, " (", VK_API_VERSION_MAJOR(vk_pAvailableExtensions[u32ExtensionIndex].specVersion), ".",VK_API_VERSION_MINOR(vk_pAvailableExtensions[u32ExtensionIndex].specVersion), ".", VK_API_VERSION_PATCH(vk_pAvailableExtensions[u32ExtensionIndex].specVersion), ")"));
			for (uint8_t u8RequiredExtensionIndex = 0U; u8RequiredExtensionIndex < RE_VK_REQUIRED_EXTENSIONS_COUNT; u8RequiredExtensionIndex++)
				if (are_string_contents_equal(vk_pAvailableExtensions[u32ExtensionIndex].extensionName, pcRequiredExtensions[u8RequiredExtensionIndex]))
					bExtensionsPresent[u8RequiredExtensionIndex] = true;
		}
		delete[] vk_pAvailableExtensions;
		for (uint8_t u8RequiredExtensionIndex = 0U; u8RequiredExtensionIndex < RE_VK_REQUIRED_EXTENSIONS_COUNT; u8RequiredExtensionIndex++)
			if (!bExtensionsPresent[u8RequiredExtensionIndex])
				missingExtensions.push(pcRequiredExtensions[u8RequiredExtensionIndex]);
		if (!missingExtensions.empty()) {
			bFailure = true;
			println_colored("\tThe following Vulkan instance extensions are missing on this computer:", RE_TERMINAL_COLOR_RED, false, false);
			do {
				println_colored(append_to_string("\t - ", missingExtensions.front()).c_str(), RE_TERMINAL_COLOR_RED, false, false);
				missingExtensions.pop();
			} while (!missingExtensions.empty());
		}

		uint32_t u32AvailableLayersCount = 0U;
		CATCH_SIGNAL(pfn_vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, nullptr));
		VkLayerProperties* vk_pAvailableLayers = new VkLayerProperties[u32AvailableLayersCount];
		CATCH_SIGNAL(pfn_vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, vk_pAvailableLayers));
		bool bRequiredLayersPresent[RE_VK_REQUIRED_LAYERS_COUNT] = {};
		std::queue<const char*>  missingLayers;
		PRINT_LN("Available Vulkan instance layers:");
		for (uint32_t u32LayerIndex = 0; u32LayerIndex < u32AvailableLayersCount; u32LayerIndex++) {
			println(append_to_string("\t", vk_pAvailableLayers[u32LayerIndex].layerName, " (", VK_API_VERSION_MAJOR(vk_pAvailableLayers[u32LayerIndex].specVersion), ".", VK_API_VERSION_MINOR(vk_pAvailableLayers[u32LayerIndex].specVersion), ".",VK_API_VERSION_PATCH(vk_pAvailableLayers[u32LayerIndex].specVersion), " - ", vk_pAvailableLayers[u32LayerIndex].implementationVersion, "): ", vk_pAvailableLayers[u32LayerIndex].description));
			for (uint8_t u8RequiredLayerIndex = 0U; u8RequiredLayerIndex < RE_VK_REQUIRED_LAYERS_COUNT; u8RequiredLayerIndex++)
				if (are_string_contents_equal(vk_pAvailableLayers[u32LayerIndex].layerName, pcRequiredLayers[u8RequiredLayerIndex]))
					bRequiredLayersPresent[u8RequiredLayerIndex] = true;
		}
		delete[] vk_pAvailableLayers;
		for (uint8_t u8RequiredLayerIndex = 0U; u8RequiredLayerIndex < RE_VK_REQUIRED_LAYERS_COUNT; u8RequiredLayerIndex++)
			if (!bRequiredLayersPresent[u8RequiredLayerIndex])
				missingLayers.push(pcRequiredLayers[u8RequiredLayerIndex]);
		if (!missingLayers.empty()) {
			bFailure = true;
			println_colored("\tThe following Vulkan instance layers are missing on this computer:", RE_TERMINAL_COLOR_RED, false, false);
			do {
				println_colored(append_to_string("\t - ", missingLayers.front()).c_str(), RE_TERMINAL_COLOR_RED, false, false);
				missingLayers.pop();
			} while (!missingLayers.empty());
		}

		if (bFailure)
			return false;
		std::string strAppName = get_app_name();
		VkApplicationInfo vk_appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = strAppName.c_str(),
			.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
			.pEngineName = "R-Engine",
			.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
			.apiVersion = VK_API_VERSION_1_3
		};
		VkInstanceCreateInfo vk_instanceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &vk_appInfo,
			.enabledLayerCount = RE_VK_REQUIRED_LAYERS_COUNT,
			.ppEnabledLayerNames = pcRequiredLayers,
			.enabledExtensionCount = RE_VK_REQUIRED_EXTENSIONS_COUNT,
			.ppEnabledExtensionNames = pcRequiredExtensions
		};
		if (!CHECK_VK_RESULT(pfn_vkCreateInstance(&vk_instanceCreateInfo, nullptr, &vk_hInstance))) {
			RE_FATAL_ERROR("Failed creating Vulkan instance");
			return false;
		}
		return true;
	}

	static bool load_vulkan_1_0() {
		// Skipped initialization of "pfn_vkCreateInstance", because it's already loaded
		pfn_vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(load_func("vkDestroyInstance"));
		if (!pfn_vkDestroyInstance)
			return false;
		pfn_vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(load_func("vkEnumeratePhysicalDevices"));
		if (!pfn_vkEnumeratePhysicalDevices)
			return false;
		pfn_vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(load_func("vkGetPhysicalDeviceFeatures"));
		if (!pfn_vkGetPhysicalDeviceFeatures)
			return false;
		pfn_vkGetPhysicalDeviceFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(load_func("vkGetPhysicalDeviceFormatProperties"));
		if (!pfn_vkGetPhysicalDeviceFormatProperties)
			return false;
		pfn_vkGetPhysicalDeviceImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties>(load_func("vkGetPhysicalDeviceImageFormatProperties"));
		if (!pfn_vkGetPhysicalDeviceImageFormatProperties)
			return false;
		pfn_vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(load_func("vkGetPhysicalDeviceProperties"));
		if (!pfn_vkGetPhysicalDeviceProperties)
			return false;
		pfn_vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(load_func("vkGetPhysicalDeviceQueueFamilyProperties"));
		if (!pfn_vkGetPhysicalDeviceQueueFamilyProperties)
			return false;
		pfn_vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(load_func("vkGetPhysicalDeviceMemoryProperties"));
		if (!pfn_vkGetPhysicalDeviceMemoryProperties)
			return false;
		pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(load_func("vkGetInstanceProcAddr"));
		if (!pfn_vkGetInstanceProcAddr)
			return false;
		pfn_vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(load_func("vkGetDeviceProcAddr"));
		if (!pfn_vkGetDeviceProcAddr)
			return false;
		pfn_vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(load_func("vkCreateDevice"));
		if (!pfn_vkCreateDevice)
			return false;
		pfn_vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(load_func("vkDestroyDevice"));
		if (!pfn_vkDestroyDevice)
			return false;
		// Skipped initialization of "pfn_vkEnumerateInstanceExtensionProperties", because it's already loaded
		pfn_vkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(load_func("vkEnumerateDeviceExtensionProperties"));
		if (!pfn_vkEnumerateDeviceExtensionProperties)
			return false;
		// Skipped initialization of "pfn_vkEnumerateInstanceLayerProperties", because it's already loaded
		pfn_vkEnumerateDeviceLayerProperties = reinterpret_cast<PFN_vkEnumerateDeviceLayerProperties>(load_func("vkEnumerateDeviceLayerProperties"));
		if (!pfn_vkEnumerateDeviceLayerProperties)
			return false;
		pfn_vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(load_func("vkGetDeviceQueue"));
		if (!pfn_vkGetDeviceQueue)
			return false;
		pfn_vkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(load_func("vkQueueSubmit"));
		if (!pfn_vkQueueSubmit)
			return false;
		pfn_vkQueueWaitIdle = reinterpret_cast<PFN_vkQueueWaitIdle>(load_func("vkQueueWaitIdle"));
		if (!pfn_vkQueueWaitIdle)
			return false;
		pfn_vkDeviceWaitIdle = reinterpret_cast<PFN_vkDeviceWaitIdle>(load_func("vkDeviceWaitIdle"));
		if (!pfn_vkDeviceWaitIdle)
			return false;
		pfn_vkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(load_func("vkAllocateMemory"));
		if (!pfn_vkAllocateMemory)
			return false;
		pfn_vkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(load_func("vkFreeMemory"));
		if (!pfn_vkFreeMemory)
			return false;
		pfn_vkMapMemory = reinterpret_cast<PFN_vkMapMemory>(load_func("vkMapMemory"));
		if (!pfn_vkMapMemory)
			return false;
		pfn_vkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(load_func("vkUnmapMemory"));
		if (!pfn_vkUnmapMemory)
			return false;
		pfn_vkFlushMappedMemoryRanges = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(load_func("vkFlushMappedMemoryRanges"));
		if (!pfn_vkFlushMappedMemoryRanges)
			return false;
		pfn_vkInvalidateMappedMemoryRanges = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(load_func("vkInvalidateMappedMemoryRanges"));
		if (!pfn_vkInvalidateMappedMemoryRanges)
			return false;
		pfn_vkGetDeviceMemoryCommitment = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(load_func("vkGetDeviceMemoryCommitment"));
		if (!pfn_vkGetDeviceMemoryCommitment)
			return false;
		pfn_vkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(load_func("vkBindBufferMemory"));
		if (!pfn_vkBindBufferMemory)
			return false;
		pfn_vkBindImageMemory = reinterpret_cast<PFN_vkBindImageMemory>(load_func("vkBindImageMemory"));
		if (!pfn_vkBindImageMemory)
			return false;
		pfn_vkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(load_func("vkGetBufferMemoryRequirements"));
		if (!pfn_vkGetBufferMemoryRequirements)
			return false;
		pfn_vkGetImageMemoryRequirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(load_func("vkGetImageMemoryRequirements"));
		if (!pfn_vkGetImageMemoryRequirements)
			return false;
		pfn_vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(load_func("vkGetImageSparseMemoryRequirements"));
		if (!pfn_vkGetImageSparseMemoryRequirements)
			return false;
		pfn_vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties>(load_func("vkGetPhysicalDeviceSparseImageFormatProperties"));
		if (!pfn_vkGetPhysicalDeviceSparseImageFormatProperties)
			return false;
		pfn_vkQueueBindSparse = reinterpret_cast<PFN_vkQueueBindSparse>(load_func("vkQueueBindSparse"));
		if (!pfn_vkQueueBindSparse)
			return false;
		pfn_vkCreateFence = reinterpret_cast<PFN_vkCreateFence>(load_func("vkCreateFence"));
		if (!pfn_vkCreateFence)
			return false;
		pfn_vkDestroyFence = reinterpret_cast<PFN_vkDestroyFence>(load_func("vkDestroyFence"));
		if (!pfn_vkDestroyFence)
			return false;
		pfn_vkResetFences = reinterpret_cast<PFN_vkResetFences>(load_func("vkResetFences"));
		if (!pfn_vkResetFences)
			return false;
		pfn_vkGetFenceStatus = reinterpret_cast<PFN_vkGetFenceStatus>(load_func("vkGetFenceStatus"));
		if (!pfn_vkGetFenceStatus)
			return false;
		pfn_vkWaitForFences = reinterpret_cast<PFN_vkWaitForFences>(load_func("vkWaitForFences"));
		if (!pfn_vkWaitForFences)
			return false;
		pfn_vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(load_func("vkCreateSemaphore"));
		if (!pfn_vkCreateSemaphore)
			return false;
		pfn_vkDestroySemaphore = reinterpret_cast<PFN_vkDestroySemaphore>(load_func("vkDestroySemaphore"));
		if (!pfn_vkDestroySemaphore)
			return false;
		pfn_vkCreateEvent = reinterpret_cast<PFN_vkCreateEvent>(load_func("vkCreateEvent"));
		if (!pfn_vkCreateEvent)
			return false;
		pfn_vkDestroyEvent = reinterpret_cast<PFN_vkDestroyEvent>(load_func("vkDestroyEvent"));
		if (!pfn_vkDestroyEvent)
			return false;
		pfn_vkGetEventStatus = reinterpret_cast<PFN_vkGetEventStatus>(load_func("vkGetEventStatus"));
		if (!pfn_vkGetEventStatus)
			return false;
		pfn_vkSetEvent = reinterpret_cast<PFN_vkSetEvent>(load_func("vkSetEvent"));
		if (!pfn_vkSetEvent)
			return false;
		pfn_vkResetEvent = reinterpret_cast<PFN_vkResetEvent>(load_func("vkResetEvent"));
		if (!pfn_vkResetEvent)
			return false;
		pfn_vkCreateQueryPool = reinterpret_cast<PFN_vkCreateQueryPool>(load_func("vkCreateQueryPool"));
		if (!pfn_vkCreateQueryPool)
			return false;
		pfn_vkDestroyQueryPool = reinterpret_cast<PFN_vkDestroyQueryPool>(load_func("vkDestroyQueryPool"));
		if (!pfn_vkDestroyQueryPool)
			return false;
		pfn_vkGetQueryPoolResults = reinterpret_cast<PFN_vkGetQueryPoolResults>(load_func("vkGetQueryPoolResults"));
		if (!pfn_vkGetQueryPoolResults)
			return false;
		pfn_vkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(load_func("vkCreateBuffer"));
		if (!pfn_vkCreateBuffer)
			return false;
		pfn_vkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(load_func("vkDestroyBuffer"));
		if (!pfn_vkDestroyBuffer)
			return false;
		pfn_vkCreateBufferView = reinterpret_cast<PFN_vkCreateBufferView>(load_func("vkCreateBufferView"));
		if (!pfn_vkCreateBufferView)
			return false;
		pfn_vkDestroyBufferView = reinterpret_cast<PFN_vkDestroyBufferView>(load_func("vkDestroyBufferView"));
		if (!pfn_vkDestroyBufferView)
			return false;
		pfn_vkCreateImage = reinterpret_cast<PFN_vkCreateImage>(load_func("vkCreateImage"));
		if (!pfn_vkCreateImage)
			return false;
		pfn_vkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(load_func("vkDestroyImage"));
		if (!pfn_vkDestroyImage)
			return false;
		pfn_vkGetImageSubresourceLayout = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(load_func("vkGetImageSubresourceLayout"));
		if (!pfn_vkGetImageSubresourceLayout)
			return false;
		pfn_vkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(load_func("vkCreateImageView"));
		if (!pfn_vkCreateImageView)
			return false;
		pfn_vkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(load_func("vkDestroyImageView"));
		if (!pfn_vkDestroyImageView)
			return false;
		pfn_vkCreateShaderModule = reinterpret_cast<PFN_vkCreateShaderModule>(load_func("vkCreateShaderModule"));
		if (!pfn_vkCreateShaderModule)
			return false;
		pfn_vkDestroyShaderModule = reinterpret_cast<PFN_vkDestroyShaderModule>(load_func("vkDestroyShaderModule"));
		if (!pfn_vkDestroyShaderModule)
			return false;
		pfn_vkCreatePipelineCache = reinterpret_cast<PFN_vkCreatePipelineCache>(load_func("vkCreatePipelineCache"));
		if (!pfn_vkCreatePipelineCache)
			return false;
		pfn_vkDestroyPipelineCache = reinterpret_cast<PFN_vkDestroyPipelineCache>(load_func("vkDestroyPipelineCache"));
		if (!pfn_vkDestroyPipelineCache)
			return false;
		pfn_vkGetPipelineCacheData = reinterpret_cast<PFN_vkGetPipelineCacheData>(load_func("vkGetPipelineCacheData"));
		if (!pfn_vkGetPipelineCacheData)
			return false;
		pfn_vkMergePipelineCaches = reinterpret_cast<PFN_vkMergePipelineCaches>(load_func("vkMergePipelineCaches"));
		if (!pfn_vkMergePipelineCaches)
			return false;
		pfn_vkCreateGraphicsPipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(load_func("vkCreateGraphicsPipelines"));
		if (!pfn_vkCreateGraphicsPipelines)
			return false;
		pfn_vkCreateComputePipelines = reinterpret_cast<PFN_vkCreateComputePipelines>(load_func("vkCreateComputePipelines"));
		if (!pfn_vkCreateComputePipelines)
			return false;
		pfn_vkDestroyPipeline = reinterpret_cast<PFN_vkDestroyPipeline>(load_func("vkDestroyPipeline"));
		if (!pfn_vkDestroyPipeline)
			return false;
		pfn_vkCreatePipelineLayout = reinterpret_cast<PFN_vkCreatePipelineLayout>(load_func("vkCreatePipelineLayout"));
		if (!pfn_vkCreatePipelineLayout)
			return false;
		pfn_vkDestroyPipelineLayout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(load_func("vkDestroyPipelineLayout"));
		if (!pfn_vkDestroyPipelineLayout)
			return false;
		pfn_vkCreateSampler = reinterpret_cast<PFN_vkCreateSampler>(load_func("vkCreateSampler"));
		if (!pfn_vkCreateSampler)
			return false;
		pfn_vkDestroySampler = reinterpret_cast<PFN_vkDestroySampler>(load_func("vkDestroySampler"));
		if (!pfn_vkDestroySampler)
			return false;
		pfn_vkCreateDescriptorSetLayout = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(load_func("vkCreateDescriptorSetLayout"));
		if (!pfn_vkCreateDescriptorSetLayout)
			return false;
		pfn_vkDestroyDescriptorSetLayout = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(load_func("vkDestroyDescriptorSetLayout"));
		if (!pfn_vkDestroyDescriptorSetLayout)
			return false;
		pfn_vkCreateDescriptorPool = reinterpret_cast<PFN_vkCreateDescriptorPool>(load_func("vkCreateDescriptorPool"));
		if (!pfn_vkCreateDescriptorPool)
			return false;
		pfn_vkDestroyDescriptorPool = reinterpret_cast<PFN_vkDestroyDescriptorPool>(load_func("vkDestroyDescriptorPool"));
		if (!pfn_vkDestroyDescriptorPool)
			return false;
		pfn_vkResetDescriptorPool = reinterpret_cast<PFN_vkResetDescriptorPool>(load_func("vkResetDescriptorPool"));
		if (!pfn_vkResetDescriptorPool)
			return false;
		pfn_vkAllocateDescriptorSets = reinterpret_cast<PFN_vkAllocateDescriptorSets>(load_func("vkAllocateDescriptorSets"));
		if (!pfn_vkAllocateDescriptorSets)
			return false;
		pfn_vkFreeDescriptorSets = reinterpret_cast<PFN_vkFreeDescriptorSets>(load_func("vkFreeDescriptorSets"));
		if (!pfn_vkFreeDescriptorSets)
			return false;
		pfn_vkUpdateDescriptorSets = reinterpret_cast<PFN_vkUpdateDescriptorSets>(load_func("vkUpdateDescriptorSets"));
		if (!pfn_vkUpdateDescriptorSets)
			return false;
		pfn_vkCreateFramebuffer = reinterpret_cast<PFN_vkCreateFramebuffer>(load_func("vkCreateFramebuffer"));
		if (!pfn_vkCreateFramebuffer)
			return false;
		pfn_vkDestroyFramebuffer = reinterpret_cast<PFN_vkDestroyFramebuffer>(load_func("vkDestroyFramebuffer"));
		if (!pfn_vkDestroyFramebuffer)
			return false;
		pfn_vkCreateRenderPass = reinterpret_cast<PFN_vkCreateRenderPass>(load_func("vkCreateRenderPass"));
		if (!pfn_vkCreateRenderPass)
			return false;
		pfn_vkDestroyRenderPass = reinterpret_cast<PFN_vkDestroyRenderPass>(load_func("vkDestroyRenderPass"));
		if (!pfn_vkDestroyRenderPass)
			return false;
		pfn_vkGetRenderAreaGranularity = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(load_func("vkGetRenderAreaGranularity"));
		if (!pfn_vkGetRenderAreaGranularity)
			return false;
		pfn_vkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(load_func("vkCreateCommandPool"));
		if (!pfn_vkCreateCommandPool)
			return false;
		pfn_vkDestroyCommandPool = reinterpret_cast<PFN_vkDestroyCommandPool>(load_func("vkDestroyCommandPool"));
		if (!pfn_vkDestroyCommandPool)
			return false;
		pfn_vkResetCommandPool = reinterpret_cast<PFN_vkResetCommandPool>(load_func("vkResetCommandPool"));
		if (!pfn_vkResetCommandPool)
			return false;
		pfn_vkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(load_func("vkAllocateCommandBuffers"));
		if (!pfn_vkAllocateCommandBuffers)
			return false;
		pfn_vkFreeCommandBuffers = reinterpret_cast<PFN_vkFreeCommandBuffers>(load_func("vkFreeCommandBuffers"));
		if (!pfn_vkFreeCommandBuffers)
			return false;
		pfn_vkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(load_func("vkBeginCommandBuffer"));
		if (!pfn_vkBeginCommandBuffer)
			return false;
		pfn_vkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(load_func("vkEndCommandBuffer"));
		if (!pfn_vkEndCommandBuffer)
			return false;
		pfn_vkResetCommandBuffer = reinterpret_cast<PFN_vkResetCommandBuffer>(load_func("vkResetCommandBuffer"));
		if (!pfn_vkResetCommandBuffer)
			return false;
		pfn_vkCmdBindPipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(load_func("vkCmdBindPipeline"));
		if (!pfn_vkCmdBindPipeline)
			return false;
		pfn_vkCmdSetViewport = reinterpret_cast<PFN_vkCmdSetViewport>(load_func("vkCmdSetViewport"));
		if (!pfn_vkCmdSetViewport)
			return false;
		pfn_vkCmdSetScissor = reinterpret_cast<PFN_vkCmdSetScissor>(load_func("vkCmdSetScissor"));
		if (!pfn_vkCmdSetScissor)
			return false;
		pfn_vkCmdSetLineWidth = reinterpret_cast<PFN_vkCmdSetLineWidth>(load_func("vkCmdSetLineWidth"));
		if (!pfn_vkCmdSetLineWidth)
			return false;
		pfn_vkCmdSetDepthBias = reinterpret_cast<PFN_vkCmdSetDepthBias>(load_func("vkCmdSetDepthBias"));
		if (!pfn_vkCmdSetDepthBias)
			return false;
		pfn_vkCmdSetBlendConstants = reinterpret_cast<PFN_vkCmdSetBlendConstants>(load_func("vkCmdSetBlendConstants"));
		if (!pfn_vkCmdSetBlendConstants)
			return false;
		pfn_vkCmdSetDepthBounds = reinterpret_cast<PFN_vkCmdSetDepthBounds>(load_func("vkCmdSetDepthBounds"));
		if (!pfn_vkCmdSetDepthBounds)
			return false;
		pfn_vkCmdSetStencilCompareMask = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(load_func("vkCmdSetStencilCompareMask"));
		if (!pfn_vkCmdSetStencilCompareMask)
			return false;
		pfn_vkCmdSetStencilWriteMask = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(load_func("vkCmdSetStencilWriteMask"));
		if (!pfn_vkCmdSetStencilWriteMask)
			return false;
		pfn_vkCmdSetStencilReference = reinterpret_cast<PFN_vkCmdSetStencilReference>(load_func("vkCmdSetStencilReference"));
		if (!pfn_vkCmdSetStencilReference)
			return false;
		pfn_vkCmdBindDescriptorSets = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(load_func("vkCmdBindDescriptorSets"));
		if (!pfn_vkCmdBindDescriptorSets)
			return false;
		pfn_vkCmdBindIndexBuffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(load_func("vkCmdBindIndexBuffer"));
		if (!pfn_vkCmdBindIndexBuffer)
			return false;
		pfn_vkCmdBindVertexBuffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(load_func("vkCmdBindVertexBuffers"));
		if (!pfn_vkCmdBindVertexBuffers)
			return false;
		pfn_vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(load_func("vkCmdDraw"));
		if (!pfn_vkCmdDraw)
			return false;
		pfn_vkCmdDrawIndexed = reinterpret_cast<PFN_vkCmdDrawIndexed>(load_func("vkCmdDrawIndexed"));
		if (!pfn_vkCmdDrawIndexed)
			return false;
		pfn_vkCmdDrawIndirect = reinterpret_cast<PFN_vkCmdDrawIndirect>(load_func("vkCmdDrawIndirect"));
		if (!pfn_vkCmdDrawIndirect)
			return false;
		pfn_vkCmdDrawIndexedIndirect = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(load_func("vkCmdDrawIndexedIndirect"));
		if (!pfn_vkCmdDrawIndexedIndirect)
			return false;
		pfn_vkCmdDispatch = reinterpret_cast<PFN_vkCmdDispatch>(load_func("vkCmdDispatch"));
		if (!pfn_vkCmdDispatch)
			return false;
		pfn_vkCmdDispatchIndirect = reinterpret_cast<PFN_vkCmdDispatchIndirect>(load_func("vkCmdDispatchIndirect"));
		if (!pfn_vkCmdDispatchIndirect)
			return false;
		pfn_vkCmdCopyBuffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(load_func("vkCmdCopyBuffer"));
		if (!pfn_vkCmdCopyBuffer)
			return false;
		pfn_vkCmdCopyImage = reinterpret_cast<PFN_vkCmdCopyImage>(load_func("vkCmdCopyImage"));
		if (!pfn_vkCmdCopyImage)
			return false;
		pfn_vkCmdBlitImage = reinterpret_cast<PFN_vkCmdBlitImage>(load_func("vkCmdBlitImage"));
		if (!pfn_vkCmdBlitImage)
			return false;
		pfn_vkCmdCopyBufferToImage = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(load_func("vkCmdCopyBufferToImage"));
		if (!pfn_vkCmdCopyBufferToImage)
			return false;
		pfn_vkCmdCopyImageToBuffer = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(load_func("vkCmdCopyImageToBuffer"));
		if (!pfn_vkCmdCopyImageToBuffer)
			return false;
		pfn_vkCmdUpdateBuffer = reinterpret_cast<PFN_vkCmdUpdateBuffer>(load_func("vkCmdUpdateBuffer"));
		if (!pfn_vkCmdUpdateBuffer)
			return false;
		pfn_vkCmdFillBuffer = reinterpret_cast<PFN_vkCmdFillBuffer>(load_func("vkCmdFillBuffer"));
		if (!pfn_vkCmdFillBuffer)
			return false;
		pfn_vkCmdClearColorImage = reinterpret_cast<PFN_vkCmdClearColorImage>(load_func("vkCmdClearColorImage"));
		if (!pfn_vkCmdClearColorImage)
			return false;
		pfn_vkCmdClearDepthStencilImage = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(load_func("vkCmdClearDepthStencilImage"));
		if (!pfn_vkCmdClearDepthStencilImage)
			return false;
		pfn_vkCmdClearAttachments = reinterpret_cast<PFN_vkCmdClearAttachments>(load_func("vkCmdClearAttachments"));
		if (!pfn_vkCmdClearAttachments)
			return false;
		pfn_vkCmdResolveImage = reinterpret_cast<PFN_vkCmdResolveImage>(load_func("vkCmdResolveImage"));
		if (!pfn_vkCmdResolveImage)
			return false;
		pfn_vkCmdSetEvent = reinterpret_cast<PFN_vkCmdSetEvent>(load_func("vkCmdSetEvent"));
		if (!pfn_vkCmdSetEvent)
			return false;
		pfn_vkCmdResetEvent = reinterpret_cast<PFN_vkCmdResetEvent>(load_func("vkCmdResetEvent"));
		if (!pfn_vkCmdResetEvent)
			return false;
		pfn_vkCmdWaitEvents = reinterpret_cast<PFN_vkCmdWaitEvents>(load_func("vkCmdWaitEvents"));
		if (!pfn_vkCmdWaitEvents)
			return false;
		pfn_vkCmdPipelineBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(load_func("vkCmdPipelineBarrier"));
		if (!pfn_vkCmdPipelineBarrier)
			return false;
		pfn_vkCmdBeginQuery = reinterpret_cast<PFN_vkCmdBeginQuery>(load_func("vkCmdBeginQuery"));
		if (!pfn_vkCmdBeginQuery)
			return false;
		pfn_vkCmdEndQuery = reinterpret_cast<PFN_vkCmdEndQuery>(load_func("vkCmdEndQuery"));
		if (!pfn_vkCmdEndQuery)
			return false;
		pfn_vkCmdResetQueryPool = reinterpret_cast<PFN_vkCmdResetQueryPool>(load_func("vkCmdResetQueryPool"));
		if (!pfn_vkCmdResetQueryPool)
			return false;
		pfn_vkCmdWriteTimestamp = reinterpret_cast<PFN_vkCmdWriteTimestamp>(load_func("vkCmdWriteTimestamp"));
		if (!pfn_vkCmdWriteTimestamp)
			return false;
		pfn_vkCmdCopyQueryPoolResults = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(load_func("vkCmdCopyQueryPoolResults"));
		if (!pfn_vkCmdCopyQueryPoolResults)
			return false;
		pfn_vkCmdPushConstants = reinterpret_cast<PFN_vkCmdPushConstants>(load_func("vkCmdPushConstants"));
		if (!pfn_vkCmdPushConstants)
			return false;
		pfn_vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(load_func("vkCmdBeginRenderPass"));
		if (!pfn_vkCmdBeginRenderPass)
			return false;
		pfn_vkCmdNextSubpass = reinterpret_cast<PFN_vkCmdNextSubpass>(load_func("vkCmdNextSubpass"));
		if (!pfn_vkCmdNextSubpass)
			return false;
		pfn_vkCmdEndRenderPass = reinterpret_cast<PFN_vkCmdEndRenderPass>(load_func("vkCmdEndRenderPass"));
		if (!pfn_vkCmdEndRenderPass)
			return false;
		pfn_vkCmdExecuteCommands = reinterpret_cast<PFN_vkCmdExecuteCommands>(load_func("vkCmdExecuteCommands"));
		if (!pfn_vkCmdExecuteCommands)
			return false;
		return true;
	}

	static bool load_vulkan_1_1() {
		pfn_vkEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(load_func_with_instance(nullptr, "vkEnumerateInstanceVersion"));
		if (!pfn_vkEnumerateInstanceVersion)
			return false;
		pfn_vkBindBufferMemory2 = reinterpret_cast<PFN_vkBindBufferMemory2>(load_func("vkBindBufferMemory2"));
		if (!pfn_vkBindBufferMemory2)
			return false;
		pfn_vkBindImageMemory2 = reinterpret_cast<PFN_vkBindImageMemory2>(load_func("vkBindImageMemory2"));
		if (!pfn_vkBindImageMemory2)
			return false;
		pfn_vkGetDeviceGroupPeerMemoryFeatures = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeatures>(load_func("vkGetDeviceGroupPeerMemoryFeatures"));
		if (!pfn_vkGetDeviceGroupPeerMemoryFeatures)
			return false;
		pfn_vkCmdSetDeviceMask = reinterpret_cast<PFN_vkCmdSetDeviceMask>(load_func("vkCmdSetDeviceMask"));
		if (!pfn_vkCmdSetDeviceMask)
			return false;
		pfn_vkCmdDispatchBase = reinterpret_cast<PFN_vkCmdDispatchBase>(load_func("vkCmdDispatchBase"));
		if (!pfn_vkCmdDispatchBase)
			return false;
		pfn_vkEnumeratePhysicalDeviceGroups = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroups>(load_func("vkEnumeratePhysicalDeviceGroups"));
		if (!pfn_vkEnumeratePhysicalDeviceGroups)
			return false;
		pfn_vkGetImageMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageMemoryRequirements2>(load_func("vkGetImageMemoryRequirements2"));
		if (!pfn_vkGetImageMemoryRequirements2)
			return false;
		pfn_vkGetBufferMemoryRequirements2 = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2>(load_func("vkGetBufferMemoryRequirements2"));
		if (!pfn_vkGetBufferMemoryRequirements2)
			return false;
		pfn_vkGetImageSparseMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2>(load_func("vkGetImageSparseMemoryRequirements2"));
		if (!pfn_vkGetImageSparseMemoryRequirements2)
			return false;
		pfn_vkGetPhysicalDeviceFeatures2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(load_func("vkGetPhysicalDeviceFeatures2"));
		if (!pfn_vkGetPhysicalDeviceFeatures2)
			return false;
		pfn_vkGetPhysicalDeviceProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(load_func("vkGetPhysicalDeviceProperties2"));
		if (!pfn_vkGetPhysicalDeviceProperties2)
			return false;
		pfn_vkGetPhysicalDeviceFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2>(load_func("vkGetPhysicalDeviceFormatProperties2"));
		if (!pfn_vkGetPhysicalDeviceFormatProperties2)
			return false;
		pfn_vkGetPhysicalDeviceImageFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2>(load_func("vkGetPhysicalDeviceImageFormatProperties2"));
		if (!pfn_vkGetPhysicalDeviceImageFormatProperties2)
			return false;
		pfn_vkGetPhysicalDeviceQueueFamilyProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2>(load_func("vkGetPhysicalDeviceQueueFamilyProperties2"));
		if (!pfn_vkGetPhysicalDeviceQueueFamilyProperties2)
			return false;
		pfn_vkGetPhysicalDeviceMemoryProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2>(load_func("vkGetPhysicalDeviceMemoryProperties2"));
		if (!pfn_vkGetPhysicalDeviceMemoryProperties2)
			return false;
		pfn_vkGetPhysicalDeviceSparseImageFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2>(load_func("vkGetPhysicalDeviceSparseImageFormatProperties2"));
		if (!pfn_vkGetPhysicalDeviceSparseImageFormatProperties2)
			return false;
		pfn_vkTrimCommandPool = reinterpret_cast<PFN_vkTrimCommandPool>(load_func("vkTrimCommandPool"));
		if (!pfn_vkTrimCommandPool)
			return false;
		pfn_vkGetDeviceQueue2 = reinterpret_cast<PFN_vkGetDeviceQueue2>(load_func("vkGetDeviceQueue2"));
		if (!pfn_vkGetDeviceQueue2)
			return false;
		pfn_vkCreateSamplerYcbcrConversion = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversion>(load_func("vkCreateSamplerYcbcrConversion"));
		if (!pfn_vkCreateSamplerYcbcrConversion)
			return false;
		pfn_vkDestroySamplerYcbcrConversion = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversion>(load_func("vkDestroySamplerYcbcrConversion"));
		if (!pfn_vkDestroySamplerYcbcrConversion)
			return false;
		pfn_vkCreateDescriptorUpdateTemplate = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplate>(load_func("vkCreateDescriptorUpdateTemplate"));
		if (!pfn_vkCreateDescriptorUpdateTemplate)
			return false;
		pfn_vkDestroyDescriptorUpdateTemplate = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplate>(load_func("vkDestroyDescriptorUpdateTemplate"));
		if (!pfn_vkDestroyDescriptorUpdateTemplate)
			return false;
		pfn_vkUpdateDescriptorSetWithTemplate = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplate>(load_func("vkUpdateDescriptorSetWithTemplate"));
		if (!pfn_vkUpdateDescriptorSetWithTemplate)
			return false;
		pfn_vkGetPhysicalDeviceExternalBufferProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferProperties>(load_func("vkGetPhysicalDeviceExternalBufferProperties"));
		if (!pfn_vkGetPhysicalDeviceExternalBufferProperties)
			return false;
		pfn_vkGetPhysicalDeviceExternalFenceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFenceProperties>(load_func("vkGetPhysicalDeviceExternalFenceProperties"));
		if (!pfn_vkGetPhysicalDeviceExternalFenceProperties)
			return false;
		pfn_vkGetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphoreProperties>(load_func("vkGetPhysicalDeviceExternalSemaphoreProperties"));
		if (!pfn_vkGetPhysicalDeviceExternalSemaphoreProperties)
			return false;
		pfn_vkGetDescriptorSetLayoutSupport = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupport>(load_func("vkGetDescriptorSetLayoutSupport"));
		if (!pfn_vkGetDescriptorSetLayoutSupport)
			return false;
		return true;
	}

	static bool load_vulkan_1_2() {
		pfn_vkCmdDrawIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndirectCount>(load_func("vkCmdDrawIndirectCount"));
		if (!pfn_vkCmdDrawIndirectCount)
			return false;
		pfn_vkCmdDrawIndexedIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCount>(load_func("vkCmdDrawIndexedIndirectCount"));
		if (!pfn_vkCmdDrawIndexedIndirectCount)
			return false;
		pfn_vkCreateRenderPass2 = reinterpret_cast<PFN_vkCreateRenderPass2>(load_func("vkCreateRenderPass2"));
		if (!pfn_vkCreateRenderPass2)
			return false;
		pfn_vkCmdBeginRenderPass2 = reinterpret_cast<PFN_vkCmdBeginRenderPass2>(load_func("vkCmdBeginRenderPass2"));
		if (!pfn_vkCmdBeginRenderPass2)
			return false;
		pfn_vkCmdNextSubpass2 = reinterpret_cast<PFN_vkCmdNextSubpass2>(load_func("vkCmdNextSubpass2"));
		if (!pfn_vkCmdNextSubpass2)
			return false;
		pfn_vkCmdEndRenderPass2 = reinterpret_cast<PFN_vkCmdEndRenderPass2>(load_func("vkCmdEndRenderPass2"));
		if (!pfn_vkCmdEndRenderPass2)
			return false;
		pfn_vkResetQueryPool = reinterpret_cast<PFN_vkResetQueryPool>(load_func("vkResetQueryPool"));
		if (!pfn_vkResetQueryPool)
			return false;
		pfn_vkGetSemaphoreCounterValue = reinterpret_cast<PFN_vkGetSemaphoreCounterValue>(load_func("vkGetSemaphoreCounterValue"));
		if (!pfn_vkGetSemaphoreCounterValue)
			return false;
		pfn_vkWaitSemaphores = reinterpret_cast<PFN_vkWaitSemaphores>(load_func("vkWaitSemaphores"));
		if (!pfn_vkWaitSemaphores)
			return false;
		pfn_vkSignalSemaphore = reinterpret_cast<PFN_vkSignalSemaphore>(load_func("vkSignalSemaphore"));
		if (!pfn_vkSignalSemaphore)
			return false;
		pfn_vkGetBufferDeviceAddress = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(load_func("vkGetBufferDeviceAddress"));
		if (!pfn_vkGetBufferDeviceAddress)
			return false;
		pfn_vkGetBufferOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddress>(load_func("vkGetBufferOpaqueCaptureAddress"));
		if (!pfn_vkGetBufferOpaqueCaptureAddress)
			return false;
		pfn_vkGetDeviceMemoryOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddress>(load_func("vkGetDeviceMemoryOpaqueCaptureAddress"));
		if (!pfn_vkGetDeviceMemoryOpaqueCaptureAddress)
			return false;
		return true;
	}

	static bool load_vulkan_1_3() {
		pfn_vkGetPhysicalDeviceToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolProperties>(load_func("vkGetPhysicalDeviceToolProperties"));
		if (!pfn_vkGetPhysicalDeviceToolProperties)
			return false;
		pfn_vkCreatePrivateDataSlot = reinterpret_cast<PFN_vkCreatePrivateDataSlot>(load_func("vkCreatePrivateDataSlot"));
		if (!pfn_vkCreatePrivateDataSlot)
			return false;
		pfn_vkDestroyPrivateDataSlot = reinterpret_cast<PFN_vkDestroyPrivateDataSlot>(load_func("vkDestroyPrivateDataSlot"));
		if (!pfn_vkDestroyPrivateDataSlot)
			return false;
		pfn_vkSetPrivateData = reinterpret_cast<PFN_vkSetPrivateData>(load_func("vkSetPrivateData"));
		if (!pfn_vkSetPrivateData)
			return false;
		pfn_vkGetPrivateData = reinterpret_cast<PFN_vkGetPrivateData>(load_func("vkGetPrivateData"));
		if (!pfn_vkGetPrivateData)
			return false;
		pfn_vkCmdSetEvent2 = reinterpret_cast<PFN_vkCmdSetEvent2>(load_func("vkCmdSetEvent2"));
		if (!pfn_vkCmdSetEvent2)
			return false;
		pfn_vkCmdResetEvent2 = reinterpret_cast<PFN_vkCmdResetEvent2>(load_func("vkCmdResetEvent2"));
		if (!pfn_vkCmdResetEvent2)
			return false;
		pfn_vkCmdWaitEvents2 = reinterpret_cast<PFN_vkCmdWaitEvents2>(load_func("vkCmdWaitEvents2"));
		if (!pfn_vkCmdWaitEvents2)
			return false;
		pfn_vkCmdPipelineBarrier2 = reinterpret_cast<PFN_vkCmdPipelineBarrier2>(load_func("vkCmdPipelineBarrier2"));
		if (!pfn_vkCmdPipelineBarrier2)
			return false;
		pfn_vkCmdWriteTimestamp2 = reinterpret_cast<PFN_vkCmdWriteTimestamp2>(load_func("vkCmdWriteTimestamp2"));
		if (!pfn_vkCmdWriteTimestamp2)
			return false;
		pfn_vkQueueSubmit2 = reinterpret_cast<PFN_vkQueueSubmit2>(load_func("vkQueueSubmit2"));
		if (!pfn_vkQueueSubmit2)
			return false;
		pfn_vkCmdCopyBuffer2 = reinterpret_cast<PFN_vkCmdCopyBuffer2>(load_func("vkCmdCopyBuffer2"));
		if (!pfn_vkCmdCopyBuffer2)
			return false;
		pfn_vkCmdCopyImage2 = reinterpret_cast<PFN_vkCmdCopyImage2>(load_func("vkCmdCopyImage2"));
		if (!pfn_vkCmdCopyImage2)
			return false;
		pfn_vkCmdCopyBufferToImage2 = reinterpret_cast<PFN_vkCmdCopyBufferToImage2>(load_func("vkCmdCopyBufferToImage2"));
		if (!pfn_vkCmdCopyBufferToImage2)
			return false;
		pfn_vkCmdCopyImageToBuffer2 = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2>(load_func("vkCmdCopyImageToBuffer2"));
		if (!pfn_vkCmdCopyImageToBuffer2)
			return false;
		pfn_vkCmdBlitImage2 = reinterpret_cast<PFN_vkCmdBlitImage2>(load_func("vkCmdBlitImage2"));
		if (!pfn_vkCmdBlitImage2)
			return false;
		pfn_vkCmdResolveImage2 = reinterpret_cast<PFN_vkCmdResolveImage2>(load_func("vkCmdResolveImage2"));
		if (!pfn_vkCmdResolveImage2)
			return false;
		pfn_vkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(load_func("vkCmdBeginRendering"));
		if (!pfn_vkCmdBeginRendering)
			return false;
		pfn_vkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(load_func("vkCmdEndRendering"));
		if (!pfn_vkCmdEndRendering)
			return false;
		pfn_vkCmdSetCullMode = reinterpret_cast<PFN_vkCmdSetCullMode>(load_func("vkCmdSetCullMode"));
		if (!pfn_vkCmdSetCullMode)
			return false;
		pfn_vkCmdSetFrontFace = reinterpret_cast<PFN_vkCmdSetFrontFace>(load_func("vkCmdSetFrontFace"));
		if (!pfn_vkCmdSetFrontFace)
			return false;
		pfn_vkCmdSetPrimitiveTopology = reinterpret_cast<PFN_vkCmdSetPrimitiveTopology>(load_func("vkCmdSetPrimitiveTopology"));
		if (!pfn_vkCmdSetPrimitiveTopology)
			return false;
		pfn_vkCmdSetViewportWithCount = reinterpret_cast<PFN_vkCmdSetViewportWithCount>(load_func("vkCmdSetViewportWithCount"));
		if (!pfn_vkCmdSetViewportWithCount)
			return false;
		pfn_vkCmdSetScissorWithCount = reinterpret_cast<PFN_vkCmdSetScissorWithCount>(load_func("vkCmdSetScissorWithCount"));
		if (!pfn_vkCmdSetScissorWithCount)
			return false;
		pfn_vkCmdBindVertexBuffers2 = reinterpret_cast<PFN_vkCmdBindVertexBuffers2>(load_func("vkCmdBindVertexBuffers2"));
		if (!pfn_vkCmdBindVertexBuffers2)
			return false;
		pfn_vkCmdSetDepthTestEnable = reinterpret_cast<PFN_vkCmdSetDepthTestEnable>(load_func("vkCmdSetDepthTestEnable"));
		if (!pfn_vkCmdSetDepthTestEnable)
			return false;
		pfn_vkCmdSetDepthWriteEnable = reinterpret_cast<PFN_vkCmdSetDepthWriteEnable>(load_func("vkCmdSetDepthWriteEnable"));
		if (!pfn_vkCmdSetDepthWriteEnable)
			return false;
		pfn_vkCmdSetDepthCompareOp = reinterpret_cast<PFN_vkCmdSetDepthCompareOp>(load_func("vkCmdSetDepthCompareOp"));
		if (!pfn_vkCmdSetDepthCompareOp)
			return false;
		pfn_vkCmdSetDepthBoundsTestEnable = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnable>(load_func("vkCmdSetDepthBoundsTestEnable"));
		if (!pfn_vkCmdSetDepthBoundsTestEnable)
			return false;
		pfn_vkCmdSetStencilTestEnable = reinterpret_cast<PFN_vkCmdSetStencilTestEnable>(load_func("vkCmdSetStencilTestEnable"));
		if (!pfn_vkCmdSetStencilTestEnable)
			return false;
		pfn_vkCmdSetStencilOp = reinterpret_cast<PFN_vkCmdSetStencilOp>(load_func("vkCmdSetStencilOp"));
		if (!pfn_vkCmdSetStencilOp)
			return false;
		pfn_vkCmdSetRasterizerDiscardEnable = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnable>(load_func("vkCmdSetRasterizerDiscardEnable"));
		if (!pfn_vkCmdSetRasterizerDiscardEnable)
			return false;
		pfn_vkCmdSetDepthBiasEnable = reinterpret_cast<PFN_vkCmdSetDepthBiasEnable>(load_func("vkCmdSetDepthBiasEnable"));
		if (!pfn_vkCmdSetDepthBiasEnable)
			return false;
		pfn_vkCmdSetPrimitiveRestartEnable = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnable>(load_func("vkCmdSetPrimitiveRestartEnable"));
		if (!pfn_vkCmdSetPrimitiveRestartEnable)
			return false;
		pfn_vkGetDeviceBufferMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirements>(load_func("vkGetDeviceBufferMemoryRequirements"));
		if (!pfn_vkGetDeviceBufferMemoryRequirements)
			return false;
		pfn_vkGetDeviceImageMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirements>(load_func("vkGetDeviceImageMemoryRequirements"));
		if (!pfn_vkGetDeviceImageMemoryRequirements)
			return false;
		pfn_vkGetDeviceImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirements>(load_func("vkGetDeviceImageSparseMemoryRequirements"));
		if (!pfn_vkGetDeviceImageSparseMemoryRequirements)
			return false;
		return true;
	}

	/* static bool load_vulkan_1_4() {
		pfn_vkCmdSetLineStipple = reinterpret_cast<PFN_vkCmdSetLineStipple>(load_func("vkCmdSetLineStipple"));
		if (!pfn_vkCmdSetLineStipple)
			return false;
		pfn_vkMapMemory2 = reinterpret_cast<PFN_vkMapMemory2>(load_func("vkMapMemory2"));
		if (!pfn_vkMapMemory2)
			return false;
		pfn_vkUnmapMemory2 = reinterpret_cast<PFN_vkUnmapMemory2>(load_func("vkUnmapMemory2"));
		if (!pfn_vkUnmapMemory2)
			return false;
		pfn_vkCmdBindIndexBuffer2 = reinterpret_cast<PFN_vkCmdBindIndexBuffer2>(load_func("vkCmdBindIndexBuffer2"));
		if (!pfn_vkCmdBindIndexBuffer2)
			return false;
		pfn_vkGetRenderingAreaGranularity = reinterpret_cast<PFN_vkGetRenderingAreaGranularity>(load_func("vkCmdSetLineStipple"));
		if (!pfn_vkGetRenderingAreaGranularity)
			return false;
		pfn_vkGetDeviceImageSubresourceLayout = reinterpret_cast<PFN_vkGetDeviceImageSubresourceLayout>(load_func("vkGetDeviceImageSubresourceLayout"));
		if (!pfn_vkGetDeviceImageSubresourceLayout)
			return false;
		pfn_vkGetImageSubresourceLayout2 = reinterpret_cast<PFN_vkGetImageSubresourceLayout2>(load_func("vkGetImageSubresourceLayout2"));
		if (!pfn_vkGetImageSubresourceLayout2)
			return false;
		pfn_vkCmdPushDescriptorSet = reinterpret_cast<PFN_vkCmdPushDescriptorSet>(load_func("vkCmdPushDescriptorSet"));
		if (!pfn_vkCmdPushDescriptorSet)
			return false;
		pfn_vkCmdPushDescriptorSetWithTemplate = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplate>(load_func("vkCmdPushDescriptorSetWithTemplate"));
		if (!pfn_vkCmdPushDescriptorSetWithTemplate)
			return false;
		pfn_vkCmdSetRenderingAttachmentLocations = reinterpret_cast<PFN_vkCmdSetRenderingAttachmentLocations>(load_func("vkCmdSetRenderingAttachmentLocations"));
		if (!pfn_vkCmdSetRenderingAttachmentLocations)
			return false;
		pfn_vkCmdSetRenderingInputAttachmentIndices = reinterpret_cast<PFN_vkCmdSetRenderingInputAttachmentIndices>(load_func("vkCmdSetRenderingInputAttachmentIndices"));
		if (!pfn_vkCmdSetRenderingInputAttachmentIndices)
			return false;
		pfn_vkCmdBindDescriptorSets2 = reinterpret_cast<PFN_vkCmdBindDescriptorSets2>(load_func("vkCmdBindDescriptorSets2"));
		if (!pfn_vkCmdBindDescriptorSets2)
			return false;
		pfn_vkCmdPushConstants2 = reinterpret_cast<PFN_vkCmdPushConstants2>(load_func("vkCmdPushConstants2"));
		if (!pfn_vkCmdPushConstants2)
			return false;
		pfn_vkCmdPushDescriptorSet2 = reinterpret_cast<PFN_vkCmdPushDescriptorSet2>(load_func("vkCmdPushDescriptorSet2"));
		if (!pfn_vkCmdPushDescriptorSet2)
			return false;
		pfn_vkCmdPushDescriptorSetWithTemplate2 = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplate2>(load_func("vkCmdPushDescriptorSetWithTemplate2"));
		if (!pfn_vkCmdPushDescriptorSetWithTemplate2)
			return false;
		pfn_vkCopyMemoryToImage = reinterpret_cast<PFN_vkCopyMemoryToImage>(load_func("vkCopyMemoryToImage"));
		if (!pfn_vkCopyMemoryToImage)
			return false;
		pfn_vkCopyImageToMemory = reinterpret_cast<PFN_vkCopyImageToMemory>(load_func("vkCopyImageToMemory"));
		if (!pfn_vkCopyImageToMemory)
			return false;
		pfn_vkCopyImageToImage = reinterpret_cast<PFN_vkCopyImageToImage>(load_func("vkCopyImageToImage"));
		if (!pfn_vkCopyImageToImage)
			return false;
		pfn_vkTransitionImageLayout = reinterpret_cast<PFN_vkTransitionImageLayout>(load_func("vkTransitionImageLayout"));
		if (!pfn_vkTransitionImageLayout)
			return false;
		return true;
	} */

	static bool load_extension_funcs() {
		pfn_vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(load_func("vkSetDebugUtilsObjectNameEXT"));
		if (!pfn_vkSetDebugUtilsObjectNameEXT)
			return false;
		pfn_vkSetDebugUtilsObjectTagEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectTagEXT>(load_func("vkSetDebugUtilsObjectTagEXT"));
		if (!pfn_vkSetDebugUtilsObjectTagEXT)
			return false;
		pfn_vkQueueBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueBeginDebugUtilsLabelEXT>(load_func("vkQueueBeginDebugUtilsLabelEXT"));
		if (!pfn_vkQueueBeginDebugUtilsLabelEXT)
			return false;
		pfn_vkQueueEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueEndDebugUtilsLabelEXT>(load_func("vkQueueEndDebugUtilsLabelEXT"));
		if (!pfn_vkQueueEndDebugUtilsLabelEXT)
			return false;
		pfn_vkQueueInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueInsertDebugUtilsLabelEXT>(load_func("vkQueueInsertDebugUtilsLabelEXT"));
		if (!pfn_vkQueueInsertDebugUtilsLabelEXT)
			return false;
		pfn_vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(load_func("vkCmdBeginDebugUtilsLabelEXT"));
		if (!pfn_vkCmdBeginDebugUtilsLabelEXT)
			return false;
		pfn_vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(load_func("vkCmdEndDebugUtilsLabelEXT"));
		if (!pfn_vkCmdEndDebugUtilsLabelEXT)
			return false;
		pfn_vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(load_func("vkCmdInsertDebugUtilsLabelEXT"));
		if (!pfn_vkCmdInsertDebugUtilsLabelEXT)
			return false;
		pfn_vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(load_func("vkCreateDebugUtilsMessengerEXT"));
		if (!pfn_vkCreateDebugUtilsMessengerEXT)
			return false;
		pfn_vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(load_func("vkDestroyDebugUtilsMessengerEXT"));
		if (!pfn_vkDestroyDebugUtilsMessengerEXT)
			return false;
		pfn_vkSubmitDebugUtilsMessageEXT = reinterpret_cast<PFN_vkSubmitDebugUtilsMessageEXT>(load_func("vkSubmitDebugUtilsMessageEXT"));
		if (!pfn_vkSubmitDebugUtilsMessageEXT)
			return false;
		pfn_vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(load_func("vkDestroySurfaceKHR"));
		if (!pfn_vkDestroySurfaceKHR)
			return false;
		pfn_vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(load_func("vkGetPhysicalDeviceSurfaceSupportKHR"));
		if (!pfn_vkGetPhysicalDeviceSurfaceSupportKHR)
			return false;
		pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(load_func("vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
		if (!pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
			return false;
		pfn_vkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(load_func("vkGetPhysicalDeviceSurfaceFormatsKHR"));
		if (!pfn_vkGetPhysicalDeviceSurfaceFormatsKHR)
			return false;
		pfn_vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(load_func("vkGetPhysicalDeviceSurfacePresentModesKHR"));
		if (!pfn_vkGetPhysicalDeviceSurfacePresentModesKHR)
			return false;
		pfn_vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(load_func("vkCreateSwapchainKHR"));
		if (!pfn_vkCreateSwapchainKHR)
			return false;
		pfn_vkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(load_func("vkDestroySwapchainKHR"));
		if (!pfn_vkDestroySwapchainKHR)
			return false;
		pfn_vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(load_func("vkGetSwapchainImagesKHR"));
		if (!pfn_vkGetSwapchainImagesKHR)
			return false;
		pfn_vkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(load_func("vkAcquireNextImageKHR"));
		if (!pfn_vkAcquireNextImageKHR)
			return false;
		pfn_vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(load_func("vkQueuePresentKHR"));
		if (!pfn_vkQueuePresentKHR)
			return false;
		pfn_vkGetDeviceGroupPresentCapabilitiesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPresentCapabilitiesKHR>(load_func("vkGetDeviceGroupPresentCapabilitiesKHR"));
		if (!pfn_vkGetDeviceGroupPresentCapabilitiesKHR)
			return false;
		pfn_vkGetDeviceGroupSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModesKHR>(load_func("vkGetDeviceGroupSurfacePresentModesKHR"));
		if (!pfn_vkGetDeviceGroupSurfacePresentModesKHR)
			return false;
		pfn_vkGetPhysicalDevicePresentRectanglesKHR = reinterpret_cast<PFN_vkGetPhysicalDevicePresentRectanglesKHR>(load_func("vkGetPhysicalDevicePresentRectanglesKHR"));
		if (!pfn_vkGetPhysicalDevicePresentRectanglesKHR)
			return false;
		pfn_vkAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(load_func("vkAcquireNextImage2KHR"));
		if (!pfn_vkAcquireNextImage2KHR)
			return false;
#ifdef RE_OS_WINDOWS
		pfn_vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(load_func("vkCreateWin32SurfaceKHR"));
		if (!pfn_vkCreateWin32SurfaceKHR)
			return false;
		pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(load_func("vkGetPhysicalDeviceWin32PresentationSupportKHR"));
		if (!pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR)
			return false;
#elif defined RE_OS_LINUX
		if (std::strcmp(Window::pInstance->get_vulkan_required_surface_extension_name(), VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME) == 0) {
			pfn_vkCreateWaylandSurfaceKHR = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(load_func("vkCreateWaylandSurfaceKHR"));
			if (!pfn_vkCreateWaylandSurfaceKHR)
				return false;
			pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>(load_func("vkGetPhysicalDeviceWaylandPresentationSupportKHR"));
			if (!pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR)
				return false;
		} else if (std::strcmp(Window::pInstance->get_vulkan_required_surface_extension_name(), VK_KHR_XLIB_SURFACE_EXTENSION_NAME) == 0) {
			pfn_vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(load_func("vkCreateXlibSurfaceKHR"));
			if (!pfn_vkCreateXlibSurfaceKHR)
				return false;
			pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(load_func("vkGetPhysicalDeviceXlibPresentationSupportKHR"));
			if (!pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR)
				return false;
		} else
			return false;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		return true;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT vk_eSeverityFlagBits, VkDebugUtilsMessageTypeFlagsEXT vk_eMsgTypeBits, const VkDebugUtilsMessengerCallbackDataEXT* vk_pCallbackData, void* vk_pUserData) {
		TerminalColor eConsoleColor = RE_TERMINAL_COLOR_WHITE;
		switch (vk_eSeverityFlagBits) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_RED;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_YELLOW;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_BRIGHT_BLACK;
				break;
			default:
				return VK_FALSE;
		}
		if (!u32VulkanDebugFocusCount) {
			if (pcVulkanDebugFocusOnFile && pcVulkanDebugFocusOnFunc && u32VulkanDebugFocusOnLine)
				println_colored(append_to_string("Vulkan validation layers triggered in ", pcVulkanDebugFocusOnFile, ", in function \"", pcVulkanDebugFocusOnFunc, "\", at line ", u32VulkanDebugFocusOnLine, ":").c_str(), eConsoleColor, false, false);
			else
				println_colored("Vulkan validation layers triggered", eConsoleColor, false, false);
		}
		u32VulkanDebugFocusCount++;
		println_colored(append_to_string("\t[", pMessageIdName, "] ", vk_pCallbackData->pMessage).c_str(), eConsoleColor, false, false);
		return VK_FALSE;
	}

	static bool setup_validation_layers() {
		VkDebugUtilsMessengerCreateInfoEXT vk_debugCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		vk_debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		vk_debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		vk_debugCreateInfo.pfnUserCallback = debug_callback;
		vk_debugCreateInfo.pUserData = nullptr;
		VkResult vk_eSuccessResult;
		CATCH_SIGNAL(vk_eSuccessResult = pfn_vkCreateDebugUtilsMessengerEXT(vk_hInstance, &vk_debugCreateInfo, nullptr, &vk_hDebugMessenger));
		if (!CHECK_VK_RESULT(vk_eSuccessResult)) {
			RE_FATAL_ERROR("Failed creating Vulkan debug messenger for validation layers");
			return false;
		}
		return true;
	}

	bool link_vulkan() {
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(hLibVulkan = LoadLibraryW(L"vulkan-1.dll"));
		if (!hLibVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return false;
		}
		CATCH_SIGNAL(pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(hLibVulkan, "vkGetInstanceProcAddr")));
#elif defined RE_OS_LINUX
		CATCH_SIGNAL(hLibVulkan = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL));
		if (!hLibVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return false;
		}
		CATCH_SIGNAL(pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(hLibVulkan, "vkGetInstanceProcAddr")));
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		if (!pfn_vkGetInstanceProcAddr) {
			RE_FATAL_ERROR("Failed loading the Vulkan function \"vkGetInstanceProcAddr\" with the OS-specific API");
			return false;
		}
		return CATCH_SIGNAL_AND_RETURN(create_vulkan_instance() && load_vulkan_1_0() && load_vulkan_1_1() && load_vulkan_1_2() && load_vulkan_1_3() /* && load_vulkan_1_4() */ && load_extension_funcs() && setup_validation_layers(), bool);
	}
	
	void unlink_vulkan() {
		if (vk_hInstance) {
			if (vk_hDebugMessenger)
				pfn_vkDestroyDebugUtilsMessengerEXT(vk_hInstance, vk_hDebugMessenger, nullptr);
			if (!pfn_vkDestroyInstance) {
				RE_NOTE("Attempting to reload function for destroying Vulkan instances");
				pfn_vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(load_func_with_instance(VK_NULL_HANDLE, "vkDestroyInstance"));
				if (!pfn_vkDestroyInstance)
					RE_ERROR("Failed reloading the function for destroying the Vulkan instance");
				else
					CATCH_SIGNAL(pfn_vkDestroyInstance(vk_hInstance, nullptr));
			} else
				CATCH_SIGNAL(pfn_vkDestroyInstance(vk_hInstance, nullptr));
		}
		if (hLibVulkan) {
#ifdef RE_OS_WINDOWS
			FreeLibrary(hLibVulkan);
#elif defined RE_OS_LINUX
			dlclose(hLibVulkan);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
			hLibVulkan = nullptr;
		}

		// Avoiding dangling pointers (don't remove!)
		pfn_vkCreateInstance = nullptr;
		pfn_vkDestroyInstance = nullptr;
		pfn_vkEnumeratePhysicalDevices = nullptr;
		pfn_vkGetPhysicalDeviceFeatures = nullptr;
		pfn_vkGetPhysicalDeviceFormatProperties = nullptr;
		pfn_vkGetPhysicalDeviceImageFormatProperties = nullptr;
		pfn_vkGetPhysicalDeviceProperties = nullptr;
		pfn_vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
		pfn_vkGetPhysicalDeviceMemoryProperties = nullptr;
		pfn_vkGetInstanceProcAddr = nullptr;
		pfn_vkGetDeviceProcAddr = nullptr;
		pfn_vkCreateDevice = nullptr;
		pfn_vkDestroyDevice = nullptr;
		pfn_vkEnumerateInstanceExtensionProperties = nullptr;
		pfn_vkEnumerateDeviceExtensionProperties = nullptr;
		pfn_vkEnumerateInstanceLayerProperties = nullptr;
		pfn_vkEnumerateDeviceLayerProperties = nullptr;
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
		pfn_vkEnumerateInstanceVersion = nullptr;
		pfn_vkBindBufferMemory2 = nullptr;
		pfn_vkBindImageMemory2 = nullptr;
		pfn_vkGetDeviceGroupPeerMemoryFeatures = nullptr;
		pfn_vkCmdSetDeviceMask = nullptr;
		pfn_vkCmdDispatchBase = nullptr;
		pfn_vkEnumeratePhysicalDeviceGroups = nullptr;
		pfn_vkGetImageMemoryRequirements2 = nullptr;
		pfn_vkGetBufferMemoryRequirements2 = nullptr;
		pfn_vkGetImageSparseMemoryRequirements2 = nullptr;
		pfn_vkGetPhysicalDeviceFeatures2 = nullptr;
		pfn_vkGetPhysicalDeviceProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceFormatProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceImageFormatProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceMemoryProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceSparseImageFormatProperties2 = nullptr;
		pfn_vkTrimCommandPool = nullptr;
		pfn_vkGetDeviceQueue2 = nullptr;
		pfn_vkCreateSamplerYcbcrConversion = nullptr;
		pfn_vkDestroySamplerYcbcrConversion = nullptr;
		pfn_vkCreateDescriptorUpdateTemplate = nullptr;
		pfn_vkDestroyDescriptorUpdateTemplate = nullptr;
		pfn_vkUpdateDescriptorSetWithTemplate = nullptr;
		pfn_vkGetPhysicalDeviceExternalBufferProperties = nullptr;
		pfn_vkGetPhysicalDeviceExternalFenceProperties = nullptr;
		pfn_vkGetPhysicalDeviceExternalSemaphoreProperties = nullptr;
		pfn_vkGetDescriptorSetLayoutSupport = nullptr;
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
		pfn_vkGetPhysicalDeviceToolProperties = nullptr;
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
		pfn_vkSetDebugUtilsObjectNameEXT = nullptr;
		pfn_vkSetDebugUtilsObjectTagEXT = nullptr;
		pfn_vkQueueBeginDebugUtilsLabelEXT = nullptr;
		pfn_vkQueueEndDebugUtilsLabelEXT = nullptr;
		pfn_vkQueueInsertDebugUtilsLabelEXT = nullptr;
		pfn_vkCmdBeginDebugUtilsLabelEXT = nullptr;
		pfn_vkCmdEndDebugUtilsLabelEXT = nullptr;
		pfn_vkCmdInsertDebugUtilsLabelEXT = nullptr;
		pfn_vkCreateDebugUtilsMessengerEXT = nullptr;
		pfn_vkDestroyDebugUtilsMessengerEXT = nullptr;
		pfn_vkSubmitDebugUtilsMessageEXT = nullptr;
		pfn_vkDestroySurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
		pfn_vkCreateSwapchainKHR = nullptr;
		pfn_vkDestroySwapchainKHR = nullptr;
		pfn_vkGetSwapchainImagesKHR = nullptr;
		pfn_vkAcquireNextImageKHR = nullptr;
		pfn_vkQueuePresentKHR = nullptr;
		pfn_vkGetDeviceGroupPresentCapabilitiesKHR = nullptr;
		pfn_vkGetDeviceGroupSurfacePresentModesKHR = nullptr;
		pfn_vkGetPhysicalDevicePresentRectanglesKHR = nullptr;
		pfn_vkAcquireNextImage2KHR = nullptr;
#ifdef RE_OS_WINDOWS
		pfn_vkCreateWin32SurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR = nullptr;
#elif defined RE_OS_LINUX
		pfn_vkCreateWaylandSurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
		pfn_vkCreateXlibSurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	void focus_vulkan_debug_on(const char *const pcFile, const char *const pcFunc, const uint32_t u32Line) {
		pcVulkanDebugFocusOnFile = pcFile;
		pcVulkanDebugFocusOnFunc = pcFunc;
		u32VulkanDebugFocusOnLine = u32Line;
		u32VulkanDebugFocusCount = 0U;
	}

	void unfocus_vulkan_debug() {
		pcVulkanDebugFocusOnFile = nullptr;
		pcVulkanDebugFocusOnFunc = nullptr;
		u32VulkanDebugFocusOnLine = 0U;
		u32VulkanDebugFocusCount = 0U;
	}

	bool check_vulkan_result(VkResult vk_eResult, const char* pcFile, const char* pcFunc, uint32_t u32Line) {
		const char* pcErrName = "Unknown Vulkan result";
		const char* pcErrDetail = "Unknown Vulkan Result enumeration value";
		switch (vk_eResult) {
			case VK_SUCCESS:
				return true;

			// Success codes, but treated as errors
			case VK_NOT_READY:
				pcErrName = "VK_NOT_READY";
				pcErrDetail = "Not ready (a fence or query has not yet completed)";
				break;
			case VK_TIMEOUT:
				pcErrName = "VK_TIMEOUT";
				pcErrDetail = "Timeout (a wait operation has not completed in the specified time)";
				break;
			case VK_EVENT_SET:
				pcErrName = "VK_EVENT_SET";
				pcErrDetail = "Event signaled";
				break;
			case VK_EVENT_RESET:
				pcErrName = "VK_EVENT_RESET";
				pcErrDetail = "Event unsignaled";
				break;
			case VK_INCOMPLETE:
				pcErrName = "VK_INCOMPLETE";
				pcErrDetail = "Incomplete (a return array was too small for the result)";
				break;
			case VK_SUBOPTIMAL_KHR:
				pcErrName = "VK_SUBOPTIMAL_KHR";
				pcErrDetail = "A swapchain no longer matches the surface properties exactly";
				break;
			case VK_THREAD_IDLE_KHR:
				pcErrName = "VK_THREAD_IDLE_KHR";
				pcErrDetail = "A deferred operation is not complete, but there's currently no work for this thread";
				break;
			case VK_THREAD_DONE_KHR:
				pcErrName = "VK_THREAD_DONE_KHR";
				pcErrDetail = "A deferred operation is not complete, but there's no work remaining";
				break;
			case VK_OPERATION_DEFERRED_KHR:
				pcErrName = "VK_OPERATION_DEFERRED_KHR";
				pcErrDetail = "A deferred operation was requested and some of the work has been deferred";
				break;
			case VK_OPERATION_NOT_DEFERRED_KHR:
				pcErrName = "VK_OPERATION_NOT_DEFERRED_KHR";
				pcErrDetail = "A deferred operation was requested and no operations were deferred";
				break;
			case VK_PIPELINE_COMPILE_REQUIRED:
				pcErrName = "VK_PIPELINE_COMPILE_REQUIRED";
				pcErrDetail = "Pipeline compilation required, but not done by the application";
				break;
#ifdef RE_OS_WINDOWS
			case VK_PIPELINE_BINARY_MISSING_KHR:
				pcErrName = "VK_PIPELINE_BINARY_MISSING_KHR";
				pcErrDetail = "Attempted to create a pipeline binary by querying an internal cache, but the internal cache entry did not exist";
				break;
			case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
				pcErrName = "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
				pcErrDetail = "The binary shader code is incompatible with the device";
				break;
#endif

			// Errors
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				pcErrName = "VK_ERROR_OUT_OF_HOST_MEMORY";
				pcErrDetail = "Out of host memory";
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				pcErrName = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
				pcErrDetail = "Out of device memory";
				break;
			case VK_ERROR_INITIALIZATION_FAILED:
				pcErrName = "VK_ERROR_INITIALIZATION_FAILED";
				pcErrDetail = "Initialization failed";
				break;
			case VK_ERROR_DEVICE_LOST:
				pcErrName = "VK_ERROR_DEVICE_LOST";
				pcErrDetail = "Device lost";
				break;
			case VK_ERROR_MEMORY_MAP_FAILED:
				pcErrName = "VK_ERROR_MEMORY_MAP_FAILED";
				pcErrDetail = "Memory map failed";
				break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				pcErrName = "VK_ERROR_LAYER_NOT_PRESENT";
				pcErrDetail = "Layer not present";
				break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				pcErrName = "VK_ERROR_EXTENSION_NOT_PRESENT";
				pcErrDetail = "Extension not present";
				break;
			case VK_ERROR_FEATURE_NOT_PRESENT:
				pcErrName = "VK_ERROR_FEATURE_NOT_PRESENT";
				pcErrDetail = "Feature not present";
				break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				pcErrName = "VK_ERROR_INCOMPATIBLE_DRIVER";
				pcErrDetail = "Incompatible driver";
				break;
			case VK_ERROR_TOO_MANY_OBJECTS:
				pcErrName = "VK_ERROR_TOO_MANY_OBJECTS";
				pcErrDetail = "Too many objects";
				break;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				pcErrName = "VK_ERROR_FORMAT_NOT_SUPPORTED";
				pcErrDetail = "Format not supported";
				break;
			case VK_ERROR_FRAGMENTED_POOL:
				pcErrName = "VK_ERROR_FRAGMENTED_POOL";
				pcErrDetail = "Fragmented pool";
				break;
			case VK_ERROR_SURFACE_LOST_KHR:
				pcErrName = "VK_ERROR_SURFACE_LOST_KHR";
				pcErrDetail = "Surface has been lost and is no longer available";
				break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				pcErrName = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
				pcErrDetail = "The window is already in use by any API";
				break;
			case VK_ERROR_OUT_OF_DATE_KHR:
				pcErrName = "VK_ERROR_OUT_OF_DATE_KHR";
				pcErrDetail = "A surface has changed in a way that it's no longer compatible with the swapchain and further operations will fail";
				break;
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				pcErrName = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
				pcErrDetail = "The display used by a swapchain doesn't use the same presentable image layout or is incompatible";
				break;
			case VK_ERROR_INVALID_SHADER_NV:
				pcErrName = "VK_ERROR_INVALID_SHADER_NV";
				pcErrDetail = "One or more shaders failed to compile or link";
				break;
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				pcErrName = "VK_ERROR_OUT_OF_POOL_MEMORY";
				pcErrDetail = "Out of pool memory";
				break;
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:
				pcErrName = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
				pcErrDetail = "Invalid external handle";
				break;
			case VK_ERROR_FRAGMENTATION:
				pcErrName = "VK_ERROR_FRAGMENTATION";
				pcErrDetail = "Fragmentation";
				break;
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
				pcErrName = "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
				pcErrDetail = "The memory address is not available";
				break;
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
				pcErrName = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
				pcErrDetail = "Operation on swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT has failed, because the application didn't have exclusive fullscreen access (outside the application's control)";
				break;
			case VK_ERROR_VALIDATION_FAILED_EXT:
				pcErrName = "VK_ERROR_VALIDATION_FAILED_EXT";
				pcErrDetail = "Invalid usage detected";
				break;
			case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
				pcErrName = "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
				pcErrDetail = "Image creation failed for internal resources required for compression are exhausted";
				break;
			case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
				pcErrName = "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
				pcErrDetail = "Requested VkImageUsageFlags aren't supported";
				break;
			case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
				pcErrName = "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
				pcErrDetail = "Requested video picture layout not supported";
				break;
			case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
				pcErrName = "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
				pcErrDetail = "Video profile operation not supported";
				break;
			case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
				pcErrName = "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
				pcErrDetail = "Video profile format parameters not supported";
				break;
			case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
				pcErrName = "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
				pcErrDetail = "Video profile codec not supported";
				break;
			case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
				pcErrName = "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
				pcErrDetail = "Specified video STD header version is not supported";
				break;
#ifdef RE_OS_WINDOWS
			case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
				pcErrName = "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
				pcErrDetail = "Invalid video STD parameters";
				break;
#endif
			case VK_ERROR_NOT_PERMITTED_KHR:
				pcErrName = "VK_ERROR_NOT_PERMITTED_KHR";
				pcErrDetail = "Action is not permitted to be executed due to the application's missing privileges";
				break;
#ifdef RE_OS_WINDOWS
			case VK_ERROR_NOT_ENOUGH_SPACE_KHR:
				pcErrName = "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
				pcErrDetail = "Application didn't provide enough space to return the data";
				break;
#endif
			case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
				pcErrName = "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
				pcErrDetail = "";
				break;
			case VK_ERROR_UNKNOWN:
				pcErrName = "VK_ERROR_UNKNOWN";
				pcErrDetail = "Unknown error";
				break;
			case VK_RESULT_MAX_ENUM:
			default:
				break;
		}
		error(pcFile, pcFunc, u32Line, append_to_string("The recently called Vulkan function threw an error: ", pcErrName, "\n", pcErrDetail).c_str(), false);
		return false;
	}

}
