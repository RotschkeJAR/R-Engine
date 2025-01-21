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
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
	PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = nullptr;
	PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR = nullptr;
	PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = nullptr;

	VkInstance vkInstance = {};
	VkDebugUtilsMessengerEXT vkDebugMessenger = {};
	VkSurfaceKHR vkSurface = {};
	VkPhysicalDevice vkPhysicalDevice = {};
	VkDevice vkDevice = {};
	VulkanQueue graphicsQueue = {};
	VulkanQueue presentQueue = {};
	VkSwapchainKHR vkSwapchain = {};
	std::vector<VkImageView> vkSwapChainImageViews;

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
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
		vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
		vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
		vkCreateDebugUtilsMessengerEXT = nullptr;
		vkDestroyDebugUtilsMessengerEXT = nullptr;
		vkCreateSwapchainKHR = nullptr;
		vkDestroySwapchainKHR = nullptr;
		vkGetSwapchainImagesKHR = nullptr;

		vkInstance = VK_NULL_HANDLE;
		vkSurface = VK_NULL_HANDLE;
		vkPhysicalDevice = VK_NULL_HANDLE;
		vkDevice = VK_NULL_HANDLE;
		vkSwapchain = VK_NULL_HANDLE;
		graphicsQueue = {};
		presentQueue = {};
		vkSwapChainImageViews.clear();
	}

	bool Vulkan::loadAllFunc() {
		if (isBitTrue<REubyte>(validation, _VK_FUNC))
			return true;
		bool functionLoadFailure = false;
		vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(loadFunc("vkDestroyInstance"));
		if (!vkDestroyInstance)
			functionLoadFailure = true;
		vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(loadFunc("vkEnumeratePhysicalDevices"));
		if (!vkEnumeratePhysicalDevices)
			functionLoadFailure = true;
		vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(loadFunc("vkGetPhysicalDeviceFeatures"));
		if (!vkGetPhysicalDeviceFeatures)
			functionLoadFailure = true;
		vkGetPhysicalDeviceFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(loadFunc("vkGetPhysicalDeviceFormatProperties"));
		if (!vkGetPhysicalDeviceFormatProperties)
			functionLoadFailure = true;
		vkGetPhysicalDeviceImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties>(loadFunc("vkGetPhysicalDeviceImageFormatProperties"));
		if (!vkGetPhysicalDeviceImageFormatProperties)
			functionLoadFailure = true;
		vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(loadFunc("vkGetPhysicalDeviceProperties"));
		if (!vkGetPhysicalDeviceProperties)
			functionLoadFailure = true;
		vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(loadFunc("vkGetPhysicalDeviceQueueFamilyProperties"));
		if (!vkGetPhysicalDeviceQueueFamilyProperties)
			functionLoadFailure = true;
		vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(loadFunc("vkGetPhysicalDeviceMemoryProperties"));
		if (!vkGetPhysicalDeviceMemoryProperties)
			functionLoadFailure = true;
		vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(loadFunc("vkGetInstanceProcAddr"));
		if (!vkGetInstanceProcAddr)
			functionLoadFailure = true;
		vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(loadFunc("vkGetDeviceProcAddr"));
		if (!vkGetDeviceProcAddr)
			functionLoadFailure = true;
		vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(loadFunc("vkCreateDevice"));
		if (!vkCreateDevice)
			functionLoadFailure = true;
		vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(loadFunc("vkDestroyDevice"));
		if (!vkDestroyDevice)
			functionLoadFailure = true;
		vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(loadFunc("vkEnumerateInstanceExtensionProperties"));
		if (!vkEnumerateInstanceExtensionProperties)
			functionLoadFailure = true;
		vkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(loadFunc("vkEnumerateDeviceExtensionProperties"));
		if (!vkEnumerateDeviceExtensionProperties)
			functionLoadFailure = true;
		vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(loadFunc("vkEnumerateInstanceLayerProperties"));
		if (!vkEnumerateInstanceLayerProperties)
			functionLoadFailure = true;
		vkEnumerateDeviceLayerProperties = reinterpret_cast<PFN_vkEnumerateDeviceLayerProperties>(loadFunc("vkEnumerateDeviceLayerProperties"));
		if (!vkEnumerateDeviceLayerProperties)
			functionLoadFailure = true;
		vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(loadFunc("vkGetDeviceQueue"));
		if (!vkGetDeviceQueue)
			functionLoadFailure = true;
		vkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(loadFunc("vkQueueSubmit"));
		if (!vkQueueSubmit)
			functionLoadFailure = true;
		vkQueueWaitIdle = reinterpret_cast<PFN_vkQueueWaitIdle>(loadFunc("vkQueueWaitIdle"));
		if (!vkQueueWaitIdle)
			functionLoadFailure = true;
		vkDeviceWaitIdle = reinterpret_cast<PFN_vkDeviceWaitIdle>(loadFunc("vkDeviceWaitIdle"));
		if (!vkDeviceWaitIdle)
			functionLoadFailure = true;
		vkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(loadFunc("vkAllocateMemory"));
		if (!vkAllocateMemory)
			functionLoadFailure = true;
		vkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(loadFunc("vkFreeMemory"));
		if (!vkFreeMemory)
			functionLoadFailure = true;
		vkMapMemory = reinterpret_cast<PFN_vkMapMemory>(loadFunc("vkMapMemory"));
		if (!vkMapMemory)
			functionLoadFailure = true;
		vkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(loadFunc("vkUnmapMemory"));
		if (!vkUnmapMemory)
			functionLoadFailure = true;
		vkFlushMappedMemoryRanges = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(loadFunc("vkFlushMappedMemoryRanges"));
		if (!vkFlushMappedMemoryRanges)
			functionLoadFailure = true;
		vkInvalidateMappedMemoryRanges = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(loadFunc("vkInvalidateMappedMemoryRanges"));
		if (!vkInvalidateMappedMemoryRanges)
			functionLoadFailure = true;
		vkGetDeviceMemoryCommitment = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(loadFunc("vkGetDeviceMemoryCommitment"));
		if (!vkGetDeviceMemoryCommitment)
			functionLoadFailure = true;
		vkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(loadFunc("vkBindBufferMemory"));
		if (!vkBindBufferMemory)
			functionLoadFailure = true;
		vkBindImageMemory = reinterpret_cast<PFN_vkBindImageMemory>(loadFunc("vkBindImageMemory"));
		if (!vkBindImageMemory)
			functionLoadFailure = true;
		vkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(loadFunc("vkGetBufferMemoryRequirements"));
		if (!vkGetBufferMemoryRequirements)
			functionLoadFailure = true;
		vkGetImageMemoryRequirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(loadFunc("vkGetImageMemoryRequirements"));
		if (!vkGetImageMemoryRequirements)
			functionLoadFailure = true;
		vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(loadFunc("vkGetImageSparseMemoryRequirements"));
		if (!vkGetImageSparseMemoryRequirements)
			functionLoadFailure = true;
		vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties>(loadFunc("vkGetPhysicalDeviceSparseImageFormatProperties"));
		if (!vkGetPhysicalDeviceSparseImageFormatProperties)
			functionLoadFailure = true;
		vkQueueBindSparse = reinterpret_cast<PFN_vkQueueBindSparse>(loadFunc("vkQueueBindSparse"));
		if (!vkQueueBindSparse)
			functionLoadFailure = true;
		vkCreateFence = reinterpret_cast<PFN_vkCreateFence>(loadFunc("vkCreateFence"));
		if (!vkCreateFence)
			functionLoadFailure = true;
		vkDestroyFence = reinterpret_cast<PFN_vkDestroyFence>(loadFunc("vkDestroyFence"));
		if (!vkDestroyFence)
			functionLoadFailure = true;
		vkResetFences = reinterpret_cast<PFN_vkResetFences>(loadFunc("vkResetFences"));
		if (!vkResetFences)
			functionLoadFailure = true;
		vkGetFenceStatus = reinterpret_cast<PFN_vkGetFenceStatus>(loadFunc("vkGetFenceStatus"));
		if (!vkGetFenceStatus)
			functionLoadFailure = true;
		vkWaitForFences = reinterpret_cast<PFN_vkWaitForFences>(loadFunc("vkWaitForFences"));
		if (!vkWaitForFences)
			functionLoadFailure = true;
		vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(loadFunc("vkCreateSemaphore"));
		if (!vkCreateSemaphore)
			functionLoadFailure = true;
		vkDestroySemaphore = reinterpret_cast<PFN_vkDestroySemaphore>(loadFunc("vkDestroySemaphore"));
		if (!vkDestroySemaphore)
			functionLoadFailure = true;
		vkCreateEvent = reinterpret_cast<PFN_vkCreateEvent>(loadFunc("vkCreateEvent"));
		if (!vkCreateEvent)
			functionLoadFailure = true;
		vkDestroyEvent = reinterpret_cast<PFN_vkDestroyEvent>(loadFunc("vkDestroyEvent"));
		if (!vkDestroyEvent)
			functionLoadFailure = true;
		vkGetEventStatus = reinterpret_cast<PFN_vkGetEventStatus>(loadFunc("vkGetEventStatus"));
		if (!vkGetEventStatus)
			functionLoadFailure = true;
		vkSetEvent = reinterpret_cast<PFN_vkSetEvent>(loadFunc("vkSetEvent"));
		if (!vkSetEvent)
			functionLoadFailure = true;
		vkResetEvent = reinterpret_cast<PFN_vkResetEvent>(loadFunc("vkResetEvent"));
		if (!vkResetEvent)
			functionLoadFailure = true;
		vkCreateQueryPool = reinterpret_cast<PFN_vkCreateQueryPool>(loadFunc("vkCreateQueryPool"));
		if (!vkCreateQueryPool)
			functionLoadFailure = true;
		vkDestroyQueryPool = reinterpret_cast<PFN_vkDestroyQueryPool>(loadFunc("vkDestroyQueryPool"));
		if (!vkDestroyQueryPool)
			functionLoadFailure = true;
		vkGetQueryPoolResults = reinterpret_cast<PFN_vkGetQueryPoolResults>(loadFunc("vkGetQueryPoolResults"));
		if (!vkGetQueryPoolResults)
			functionLoadFailure = true;
		vkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(loadFunc("vkCreateBuffer"));
		if (!vkCreateBuffer)
			functionLoadFailure = true;
		vkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(loadFunc("vkDestroyBuffer"));
		if (!vkDestroyBuffer)
			functionLoadFailure = true;
		vkCreateBufferView = reinterpret_cast<PFN_vkCreateBufferView>(loadFunc("vkCreateBufferView"));
		if (!vkCreateBufferView)
			functionLoadFailure = true;
		vkDestroyBufferView = reinterpret_cast<PFN_vkDestroyBufferView>(loadFunc("vkDestroyBufferView"));
		if (!vkDestroyBufferView)
			functionLoadFailure = true;
		vkCreateImage = reinterpret_cast<PFN_vkCreateImage>(loadFunc("vkCreateImage"));
		if (!vkCreateImage)
			functionLoadFailure = true;
		vkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(loadFunc("vkDestroyImage"));
		if (!vkDestroyImage)
			functionLoadFailure = true;
		vkGetImageSubresourceLayout = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(loadFunc("vkGetImageSubresourceLayout"));
		if (!vkGetImageSubresourceLayout)
			functionLoadFailure = true;
		vkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(loadFunc("vkCreateImageView"));
		if (!vkCreateImageView)
			functionLoadFailure = true;
		vkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(loadFunc("vkDestroyImageView"));
		if (!vkDestroyImageView)
			functionLoadFailure = true;
		vkCreateShaderModule = reinterpret_cast<PFN_vkCreateShaderModule>(loadFunc("vkCreateShaderModule"));
		if (!vkCreateShaderModule)
			functionLoadFailure = true;
		vkDestroyShaderModule = reinterpret_cast<PFN_vkDestroyShaderModule>(loadFunc("vkDestroyShaderModule"));
		if (!vkDestroyShaderModule)
			functionLoadFailure = true;
		vkCreatePipelineCache = reinterpret_cast<PFN_vkCreatePipelineCache>(loadFunc("vkCreatePipelineCache"));
		if (!vkCreatePipelineCache)
			functionLoadFailure = true;
		vkDestroyPipelineCache = reinterpret_cast<PFN_vkDestroyPipelineCache>(loadFunc("vkDestroyPipelineCache"));
		if (!vkDestroyPipelineCache)
			functionLoadFailure = true;
		vkGetPipelineCacheData = reinterpret_cast<PFN_vkGetPipelineCacheData>(loadFunc("vkGetPipelineCacheData"));
		if (!vkGetPipelineCacheData)
			functionLoadFailure = true;
		vkMergePipelineCaches = reinterpret_cast<PFN_vkMergePipelineCaches>(loadFunc("vkMergePipelineCaches"));
		if (!vkMergePipelineCaches)
			functionLoadFailure = true;
		vkCreateGraphicsPipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(loadFunc("vkCreateGraphicsPipelines"));
		if (!vkCreateGraphicsPipelines)
			functionLoadFailure = true;
		vkCreateComputePipelines = reinterpret_cast<PFN_vkCreateComputePipelines>(loadFunc("vkCreateComputePipelines"));
		if (!vkCreateComputePipelines)
			functionLoadFailure = true;
		vkDestroyPipeline = reinterpret_cast<PFN_vkDestroyPipeline>(loadFunc("vkDestroyPipeline"));
		if (!vkDestroyPipeline)
			functionLoadFailure = true;
		vkCreatePipelineLayout = reinterpret_cast<PFN_vkCreatePipelineLayout>(loadFunc("vkCreatePipelineLayout"));
		if (!vkCreatePipelineLayout)
			functionLoadFailure = true;
		vkDestroyPipelineLayout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(loadFunc("vkDestroyPipelineLayout"));
		if (!vkDestroyPipelineLayout)
			functionLoadFailure = true;
		vkCreateSampler = reinterpret_cast<PFN_vkCreateSampler>(loadFunc("vkCreateSampler"));
		if (!vkCreateSampler)
			functionLoadFailure = true;
		vkDestroySampler = reinterpret_cast<PFN_vkDestroySampler>(loadFunc("vkDestroySampler"));
		if (!vkDestroySampler)
			functionLoadFailure = true;
		vkCreateDescriptorSetLayout = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(loadFunc("vkCreateDescriptorSetLayout"));
		if (!vkCreateDescriptorSetLayout)
			functionLoadFailure = true;
		vkDestroyDescriptorSetLayout = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(loadFunc("vkDestroyDescriptorSetLayout"));
		if (!vkDestroyDescriptorSetLayout)
			functionLoadFailure = true;
		vkCreateDescriptorPool = reinterpret_cast<PFN_vkCreateDescriptorPool>(loadFunc("vkCreateDescriptorPool"));
		if (!vkCreateDescriptorPool)
			functionLoadFailure = true;
		vkDestroyDescriptorPool = reinterpret_cast<PFN_vkDestroyDescriptorPool>(loadFunc("vkDestroyDescriptorPool"));
		if (!vkDestroyDescriptorPool)
			functionLoadFailure = true;
		vkResetDescriptorPool = reinterpret_cast<PFN_vkResetDescriptorPool>(loadFunc("vkResetDescriptorPool"));
		if (!vkResetDescriptorPool)
			functionLoadFailure = true;
		vkAllocateDescriptorSets = reinterpret_cast<PFN_vkAllocateDescriptorSets>(loadFunc("vkAllocateDescriptorSets"));
		if (!vkAllocateDescriptorSets)
			functionLoadFailure = true;
		vkFreeDescriptorSets = reinterpret_cast<PFN_vkFreeDescriptorSets>(loadFunc("vkFreeDescriptorSets"));
		if (!vkFreeDescriptorSets)
			functionLoadFailure = true;
		vkUpdateDescriptorSets = reinterpret_cast<PFN_vkUpdateDescriptorSets>(loadFunc("vkUpdateDescriptorSets"));
		if (!vkUpdateDescriptorSets)
			functionLoadFailure = true;
		vkCreateFramebuffer = reinterpret_cast<PFN_vkCreateFramebuffer>(loadFunc("vkCreateFramebuffer"));
		if (!vkCreateFramebuffer)
			functionLoadFailure = true;
		vkDestroyFramebuffer = reinterpret_cast<PFN_vkDestroyFramebuffer>(loadFunc("vkDestroyFramebuffer"));
		if (!vkDestroyFramebuffer)
			functionLoadFailure = true;
		vkCreateRenderPass = reinterpret_cast<PFN_vkCreateRenderPass>(loadFunc("vkCreateRenderPass"));
		if (!vkCreateRenderPass)
			functionLoadFailure = true;
		vkDestroyRenderPass = reinterpret_cast<PFN_vkDestroyRenderPass>(loadFunc("vkDestroyRenderPass"));
		if (!vkDestroyRenderPass)
			functionLoadFailure = true;
		vkGetRenderAreaGranularity = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(loadFunc("vkGetRenderAreaGranularity"));
		if (!vkGetRenderAreaGranularity)
			functionLoadFailure = true;
		vkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(loadFunc("vkCreateCommandPool"));
		if (!vkCreateCommandPool)
			functionLoadFailure = true;
		vkDestroyCommandPool = reinterpret_cast<PFN_vkDestroyCommandPool>(loadFunc("vkDestroyCommandPool"));
		if (!vkDestroyCommandPool)
			functionLoadFailure = true;
		vkResetCommandPool = reinterpret_cast<PFN_vkResetCommandPool>(loadFunc("vkResetCommandPool"));
		if (!vkResetCommandPool)
			functionLoadFailure = true;
		vkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(loadFunc("vkAllocateCommandBuffers"));
		if (!vkAllocateCommandBuffers)
			functionLoadFailure = true;
		vkFreeCommandBuffers = reinterpret_cast<PFN_vkFreeCommandBuffers>(loadFunc("vkFreeCommandBuffers"));
		if (!vkFreeCommandBuffers)
			functionLoadFailure = true;
		vkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(loadFunc("vkBeginCommandBuffer"));
		if (!vkBeginCommandBuffer)
			functionLoadFailure = true;
		vkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(loadFunc("vkEndCommandBuffer"));
		if (!vkEndCommandBuffer)
			functionLoadFailure = true;
		vkResetCommandBuffer = reinterpret_cast<PFN_vkResetCommandBuffer>(loadFunc("vkResetCommandBuffer"));
		if (!vkResetCommandBuffer)
			functionLoadFailure = true;
		vkCmdBindPipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(loadFunc("vkCmdBindPipeline"));
		if (!vkCmdBindPipeline)
			functionLoadFailure = true;
		vkCmdSetViewport = reinterpret_cast<PFN_vkCmdSetViewport>(loadFunc("vkCmdSetViewport"));
		if (!vkCmdSetViewport)
			functionLoadFailure = true;
		vkCmdSetScissor = reinterpret_cast<PFN_vkCmdSetScissor>(loadFunc("vkCmdSetScissor"));
		if (!vkCmdSetScissor)
			functionLoadFailure = true;
		vkCmdSetLineWidth = reinterpret_cast<PFN_vkCmdSetLineWidth>(loadFunc("vkCmdSetLineWidth"));
		if (!vkCmdSetLineWidth)
			functionLoadFailure = true;
		vkCmdSetDepthBias = reinterpret_cast<PFN_vkCmdSetDepthBias>(loadFunc("vkCmdSetDepthBias"));
		if (!vkCmdSetDepthBias)
			functionLoadFailure = true;
		vkCmdSetBlendConstants = reinterpret_cast<PFN_vkCmdSetBlendConstants>(loadFunc("vkCmdSetBlendConstants"));
		if (!vkCmdSetBlendConstants)
			functionLoadFailure = true;
		vkCmdSetDepthBounds = reinterpret_cast<PFN_vkCmdSetDepthBounds>(loadFunc("vkCmdSetDepthBounds"));
		if (!vkCmdSetDepthBounds)
			functionLoadFailure = true;
		vkCmdSetStencilCompareMask = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(loadFunc("vkCmdSetStencilCompareMask"));
		if (!vkCmdSetStencilCompareMask)
			functionLoadFailure = true;
		vkCmdSetStencilWriteMask = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(loadFunc("vkCmdSetStencilWriteMask"));
		if (!vkCmdSetStencilWriteMask)
			functionLoadFailure = true;
		vkCmdSetStencilReference = reinterpret_cast<PFN_vkCmdSetStencilReference>(loadFunc("vkCmdSetStencilReference"));
		if (!vkCmdSetStencilReference)
			functionLoadFailure = true;
		vkCmdBindDescriptorSets = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(loadFunc("vkCmdBindDescriptorSets"));
		if (!vkCmdBindDescriptorSets)
			functionLoadFailure = true;
		vkCmdBindIndexBuffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(loadFunc("vkCmdBindIndexBuffer"));
		if (!vkCmdBindIndexBuffer)
			functionLoadFailure = true;
		vkCmdBindVertexBuffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(loadFunc("vkCmdBindVertexBuffers"));
		if (!vkCmdBindVertexBuffers)
			functionLoadFailure = true;
		vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(loadFunc("vkCmdDraw"));
		if (!vkCmdDraw)
			functionLoadFailure = true;
		vkCmdDrawIndexed = reinterpret_cast<PFN_vkCmdDrawIndexed>(loadFunc("vkCmdDrawIndexed"));
		if (!vkCmdDrawIndexed)
			functionLoadFailure = true;
		vkCmdDrawIndirect = reinterpret_cast<PFN_vkCmdDrawIndirect>(loadFunc("vkCmdDrawIndirect"));
		if (!vkCmdDrawIndirect)
			functionLoadFailure = true;
		vkCmdDrawIndexedIndirect = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(loadFunc("vkCmdDrawIndexedIndirect"));
		if (!vkCmdDrawIndexedIndirect)
			functionLoadFailure = true;
		vkCmdDispatch = reinterpret_cast<PFN_vkCmdDispatch>(loadFunc("vkCmdDispatch"));
		if (!vkCmdDispatch)
			functionLoadFailure = true;
		vkCmdDispatchIndirect = reinterpret_cast<PFN_vkCmdDispatchIndirect>(loadFunc("vkCmdDispatchIndirect"));
		if (!vkCmdDispatchIndirect)
			functionLoadFailure = true;
		vkCmdCopyBuffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(loadFunc("vkCmdCopyBuffer"));
		if (!vkCmdCopyBuffer)
			functionLoadFailure = true;
		vkCmdCopyImage = reinterpret_cast<PFN_vkCmdCopyImage>(loadFunc("vkCmdCopyImage"));
		if (!vkCmdCopyImage)
			functionLoadFailure = true;
		vkCmdBlitImage = reinterpret_cast<PFN_vkCmdBlitImage>(loadFunc("vkCmdBlitImage"));
		if (!vkCmdBlitImage)
			functionLoadFailure = true;
		vkCmdCopyBufferToImage = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(loadFunc("vkCmdCopyBufferToImage"));
		if (!vkCmdCopyBufferToImage)
			functionLoadFailure = true;
		vkCmdCopyImageToBuffer = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(loadFunc("vkCmdCopyImageToBuffer"));
		if (!vkCmdCopyImageToBuffer)
			functionLoadFailure = true;
		vkCmdUpdateBuffer = reinterpret_cast<PFN_vkCmdUpdateBuffer>(loadFunc("vkCmdUpdateBuffer"));
		if (!vkCmdUpdateBuffer)
			functionLoadFailure = true;
		vkCmdFillBuffer = reinterpret_cast<PFN_vkCmdFillBuffer>(loadFunc("vkCmdFillBuffer"));
		if (!vkCmdFillBuffer)
			functionLoadFailure = true;
		vkCmdClearColorImage = reinterpret_cast<PFN_vkCmdClearColorImage>(loadFunc("vkCmdClearColorImage"));
		if (!vkCmdClearColorImage)
			functionLoadFailure = true;
		vkCmdClearDepthStencilImage = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(loadFunc("vkCmdClearDepthStencilImage"));
		if (!vkCmdClearDepthStencilImage)
			functionLoadFailure = true;
		vkCmdClearAttachments = reinterpret_cast<PFN_vkCmdClearAttachments>(loadFunc("vkCmdClearAttachments"));
		if (!vkCmdClearAttachments)
			functionLoadFailure = true;
		vkCmdResolveImage = reinterpret_cast<PFN_vkCmdResolveImage>(loadFunc("vkCmdResolveImage"));
		if (!vkCmdResolveImage)
			functionLoadFailure = true;
		vkCmdSetEvent = reinterpret_cast<PFN_vkCmdSetEvent>(loadFunc("vkCmdSetEvent"));
		if (!vkCmdSetEvent)
			functionLoadFailure = true;
		vkCmdResetEvent = reinterpret_cast<PFN_vkCmdResetEvent>(loadFunc("vkCmdResetEvent"));
		if (!vkCmdResetEvent)
			functionLoadFailure = true;
		vkCmdWaitEvents = reinterpret_cast<PFN_vkCmdWaitEvents>(loadFunc("vkCmdWaitEvents"));
		if (!vkCmdWaitEvents)
			functionLoadFailure = true;
		vkCmdPipelineBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(loadFunc("vkCmdPipelineBarrier"));
		if (!vkCmdPipelineBarrier)
			functionLoadFailure = true;
		vkCmdBeginQuery = reinterpret_cast<PFN_vkCmdBeginQuery>(loadFunc("vkCmdBeginQuery"));
		if (!vkCmdBeginQuery)
			functionLoadFailure = true;
		vkCmdEndQuery = reinterpret_cast<PFN_vkCmdEndQuery>(loadFunc("vkCmdEndQuery"));
		if (!vkCmdEndQuery)
			functionLoadFailure = true;
		vkCmdResetQueryPool = reinterpret_cast<PFN_vkCmdResetQueryPool>(loadFunc("vkCmdResetQueryPool"));
		if (!vkCmdResetQueryPool)
			functionLoadFailure = true;
		vkCmdWriteTimestamp = reinterpret_cast<PFN_vkCmdWriteTimestamp>(loadFunc("vkCmdWriteTimestamp"));
		if (!vkCmdWriteTimestamp)
			functionLoadFailure = true;
		vkCmdCopyQueryPoolResults = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(loadFunc("vkCmdCopyQueryPoolResults"));
		if (!vkCmdCopyQueryPoolResults)
			functionLoadFailure = true;
		vkCmdPushConstants = reinterpret_cast<PFN_vkCmdPushConstants>(loadFunc("vkCmdPushConstants"));
		if (!vkCmdPushConstants)
			functionLoadFailure = true;
		vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(loadFunc("vkCmdBeginRenderPass"));
		if (!vkCmdBeginRenderPass)
			functionLoadFailure = true;
		vkCmdNextSubpass = reinterpret_cast<PFN_vkCmdNextSubpass>(loadFunc("vkCmdNextSubpass"));
		if (!vkCmdNextSubpass)
			functionLoadFailure = true;
		vkCmdEndRenderPass = reinterpret_cast<PFN_vkCmdEndRenderPass>(loadFunc("vkCmdEndRenderPass"));
		if (!vkCmdEndRenderPass)
			functionLoadFailure = true;
		vkCmdExecuteCommands = reinterpret_cast<PFN_vkCmdExecuteCommands>(loadFunc("vkCmdExecuteCommands"));
		if (!vkCmdExecuteCommands)
			functionLoadFailure = true;
		vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(loadFunc("vkDestroySurfaceKHR"));
		if (!vkDestroySurfaceKHR)
			functionLoadFailure = true;
		vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(loadFunc("vkGetPhysicalDeviceSurfaceSupportKHR"));
		if (!vkGetPhysicalDeviceSurfaceSupportKHR)
			functionLoadFailure = true;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(loadFunc("vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
		if (!vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
			functionLoadFailure = true;
		vkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(loadFunc("vkGetPhysicalDeviceSurfaceFormatsKHR"));
		if (!vkGetPhysicalDeviceSurfaceFormatsKHR)
			functionLoadFailure = true;
		vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(loadFunc("vkGetPhysicalDeviceSurfacePresentModesKHR"));
		if (!vkGetPhysicalDeviceSurfacePresentModesKHR)
			functionLoadFailure = true;
		vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(loadFunc("vkCreateDebugUtilsMessengerEXT"));
		if (!vkCreateDebugUtilsMessengerEXT)
			functionLoadFailure = true;
		vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(loadFunc("vkDestroyDebugUtilsMessengerEXT"));
		if (!vkDestroyDebugUtilsMessengerEXT)
			functionLoadFailure = true;
		vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(loadFunc("vkCreateSwapchainKHR"));
		if (!vkCreateSwapchainKHR)
			functionLoadFailure = true;
		vkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(loadFunc("vkDestroySwapchainKHR"));
		if (!vkDestroySwapchainKHR)
			functionLoadFailure = true;
		vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(loadFunc("vkGetSwapchainImagesKHR"));
		if (!vkGetSwapchainImagesKHR)
			functionLoadFailure = true;
		if (functionLoadFailure)
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

	bool Vulkan::createVulkanInstance(std::vector<const char*>& nameExt) {
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
		createInstInfo.enabledLayerCount = 1;
		const char** validationLayerName = new const char*[1] {"VK_LAYER_KHRONOS_validation"};
		createInstInfo.ppEnabledLayerNames = validationLayerName;
		nameExt.push_back("VK_EXT_debug_utils");
		createInstInfo.enabledExtensionCount = nameExt.size();
		createInstInfo.ppEnabledExtensionNames = nameExt.data();
		if (vkCreateInstance(&createInstInfo, nullptr, &vkInstance) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating a Vulkan instance");
			return false;
		}
		delete[] validationLayerName;
		setBit<REubyte>(validation, _VK_INST, true);
		return true;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgType, const VkDebugUtilsMessengerCallbackDataEXT* data, void* userData) {
		if (msgSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			RE_ERROR("Vulkan validation layer triggered an error (may result a crash)");
		else
			RE_WARNING("Vulkan validation layer triggered warning");
		println(data->pMessage);
		return VK_FALSE;
	}

	bool Vulkan::setupValidationLayers() {
		if (!isBitTrue<REubyte>(validation, _VK_INST))
			return false;
		if (isBitTrue<REubyte>(validation, _VK_VALI))
			return true;
		VkDebugUtilsMessengerCreateInfoEXT debugMsgCreate = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		debugMsgCreate.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugMsgCreate.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		debugMsgCreate.pfnUserCallback = debugCallback;
		debugMsgCreate.pUserData = nullptr;
		if (vkCreateDebugUtilsMessengerEXT(vkInstance, &debugMsgCreate, nullptr, &vkDebugMessenger) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed setting validation layers in Vulkan up");
			return false;
		}
		setBit<REubyte>(validation, _VK_VALI, true);
		return true;
	}

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	SwapChainSupportDetails getSwapChainDetails(VkPhysicalDevice physicalDevice) {
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vkSurface, &details.surfaceCapabilities);
		REuint formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkSurface, &formatCount, nullptr);
		if (formatCount > 0) {
			details.surfaceFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkSurface, &formatCount, details.surfaceFormats.data());
		}
		REuint presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkSurface, &presentModeCount, nullptr);
		if (presentModeCount > 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkSurface, &presentModeCount, nullptr);
		}
		return details;
	}

	struct DeviceCapacities {
		std::optional<REuint> graphicsQueueIndex;
		std::optional<REuint> presentQueueIndex;
		SwapChainSupportDetails swapChainDetails;

		bool isComplete() {
			return graphicsQueueIndex.has_value() && presentQueueIndex.has_value();
		}
	};

	bool isSuitablePhysicalDevice(VkPhysicalDevice physicalDevice, DeviceCapacities& saveDevice) {
		DeviceCapacities device;

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
				device.graphicsQueueIndex = i;
			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, vkSurface, &presentSupport);
			if (presentSupport)
				device.presentQueueIndex = i;
		}
		if (!device.isComplete())
			return false;

		REuint extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
		VkExtensionProperties extensions[extensionCount];
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions);
		std::string requiredExtension("VK_KHR_swapchain");
		bool swapchainSupport = false;
		for (REuint i = 0; i < extensionCount; i++) {
			if (!requiredExtension.compare(extensions[i].extensionName)) {
				swapchainSupport = true;
				break;
			}
		}
		if (!swapchainSupport)
			return false;
		device.swapChainDetails = getSwapChainDetails(physicalDevice);
		if (device.swapChainDetails.surfaceFormats.empty() || device.swapChainDetails.presentModes.empty())
			return false;
		return true;
	}

	bool Vulkan::selectPhysicalDevice() {
		if (!isBitTrue<REubyte>(validation, _VK_SURF))
			return false;
		if (isBitTrue<REubyte>(validation, _VK_PHDV))
			return true;
		REuint physicalDevicesCount = 0;
		vkEnumeratePhysicalDevices(vkInstance, &physicalDevicesCount, nullptr);
		if (!physicalDevicesCount) {
			RE_FATAL_ERROR("No installed GPU supports Vulkan");
			return false;
		}
		VkPhysicalDevice physicalDevices[physicalDevicesCount];
		vkEnumeratePhysicalDevices(vkInstance, &physicalDevicesCount, physicalDevices);
		DeviceCapacities deviceProps[physicalDevicesCount];
		REuint foundDeviceIndex = -1;
		for (REuint i = 0; i < physicalDevicesCount; i++) {
			if (isSuitablePhysicalDevice(physicalDevices[i], deviceProps[i])) {
				foundDeviceIndex = i;
				vkPhysicalDevice = physicalDevices[i];
				break;
			}
		}
		if (foundDeviceIndex < 0) {
			RE_FATAL_ERROR("No GPU supporting Vulkan is suitable for R-Engine's rendering system");
			return false;
		}
		setBit<REubyte>(validation, _VK_PHDV, true);
		return true;
	}

	bool Vulkan::createLogicalDevice() {
		if (!isBitTrue<REubyte>(validation, _VK_PHDV))
			return false;
		if (isBitTrue<REubyte>(validation, _VK_LGDV))
			return true;
		REuint queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
		VkQueueFamilyProperties queueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties);
		for (REuint i = 0; i < queueFamilyCount; i++) {
			if (queueFamilyProperties[i].queueCount == 0)
				break;
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				graphicsQueue.familyIndex = i;
			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vkSurface, &presentSupport);
			if (presentSupport == VK_TRUE)
				presentQueue.familyIndex = i;
		}
		float priority = 1.0f;
		VkDeviceQueueCreateInfo queueCreateInfos[2] = {{}, {}};
		for (REuint i = 0; i < (sizeof(queueCreateInfos) / sizeof(VkDeviceQueueCreateInfo)); i++) {
			REuint index = 0;
			switch (i) {
				case 0:
					index = graphicsQueue.familyIndex;
					break;
				case 1:
					index = presentQueue.familyIndex;
					break;
			}
			queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[i].queueFamilyIndex = index;
			queueCreateInfos[i].queueCount = 1;
			queueCreateInfos[i].pQueuePriorities = &priority;
		}
		VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
		deviceCreateInfo.queueCreateInfoCount = 2;
		VkPhysicalDeviceFeatures enableFeatures = {};
		deviceCreateInfo.pEnabledFeatures = &enableFeatures;
		deviceCreateInfo.enabledExtensionCount = 1;
		const char* deviceExtensions[] = {"VK_KHR_swapchain"};
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
		if (vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating logical vulkan device");
			return false;
		}
		vkGetDeviceQueue(vkDevice, graphicsQueue.familyIndex, 0, &graphicsQueue.vkQueue);
		vkGetDeviceQueue(vkDevice, presentQueue.familyIndex, 0, &presentQueue.vkQueue);
		setBit<REubyte>(validation, _VK_LGDV, true);
		return true;
	}

	bool Vulkan::createSwapChain(Vector<REushort, 2>& winSize) {
		if (!isBitTrue<REubyte>(validation, _VK_LGDV))
			return false;
		if (isBitTrue<REubyte>(validation, _VK_SWAP))
			return true;
		SwapChainSupportDetails swapChainDetails = getSwapChainDetails(vkPhysicalDevice);

		VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (VkPresentModeKHR mode : swapChainDetails.presentModes)
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				selectedPresentMode = mode;
				break;
			}

		VkSurfaceFormatKHR selectedSurfaceFormat = swapChainDetails.surfaceFormats[0];
		for (VkSurfaceFormatKHR availableFormat : swapChainDetails.surfaceFormats)
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				selectedSurfaceFormat = availableFormat;
				break;
			}

		VkExtent2D swapSize = {0, 0};
		if (swapChainDetails.surfaceCapabilities.currentExtent.width != std::numeric_limits<REuint>::max() && swapChainDetails.surfaceCapabilities.currentExtent.height != std::numeric_limits<REuint>::max())
			swapSize = swapChainDetails.surfaceCapabilities.currentExtent;
		else {
			swapSize = {static_cast<REuint>(winSize[0]), static_cast<REuint>(winSize[1])};
			swapSize.width = std::clamp(swapSize.width, swapChainDetails.surfaceCapabilities.minImageExtent.width, swapChainDetails.surfaceCapabilities.maxImageExtent.width);
			swapSize.height = std::clamp(swapSize.height, swapChainDetails.surfaceCapabilities.minImageExtent.height, swapChainDetails.surfaceCapabilities.maxImageExtent.height);
		}

		REuint imageCount = swapChainDetails.surfaceCapabilities.minImageCount + 1;
		if (swapChainDetails.surfaceCapabilities.maxImageCount > 0 && imageCount > swapChainDetails.surfaceCapabilities.maxImageCount)
			imageCount = swapChainDetails.surfaceCapabilities.maxImageCount;
		VkSwapchainCreateInfoKHR swapchainCreate = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		swapchainCreate.surface = vkSurface;
		swapchainCreate.minImageCount = imageCount;
		swapchainCreate.imageFormat = selectedSurfaceFormat.format;
		swapchainCreate.imageColorSpace = selectedSurfaceFormat.colorSpace;
		swapchainCreate.imageExtent = swapSize;
		swapchainCreate.imageArrayLayers = 1;
		swapchainCreate.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		REuint queueFamilyIndices[2] = {graphicsQueue.familyIndex, presentQueue.familyIndex};
		if (graphicsQueue.familyIndex != presentQueue.familyIndex) {
			swapchainCreate.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreate.queueFamilyIndexCount = 2;
			swapchainCreate.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			swapchainCreate.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreate.queueFamilyIndexCount = 0;
			swapchainCreate.pQueueFamilyIndices = nullptr;
		}
		swapchainCreate.preTransform = swapChainDetails.surfaceCapabilities.currentTransform;
		swapchainCreate.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreate.presentMode = selectedPresentMode;
		swapchainCreate.clipped = VK_TRUE;
		swapchainCreate.oldSwapchain = VK_NULL_HANDLE;
		if (vkCreateSwapchainKHR(vkDevice, &swapchainCreate, nullptr, &vkSwapchain) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating a swapchain in Vulkan");
			return false;
		}
		setBit<REubyte>(validation, _VK_SWAP, true);

		std::vector<VkImage> swapChainImages;
		REuint swapChainImgCount = 0;
		vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &swapChainImgCount, nullptr);
		swapChainImages.resize(swapChainImgCount);
		vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &swapChainImgCount, swapChainImages.data());
		vkSwapChainImageViews.resize(swapChainImages.size());
		for (REuint i = 0; i < swapChainImgCount; i++) {
			VkImageViewCreateInfo imgViewCreate = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			imgViewCreate.image = swapChainImages[i];
			imgViewCreate.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imgViewCreate.format = selectedSurfaceFormat.format;
			imgViewCreate.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreate.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreate.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreate.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imgViewCreate.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imgViewCreate.subresourceRange.baseMipLevel = 0;
			imgViewCreate.subresourceRange.levelCount = 1;
			imgViewCreate.subresourceRange.baseArrayLayer = 0;
			imgViewCreate.subresourceRange.layerCount = 1;
			if (vkCreateImageView(vkDevice, &imgViewCreate, nullptr, &vkSwapChainImageViews[i]) != VK_SUCCESS) {
				RE_FATAL_ERROR("Failed creating a swapchain image view");
				return false;
			}
		}
		setBit<REubyte>(validation, _VK_SIMG, true);
		return true;
	}

	bool Vulkan::initVulkan(std::vector<const char*>& nameExt, Vector<REushort, 2>& winSize) {
		if (!createVulkanInstance(nameExt))
			return false;
		if (!loadAllFunc())
			return false;
		if (!setupValidationLayers())
			return false;
		if (!createSurface())
			return false;
		setBit<REubyte>(validation, _VK_SURF, true);
		if (!selectPhysicalDevice())
			return false;
		if (!createLogicalDevice())
			return false;
		if (!createSwapChain(winSize))
			return false;
		return true;
	}

	void Vulkan::destroyVulkan() {
		if (isBitTrue<REubyte>(validation, _VK_INST)) {
			if (isBitTrue<REubyte>(validation, _VK_VALI)) {
				if (isBitTrue<REubyte>(validation, _VK_SURF)) {
					if (isBitTrue<REubyte>(validation, _VK_LGDV)) {
						if (isBitTrue<REubyte>(validation, _VK_SWAP)) {
							if (isBitTrue<REubyte>(validation, _VK_SIMG)) {
								for (VkImageView img : vkSwapChainImageViews)
									vkDestroyImageView(vkDevice, img, nullptr);
							}
							vkDestroySwapchainKHR(vkDevice, vkSwapchain, nullptr);
						}
						vkDestroyDevice(vkDevice, nullptr);
					}
					vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
				}
				vkDestroyDebugUtilsMessengerEXT(vkInstance, vkDebugMessenger, nullptr);
			}
			vkDestroyInstance(vkInstance, nullptr);
		}
	}

	bool Vulkan::isValid() {
		return areBitsTrueRange<REubyte>(validation, 0, _VK_LAST);
	}

}
