#include "RE_Vulkan_Device.hpp"
#include "RE_Renderer.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"

#include <queue>

namespace RE {

	// Attributes initialized at beginning and rarely changed
	VkPhysicalDevice *vk_phPhysicalDevicesAvailable = nullptr;
	uint32_t u32PhysicalDevicesAvailableCount = 0U;
	VkPhysicalDevice vk_hPhysicalDeviceSelected = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties = {};
	VkDevice vk_hDevice = VK_NULL_HANDLE;
	VkQueue vk_hDeviceQueueFamilies[RE_VK_QUEUE_COUNT] = {};
	uint32_t u32DeviceQueueFamilyIndices[RE_VK_QUEUE_COUNT] = {};
	VkSurfaceKHR vk_hSurface = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR vk_surfaceCapabilities = {};
	uint32_t u32SurfaceFormatsAvailableCount = 0U;
	VkSurfaceFormatKHR *vk_pSurfaceFormatsAvailable = nullptr;
	VkSurfaceFormatKHR vk_surfaceFormatSelected = {};
	VkPresentModeKHR vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR, vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
	VkSwapchainKHR vk_hSwapchain = VK_NULL_HANDLE;
	VkFormat vk_eSwapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D vk_swapchainResolution = {};
	uint32_t u32SwapchainImageCount = 0U;
	VkImage *vk_phSwapchainImages = nullptr;
	VkImageView *vk_phSwapchainImageViews = nullptr;
	VkCommandPool vk_hCommandPools[RE_VK_COMMAND_POOL_COUNT] = {};
	VkCommandBuffer vk_hDummyTransferCommandBuffer = VK_NULL_HANDLE;

	uint8_t u8RenderSystemFlags = 0b00000110U;
#define SWAPCHAIN_DIRTY_INDEX 0
#define VSYNC_SETTING_INDEX 1
#define FPS_BOUND_TO_VSYNC_INDEX 2

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

	static PFN_vkVoidFunction load_func_with_device(const char* pcFuncName) {
		PFN_vkVoidFunction pFunc;
		CATCH_SIGNAL(pFunc = vkGetDeviceProcAddr(vk_hDevice, pcFuncName));
		if (!pFunc)
			RE_FATAL_ERROR(append_to_string("Failed loading the Vulkan logical device-level function \"", pcFuncName, "\""));
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

	/* static bool load_vulkan_1_3_device() {
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
	} */

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
		/* pfn_vkCreatePrivateDataSlot = nullptr;
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
		pfn_vkGetDeviceImageSparseMemoryRequirements = nullptr; */

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
		// Surface extension
		pfn_vkDestroySurfaceKHR = nullptr;
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

	static void println_vkbool32(const char* pcName, VkBool32 vk_bState) {
		print("\t\t\t", pcName, ": ");
		if (vk_bState)
			println_colored("true", RE_TERMINAL_COLOR_GREEN, false, false);
		else
			println_colored("false", RE_TERMINAL_COLOR_RED, false, false);
	}

	static void select_best_physical_vulkan_device() {
		int32_t i32BestDeviceScore = std::numeric_limits<int32_t>::min();
		for (uint32_t i = 0U; i < u32PhysicalDevicesAvailableCount; i++) {
			VkPhysicalDeviceProperties vk_physicalDeviceProperties;
			CATCH_SIGNAL(vkGetPhysicalDeviceProperties(vk_phPhysicalDevicesAvailable[i], &vk_physicalDeviceProperties));
			int32_t i32CurrentDeviceScore = 0;
			switch (vk_physicalDeviceProperties.deviceType) {
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					i32CurrentDeviceScore += 1000;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					i32CurrentDeviceScore += 500;
					break;
				default:
					i32CurrentDeviceScore -= 1000;
					break;
			}
			i32CurrentDeviceScore += vk_physicalDeviceProperties.limits.maxImageDimension2D;
			if (i32BestDeviceScore < i32CurrentDeviceScore) {
				i32BestDeviceScore = i32CurrentDeviceScore;
				vk_hPhysicalDeviceSelected = vk_phPhysicalDevicesAvailable[i];
			}
		}
	}

	static void select_best_surface_format() {
		int32_t i32BestSurfaceFormatScore = std::numeric_limits<int32_t>::min();
		for (uint32_t i = 0U; i < u32SurfaceFormatsAvailableCount; i++) {
			int32_t i32CurrentSurfaceFormatScore = 0;
			switch (vk_pSurfaceFormatsAvailable[i].format) {
				case VK_FORMAT_R8G8B8A8_UNORM:
				case VK_FORMAT_B8G8R8A8_UNORM:
					i32CurrentSurfaceFormatScore += 500;
					break;
				case VK_FORMAT_R8G8B8A8_SRGB:
				case VK_FORMAT_B8G8R8A8_SRGB:
					i32CurrentSurfaceFormatScore += 1000;
					break;
				default:
					i32CurrentSurfaceFormatScore -= 2000;
					break;
			}
			i32CurrentSurfaceFormatScore += (vk_pSurfaceFormatsAvailable[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ? -1 : 1) * 5000;
			if (i32BestSurfaceFormatScore < i32CurrentSurfaceFormatScore) {
				i32BestSurfaceFormatScore = i32CurrentSurfaceFormatScore;
				vk_surfaceFormatSelected = vk_pSurfaceFormatsAvailable[i];
			}
		}
	}

	static bool create_surface() {
		if (vk_hSurface)
			return false;
		return CATCH_SIGNAL_AND_RETURN(Window::pInstance->create_vulkan_surface(vk_hSurface), bool);
	}
	
	static void destroy_surface() {
		if (!vk_hSurface)
			return;
		CATCH_SIGNAL(vkDestroySurfaceKHR(vk_hInstance, vk_hSurface, nullptr));
		DELETE_ARRAY_SAFELY(vk_pSurfaceFormatsAvailable);
		vk_hSurface = VK_NULL_HANDLE;
	}

	static void fetch_surface_infos() {
		if (!vk_hSurface)
			return;
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities));
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32SurfaceFormatsAvailableCount, nullptr));
		vk_pSurfaceFormatsAvailable = new VkSurfaceFormatKHR[u32SurfaceFormatsAvailableCount];
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32SurfaceFormatsAvailableCount, vk_pSurfaceFormatsAvailable));
		uint32_t u32PresentModesCount;
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32PresentModesCount, nullptr));
		VkPresentModeKHR *const vk_peAllPresentModes = new VkPresentModeKHR[u32PresentModesCount];
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32PresentModesCount, vk_peAllPresentModes));
		vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR;
		vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
		for (uint32_t u32PresentModeIndex = 0U; u32PresentModeIndex < u32PresentModesCount; u32PresentModeIndex++) {
			switch (vk_peAllPresentModes[u32PresentModeIndex]) {
				case VK_PRESENT_MODE_IMMEDIATE_KHR:
					vk_ePresentModeNoVsync = VK_PRESENT_MODE_IMMEDIATE_KHR;
					break;
				case VK_PRESENT_MODE_MAILBOX_KHR:
					vk_ePresentModeVsync = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				default:
					break;
			}
		}
		delete[] vk_peAllPresentModes;
	}

	static bool alloc_physical_device_list() {
		if (vk_phPhysicalDevicesAvailable)
			return false;
		DEFINE_SIGNAL_GUARD(sigGuardAllocPhysicalDeviceList);
		uint32_t u32TotalPhysicalDeviceCount;
		CATCH_SIGNAL(vkEnumeratePhysicalDevices(vk_hInstance, &u32TotalPhysicalDeviceCount, nullptr));
		if (!u32TotalPhysicalDeviceCount) {
			RE_FATAL_ERROR("There aren't any devices supporting Vulkan");
			return false;
		}
		PRINT_LN("Available GPUs:");
		VkPhysicalDevice *const vk_phTotalPhysicalDevice = new VkPhysicalDevice[u32TotalPhysicalDeviceCount];
		CATCH_SIGNAL(vkEnumeratePhysicalDevices(vk_hInstance, &u32TotalPhysicalDeviceCount, vk_phTotalPhysicalDevice));
		std::queue<VkPhysicalDevice> suitablePhysicalDevices;
		for (uint32_t u32PhysicalDeviceIndex = 0U; u32PhysicalDeviceIndex < u32TotalPhysicalDeviceCount; u32PhysicalDeviceIndex++) {
			const VkPhysicalDevice vk_hPhysicalDevice = vk_phTotalPhysicalDevice[u32PhysicalDeviceIndex];

			// Fetch general information about the GPU
			VkPhysicalDeviceProperties vk_physicalDeviceProperties;
			CATCH_SIGNAL(vkGetPhysicalDeviceProperties(vk_hPhysicalDevice, &vk_physicalDeviceProperties));

			// Fetch extensions-data about the GPU
			uint32_t u32PhysicalDeviceExtensionCount;
			CATCH_SIGNAL(vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32PhysicalDeviceExtensionCount, nullptr));
			VkExtensionProperties *const vk_pPhysicalDeviceExtensionProperties = new VkExtensionProperties[u32PhysicalDeviceExtensionCount];
			CATCH_SIGNAL(vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32PhysicalDeviceExtensionCount, vk_pPhysicalDeviceExtensionProperties));

			// Fetch queue-data about the GPU
			uint32_t u32PhysicalDeviceQueueFamilyCount;
			CATCH_SIGNAL(vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, nullptr));
			VkQueueFamilyProperties *const vk_pPhysicalDeviceQueueFamilyProperties = new VkQueueFamilyProperties[u32PhysicalDeviceQueueFamilyCount];
			CATCH_SIGNAL(vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, vk_pPhysicalDeviceQueueFamilyProperties));

			// Fetch surface-data about the GPU
			VkSurfaceCapabilitiesKHR vk_physicalDeviceSurfaceCapabilities;
			CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDevice, vk_hSurface, &vk_physicalDeviceSurfaceCapabilities));

			{ // Prints information about physical device to console
				print("\t", vk_physicalDeviceProperties.deviceName, " [Driver version: ", VK_VERSION_MAJOR(vk_physicalDeviceProperties.driverVersion), '.', VK_VERSION_MINOR(vk_physicalDeviceProperties.driverVersion), '.', VK_VERSION_PATCH(vk_physicalDeviceProperties.driverVersion), "; Device type: ");
				switch (vk_physicalDeviceProperties.deviceType) {
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						print("integrated GPU");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						print("discrete GPU");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
						print("virtual GPU");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_CPU:
						print("CPU");
						break;
					default:
						print("unknown");
						break;
				}
				println("] supporting Vulkan ", VK_VERSION_MAJOR(vk_physicalDeviceProperties.apiVersion), '.', VK_VERSION_MINOR(vk_physicalDeviceProperties.apiVersion), '.', VK_VERSION_PATCH(vk_physicalDeviceProperties.apiVersion));
				println("\t\tAvailable device extensions:");
				for (uint32_t u32PhysicalDeviceExtensionIndex = 0U; u32PhysicalDeviceExtensionIndex < u32PhysicalDeviceExtensionCount; u32PhysicalDeviceExtensionIndex++)
					println("\t\t\t", vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].extensionName, " (", VK_VERSION_MAJOR(vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].specVersion), '.', VK_VERSION_MINOR(vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].specVersion), '.', VK_VERSION_PATCH(vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].specVersion), ")");
				uint32_t u32PhysicalDeviceLayerCount;
				CATCH_SIGNAL(vkEnumerateDeviceLayerProperties(vk_hPhysicalDevice, &u32PhysicalDeviceLayerCount, nullptr));
				VkLayerProperties *vk_pPhysicalDeviceLayerProperties = new VkLayerProperties[u32PhysicalDeviceLayerCount];
				CATCH_SIGNAL(vkEnumerateDeviceLayerProperties(vk_hPhysicalDevice, &u32PhysicalDeviceLayerCount, vk_pPhysicalDeviceLayerProperties));
				println("\t\tVulkan layers present on the device (deprecated):");
				for (uint32_t u32PhysicalDeviceLayerIndex = 0U; u32PhysicalDeviceLayerIndex < u32PhysicalDeviceLayerCount; u32PhysicalDeviceLayerIndex++)
					println("\t\t\t", vk_pPhysicalDeviceLayerProperties[u32PhysicalDeviceLayerIndex].layerName, " (", VK_VERSION_MAJOR(vk_pPhysicalDeviceLayerProperties[u32PhysicalDeviceLayerIndex].implementationVersion), '.', VK_VERSION_MINOR(vk_pPhysicalDeviceLayerProperties[u32PhysicalDeviceLayerIndex].implementationVersion), '.', VK_VERSION_PATCH(vk_pPhysicalDeviceLayerProperties[u32PhysicalDeviceLayerIndex].implementationVersion), "): ", vk_pPhysicalDeviceLayerProperties[u32PhysicalDeviceLayerIndex].description);
				delete[] vk_pPhysicalDeviceLayerProperties;
				println("\t\tQueues available on the GPU:");
				for (uint32_t u32PhysicalDeviceQueueFamilyIndex = 0U; u32PhysicalDeviceQueueFamilyIndex < u32PhysicalDeviceQueueFamilyCount; u32PhysicalDeviceQueueFamilyIndex++) {
					print("\t\t\t", u32PhysicalDeviceQueueFamilyIndex, ": ");
					bool bMultiPurposeQueue = false;
#define PRINT_COMMA() bMultiPurposeQueue ? print(", ") : print(), bMultiPurposeQueue = true
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
						PRINT_COMMA();
						print("graphics");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT) {
						PRINT_COMMA();
						print("compute");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT) {
						PRINT_COMMA();
						print("transfer");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
						PRINT_COMMA();
						print("sparse binding");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) {
						PRINT_COMMA();
						print("video decoding (extension)");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV) {
						PRINT_COMMA();
						print("optical flow (NVIDIA extension)");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_PROTECTED_BIT)
						bMultiPurposeQueue ? print("; (protected queue)") : print("(protected queue)");
#undef PRINT_COMMA
					println();
				}
				VkPhysicalDeviceFeatures vk_physicalDeviceFeaturesAvailable;
				CATCH_SIGNAL(vkGetPhysicalDeviceFeatures(vk_hPhysicalDevice, &vk_physicalDeviceFeaturesAvailable));
				println("\t\tFeatures supported by the GPU device:");
				println_vkbool32("robustBufferAccess", vk_physicalDeviceFeaturesAvailable.robustBufferAccess);
				println_vkbool32("fullDrawIndexUint32", vk_physicalDeviceFeaturesAvailable.fullDrawIndexUint32);
				println_vkbool32("imageCubeArray", vk_physicalDeviceFeaturesAvailable.imageCubeArray);
				println_vkbool32("independentBlend", vk_physicalDeviceFeaturesAvailable.independentBlend);
				println_vkbool32("geometryShader", vk_physicalDeviceFeaturesAvailable.geometryShader);
				println_vkbool32("tessellationShader", vk_physicalDeviceFeaturesAvailable.tessellationShader);
				println_vkbool32("sampleRateShading", vk_physicalDeviceFeaturesAvailable.sampleRateShading);
				println_vkbool32("dualSrcBlend", vk_physicalDeviceFeaturesAvailable.dualSrcBlend);
				println_vkbool32("logicOp", vk_physicalDeviceFeaturesAvailable.logicOp);
				println_vkbool32("multiDrawIndirect", vk_physicalDeviceFeaturesAvailable.multiDrawIndirect);
				println_vkbool32("drawIndirectFirstInstance", vk_physicalDeviceFeaturesAvailable.drawIndirectFirstInstance);
				println_vkbool32("depthClamp", vk_physicalDeviceFeaturesAvailable.depthClamp);
				println_vkbool32("depthBiasClamp", vk_physicalDeviceFeaturesAvailable.depthBiasClamp);
				println_vkbool32("fillModeNonSolid", vk_physicalDeviceFeaturesAvailable.fillModeNonSolid);
				println_vkbool32("depthBounds", vk_physicalDeviceFeaturesAvailable.depthBounds);
				println_vkbool32("wideLines", vk_physicalDeviceFeaturesAvailable.wideLines);
				println_vkbool32("largePoints", vk_physicalDeviceFeaturesAvailable.largePoints);
				println_vkbool32("alphaToOne", vk_physicalDeviceFeaturesAvailable.alphaToOne);
				println_vkbool32("multiViewport", vk_physicalDeviceFeaturesAvailable.multiViewport);
				println_vkbool32("samplerAnisotropy", vk_physicalDeviceFeaturesAvailable.samplerAnisotropy);
				println_vkbool32("textureCompressionETC2", vk_physicalDeviceFeaturesAvailable.textureCompressionETC2);
				println_vkbool32("textureCompressionASTC_LDR", vk_physicalDeviceFeaturesAvailable.textureCompressionASTC_LDR);
				println_vkbool32("textureCompressionBC", vk_physicalDeviceFeaturesAvailable.textureCompressionBC);
				println_vkbool32("occlusionQueryPrecise", vk_physicalDeviceFeaturesAvailable.occlusionQueryPrecise);
				println_vkbool32("pipelineStatisticsQuery", vk_physicalDeviceFeaturesAvailable.pipelineStatisticsQuery);
				println_vkbool32("vertexPipelineStoresAndAtomics", vk_physicalDeviceFeaturesAvailable.vertexPipelineStoresAndAtomics);
				println_vkbool32("fragmentStoresAndAtomics", vk_physicalDeviceFeaturesAvailable.fragmentStoresAndAtomics);
				println_vkbool32("shaderTessellationAndGeometryPointSize", vk_physicalDeviceFeaturesAvailable.shaderTessellationAndGeometryPointSize);
				println_vkbool32("shaderImageGatherExtended", vk_physicalDeviceFeaturesAvailable.shaderImageGatherExtended);
				println_vkbool32("shaderStorageImageExtendedFormats", vk_physicalDeviceFeaturesAvailable.shaderStorageImageExtendedFormats);
				println_vkbool32("shaderStorageImageMultisample", vk_physicalDeviceFeaturesAvailable.shaderStorageImageMultisample);
				println_vkbool32("shaderStorageImageReadWithoutFormat", vk_physicalDeviceFeaturesAvailable.shaderStorageImageReadWithoutFormat);
				println_vkbool32("shaderStorageImageWriteWithoutFormat", vk_physicalDeviceFeaturesAvailable.shaderStorageImageWriteWithoutFormat);
				println_vkbool32("shaderUniformBufferArrayDynamicIndexing", vk_physicalDeviceFeaturesAvailable.shaderUniformBufferArrayDynamicIndexing);
				println_vkbool32("shaderSampledImageArrayDynamicIndexing", vk_physicalDeviceFeaturesAvailable.shaderSampledImageArrayDynamicIndexing);
				println_vkbool32("shaderStorageBufferArrayDynamicIndexing", vk_physicalDeviceFeaturesAvailable.shaderStorageBufferArrayDynamicIndexing);
				println_vkbool32("shaderStorageImageArrayDynamicIndexing", vk_physicalDeviceFeaturesAvailable.shaderStorageImageArrayDynamicIndexing);
				println_vkbool32("shaderClipDistance", vk_physicalDeviceFeaturesAvailable.shaderClipDistance);
				println_vkbool32("shaderCullDistance", vk_physicalDeviceFeaturesAvailable.shaderCullDistance);
				println_vkbool32("shaderFloat64", vk_physicalDeviceFeaturesAvailable.shaderFloat64);
				println_vkbool32("shaderInt64", vk_physicalDeviceFeaturesAvailable.shaderInt64);
				println_vkbool32("shaderInt16", vk_physicalDeviceFeaturesAvailable.shaderInt16);
				println_vkbool32("shaderResourceResidency", vk_physicalDeviceFeaturesAvailable.shaderResourceResidency);
				println_vkbool32("shaderResourceMinLod", vk_physicalDeviceFeaturesAvailable.shaderResourceMinLod);
				println_vkbool32("sparseBinding", vk_physicalDeviceFeaturesAvailable.sparseBinding);
				println_vkbool32("sparseResidencyBuffer", vk_physicalDeviceFeaturesAvailable.sparseResidencyBuffer);
				println_vkbool32("sparseResidencyImage2D", vk_physicalDeviceFeaturesAvailable.sparseResidencyImage2D);
				println_vkbool32("sparseResidencyImage3D", vk_physicalDeviceFeaturesAvailable.sparseResidencyImage3D);
				println_vkbool32("sparseResidency2Samples", vk_physicalDeviceFeaturesAvailable.sparseResidency2Samples);
				println_vkbool32("sparseResidency4Samples", vk_physicalDeviceFeaturesAvailable.sparseResidency4Samples);
				println_vkbool32("sparseResidency8Samples", vk_physicalDeviceFeaturesAvailable.sparseResidency8Samples);
				println_vkbool32("sparseResidency16Samples", vk_physicalDeviceFeaturesAvailable.sparseResidency16Samples);
				println_vkbool32("sparseResidencyAliased", vk_physicalDeviceFeaturesAvailable.sparseResidencyAliased);
				println_vkbool32("variableMultisampleRate", vk_physicalDeviceFeaturesAvailable.variableMultisampleRate);
				println_vkbool32("inheritedQueries", vk_physicalDeviceFeaturesAvailable.inheritedQueries);
				println("\t\tGPU limits:");
				println("\t\t\tmaxImageDimension1D: ", vk_physicalDeviceProperties.limits.maxImageDimension1D);
				println("\t\t\tmaxImageDimension2D: ", vk_physicalDeviceProperties.limits.maxImageDimension2D);
				println("\t\t\tmaxImageDimension3D: ", vk_physicalDeviceProperties.limits.maxImageDimension3D);
				println("\t\t\tmaxImageDimensionCube: ", vk_physicalDeviceProperties.limits.maxImageDimensionCube);
				println("\t\t\tmaxImageArrayLayers: ", vk_physicalDeviceProperties.limits.maxImageArrayLayers);
				println("\t\t\tmaxTexelBufferElements: ", vk_physicalDeviceProperties.limits.maxTexelBufferElements);
				println("\t\t\tmaxUniformBufferRange: ", vk_physicalDeviceProperties.limits.maxUniformBufferRange);
				println("\t\t\tmaxStorageBufferRange: ", vk_physicalDeviceProperties.limits.maxStorageBufferRange);
				println("\t\t\tmaxPushConstantsSize: ", vk_physicalDeviceProperties.limits.maxPushConstantsSize);
				println("\t\t\tmaxMemoryAllocationCount: ", vk_physicalDeviceProperties.limits.maxMemoryAllocationCount);
				println("\t\t\tmaxSamplerAllocationCount: ", vk_physicalDeviceProperties.limits.maxSamplerAllocationCount);
				println("\t\t\tbufferImageGranularity: ", vk_physicalDeviceProperties.limits.bufferImageGranularity);
				println("\t\t\tsparseAddressSpaceSize: ", vk_physicalDeviceProperties.limits.sparseAddressSpaceSize);
				println("\t\t\tmaxBoundDescriptorSets: ", vk_physicalDeviceProperties.limits.maxBoundDescriptorSets);
				println("\t\t\tmaxPerStageDescriptorSamplers: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorSamplers);
				println("\t\t\tmaxPerStageDescriptorUniformBuffers: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorUniformBuffers);
				println("\t\t\tmaxPerStageDescriptorStorageBuffers: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorStorageBuffers);
				println("\t\t\tmaxPerStageDescriptorSampledImages: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorSampledImages);
				println("\t\t\tmaxPerStageDescriptorStorageImages: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorStorageImages);
				println("\t\t\tmaxPerStageDescriptorInputAttachments: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorInputAttachments);
				println("\t\t\tmaxPerStageResources: ", vk_physicalDeviceProperties.limits.maxPerStageResources);
				println("\t\t\tmaxDescriptorSetSamplers: ", vk_physicalDeviceProperties.limits.maxDescriptorSetSamplers);
				println("\t\t\tmaxDescriptorSetUniformBuffers: ", vk_physicalDeviceProperties.limits.maxDescriptorSetUniformBuffers);
				println("\t\t\tmaxDescriptorSetUniformBuffersDynamic: ", vk_physicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic);
				println("\t\t\tmaxDescriptorSetStorageBuffers: ", vk_physicalDeviceProperties.limits.maxDescriptorSetStorageBuffers);
				println("\t\t\tmaxDescriptorSetStorageBuffersDynamic: ", vk_physicalDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic);
				println("\t\t\tmaxDescriptorSetSampledImages: ", vk_physicalDeviceProperties.limits.maxDescriptorSetSampledImages);
				println("\t\t\tmaxDescriptorSetStorageImages: ", vk_physicalDeviceProperties.limits.maxDescriptorSetStorageImages);
				println("\t\t\tmaxDescriptorSetInputAttachments: ", vk_physicalDeviceProperties.limits.maxDescriptorSetInputAttachments);
				println("\t\t\tmaxVertexInputAttributes: ", vk_physicalDeviceProperties.limits.maxVertexInputAttributes);
				println("\t\t\tmaxVertexInputBindings: ", vk_physicalDeviceProperties.limits.maxVertexInputBindings);
				println("\t\t\tmaxVertexInputAttributeOffset: ", vk_physicalDeviceProperties.limits.maxVertexInputAttributeOffset);
				println("\t\t\tmaxVertexInputBindingStride: ", vk_physicalDeviceProperties.limits.maxVertexInputBindingStride);
				println("\t\t\tmaxVertexOutputComponents: ", vk_physicalDeviceProperties.limits.maxVertexOutputComponents);
				println("\t\t\tmaxTessellationGenerationLevel: ", vk_physicalDeviceProperties.limits.maxTessellationGenerationLevel);
				println("\t\t\tmaxTessellationPatchSize: ", vk_physicalDeviceProperties.limits.maxTessellationPatchSize);
				println("\t\t\tmaxTessellationControlPerVertexInputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationControlPerVertexInputComponents);
				println("\t\t\tmaxTessellationControlPerVertexOutputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationControlPerVertexOutputComponents);
				println("\t\t\tmaxTessellationControlPerPatchOutputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationControlPerPatchOutputComponents);
				println("\t\t\tmaxTessellationControlTotalOutputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationControlTotalOutputComponents);
				println("\t\t\tmaxTessellationEvaluationInputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationEvaluationInputComponents);
				println("\t\t\tmaxTessellationEvaluationOutputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationEvaluationOutputComponents);
				println("\t\t\tmaxGeometryShaderInvocations: ", vk_physicalDeviceProperties.limits.maxGeometryShaderInvocations);
				println("\t\t\tmaxGeometryInputComponents: ", vk_physicalDeviceProperties.limits.maxGeometryInputComponents);
				println("\t\t\tmaxGeometryOutputComponents: ", vk_physicalDeviceProperties.limits.maxGeometryOutputComponents);
				println("\t\t\tmaxGeometryOutputVertices: ", vk_physicalDeviceProperties.limits.maxGeometryOutputVertices);
				println("\t\t\tmaxGeometryTotalOutputComponents: ", vk_physicalDeviceProperties.limits.maxGeometryTotalOutputComponents);
				println("\t\t\tmaxFragmentInputComponents: ", vk_physicalDeviceProperties.limits.maxFragmentInputComponents);
				println("\t\t\tmaxFragmentOutputAttachments: ", vk_physicalDeviceProperties.limits.maxFragmentOutputAttachments);
				println("\t\t\tmaxFragmentDualSrcAttachments: ", vk_physicalDeviceProperties.limits.maxFragmentDualSrcAttachments);
				println("\t\t\tmaxFragmentCombinedOutputResources: ", vk_physicalDeviceProperties.limits.maxFragmentCombinedOutputResources);
				println("\t\t\tmaxComputeSharedMemorySize: ", vk_physicalDeviceProperties.limits.maxComputeSharedMemorySize);
				println("\t\t\tmaxComputeWorkGroupCount: ", array_to_string(vk_physicalDeviceProperties.limits.maxComputeWorkGroupCount, sizeof(vk_physicalDeviceProperties.limits.maxComputeWorkGroupCount) / sizeof(vk_physicalDeviceProperties.limits.maxComputeWorkGroupCount[0])));
				println("\t\t\tmaxComputeWorkGroupInvocations: ", vk_physicalDeviceProperties.limits.maxComputeWorkGroupInvocations);
				println("\t\t\tmaxComputeWorkGroupSize: ", array_to_string(vk_physicalDeviceProperties.limits.maxComputeWorkGroupSize, sizeof(vk_physicalDeviceProperties.limits.maxComputeWorkGroupSize) / sizeof(vk_physicalDeviceProperties.limits.maxComputeWorkGroupSize[0])));
				println("\t\t\tsubPixelPrecisionBits: ", vk_physicalDeviceProperties.limits.subPixelPrecisionBits);
				println("\t\t\tsubTexelPrecisionBits: ", vk_physicalDeviceProperties.limits.subTexelPrecisionBits);
				println("\t\t\tmipmapPrecisionBits: ", vk_physicalDeviceProperties.limits.mipmapPrecisionBits);
				println("\t\t\tmaxDrawIndexedIndexValue: ", vk_physicalDeviceProperties.limits.maxDrawIndexedIndexValue);
				println("\t\t\tmaxDrawIndirectCount: ", vk_physicalDeviceProperties.limits.maxDrawIndirectCount);
				println("\t\t\tmaxSamplerLodBias: ", vk_physicalDeviceProperties.limits.maxSamplerLodBias);
				println("\t\t\tmaxSamplerAnisotropy: ", vk_physicalDeviceProperties.limits.maxSamplerAnisotropy);
				println("\t\t\tmaxViewports: ", vk_physicalDeviceProperties.limits.maxViewports);
				println("\t\t\tmaxViewportDimensions: ", array_to_string(vk_physicalDeviceProperties.limits.maxViewportDimensions, sizeof(vk_physicalDeviceProperties.limits.maxViewportDimensions) / sizeof(vk_physicalDeviceProperties.limits.maxViewportDimensions[0])));
				println("\t\t\tviewportBoundsRange: ", array_to_string(vk_physicalDeviceProperties.limits.viewportBoundsRange, sizeof(vk_physicalDeviceProperties.limits.viewportBoundsRange) / sizeof(vk_physicalDeviceProperties.limits.viewportBoundsRange[0])));
				println("\t\t\tviewportSubPixelBits: ", vk_physicalDeviceProperties.limits.viewportSubPixelBits);
				println("\t\t\tminMemoryMapAlignment: ", vk_physicalDeviceProperties.limits.minMemoryMapAlignment);
				println("\t\t\tminTexelBufferOffsetAlignment: ", vk_physicalDeviceProperties.limits.minTexelBufferOffsetAlignment);
				println("\t\t\tminUniformBufferOffsetAlignment: ", vk_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
				println("\t\t\tminStorageBufferOffsetAlignment: ", vk_physicalDeviceProperties.limits.minStorageBufferOffsetAlignment);
				println("\t\t\tminTexelOffset: ", vk_physicalDeviceProperties.limits.minTexelOffset);
				println("\t\t\tmaxTexelOffset: ", vk_physicalDeviceProperties.limits.maxTexelOffset);
				println("\t\t\tminTexelGatherOffset: ", vk_physicalDeviceProperties.limits.minTexelGatherOffset);
				println("\t\t\tmaxTexelGatherOffset: ", vk_physicalDeviceProperties.limits.maxTexelGatherOffset);
				println("\t\t\tminInterpolationOffset: ", vk_physicalDeviceProperties.limits.minInterpolationOffset);
				println("\t\t\tmaxInterpolationOffset: ", vk_physicalDeviceProperties.limits.maxInterpolationOffset);
				println("\t\t\tsubPixelInterpolationOffsetBits: ", vk_physicalDeviceProperties.limits.subPixelInterpolationOffsetBits);
				println("\t\t\tmaxFramebufferWidth: ", vk_physicalDeviceProperties.limits.maxFramebufferWidth);
				println("\t\t\tmaxFramebufferHeight: ", vk_physicalDeviceProperties.limits.maxFramebufferHeight);
				println("\t\t\tmaxFramebufferLayers: ", vk_physicalDeviceProperties.limits.maxFramebufferLayers);
				println("\t\t\tframebufferColorSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.framebufferColorSampleCounts));
				println("\t\t\tframebufferDepthSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.framebufferDepthSampleCounts));
				println("\t\t\tframebufferStencilSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.framebufferStencilSampleCounts));
				println("\t\t\tframebufferNoAttachmentsSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.framebufferNoAttachmentsSampleCounts));
				println("\t\t\tmaxColorAttachments: ", vk_physicalDeviceProperties.limits.maxColorAttachments);
				println("\t\t\tsampledImageColorSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.sampledImageColorSampleCounts));
				println("\t\t\tsampledImageIntegerSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.sampledImageIntegerSampleCounts));
				println("\t\t\tsampledImageDepthSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.sampledImageDepthSampleCounts));
				println("\t\t\tsampledImageStencilSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.sampledImageStencilSampleCounts));
				println("\t\t\tstorageImageSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.storageImageSampleCounts));
				println("\t\t\tmaxSampleMaskWords: ", vk_physicalDeviceProperties.limits.maxSampleMaskWords);
				println_vkbool32("timestampComputeAndGraphics: ", vk_physicalDeviceProperties.limits.timestampComputeAndGraphics);
				println("\t\t\ttimestampPeriod: ", vk_physicalDeviceProperties.limits.timestampPeriod);
				println("\t\t\tmaxClipDistances: ", vk_physicalDeviceProperties.limits.maxClipDistances);
				println("\t\t\tmaxCullDistances: ", vk_physicalDeviceProperties.limits.maxCullDistances);
				println("\t\t\tmaxCombinedClipAndCullDistances: ", vk_physicalDeviceProperties.limits.maxCombinedClipAndCullDistances);
				println("\t\t\tdiscreteQueuePriorities: ", vk_physicalDeviceProperties.limits.discreteQueuePriorities);
				println("\t\t\tpointSizeRange: ", array_to_string(vk_physicalDeviceProperties.limits.pointSizeRange, sizeof(vk_physicalDeviceProperties.limits.pointSizeRange) / sizeof(vk_physicalDeviceProperties.limits.pointSizeRange[0])));
				println("\t\t\tlineWidthRange: ", array_to_string(vk_physicalDeviceProperties.limits.lineWidthRange, sizeof(vk_physicalDeviceProperties.limits.lineWidthRange) / sizeof(vk_physicalDeviceProperties.limits.lineWidthRange[0])));
				println("\t\t\tpointSizeGranularity: ", vk_physicalDeviceProperties.limits.pointSizeGranularity);
				println("\t\t\tlineWidthGranularity: ", vk_physicalDeviceProperties.limits.lineWidthGranularity);
				println_vkbool32("strictLines: ", vk_physicalDeviceProperties.limits.strictLines);
				println_vkbool32("standardSampleLocations: ", vk_physicalDeviceProperties.limits.standardSampleLocations);
				println("\t\t\toptimalBufferCopyOffsetAlignment: ", vk_physicalDeviceProperties.limits.optimalBufferCopyOffsetAlignment);
				println("\t\t\toptimalBufferCopyRowPitchAlignment: ", vk_physicalDeviceProperties.limits.optimalBufferCopyRowPitchAlignment);
				println("\t\t\tnonCoherentAtomSize: ", vk_physicalDeviceProperties.limits.nonCoherentAtomSize);
				println("\t\tSupported surface capabilities:");
				println("\t\t\tsupportedTransforms: ", bitmask_to_string(vk_physicalDeviceSurfaceCapabilities.supportedTransforms));
				println("\t\t\tsupportedCompositeAlpha: ", bitmask_to_string(vk_physicalDeviceSurfaceCapabilities.supportedCompositeAlpha));
				println("\t\t\tsupportedUsageFlags: ", bitmask_to_string(vk_physicalDeviceSurfaceCapabilities.supportedUsageFlags));
			} // End of printing information about physical device to console


			std::queue<const char*> missingFeatures;

			// Check if there are surface formats defined
			uint32_t u32PhysicalDeviceSurfaceFormatCount;
			CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDevice, vk_hSurface, &u32PhysicalDeviceSurfaceFormatCount, nullptr));
			if (!u32PhysicalDeviceSurfaceFormatCount)
				missingFeatures.push("No surface formats available");

			// Check if there are present modes defined
			uint32_t u32PhysicalDevicePresentModeCount;
			CATCH_SIGNAL(vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDevice, vk_hSurface, &u32PhysicalDevicePresentModeCount, nullptr));
			if (!u32PhysicalDevicePresentModeCount)
				missingFeatures.push("No presentation modes available");

			// Check if swapchain images support the required image usages
			if ((vk_physicalDeviceSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == 0 || (vk_physicalDeviceSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0)
				missingFeatures.push("Swapchain images neither support being used for color attachments and/or transfer operations");

			// Check if the required extensions exist
			bool bSwapchainExtists = false;
			for (uint32_t u32PhysicalDeviceExtensionIndex = 0U; u32PhysicalDeviceExtensionIndex < u32PhysicalDeviceExtensionCount; u32PhysicalDeviceExtensionIndex++) {
				if (!bSwapchainExtists && are_string_contents_equal(vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
					bSwapchainExtists = true;
				if (bSwapchainExtists)
					break;
			}
			if (!bSwapchainExtists)
				missingFeatures.push("The swapchain extension doesn't exist on this GPU");

			// Check if required queues exist
			bool bGraphicsQueueExists = false, bPresentQueueExists = false, bTransferQueueExists = false;
			for (uint32_t u32PhysicalDeviceQueueFamilyIndex = 0U; u32PhysicalDeviceQueueFamilyIndex < u32PhysicalDeviceQueueFamilyCount; u32PhysicalDeviceQueueFamilyIndex++) {
				if (!bPresentQueueExists) {
					VkBool32 surfaceSupportExists;
					CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDevice, u32PhysicalDeviceQueueFamilyIndex, vk_hSurface, &surfaceSupportExists));
					if (surfaceSupportExists)
						bPresentQueueExists = true;
				}
				if (!bGraphicsQueueExists && (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT))
					bGraphicsQueueExists = true;
				if (!bTransferQueueExists && (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT))
					bTransferQueueExists = true;
				if (bGraphicsQueueExists && bPresentQueueExists && bTransferQueueExists)
					break;
			}
			if (!bGraphicsQueueExists || !bPresentQueueExists || !bTransferQueueExists)
				missingFeatures.push("The graphics, presentation or transfer queue doesn't exist");

			delete[] vk_pPhysicalDeviceExtensionProperties;
			delete[] vk_pPhysicalDeviceQueueFamilyProperties;
			if (!missingFeatures.empty()) {
				println_colored(append_to_string("\tPhysical Vulkan device ", vk_physicalDeviceProperties.deviceName, " has been discarded for not fulfilling the requirement(s):").c_str(), RE_TERMINAL_COLOR_RED, false, true);
				do {
					println_colored(append_to_string("\t - ", missingFeatures.front()).c_str(), RE_TERMINAL_COLOR_RED, false, true);
					missingFeatures.pop();
				} while (!missingFeatures.empty());
				continue;
			} else
				suitablePhysicalDevices.push(vk_hPhysicalDevice);
		}
		delete[] vk_phTotalPhysicalDevice;
		u32PhysicalDevicesAvailableCount = suitablePhysicalDevices.size();
		if (!u32PhysicalDevicesAvailableCount) {
			RE_FATAL_ERROR("There aren't any physical Vulkan devices supporting the necessary features");
			return false;
		}
		vk_phPhysicalDevicesAvailable = new VkPhysicalDevice[u32PhysicalDevicesAvailableCount];
		uint32_t u32CurrentIndex = 0U;
		do {
			vk_phPhysicalDevicesAvailable[u32CurrentIndex] = suitablePhysicalDevices.front();
			u32CurrentIndex++;
			suitablePhysicalDevices.pop();
		} while (!suitablePhysicalDevices.empty());
		return true;
	}

	static void free_physical_device_list() {
		DELETE_ARRAY_SAFELY(vk_phPhysicalDevicesAvailable);
	}

	static bool create_device() {
		if (vk_hDevice)
			return false;
		DEFINE_SIGNAL_GUARD(sigGuardCreateDevice);
		constexpr uint32_t u32PhysicalDeviceExtensionCount = 1U;
		const char *const pcPhysicalDeviceExtensions[u32PhysicalDeviceExtensionCount] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		uint32_t u32PhysicalDeviceQueueFamilyCount;
		CATCH_SIGNAL(vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDeviceSelected, &u32PhysicalDeviceQueueFamilyCount, nullptr));
		VkQueueFamilyProperties *vk_pPhysicalDeviceQueueFamilyProperties = new VkQueueFamilyProperties[u32PhysicalDeviceQueueFamilyCount];
		CATCH_SIGNAL(vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDeviceSelected, &u32PhysicalDeviceQueueFamilyCount, vk_pPhysicalDeviceQueueFamilyProperties));
		std::optional<uint32_t> graphicsQueueIndex, presentQueueIndex, transferQueueIndex;
		std::vector<uint32_t> uniqueQueueIndices;
		for (uint32_t u32PhysicalDeviceQueueFamilyIndex = 0U; u32PhysicalDeviceQueueFamilyIndex < u32PhysicalDeviceQueueFamilyCount; u32PhysicalDeviceQueueFamilyIndex++) {
			bool bQueueUseful = false;
			// Get graphics-queue index
			if (!graphicsQueueIndex.has_value() && (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				graphicsQueueIndex = u32PhysicalDeviceQueueFamilyIndex;
				// Check if graphics queue supports presenting too for better performance
				VkBool32 surfaceSupportExists;
				CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDeviceSelected, u32PhysicalDeviceQueueFamilyIndex, vk_hSurface, &surfaceSupportExists));
				if (surfaceSupportExists)
					presentQueueIndex = u32PhysicalDeviceQueueFamilyIndex;
				bQueueUseful = true;
			}
			// Get presenting-queue index
			if (!presentQueueIndex.has_value()) {
				VkBool32 surfaceSupportExists;
				CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDeviceSelected, u32PhysicalDeviceQueueFamilyIndex, vk_hSurface, &surfaceSupportExists));
				if (surfaceSupportExists)
					presentQueueIndex = u32PhysicalDeviceQueueFamilyIndex;
				bQueueUseful = true;
			}
			// Get transfer-queue index, but avoid being a graphics queue too
			if (!transferQueueIndex.has_value() && (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT) && !((vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)) {
				transferQueueIndex = u32PhysicalDeviceQueueFamilyIndex;
				bQueueUseful = true;
			}
			// Check if current queue index is unique and add it
			if (bQueueUseful && std::find(uniqueQueueIndices.begin(), uniqueQueueIndices.end(), u32PhysicalDeviceQueueFamilyIndex) == uniqueQueueIndices.end())
				uniqueQueueIndices.push_back(u32PhysicalDeviceQueueFamilyIndex);
			if (graphicsQueueIndex.has_value() && presentQueueIndex.has_value() && transferQueueIndex.has_value())
				break;
		}
		// If no transfer-queue found, use graphics-queue instead, which is also a transfer-queue
		if (!transferQueueIndex.has_value())
			transferQueueIndex = graphicsQueueIndex.value();
		constexpr float fQueuePriority = 1.0f;
		const size_t uniqueQueueIndexCount = uniqueQueueIndices.size();
		VkDeviceQueueCreateInfo *vk_pDeviceQueueCreateInfos = new VkDeviceQueueCreateInfo[uniqueQueueIndexCount];
		uint32_t u32UniqueQueueIndexIndex = 0U;
		for (uint32_t u32UniqueQueueIndex : uniqueQueueIndices) {
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].pNext = nullptr;
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].flags = 0;
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].queueFamilyIndex = u32UniqueQueueIndex;
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].queueCount = 1U;
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].pQueuePriorities = &fQueuePriority;
			u32UniqueQueueIndexIndex++;
		}

		VkPhysicalDeviceFeatures vk_physicalDeviceFeaturesEnabled = {};

		VkDeviceCreateInfo vk_deviceCreateInfo = {};
		vk_deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		vk_deviceCreateInfo.pQueueCreateInfos = vk_pDeviceQueueCreateInfos;
		vk_deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(uniqueQueueIndexCount);
		vk_deviceCreateInfo.enabledExtensionCount = u32PhysicalDeviceExtensionCount;
		vk_deviceCreateInfo.ppEnabledExtensionNames = static_cast<const char *const *>(pcPhysicalDeviceExtensions);
		vk_deviceCreateInfo.pEnabledFeatures = &vk_physicalDeviceFeaturesEnabled;
		const bool bCreatedDeviceSuccessfully = CHECK_VK_RESULT(vkCreateDevice(vk_hPhysicalDeviceSelected, &vk_deviceCreateInfo, nullptr, &vk_hDevice));
		DELETE_ARRAY_SAFELY(vk_pDeviceQueueCreateInfos);
		if (!bCreatedDeviceSuccessfully) {
			RE_FATAL_ERROR("Failed creating logical Vulkan device");
			return false;
		}

		if (!CATCH_SIGNAL_AND_RETURN(load_vulkan_1_0_device() && load_vulkan_1_1_device() && load_vulkan_1_2_device() /* && load_vulkan_1_3_device() && load_vulkan_1_4_device() */ && load_extension_funcs_with_device(), bool)) {
			unload_all_vulkan_functions_of_device();
			CATCH_SIGNAL(vkDestroyDevice(vk_hDevice, nullptr));
			vk_hDevice = VK_NULL_HANDLE;
			return false;
		}

		u32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX] = graphicsQueueIndex.value();
		u32DeviceQueueFamilyIndices[RE_VK_QUEUE_PRESENT_INDEX] = presentQueueIndex.value();
		u32DeviceQueueFamilyIndices[RE_VK_QUEUE_TRANSFER_INDEX] = transferQueueIndex.value();
		CATCH_SIGNAL(vkGetDeviceQueue(vk_hDevice, u32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX], 0, &vk_hDeviceQueueFamilies[RE_VK_QUEUE_GRAPHICS_INDEX]));
		CATCH_SIGNAL(vkGetDeviceQueue(vk_hDevice, u32DeviceQueueFamilyIndices[RE_VK_QUEUE_PRESENT_INDEX], 0, &vk_hDeviceQueueFamilies[RE_VK_QUEUE_PRESENT_INDEX]));
		CATCH_SIGNAL(vkGetDeviceQueue(vk_hDevice, u32DeviceQueueFamilyIndices[RE_VK_QUEUE_TRANSFER_INDEX], 0, &vk_hDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX]));

		VkCommandPoolCreateInfo vk_commandPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = u32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX]
		};
		CATCH_SIGNAL(vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX]));
		vk_commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		CATCH_SIGNAL(vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_TRANSIENT_INDEX]));
		vk_commandPoolCreateInfo.queueFamilyIndex = u32DeviceQueueFamilyIndices[RE_VK_QUEUE_TRANSFER_INDEX];
		CATCH_SIGNAL(vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX]));
		vk_commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CATCH_SIGNAL(vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX]));
		const VkCommandBufferAllocateInfo vk_commandBufferAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX],
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1U
		};
		if (!CHECK_VK_RESULT(vkAllocateCommandBuffers(vk_hDevice, &vk_commandBufferAllocInfo, &vk_hDummyTransferCommandBuffer)))
			return false;
		const VkCommandBufferBeginInfo vk_beginRecordingCommandBuffer = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
		};
		if (!CHECK_VK_RESULT(vkBeginCommandBuffer(vk_hDummyTransferCommandBuffer, &vk_beginRecordingCommandBuffer)))
			return false;
		CATCH_SIGNAL(vkEndCommandBuffer(vk_hDummyTransferCommandBuffer));
		return true;
	}
	
	static void destroy_device() {
		if (!vk_hDevice)
			return;
		for (uint8_t u8CommandPoolIndex = 0U; u8CommandPoolIndex < RE_VK_COMMAND_POOL_COUNT; u8CommandPoolIndex++) {
			CATCH_SIGNAL(vkDestroyCommandPool(vk_hDevice, vk_hCommandPools[u8CommandPoolIndex], nullptr));
			vk_hCommandPools[u8CommandPoolIndex] = VK_NULL_HANDLE;
		}
		for (uint8_t u8QueueFamilyIndex = 0U; u8QueueFamilyIndex < RE_VK_QUEUE_COUNT; u8QueueFamilyIndex++) {
			u32DeviceQueueFamilyIndices[u8QueueFamilyIndex] = 0U;
			vk_hDeviceQueueFamilies[u8QueueFamilyIndex] = VK_NULL_HANDLE;
		}
		CATCH_SIGNAL(vkDestroyDevice(vk_hDevice, nullptr));
		vk_hDevice = VK_NULL_HANDLE;
	}

	static bool create_swapchain() {
		// Create actual sweapchain
		const VkSwapchainKHR vk_hOldSwapchain = vk_hSwapchain;
		if (vk_hOldSwapchain) {
			CATCH_SIGNAL(swapchain_destroyed_renderer());
			for (uint32_t u32SwapchainImageIndex = 0U; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++)
				CATCH_SIGNAL(vkDestroyImageView(vk_hDevice, vk_phSwapchainImageViews[u32SwapchainImageIndex], nullptr));
			DELETE_ARRAY_SAFELY(vk_phSwapchainImages);
			DELETE_ARRAY_SAFELY(vk_phSwapchainImageViews);
		}
		if (vk_surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			vk_swapchainResolution = vk_surfaceCapabilities.currentExtent;
		else {
			vk_swapchainResolution.width = std::clamp<uint32_t>(windowSize[0], vk_surfaceCapabilities.minImageExtent.width, vk_surfaceCapabilities.maxImageExtent.width);
			vk_swapchainResolution.height = std::clamp<uint32_t>(windowSize[1], vk_surfaceCapabilities.minImageExtent.height, vk_surfaceCapabilities.maxImageExtent.height);
		}
		VkSwapchainCreateInfoKHR vk_swapchainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = vk_hSurface,
			.minImageCount = std::clamp(vk_surfaceCapabilities.minImageCount + 1U, vk_surfaceCapabilities.minImageCount, vk_surfaceCapabilities.maxImageCount > 0U ? vk_surfaceCapabilities.maxImageCount : std::numeric_limits<uint32_t>::max()),
			.imageFormat = vk_surfaceFormatSelected.format,
			.imageColorSpace = vk_surfaceFormatSelected.colorSpace,
			.imageExtent = vk_swapchainResolution,
			.imageArrayLayers = 1U,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.preTransform = vk_surfaceCapabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.clipped = VK_TRUE,
			.oldSwapchain = vk_hOldSwapchain
		};
		const uint32_t u32SwapchainRelevantQueueIndices[2] = {u32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX], u32DeviceQueueFamilyIndices[RE_VK_QUEUE_PRESENT_INDEX]};
		if (u32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX] != u32DeviceQueueFamilyIndices[RE_VK_QUEUE_PRESENT_INDEX]) {
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_swapchainCreateInfo.queueFamilyIndexCount = 2U;
			vk_swapchainCreateInfo.pQueueFamilyIndices = u32SwapchainRelevantQueueIndices;
		} else
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (is_bit_true<uint8_t>(u8RenderSystemFlags, FPS_BOUND_TO_VSYNC_INDEX))
			vk_swapchainCreateInfo.presentMode = is_bit_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_INDEX) ? VK_PRESENT_MODE_FIFO_KHR : vk_ePresentModeNoVsync;
		else
			vk_swapchainCreateInfo.presentMode = is_bit_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_INDEX) ? vk_ePresentModeVsync : vk_ePresentModeNoVsync;
		if (!CHECK_VK_RESULT(vkCreateSwapchainKHR(vk_hDevice, &vk_swapchainCreateInfo, nullptr, &vk_hSwapchain))) {
			RE_FATAL_ERROR("Failed creating Vulkan swapchain");
			return false;
		}
		vk_eSwapchainImageFormat = vk_surfaceFormatSelected.format;
		if (vk_hOldSwapchain != VK_NULL_HANDLE)
			CATCH_SIGNAL(vkDestroySwapchainKHR(vk_hDevice, vk_hOldSwapchain, nullptr));
		CATCH_SIGNAL(vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, nullptr));
		vk_phSwapchainImages = new VkImage[u32SwapchainImageCount];
		CATCH_SIGNAL(vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, vk_phSwapchainImages));
		// End of creating actual swapchain

		// Create swapchain image views
		vk_phSwapchainImageViews = new VkImageView[u32SwapchainImageCount];
		uint32_t u32SwapchainImageViewsCreated = 0U;
		while (u32SwapchainImageViewsCreated < u32SwapchainImageCount) {
			VkImageViewCreateInfo vk_swapchainImageViewCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = vk_phSwapchainImages[u32SwapchainImageViewsCreated],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = vk_eSwapchainImageFormat,
				.components = {
					.r = VK_COMPONENT_SWIZZLE_IDENTITY,
					.g = VK_COMPONENT_SWIZZLE_IDENTITY,
					.b = VK_COMPONENT_SWIZZLE_IDENTITY,
					.a = VK_COMPONENT_SWIZZLE_IDENTITY
				},
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0U,
					.levelCount = 1U,
					.baseArrayLayer = 0U,
					.layerCount = 1U
				}
			};
			if (!CHECK_VK_RESULT(vkCreateImageView(vk_hDevice, &vk_swapchainImageViewCreateInfo, nullptr, &vk_phSwapchainImageViews[u32SwapchainImageViewsCreated]))) {
				RE_FATAL_ERROR(append_to_string("Failed to create Vulkan image view at index ", u32SwapchainImageViewsCreated));
				break;
			}
			u32SwapchainImageViewsCreated++;
		}
		if (u32SwapchainImageViewsCreated != u32SwapchainImageCount) {
			for (uint32_t u32SwapchainImageDeleteIndex = 0U; u32SwapchainImageDeleteIndex < u32SwapchainImageViewsCreated; u32SwapchainImageDeleteIndex++)
				CATCH_SIGNAL(vkDestroyImageView(vk_hDevice, vk_phSwapchainImageViews[u32SwapchainImageDeleteIndex], nullptr));
			DELETE_ARRAY_SAFELY(vk_phSwapchainImages);
			DELETE_ARRAY_SAFELY(vk_phSwapchainImageViews);
			CATCH_SIGNAL(vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr));
			vk_hSwapchain = VK_NULL_HANDLE;
			return false;
		}
		// End of creating swapchain image views

		return true;
	}
	
	static void destroy_swapchain() {
		if (!vk_hSwapchain)
			return;
		CATCH_SIGNAL(swapchain_destroyed_renderer());
		for (uint32_t u32SwapchainImageIndex = 0U; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++)
			CATCH_SIGNAL(vkDestroyImageView(vk_hDevice, vk_phSwapchainImageViews[u32SwapchainImageIndex], nullptr));
		DELETE_ARRAY_SAFELY(vk_phSwapchainImages);
		DELETE_ARRAY_SAFELY(vk_phSwapchainImageViews);
		CATCH_SIGNAL(vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr));
		vk_hSwapchain = VK_NULL_HANDLE;
	}

	static bool recreate_swapchain() {
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities));
		WAIT_FOR_IDLE_VULKAN_DEVICE();
		if (!CATCH_SIGNAL_AND_RETURN(create_swapchain() && swapchain_created_renderer(), bool)) {
			RE_ERROR("Failed recreating the swapchain");
			return false;
		}
		return true;
	}

	bool init_vulkan_device() {
		DEFINE_SIGNAL_GUARD(sigGuardConstructorRenderSystem);
		if (!create_surface() || !alloc_physical_device_list()) {
			RE_ERROR("Failed initializing the render system");
			return false;
		}
		CATCH_SIGNAL(select_best_physical_vulkan_device());
		VkPhysicalDeviceProperties vk_physicalDeviceSelectedProperties;
		CATCH_SIGNAL(vkGetPhysicalDeviceProperties(vk_hPhysicalDeviceSelected, &vk_physicalDeviceSelectedProperties));
		PRINT_LN(append_to_string("Device selected: ", vk_physicalDeviceSelectedProperties.deviceName).c_str());
		CATCH_SIGNAL(vkGetPhysicalDeviceMemoryProperties(vk_hPhysicalDeviceSelected, &vk_physicalDeviceMemoryProperties));
		CATCH_SIGNAL(fetch_surface_infos());
		CATCH_SIGNAL(select_best_surface_format());
		if (!create_device() || !create_swapchain()) {
			RE_ERROR("Failed initializing the render system");
			return false;
		}
		return true;
	}

	void destroy_vulkan_device() {
		CATCH_SIGNAL(destroy_swapchain());
		CATCH_SIGNAL(destroy_surface());
		CATCH_SIGNAL(destroy_device());
		CATCH_SIGNAL(free_physical_device_list());
	}

	bool refresh_swapchain() {
		if (is_bit_true<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_INDEX)) {
			if (!CATCH_SIGNAL_AND_RETURN(recreate_swapchain(), bool))
				return false;
			set_bit<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_INDEX, false);
		}
		return true;
	}

	void mark_swapchain_dirty() {
		set_bit<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_INDEX, bRunning);
	}

	void enable_vsync(bool bEnableVsync) {
		if (is_bit_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_INDEX) != bEnableVsync) {
			set_bit<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_INDEX, bEnableVsync);
			set_bit<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_INDEX, bRunning);
		}
	}

	bool is_vsync_enabled() {
		return is_bit_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_INDEX);
	}

	void bind_fps_to_vsync(bool bBindFpsToVsync) {
		if (is_bit_true<uint8_t>(u8RenderSystemFlags, FPS_BOUND_TO_VSYNC_INDEX) != bBindFpsToVsync) {
			set_bit<uint8_t>(u8RenderSystemFlags, FPS_BOUND_TO_VSYNC_INDEX, bBindFpsToVsync);
			set_bit<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_INDEX, bRunning);
		}
	}

	bool is_fps_bound_to_vsync() {
		return is_bit_true<uint8_t>(u8RenderSystemFlags, FPS_BOUND_TO_VSYNC_INDEX);
	}

}