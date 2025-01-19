#include "RE_Vulkan.hpp"

namespace RE {

	PFN_vkCreateInstance vkCreateInstance = nullptr;
	PFN_vkDestroyInstance vkDestroyInstance = nullptr;
	PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;
	PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures = nullptr;
	PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties = nullptr;
	PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties = nullptr;
	PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = nullptr;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
	PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = nullptr;
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;
	PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = nullptr;
	PFN_vkCreateDevice vkCreateDevice = nullptr;
	PFN_vkDestroyDevice vkDestroyDevice = nullptr;
	PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = nullptr;
	PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties = nullptr;
	PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = nullptr;
	PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties = nullptr;
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
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;
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
	PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;

	VkInstance vkInstance = {};
	VkSurfaceKHR vkSurface = {};
	VkPhysicalDevice vkPhysicalDevice = {};
	VkDevice vkDevice = {};
	VulkanQueue graphicsQueue = {};

	Vulkan* Vulkan::instance = nullptr;

	Vulkan::~Vulkan() {
		if (instance != this)
			return;
		instance = nullptr;
		vkCreateInstance = nullptr;
		vkDestroyInstance = nullptr;
		vkEnumeratePhysicalDevices = nullptr;
		vkGetPhysicalDeviceFeatures = nullptr;
		vkGetPhysicalDeviceFormatProperties = nullptr;
		vkGetPhysicalDeviceImageFormatProperties = nullptr;
		vkGetPhysicalDeviceProperties = nullptr;
		vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
		vkGetPhysicalDeviceMemoryProperties = nullptr;
		vkGetInstanceProcAddr = nullptr;
		vkGetDeviceProcAddr = nullptr;
		vkCreateDevice = nullptr;
		vkDestroyDevice = nullptr;
		vkEnumerateInstanceExtensionProperties = nullptr;
		vkEnumerateDeviceExtensionProperties = nullptr;
		vkEnumerateInstanceLayerProperties = nullptr;
		vkEnumerateDeviceLayerProperties = nullptr;
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
		vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;
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
		vkDestroySurfaceKHR = nullptr;
		vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
	}

	bool Vulkan::loadAllFunc() {
		if (isBitTrue<REubyte>(validation, _VK_FUNC))
			return true;
		vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(loadFunc("vkDestroyInstance"));
		if (!vkDestroyInstance)
			return false;
		vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(loadFunc("vkEnumeratePhysicalDevices"));
		if (!vkEnumeratePhysicalDevices)
			return false;
		vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(loadFunc("vkGetPhysicalDeviceFeatures"));
		if (!vkGetPhysicalDeviceFeatures)
			return false;
		vkGetPhysicalDeviceFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(loadFunc("vkGetPhysicalDeviceFormatProperties"));
		if (!vkGetPhysicalDeviceFormatProperties)
			return false;
		vkGetPhysicalDeviceImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties>(loadFunc("vkGetPhysicalDeviceImageFormatProperties"));
		if (!vkGetPhysicalDeviceImageFormatProperties)
			return false;
		vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(loadFunc("vkGetPhysicalDeviceProperties"));
		if (!vkGetPhysicalDeviceProperties)
			return false;
		vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(loadFunc("vkGetPhysicalDeviceQueueFamilyProperties"));
		if (!vkGetPhysicalDeviceQueueFamilyProperties)
			return false;
		vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(loadFunc("vkGetPhysicalDeviceMemoryProperties"));
		if (!vkGetPhysicalDeviceMemoryProperties)
			return false;
		vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(loadFunc("vkGetInstanceProcAddr"));
		if (!vkGetInstanceProcAddr)
			return false;
		vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(loadFunc("vkGetDeviceProcAddr"));
		if (!vkGetDeviceProcAddr)
			return false;
		vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(loadFunc("vkCreateDevice"));
		if (!vkCreateDevice)
			return false;
		vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(loadFunc("vkDestroyDevice"));
		if (!vkDestroyDevice)
			return false;
		vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(loadFunc("vkEnumerateInstanceExtensionProperties"));
		if (!vkEnumerateInstanceExtensionProperties)
			return false;
		vkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(loadFunc("vkEnumerateDeviceExtensionProperties"));
		if (!vkEnumerateDeviceExtensionProperties)
			return false;
		vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(loadFunc("vkEnumerateInstanceLayerProperties"));
		if (!vkEnumerateInstanceLayerProperties)
			return false;
		vkEnumerateDeviceLayerProperties = reinterpret_cast<PFN_vkEnumerateDeviceLayerProperties>(loadFunc("vkEnumerateDeviceLayerProperties"));
		if (!vkEnumerateDeviceLayerProperties)
			return false;
		vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(loadFunc("vkGetDeviceQueue"));
		if (!vkGetDeviceQueue)
			return false;
		vkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(loadFunc("vkQueueSubmit"));
		if (!vkQueueSubmit)
			return false;
		vkQueueWaitIdle = reinterpret_cast<PFN_vkQueueWaitIdle>(loadFunc("vkQueueWaitIdle"));
		if (!vkQueueWaitIdle)
			return false;
		vkDeviceWaitIdle = reinterpret_cast<PFN_vkDeviceWaitIdle>(loadFunc("vkDeviceWaitIdle"));
		if (!vkDeviceWaitIdle)
			return false;
		vkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(loadFunc("vkAllocateMemory"));
		if (!vkAllocateMemory)
			return false;
		vkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(loadFunc("vkFreeMemory"));
		if (!vkFreeMemory)
			return false;
		vkMapMemory = reinterpret_cast<PFN_vkMapMemory>(loadFunc("vkMapMemory"));
		if (!vkMapMemory)
			return false;
		vkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(loadFunc("vkUnmapMemory"));
		if (!vkUnmapMemory)
			return false;
		vkFlushMappedMemoryRanges = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(loadFunc("vkFlushMappedMemoryRanges"));
		if (!vkFlushMappedMemoryRanges)
			return false;
		vkInvalidateMappedMemoryRanges = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(loadFunc("vkInvalidateMappedMemoryRanges"));
		if (!vkInvalidateMappedMemoryRanges)
			return false;
		vkGetDeviceMemoryCommitment = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(loadFunc("vkGetDeviceMemoryCommitment"));
		if (!vkGetDeviceMemoryCommitment)
			return false;
		vkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(loadFunc("vkBindBufferMemory"));
		if (!vkBindBufferMemory)
			return false;
		vkBindImageMemory = reinterpret_cast<PFN_vkBindImageMemory>(loadFunc("vkBindImageMemory"));
		if (!vkBindImageMemory)
			return false;
		vkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(loadFunc("vkGetBufferMemoryRequirements"));
		if (!vkGetBufferMemoryRequirements)
			return false;
		vkGetImageMemoryRequirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(loadFunc("vkGetImageMemoryRequirements"));
		if (!vkGetImageMemoryRequirements)
			return false;
		vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(loadFunc("vkGetImageSparseMemoryRequirements"));
		if (!vkGetImageSparseMemoryRequirements)
			return false;
		vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties>(loadFunc("vkGetPhysicalDeviceSparseImageFormatProperties"));
		if (!vkGetPhysicalDeviceSparseImageFormatProperties)
			return false;
		vkQueueBindSparse = reinterpret_cast<PFN_vkQueueBindSparse>(loadFunc("vkQueueBindSparse"));
		if (!vkQueueBindSparse)
			return false;
		vkCreateFence = reinterpret_cast<PFN_vkCreateFence>(loadFunc("vkCreateFence"));
		if (!vkCreateFence)
			return false;
		vkDestroyFence = reinterpret_cast<PFN_vkDestroyFence>(loadFunc("vkDestroyFence"));
		if (!vkDestroyFence)
			return false;
		vkResetFences = reinterpret_cast<PFN_vkResetFences>(loadFunc("vkResetFences"));
		if (!vkResetFences)
			return false;
		vkGetFenceStatus = reinterpret_cast<PFN_vkGetFenceStatus>(loadFunc("vkGetFenceStatus"));
		if (!vkGetFenceStatus)
			return false;
		vkWaitForFences = reinterpret_cast<PFN_vkWaitForFences>(loadFunc("vkWaitForFences"));
		if (!vkWaitForFences)
			return false;
		vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(loadFunc("vkCreateSemaphore"));
		if (!vkCreateSemaphore)
			return false;
		vkDestroySemaphore = reinterpret_cast<PFN_vkDestroySemaphore>(loadFunc("vkDestroySemaphore"));
		if (!vkDestroySemaphore)
			return false;
		vkCreateEvent = reinterpret_cast<PFN_vkCreateEvent>(loadFunc("vkCreateEvent"));
		if (!vkCreateEvent)
			return false;
		vkDestroyEvent = reinterpret_cast<PFN_vkDestroyEvent>(loadFunc("vkDestroyEvent"));
		if (!vkDestroyEvent)
			return false;
		vkGetEventStatus = reinterpret_cast<PFN_vkGetEventStatus>(loadFunc("vkGetEventStatus"));
		if (!vkGetEventStatus)
			return false;
		vkSetEvent = reinterpret_cast<PFN_vkSetEvent>(loadFunc("vkSetEvent"));
		if (!vkSetEvent)
			return false;
		vkResetEvent = reinterpret_cast<PFN_vkResetEvent>(loadFunc("vkResetEvent"));
		if (!vkResetEvent)
			return false;
		vkCreateQueryPool = reinterpret_cast<PFN_vkCreateQueryPool>(loadFunc("vkCreateQueryPool"));
		if (!vkCreateQueryPool)
			return false;
		vkDestroyQueryPool = reinterpret_cast<PFN_vkDestroyQueryPool>(loadFunc("vkDestroyQueryPool"));
		if (!vkDestroyQueryPool)
			return false;
		vkGetQueryPoolResults = reinterpret_cast<PFN_vkGetQueryPoolResults>(loadFunc("vkGetQueryPoolResults"));
		if (!vkGetQueryPoolResults)
			return false;
		vkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(loadFunc("vkCreateBuffer"));
		if (!vkCreateBuffer)
			return false;
		vkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(loadFunc("vkDestroyBuffer"));
		if (!vkDestroyBuffer)
			return false;
		vkCreateBufferView = reinterpret_cast<PFN_vkCreateBufferView>(loadFunc("vkCreateBufferView"));
		if (!vkCreateBufferView)
			return false;
		vkDestroyBufferView = reinterpret_cast<PFN_vkDestroyBufferView>(loadFunc("vkDestroyBufferView"));
		if (!vkDestroyBufferView)
			return false;
		vkCreateImage = reinterpret_cast<PFN_vkCreateImage>(loadFunc("vkCreateImage"));
		if (!vkCreateImage)
			return false;
		vkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(loadFunc("vkDestroyImage"));
		if (!vkDestroyImage)
			return false;
		vkGetImageSubresourceLayout = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(loadFunc("vkGetImageSubresourceLayout"));
		if (!vkGetImageSubresourceLayout)
			return false;
		vkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(loadFunc("vkCreateImageView"));
		if (!vkCreateImageView)
			return false;
		vkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(loadFunc("vkDestroyImageView"));
		if (!vkDestroyImageView)
			return false;
		vkCreateShaderModule = reinterpret_cast<PFN_vkCreateShaderModule>(loadFunc("vkCreateShaderModule"));
		if (!vkCreateShaderModule)
			return false;
		vkDestroyShaderModule = reinterpret_cast<PFN_vkDestroyShaderModule>(loadFunc("vkDestroyShaderModule"));
		if (!vkDestroyShaderModule)
			return false;
		vkCreatePipelineCache = reinterpret_cast<PFN_vkCreatePipelineCache>(loadFunc("vkCreatePipelineCache"));
		if (!vkCreatePipelineCache)
			return false;
		vkDestroyPipelineCache = reinterpret_cast<PFN_vkDestroyPipelineCache>(loadFunc("vkDestroyPipelineCache"));
		if (!vkDestroyPipelineCache)
			return false;
		vkGetPipelineCacheData = reinterpret_cast<PFN_vkGetPipelineCacheData>(loadFunc("vkGetPipelineCacheData"));
		if (!vkGetPipelineCacheData)
			return false;
		vkMergePipelineCaches = reinterpret_cast<PFN_vkMergePipelineCaches>(loadFunc("vkMergePipelineCaches"));
		if (!vkMergePipelineCaches)
			return false;
		vkCreateGraphicsPipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(loadFunc("vkCreateGraphicsPipelines"));
		if (!vkCreateGraphicsPipelines)
			return false;
		vkCreateComputePipelines = reinterpret_cast<PFN_vkCreateComputePipelines>(loadFunc("vkCreateComputePipelines"));
		if (!vkCreateComputePipelines)
			return false;
		vkDestroyPipeline = reinterpret_cast<PFN_vkDestroyPipeline>(loadFunc("vkDestroyPipeline"));
		if (!vkDestroyPipeline)
			return false;
		vkCreatePipelineLayout = reinterpret_cast<PFN_vkCreatePipelineLayout>(loadFunc("vkCreatePipelineLayout"));
		if (!vkCreatePipelineLayout)
			return false;
		vkDestroyPipelineLayout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(loadFunc("vkDestroyPipelineLayout"));
		if (!vkDestroyPipelineLayout)
			return false;
		vkCreateSampler = reinterpret_cast<PFN_vkCreateSampler>(loadFunc("vkCreateSampler"));
		if (!vkCreateSampler)
			return false;
		vkDestroySampler = reinterpret_cast<PFN_vkDestroySampler>(loadFunc("vkDestroySampler"));
		if (!vkDestroySampler)
			return false;
		vkCreateDescriptorSetLayout = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(loadFunc("vkCreateDescriptorSetLayout"));
		if (!vkCreateDescriptorSetLayout)
			return false;
		vkDestroyDescriptorSetLayout = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(loadFunc("vkDestroyDescriptorSetLayout"));
		if (!vkDestroyDescriptorSetLayout)
			return false;
		vkCreateDescriptorPool = reinterpret_cast<PFN_vkCreateDescriptorPool>(loadFunc("vkCreateDescriptorPool"));
		if (!vkCreateDescriptorPool)
			return false;
		vkDestroyDescriptorPool = reinterpret_cast<PFN_vkDestroyDescriptorPool>(loadFunc("vkDestroyDescriptorPool"));
		if (!vkDestroyDescriptorPool)
			return false;
		vkResetDescriptorPool = reinterpret_cast<PFN_vkResetDescriptorPool>(loadFunc("vkResetDescriptorPool"));
		if (!vkResetDescriptorPool)
			return false;
		vkAllocateDescriptorSets = reinterpret_cast<PFN_vkAllocateDescriptorSets>(loadFunc("vkAllocateDescriptorSets"));
		if (!vkAllocateDescriptorSets)
			return false;
		vkFreeDescriptorSets = reinterpret_cast<PFN_vkFreeDescriptorSets>(loadFunc("vkFreeDescriptorSets"));
		if (!vkFreeDescriptorSets)
			return false;
		vkUpdateDescriptorSets = reinterpret_cast<PFN_vkUpdateDescriptorSets>(loadFunc("vkUpdateDescriptorSets"));
		if (!vkUpdateDescriptorSets)
			return false;
		vkCreateFramebuffer = reinterpret_cast<PFN_vkCreateFramebuffer>(loadFunc("vkCreateFramebuffer"));
		if (!vkCreateFramebuffer)
			return false;
		vkDestroyFramebuffer = reinterpret_cast<PFN_vkDestroyFramebuffer>(loadFunc("vkDestroyFramebuffer"));
		if (!vkDestroyFramebuffer)
			return false;
		vkCreateRenderPass = reinterpret_cast<PFN_vkCreateRenderPass>(loadFunc("vkCreateRenderPass"));
		if (!vkCreateRenderPass)
			return false;
		vkDestroyRenderPass = reinterpret_cast<PFN_vkDestroyRenderPass>(loadFunc("vkDestroyRenderPass"));
		if (!vkDestroyRenderPass)
			return false;
		vkGetRenderAreaGranularity = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(loadFunc("vkGetRenderAreaGranularity"));
		if (!vkGetRenderAreaGranularity)
			return false;
		vkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(loadFunc("vkCreateCommandPool"));
		if (!vkCreateCommandPool)
			return false;
		vkDestroyCommandPool = reinterpret_cast<PFN_vkDestroyCommandPool>(loadFunc("vkDestroyCommandPool"));
		if (!vkDestroyCommandPool)
			return false;
		vkResetCommandPool = reinterpret_cast<PFN_vkResetCommandPool>(loadFunc("vkResetCommandPool"));
		if (!vkResetCommandPool)
			return false;
		vkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(loadFunc("vkAllocateCommandBuffers"));
		if (!vkAllocateCommandBuffers)
			return false;
		vkFreeCommandBuffers = reinterpret_cast<PFN_vkFreeCommandBuffers>(loadFunc("vkFreeCommandBuffers"));
		if (!vkFreeCommandBuffers)
			return false;
		vkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(loadFunc("vkBeginCommandBuffer"));
		if (!vkBeginCommandBuffer)
			return false;
		vkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(loadFunc("vkEndCommandBuffer"));
		if (!vkEndCommandBuffer)
			return false;
		vkResetCommandBuffer = reinterpret_cast<PFN_vkResetCommandBuffer>(loadFunc("vkResetCommandBuffer"));
		if (!vkResetCommandBuffer)
			return false;
		vkCmdBindPipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(loadFunc("vkCmdBindPipeline"));
		if (!vkCmdBindPipeline)
			return false;
		vkCmdSetViewport = reinterpret_cast<PFN_vkCmdSetViewport>(loadFunc("vkCmdSetViewport"));
		if (!vkCmdSetViewport)
			return false;
		vkCmdSetScissor = reinterpret_cast<PFN_vkCmdSetScissor>(loadFunc("vkCmdSetScissor"));
		if (!vkCmdSetScissor)
			return false;
		vkCmdSetLineWidth = reinterpret_cast<PFN_vkCmdSetLineWidth>(loadFunc("vkCmdSetLineWidth"));
		if (!vkCmdSetLineWidth)
			return false;
		vkCmdSetDepthBias = reinterpret_cast<PFN_vkCmdSetDepthBias>(loadFunc("vkCmdSetDepthBias"));
		if (!vkCmdSetDepthBias)
			return false;
		vkCmdSetBlendConstants = reinterpret_cast<PFN_vkCmdSetBlendConstants>(loadFunc("vkCmdSetBlendConstants"));
		if (!vkCmdSetBlendConstants)
			return false;
		vkCmdSetDepthBounds = reinterpret_cast<PFN_vkCmdSetDepthBounds>(loadFunc("vkCmdSetDepthBounds"));
		if (!vkCmdSetDepthBounds)
			return false;
		vkCmdSetStencilCompareMask = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(loadFunc("vkCmdSetStencilCompareMask"));
		if (!vkCmdSetStencilCompareMask)
			return false;
		vkCmdSetStencilWriteMask = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(loadFunc("vkCmdSetStencilWriteMask"));
		if (!vkCmdSetStencilWriteMask)
			return false;
		vkCmdSetStencilReference = reinterpret_cast<PFN_vkCmdSetStencilReference>(loadFunc("vkCmdSetStencilReference"));
		if (!vkCmdSetStencilReference)
			return false;
		vkCmdBindDescriptorSets = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(loadFunc("vkCmdBindDescriptorSets"));
		if (!vkCmdBindDescriptorSets)
			return false;
		vkCmdBindIndexBuffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(loadFunc("vkCmdBindIndexBuffer"));
		if (!vkCmdBindIndexBuffer)
			return false;
		vkCmdBindVertexBuffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(loadFunc("vkCmdBindVertexBuffers"));
		if (!vkCmdBindVertexBuffers)
			return false;
		vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(loadFunc("vkCmdDraw"));
		if (!vkCmdDraw)
			return false;
		vkCmdDrawIndexed = reinterpret_cast<PFN_vkCmdDrawIndexed>(loadFunc("vkCmdDrawIndexed"));
		if (!vkCmdDrawIndexed)
			return false;
		vkCmdDrawIndirect = reinterpret_cast<PFN_vkCmdDrawIndirect>(loadFunc("vkCmdDrawIndirect"));
		if (!vkCmdDrawIndirect)
			return false;
		vkCmdDrawIndexedIndirect = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(loadFunc("vkCmdDrawIndexedIndirect"));
		if (!vkCmdDrawIndexedIndirect)
			return false;
		vkCmdDispatch = reinterpret_cast<PFN_vkCmdDispatch>(loadFunc("vkCmdDispatch"));
		if (!vkCmdDispatch)
			return false;
		vkCmdDispatchIndirect = reinterpret_cast<PFN_vkCmdDispatchIndirect>(loadFunc("vkCmdDispatchIndirect"));
		if (!vkCmdDispatchIndirect)
			return false;
		vkCmdCopyBuffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(loadFunc("vkCmdCopyBuffer"));
		if (!vkCmdCopyBuffer)
			return false;
		vkCmdCopyImage = reinterpret_cast<PFN_vkCmdCopyImage>(loadFunc("vkCmdCopyImage"));
		if (!vkCmdCopyImage)
			return false;
		vkCmdBlitImage = reinterpret_cast<PFN_vkCmdBlitImage>(loadFunc("vkCmdBlitImage"));
		if (!vkCmdBlitImage)
			return false;
		vkCmdCopyBufferToImage = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(loadFunc("vkCmdCopyBufferToImage"));
		if (!vkCmdCopyBufferToImage)
			return false;
		vkCmdCopyImageToBuffer = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(loadFunc("vkCmdCopyImageToBuffer"));
		if (!vkCmdCopyImageToBuffer)
			return false;
		vkCmdUpdateBuffer = reinterpret_cast<PFN_vkCmdUpdateBuffer>(loadFunc("vkCmdUpdateBuffer"));
		if (!vkCmdUpdateBuffer)
			return false;
		vkCmdFillBuffer = reinterpret_cast<PFN_vkCmdFillBuffer>(loadFunc("vkCmdFillBuffer"));
		if (!vkCmdFillBuffer)
			return false;
		vkCmdClearColorImage = reinterpret_cast<PFN_vkCmdClearColorImage>(loadFunc("vkCmdClearColorImage"));
		if (!vkCmdClearColorImage)
			return false;
		vkCmdClearDepthStencilImage = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(loadFunc("vkCmdClearDepthStencilImage"));
		if (!vkCmdClearDepthStencilImage)
			return false;
		vkCmdClearAttachments = reinterpret_cast<PFN_vkCmdClearAttachments>(loadFunc("vkCmdClearAttachments"));
		if (!vkCmdClearAttachments)
			return false;
		vkCmdResolveImage = reinterpret_cast<PFN_vkCmdResolveImage>(loadFunc("vkCmdResolveImage"));
		if (!vkCmdResolveImage)
			return false;
		vkCmdSetEvent = reinterpret_cast<PFN_vkCmdSetEvent>(loadFunc("vkCmdSetEvent"));
		if (!vkCmdSetEvent)
			return false;
		vkCmdResetEvent = reinterpret_cast<PFN_vkCmdResetEvent>(loadFunc("vkCmdResetEvent"));
		if (!vkCmdResetEvent)
			return false;
		vkCmdWaitEvents = reinterpret_cast<PFN_vkCmdWaitEvents>(loadFunc("vkCmdWaitEvents"));
		if (!vkCmdWaitEvents)
			return false;
		vkCmdPipelineBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(loadFunc("vkCmdPipelineBarrier"));
		if (!vkCmdPipelineBarrier)
			return false;
		vkCmdBeginQuery = reinterpret_cast<PFN_vkCmdBeginQuery>(loadFunc("vkCmdBeginQuery"));
		if (!vkCmdBeginQuery)
			return false;
		vkCmdEndQuery = reinterpret_cast<PFN_vkCmdEndQuery>(loadFunc("vkCmdEndQuery"));
		if (!vkCmdEndQuery)
			return false;
		vkCmdResetQueryPool = reinterpret_cast<PFN_vkCmdResetQueryPool>(loadFunc("vkCmdResetQueryPool"));
		if (!vkCmdResetQueryPool)
			return false;
		vkCmdWriteTimestamp = reinterpret_cast<PFN_vkCmdWriteTimestamp>(loadFunc("vkCmdWriteTimestamp"));
		if (!vkCmdWriteTimestamp)
			return false;
		vkCmdCopyQueryPoolResults = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(loadFunc("vkCmdCopyQueryPoolResults"));
		if (!vkCmdCopyQueryPoolResults)
			return false;
		vkCmdPushConstants = reinterpret_cast<PFN_vkCmdPushConstants>(loadFunc("vkCmdPushConstants"));
		if (!vkCmdPushConstants)
			return false;
		vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(loadFunc("vkCmdBeginRenderPass"));
		if (!vkCmdBeginRenderPass)
			return false;
		vkCmdNextSubpass = reinterpret_cast<PFN_vkCmdNextSubpass>(loadFunc("vkCmdNextSubpass"));
		if (!vkCmdNextSubpass)
			return false;
		vkCmdEndRenderPass = reinterpret_cast<PFN_vkCmdEndRenderPass>(loadFunc("vkCmdEndRenderPass"));
		if (!vkCmdEndRenderPass)
			return false;
		vkCmdExecuteCommands = reinterpret_cast<PFN_vkCmdExecuteCommands>(loadFunc("vkCmdExecuteCommands"));
		if (!vkCmdExecuteCommands)
			return false;
		vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(loadFunc("vkDestroySurfaceKHR"));
		if (!vkDestroySurfaceKHR)
			return false;
		vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(loadFunc("vkGetPhysicalDeviceSurfaceSupportKHR"));
		if (!vkGetPhysicalDeviceSurfaceSupportKHR)
			return false;
		setBit<REubyte>(validation, _VK_FUNC, true);
		return true;
	}
	
	Vulkan::Vulkan() : validation(0) {
		if (instance)
			RE_FATAL_ERROR("Another instance of the Vulkan-class has been initialized");
		else
			instance = this;
	}

	void* Vulkan::loadFunc(const char* funcName) {
		void* func = nullptr;
		if (isBitTrue<REubyte>(validation, 0))
			func = reinterpret_cast<void*>(vkGetInstanceProcAddr(vkInstance, funcName));
		else
			func = loadFuncInternal(funcName);
		if (!func)
			RE_FATAL_ERROR(appendStrings("Failed loading the Vulkan function \"", funcName, "\""));
		return func;
	}

	bool Vulkan::createVulkanInstance(const char** nameExt, REuint numberExt) {
		if (isBitTrue<REubyte>(validation, _VK_INST))
			return true;
		vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(loadFunc("vkCreateInstance"));
		vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(loadFunc("vkGetInstanceProcAddr"));
		if (!vkCreateInstance || !vkGetInstanceProcAddr)
			return false;
		VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
		appInfo.pApplicationName = getAppName();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "R-Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		VkInstanceCreateInfo createInstInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		createInstInfo.pApplicationInfo = &appInfo;
		createInstInfo.enabledLayerCount = 0;
		createInstInfo.ppEnabledLayerNames = nullptr;
		createInstInfo.enabledExtensionCount = numberExt;
		createInstInfo.ppEnabledExtensionNames = nameExt;
		if (vkCreateInstance(&createInstInfo, nullptr, &vkInstance) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating a Vulkan instance");
			return false;
		}
		setBit<REubyte>(validation, _VK_INST, true);
		return true;
	}

	void Vulkan::destroyVulkanInstance() {
		if (!isBitTrue<REubyte>(validation, _VK_INST))
			return;
		vkDestroyInstance(vkInstance, nullptr);
		setBit<REubyte>(validation, _VK_INST, false);
	}

	struct DeviceQueues {
		std::optional<REuint> graphicsQueueIndex;
		std::optional<REuint> presentQueueIndex;

		bool isComplete() {
			return graphicsQueueIndex.has_value() && presentQueueIndex.has_value();
		}
	};

	DeviceQueues isSuitablePhysicalDevice(VkPhysicalDevice physicalDevice) {
		DeviceQueues result;

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(physicalDevice, &features);

		REuint queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		VkQueueFamilyProperties queueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);

		for (REuint i = 0; i < queueFamilyCount; i++) {
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				result.graphicsQueueIndex = i;
			VkBool32 presentSupport = false;
			println("Func: ", vkGetPhysicalDeviceSurfaceSupportKHR);
			vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vkSurface, &presentSupport); // function pointer invalid
			if (presentSupport)
				result.presentQueueIndex = i;
		}
		return result;
	}

	bool Vulkan::initVulkan(const char** nameExt, REuint numberExt) {
		if (!createVulkanInstance(nameExt, numberExt))
			return false;
		if (!loadAllFunc())
			return false;
		if (!createSurface())
			return false;

		REuint physicalDevicesCount = 0;
		vkEnumeratePhysicalDevices(vkInstance, &physicalDevicesCount, nullptr);
		if (!physicalDevicesCount) {
			RE_FATAL_ERROR("No installed GPU supports Vulkan");
			return false;
		}
		VkPhysicalDevice physicalDevices[physicalDevicesCount];
		vkEnumeratePhysicalDevices(vkInstance, &physicalDevicesCount, physicalDevices);
		DeviceQueues deviceProps[physicalDevicesCount];
		REuint foundDeviceIndex = -1;
		for (REuint i = 0; i < physicalDevicesCount; i++) {
			deviceProps[i] = isSuitablePhysicalDevice(physicalDevices[i]);
			println("1");
			if (deviceProps[i].isComplete()) {
				println("2");
				foundDeviceIndex = i;
				vkPhysicalDevice = physicalDevices[i];
				break;
			}
			println("3");
		}
		if (foundDeviceIndex < 0) {
			RE_FATAL_ERROR("No GPU supporting Vulkan is suitable for R-Engine's rendering system");
			return false;
		}

		println("test");
		REuint queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
		VkQueueFamilyProperties queueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties);
		for (REuint i = 0; i < queueFamilyCount; i++)
			if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				graphicsQueue.familyIndex = i;
				break;
			}
		VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		queueCreateInfo.queueFamilyIndex = graphicsQueue.familyIndex;
		queueCreateInfo.queueCount = 1;
		float priority[1] = {1.0f};
		queueCreateInfo.pQueuePriorities = priority;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
		VkPhysicalDeviceFeatures enableFeatures = {};
		deviceCreateInfo.pEnabledFeatures = &enableFeatures;
		if (vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating logical vulkan device");
			return false;
		}
		vkGetDeviceQueue(vkDevice, graphicsQueue.familyIndex, 0, &graphicsQueue.vkQueue);
		setBit<REubyte>(validation, _VK_DEVI, true);

		return true;
	}

	void Vulkan::destroyVulkan() {
		if (isBitTrue<REubyte>(validation, _VK_INST)) {
			if (isBitTrue<REubyte>(validation, _VK_SURF)) {
				if (isBitTrue<REubyte>(validation, _VK_DEVI)) {
					vkDestroyDevice(vkDevice, nullptr);
				}
				vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
			}
			vkDestroyInstance(vkInstance, nullptr);
		}
	}

	bool Vulkan::isValid() {
		return areBitsTrueRange<REubyte>(validation, 0, _VK_LAST);
	}

}
