#include "RE_Vulkan.hpp"

namespace RE {

	Vulkan* Vulkan::instance = nullptr;

	bool Vulkan::loadVulkan_1_0() {
		// Skipped initialization of "pfn_vkCreateInstance", because it's already loaded
		pfn_vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(loadFunc("vkDestroyInstance"));
		if (!pfn_vkDestroyInstance)
			return false;
		pfn_vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(loadFunc("vkEnumeratePhysicalDevices"));
		if (!pfn_vkEnumeratePhysicalDevices)
			return false;
		pfn_vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(loadFunc("vkGetPhysicalDeviceFeatures"));
		if (!pfn_vkGetPhysicalDeviceFeatures)
			return false;
		pfn_vkGetPhysicalDeviceFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(loadFunc("vkGetPhysicalDeviceFormatProperties"));
		if (!pfn_vkGetPhysicalDeviceFormatProperties)
			return false;
		pfn_vkGetPhysicalDeviceImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties>(loadFunc("vkGetPhysicalDeviceImageFormatProperties"));
		if (!pfn_vkGetPhysicalDeviceImageFormatProperties)
			return false;
		pfn_vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(loadFunc("vkGetPhysicalDeviceProperties"));
		if (!pfn_vkGetPhysicalDeviceProperties)
			return false;
		pfn_vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(loadFunc("vkGetPhysicalDeviceQueueFamilyProperties"));
		if (!pfn_vkGetPhysicalDeviceQueueFamilyProperties)
			return false;
		pfn_vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(loadFunc("vkGetPhysicalDeviceMemoryProperties"));
		if (!pfn_vkGetPhysicalDeviceMemoryProperties)
			return false;
		pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(loadFunc("vkGetInstanceProcAddr"));
		if (!pfn_vkGetInstanceProcAddr)
			return false;
		pfn_vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(loadFunc("vkGetDeviceProcAddr"));
		if (!pfn_vkGetDeviceProcAddr)
			return false;
		pfn_vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(loadFunc("vkCreateDevice"));
		if (!pfn_vkCreateDevice)
			return false;
		pfn_vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(loadFunc("vkDestroyDevice"));
		if (!pfn_vkDestroyDevice)
			return false;
		// Skipped initialization of "pfn_vkEnumerateInstanceExtensionProperties", because it's already loaded
		pfn_vkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(loadFunc("vkEnumerateDeviceExtensionProperties"));
		if (!pfn_vkEnumerateDeviceExtensionProperties)
			return false;
		// Skipped initialization of "pfn_vkEnumerateInstanceLayerProperties", because it's already loaded
		pfn_vkEnumerateDeviceLayerProperties = reinterpret_cast<PFN_vkEnumerateDeviceLayerProperties>(loadFunc("vkEnumerateDeviceLayerProperties"));
		if (!pfn_vkEnumerateDeviceLayerProperties)
			return false;
		pfn_vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(loadFunc("vkGetDeviceQueue"));
		if (!pfn_vkGetDeviceQueue)
			return false;
		pfn_vkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(loadFunc("vkQueueSubmit"));
		if (!pfn_vkQueueSubmit)
			return false;
		pfn_vkQueueWaitIdle = reinterpret_cast<PFN_vkQueueWaitIdle>(loadFunc("vkQueueWaitIdle"));
		if (!pfn_vkQueueWaitIdle)
			return false;
		pfn_vkDeviceWaitIdle = reinterpret_cast<PFN_vkDeviceWaitIdle>(loadFunc("vkDeviceWaitIdle"));
		if (!pfn_vkDeviceWaitIdle)
			return false;
		pfn_vkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(loadFunc("vkAllocateMemory"));
		if (!pfn_vkAllocateMemory)
			return false;
		pfn_vkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(loadFunc("vkFreeMemory"));
		if (!pfn_vkFreeMemory)
			return false;
		pfn_vkMapMemory = reinterpret_cast<PFN_vkMapMemory>(loadFunc("vkMapMemory"));
		if (!pfn_vkMapMemory)
			return false;
		pfn_vkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(loadFunc("vkUnmapMemory"));
		if (!pfn_vkUnmapMemory)
			return false;
		pfn_vkFlushMappedMemoryRanges = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(loadFunc("vkFlushMappedMemoryRanges"));
		if (!pfn_vkFlushMappedMemoryRanges)
			return false;
		pfn_vkInvalidateMappedMemoryRanges = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(loadFunc("vkInvalidateMappedMemoryRanges"));
		if (!pfn_vkInvalidateMappedMemoryRanges)
			return false;
		pfn_vkGetDeviceMemoryCommitment = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(loadFunc("vkGetDeviceMemoryCommitment"));
		if (!pfn_vkGetDeviceMemoryCommitment)
			return false;
		pfn_vkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(loadFunc("vkBindBufferMemory"));
		if (!pfn_vkBindBufferMemory)
			return false;
		pfn_vkBindImageMemory = reinterpret_cast<PFN_vkBindImageMemory>(loadFunc("vkBindImageMemory"));
		if (!pfn_vkBindImageMemory)
			return false;
		pfn_vkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(loadFunc("vkGetBufferMemoryRequirements"));
		if (!pfn_vkGetBufferMemoryRequirements)
			return false;
		pfn_vkGetImageMemoryRequirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(loadFunc("vkGetImageMemoryRequirements"));
		if (!pfn_vkGetImageMemoryRequirements)
			return false;
		pfn_vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(loadFunc("vkGetImageSparseMemoryRequirements"));
		if (!pfn_vkGetImageSparseMemoryRequirements)
			return false;
		pfn_vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties>(loadFunc("vkGetPhysicalDeviceSparseImageFormatProperties"));
		if (!pfn_vkGetPhysicalDeviceSparseImageFormatProperties)
			return false;
		pfn_vkQueueBindSparse = reinterpret_cast<PFN_vkQueueBindSparse>(loadFunc("vkQueueBindSparse"));
		if (!pfn_vkQueueBindSparse)
			return false;
		pfn_vkCreateFence = reinterpret_cast<PFN_vkCreateFence>(loadFunc("vkCreateFence"));
		if (!pfn_vkCreateFence)
			return false;
		pfn_vkDestroyFence = reinterpret_cast<PFN_vkDestroyFence>(loadFunc("vkDestroyFence"));
		if (!pfn_vkDestroyFence)
			return false;
		pfn_vkResetFences = reinterpret_cast<PFN_vkResetFences>(loadFunc("vkResetFences"));
		if (!pfn_vkResetFences)
			return false;
		pfn_vkGetFenceStatus = reinterpret_cast<PFN_vkGetFenceStatus>(loadFunc("vkGetFenceStatus"));
		if (!pfn_vkGetFenceStatus)
			return false;
		pfn_vkWaitForFences = reinterpret_cast<PFN_vkWaitForFences>(loadFunc("vkWaitForFences"));
		if (!pfn_vkWaitForFences)
			return false;
		pfn_vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(loadFunc("vkCreateSemaphore"));
		if (!pfn_vkCreateSemaphore)
			return false;
		pfn_vkDestroySemaphore = reinterpret_cast<PFN_vkDestroySemaphore>(loadFunc("vkDestroySemaphore"));
		if (!pfn_vkDestroySemaphore)
			return false;
		pfn_vkCreateEvent = reinterpret_cast<PFN_vkCreateEvent>(loadFunc("vkCreateEvent"));
		if (!pfn_vkCreateEvent)
			return false;
		pfn_vkDestroyEvent = reinterpret_cast<PFN_vkDestroyEvent>(loadFunc("vkDestroyEvent"));
		if (!pfn_vkDestroyEvent)
			return false;
		pfn_vkGetEventStatus = reinterpret_cast<PFN_vkGetEventStatus>(loadFunc("vkGetEventStatus"));
		if (!pfn_vkGetEventStatus)
			return false;
		pfn_vkSetEvent = reinterpret_cast<PFN_vkSetEvent>(loadFunc("vkSetEvent"));
		if (!pfn_vkSetEvent)
			return false;
		pfn_vkResetEvent = reinterpret_cast<PFN_vkResetEvent>(loadFunc("vkResetEvent"));
		if (!pfn_vkResetEvent)
			return false;
		pfn_vkCreateQueryPool = reinterpret_cast<PFN_vkCreateQueryPool>(loadFunc("vkCreateQueryPool"));
		if (!pfn_vkCreateQueryPool)
			return false;
		pfn_vkDestroyQueryPool = reinterpret_cast<PFN_vkDestroyQueryPool>(loadFunc("vkDestroyQueryPool"));
		if (!pfn_vkDestroyQueryPool)
			return false;
		pfn_vkGetQueryPoolResults = reinterpret_cast<PFN_vkGetQueryPoolResults>(loadFunc("vkGetQueryPoolResults"));
		if (!pfn_vkGetQueryPoolResults)
			return false;
		pfn_vkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(loadFunc("vkCreateBuffer"));
		if (!pfn_vkCreateBuffer)
			return false;
		pfn_vkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(loadFunc("vkDestroyBuffer"));
		if (!pfn_vkDestroyBuffer)
			return false;
		pfn_vkCreateBufferView = reinterpret_cast<PFN_vkCreateBufferView>(loadFunc("vkCreateBufferView"));
		if (!pfn_vkCreateBufferView)
			return false;
		pfn_vkDestroyBufferView = reinterpret_cast<PFN_vkDestroyBufferView>(loadFunc("vkDestroyBufferView"));
		if (!pfn_vkDestroyBufferView)
			return false;
		pfn_vkCreateImage = reinterpret_cast<PFN_vkCreateImage>(loadFunc("vkCreateImage"));
		if (!pfn_vkCreateImage)
			return false;
		pfn_vkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(loadFunc("vkDestroyImage"));
		if (!pfn_vkDestroyImage)
			return false;
		pfn_vkGetImageSubresourceLayout = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(loadFunc("vkGetImageSubresourceLayout"));
		if (!pfn_vkGetImageSubresourceLayout)
			return false;
		pfn_vkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(loadFunc("vkCreateImageView"));
		if (!pfn_vkCreateImageView)
			return false;
		pfn_vkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(loadFunc("vkDestroyImageView"));
		if (!pfn_vkDestroyImageView)
			return false;
		pfn_vkCreateShaderModule = reinterpret_cast<PFN_vkCreateShaderModule>(loadFunc("vkCreateShaderModule"));
		if (!pfn_vkCreateShaderModule)
			return false;
		pfn_vkDestroyShaderModule = reinterpret_cast<PFN_vkDestroyShaderModule>(loadFunc("vkDestroyShaderModule"));
		if (!pfn_vkDestroyShaderModule)
			return false;
		pfn_vkCreatePipelineCache = reinterpret_cast<PFN_vkCreatePipelineCache>(loadFunc("vkCreatePipelineCache"));
		if (!pfn_vkCreatePipelineCache)
			return false;
		pfn_vkDestroyPipelineCache = reinterpret_cast<PFN_vkDestroyPipelineCache>(loadFunc("vkDestroyPipelineCache"));
		if (!pfn_vkDestroyPipelineCache)
			return false;
		pfn_vkGetPipelineCacheData = reinterpret_cast<PFN_vkGetPipelineCacheData>(loadFunc("vkGetPipelineCacheData"));
		if (!pfn_vkGetPipelineCacheData)
			return false;
		pfn_vkMergePipelineCaches = reinterpret_cast<PFN_vkMergePipelineCaches>(loadFunc("vkMergePipelineCaches"));
		if (!pfn_vkMergePipelineCaches)
			return false;
		pfn_vkCreateGraphicsPipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(loadFunc("vkCreateGraphicsPipelines"));
		if (!pfn_vkCreateGraphicsPipelines)
			return false;
		pfn_vkCreateComputePipelines = reinterpret_cast<PFN_vkCreateComputePipelines>(loadFunc("vkCreateComputePipelines"));
		if (!pfn_vkCreateComputePipelines)
			return false;
		pfn_vkDestroyPipeline = reinterpret_cast<PFN_vkDestroyPipeline>(loadFunc("vkDestroyPipeline"));
		if (!pfn_vkDestroyPipeline)
			return false;
		pfn_vkCreatePipelineLayout = reinterpret_cast<PFN_vkCreatePipelineLayout>(loadFunc("vkCreatePipelineLayout"));
		if (!pfn_vkCreatePipelineLayout)
			return false;
		pfn_vkDestroyPipelineLayout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(loadFunc("vkDestroyPipelineLayout"));
		if (!pfn_vkDestroyPipelineLayout)
			return false;
		pfn_vkCreateSampler = reinterpret_cast<PFN_vkCreateSampler>(loadFunc("vkCreateSampler"));
		if (!pfn_vkCreateSampler)
			return false;
		pfn_vkDestroySampler = reinterpret_cast<PFN_vkDestroySampler>(loadFunc("vkDestroySampler"));
		if (!pfn_vkDestroySampler)
			return false;
		pfn_vkCreateDescriptorSetLayout = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(loadFunc("vkCreateDescriptorSetLayout"));
		if (!pfn_vkCreateDescriptorSetLayout)
			return false;
		pfn_vkDestroyDescriptorSetLayout = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(loadFunc("vkDestroyDescriptorSetLayout"));
		if (!pfn_vkDestroyDescriptorSetLayout)
			return false;
		pfn_vkCreateDescriptorPool = reinterpret_cast<PFN_vkCreateDescriptorPool>(loadFunc("vkCreateDescriptorPool"));
		if (!pfn_vkCreateDescriptorPool)
			return false;
		pfn_vkDestroyDescriptorPool = reinterpret_cast<PFN_vkDestroyDescriptorPool>(loadFunc("vkDestroyDescriptorPool"));
		if (!pfn_vkDestroyDescriptorPool)
			return false;
		pfn_vkResetDescriptorPool = reinterpret_cast<PFN_vkResetDescriptorPool>(loadFunc("vkResetDescriptorPool"));
		if (!pfn_vkResetDescriptorPool)
			return false;
		pfn_vkAllocateDescriptorSets = reinterpret_cast<PFN_vkAllocateDescriptorSets>(loadFunc("vkAllocateDescriptorSets"));
		if (!pfn_vkAllocateDescriptorSets)
			return false;
		pfn_vkFreeDescriptorSets = reinterpret_cast<PFN_vkFreeDescriptorSets>(loadFunc("vkFreeDescriptorSets"));
		if (!pfn_vkFreeDescriptorSets)
			return false;
		pfn_vkUpdateDescriptorSets = reinterpret_cast<PFN_vkUpdateDescriptorSets>(loadFunc("vkUpdateDescriptorSets"));
		if (!pfn_vkUpdateDescriptorSets)
			return false;
		pfn_vkCreateFramebuffer = reinterpret_cast<PFN_vkCreateFramebuffer>(loadFunc("vkCreateFramebuffer"));
		if (!pfn_vkCreateFramebuffer)
			return false;
		pfn_vkDestroyFramebuffer = reinterpret_cast<PFN_vkDestroyFramebuffer>(loadFunc("vkDestroyFramebuffer"));
		if (!pfn_vkDestroyFramebuffer)
			return false;
		pfn_vkCreateRenderPass = reinterpret_cast<PFN_vkCreateRenderPass>(loadFunc("vkCreateRenderPass"));
		if (!pfn_vkCreateRenderPass)
			return false;
		pfn_vkDestroyRenderPass = reinterpret_cast<PFN_vkDestroyRenderPass>(loadFunc("vkDestroyRenderPass"));
		if (!pfn_vkDestroyRenderPass)
			return false;
		pfn_vkGetRenderAreaGranularity = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(loadFunc("vkGetRenderAreaGranularity"));
		if (!pfn_vkGetRenderAreaGranularity)
			return false;
		pfn_vkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(loadFunc("vkCreateCommandPool"));
		if (!pfn_vkCreateCommandPool)
			return false;
		pfn_vkDestroyCommandPool = reinterpret_cast<PFN_vkDestroyCommandPool>(loadFunc("vkDestroyCommandPool"));
		if (!pfn_vkDestroyCommandPool)
			return false;
		pfn_vkResetCommandPool = reinterpret_cast<PFN_vkResetCommandPool>(loadFunc("vkResetCommandPool"));
		if (!pfn_vkResetCommandPool)
			return false;
		pfn_vkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(loadFunc("vkAllocateCommandBuffers"));
		if (!pfn_vkAllocateCommandBuffers)
			return false;
		pfn_vkFreeCommandBuffers = reinterpret_cast<PFN_vkFreeCommandBuffers>(loadFunc("vkFreeCommandBuffers"));
		if (!pfn_vkFreeCommandBuffers)
			return false;
		pfn_vkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(loadFunc("vkBeginCommandBuffer"));
		if (!pfn_vkBeginCommandBuffer)
			return false;
		pfn_vkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(loadFunc("vkEndCommandBuffer"));
		if (!pfn_vkEndCommandBuffer)
			return false;
		pfn_vkResetCommandBuffer = reinterpret_cast<PFN_vkResetCommandBuffer>(loadFunc("vkResetCommandBuffer"));
		if (!pfn_vkResetCommandBuffer)
			return false;
		pfn_vkCmdBindPipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(loadFunc("vkCmdBindPipeline"));
		if (!pfn_vkCmdBindPipeline)
			return false;
		pfn_vkCmdSetViewport = reinterpret_cast<PFN_vkCmdSetViewport>(loadFunc("vkCmdSetViewport"));
		if (!pfn_vkCmdSetViewport)
			return false;
		pfn_vkCmdSetScissor = reinterpret_cast<PFN_vkCmdSetScissor>(loadFunc("vkCmdSetScissor"));
		if (!pfn_vkCmdSetScissor)
			return false;
		pfn_vkCmdSetLineWidth = reinterpret_cast<PFN_vkCmdSetLineWidth>(loadFunc("vkCmdSetLineWidth"));
		if (!pfn_vkCmdSetLineWidth)
			return false;
		pfn_vkCmdSetDepthBias = reinterpret_cast<PFN_vkCmdSetDepthBias>(loadFunc("vkCmdSetDepthBias"));
		if (!pfn_vkCmdSetDepthBias)
			return false;
		pfn_vkCmdSetBlendConstants = reinterpret_cast<PFN_vkCmdSetBlendConstants>(loadFunc("vkCmdSetBlendConstants"));
		if (!pfn_vkCmdSetBlendConstants)
			return false;
		pfn_vkCmdSetDepthBounds = reinterpret_cast<PFN_vkCmdSetDepthBounds>(loadFunc("vkCmdSetDepthBounds"));
		if (!pfn_vkCmdSetDepthBounds)
			return false;
		pfn_vkCmdSetStencilCompareMask = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(loadFunc("vkCmdSetStencilCompareMask"));
		if (!pfn_vkCmdSetStencilCompareMask)
			return false;
		pfn_vkCmdSetStencilWriteMask = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(loadFunc("vkCmdSetStencilWriteMask"));
		if (!pfn_vkCmdSetStencilWriteMask)
			return false;
		pfn_vkCmdSetStencilReference = reinterpret_cast<PFN_vkCmdSetStencilReference>(loadFunc("vkCmdSetStencilReference"));
		if (!pfn_vkCmdSetStencilReference)
			return false;
		pfn_vkCmdBindDescriptorSets = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(loadFunc("vkCmdBindDescriptorSets"));
		if (!pfn_vkCmdBindDescriptorSets)
			return false;
		pfn_vkCmdBindIndexBuffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(loadFunc("vkCmdBindIndexBuffer"));
		if (!pfn_vkCmdBindIndexBuffer)
			return false;
		pfn_vkCmdBindVertexBuffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(loadFunc("vkCmdBindVertexBuffers"));
		if (!pfn_vkCmdBindVertexBuffers)
			return false;
		pfn_vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(loadFunc("vkCmdDraw"));
		if (!pfn_vkCmdDraw)
			return false;
		pfn_vkCmdDrawIndexed = reinterpret_cast<PFN_vkCmdDrawIndexed>(loadFunc("vkCmdDrawIndexed"));
		if (!pfn_vkCmdDrawIndexed)
			return false;
		pfn_vkCmdDrawIndirect = reinterpret_cast<PFN_vkCmdDrawIndirect>(loadFunc("vkCmdDrawIndirect"));
		if (!pfn_vkCmdDrawIndirect)
			return false;
		pfn_vkCmdDrawIndexedIndirect = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(loadFunc("vkCmdDrawIndexedIndirect"));
		if (!pfn_vkCmdDrawIndexedIndirect)
			return false;
		pfn_vkCmdDispatch = reinterpret_cast<PFN_vkCmdDispatch>(loadFunc("vkCmdDispatch"));
		if (!pfn_vkCmdDispatch)
			return false;
		pfn_vkCmdDispatchIndirect = reinterpret_cast<PFN_vkCmdDispatchIndirect>(loadFunc("vkCmdDispatchIndirect"));
		if (!pfn_vkCmdDispatchIndirect)
			return false;
		pfn_vkCmdCopyBuffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(loadFunc("vkCmdCopyBuffer"));
		if (!pfn_vkCmdCopyBuffer)
			return false;
		pfn_vkCmdCopyImage = reinterpret_cast<PFN_vkCmdCopyImage>(loadFunc("vkCmdCopyImage"));
		if (!pfn_vkCmdCopyImage)
			return false;
		pfn_vkCmdBlitImage = reinterpret_cast<PFN_vkCmdBlitImage>(loadFunc("vkCmdBlitImage"));
		if (!pfn_vkCmdBlitImage)
			return false;
		pfn_vkCmdCopyBufferToImage = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(loadFunc("vkCmdCopyBufferToImage"));
		if (!pfn_vkCmdCopyBufferToImage)
			return false;
		pfn_vkCmdCopyImageToBuffer = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(loadFunc("vkCmdCopyImageToBuffer"));
		if (!pfn_vkCmdCopyImageToBuffer)
			return false;
		pfn_vkCmdUpdateBuffer = reinterpret_cast<PFN_vkCmdUpdateBuffer>(loadFunc("vkCmdUpdateBuffer"));
		if (!pfn_vkCmdUpdateBuffer)
			return false;
		pfn_vkCmdFillBuffer = reinterpret_cast<PFN_vkCmdFillBuffer>(loadFunc("vkCmdFillBuffer"));
		if (!pfn_vkCmdFillBuffer)
			return false;
		pfn_vkCmdClearColorImage = reinterpret_cast<PFN_vkCmdClearColorImage>(loadFunc("vkCmdClearColorImage"));
		if (!pfn_vkCmdClearColorImage)
			return false;
		pfn_vkCmdClearDepthStencilImage = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(loadFunc("vkCmdClearDepthStencilImage"));
		if (!pfn_vkCmdClearDepthStencilImage)
			return false;
		pfn_vkCmdClearAttachments = reinterpret_cast<PFN_vkCmdClearAttachments>(loadFunc("vkCmdClearAttachments"));
		if (!pfn_vkCmdClearAttachments)
			return false;
		pfn_vkCmdResolveImage = reinterpret_cast<PFN_vkCmdResolveImage>(loadFunc("vkCmdResolveImage"));
		if (!pfn_vkCmdResolveImage)
			return false;
		pfn_vkCmdSetEvent = reinterpret_cast<PFN_vkCmdSetEvent>(loadFunc("vkCmdSetEvent"));
		if (!pfn_vkCmdSetEvent)
			return false;
		pfn_vkCmdResetEvent = reinterpret_cast<PFN_vkCmdResetEvent>(loadFunc("vkCmdResetEvent"));
		if (!pfn_vkCmdResetEvent)
			return false;
		pfn_vkCmdWaitEvents = reinterpret_cast<PFN_vkCmdWaitEvents>(loadFunc("vkCmdWaitEvents"));
		if (!pfn_vkCmdWaitEvents)
			return false;
		pfn_vkCmdPipelineBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(loadFunc("vkCmdPipelineBarrier"));
		if (!pfn_vkCmdPipelineBarrier)
			return false;
		pfn_vkCmdBeginQuery = reinterpret_cast<PFN_vkCmdBeginQuery>(loadFunc("vkCmdBeginQuery"));
		if (!pfn_vkCmdBeginQuery)
			return false;
		pfn_vkCmdEndQuery = reinterpret_cast<PFN_vkCmdEndQuery>(loadFunc("vkCmdEndQuery"));
		if (!pfn_vkCmdEndQuery)
			return false;
		pfn_vkCmdResetQueryPool = reinterpret_cast<PFN_vkCmdResetQueryPool>(loadFunc("vkCmdResetQueryPool"));
		if (!pfn_vkCmdResetQueryPool)
			return false;
		pfn_vkCmdWriteTimestamp = reinterpret_cast<PFN_vkCmdWriteTimestamp>(loadFunc("vkCmdWriteTimestamp"));
		if (!pfn_vkCmdWriteTimestamp)
			return false;
		pfn_vkCmdCopyQueryPoolResults = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(loadFunc("vkCmdCopyQueryPoolResults"));
		if (!pfn_vkCmdCopyQueryPoolResults)
			return false;
		pfn_vkCmdPushConstants = reinterpret_cast<PFN_vkCmdPushConstants>(loadFunc("vkCmdPushConstants"));
		if (!pfn_vkCmdPushConstants)
			return false;
		pfn_vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(loadFunc("vkCmdBeginRenderPass"));
		if (!pfn_vkCmdBeginRenderPass)
			return false;
		pfn_vkCmdNextSubpass = reinterpret_cast<PFN_vkCmdNextSubpass>(loadFunc("vkCmdNextSubpass"));
		if (!pfn_vkCmdNextSubpass)
			return false;
		pfn_vkCmdEndRenderPass = reinterpret_cast<PFN_vkCmdEndRenderPass>(loadFunc("vkCmdEndRenderPass"));
		if (!pfn_vkCmdEndRenderPass)
			return false;
		pfn_vkCmdExecuteCommands = reinterpret_cast<PFN_vkCmdExecuteCommands>(loadFunc("vkCmdExecuteCommands"));
		if (!pfn_vkCmdExecuteCommands)
			return false;
		return true;
	}

	bool Vulkan::loadVulkan_1_1() {
		pfn_vkEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(loadFuncInstance(nullptr, "vkEnumerateInstanceVersion"));
		if (!pfn_vkEnumerateInstanceVersion)
			return false;
		pfn_vkBindBufferMemory2 = reinterpret_cast<PFN_vkBindBufferMemory2>(loadFunc("vkBindBufferMemory2"));
		if (!pfn_vkBindBufferMemory2)
			return false;
		pfn_vkBindImageMemory2 = reinterpret_cast<PFN_vkBindImageMemory2>(loadFunc("vkBindImageMemory2"));
		if (!pfn_vkBindImageMemory2)
			return false;
		pfn_vkGetDeviceGroupPeerMemoryFeatures = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeatures>(loadFunc("vkGetDeviceGroupPeerMemoryFeatures"));
		if (!pfn_vkGetDeviceGroupPeerMemoryFeatures)
			return false;
		pfn_vkCmdSetDeviceMask = reinterpret_cast<PFN_vkCmdSetDeviceMask>(loadFunc("vkCmdSetDeviceMask"));
		if (!pfn_vkCmdSetDeviceMask)
			return false;
		pfn_vkCmdDispatchBase = reinterpret_cast<PFN_vkCmdDispatchBase>(loadFunc("vkCmdDispatchBase"));
		if (!pfn_vkCmdDispatchBase)
			return false;
		pfn_vkEnumeratePhysicalDeviceGroups = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroups>(loadFunc("vkEnumeratePhysicalDeviceGroups"));
		if (!pfn_vkEnumeratePhysicalDeviceGroups)
			return false;
		pfn_vkGetImageMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageMemoryRequirements2>(loadFunc("vkGetImageMemoryRequirements2"));
		if (!pfn_vkGetImageMemoryRequirements2)
			return false;
		pfn_vkGetBufferMemoryRequirements2 = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2>(loadFunc("vkGetBufferMemoryRequirements2"));
		if (!pfn_vkGetBufferMemoryRequirements2)
			return false;
		pfn_vkGetImageSparseMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2>(loadFunc("vkGetImageSparseMemoryRequirements2"));
		if (!pfn_vkGetImageSparseMemoryRequirements2)
			return false;
		pfn_vkGetPhysicalDeviceFeatures2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(loadFunc("vkGetPhysicalDeviceFeatures2"));
		if (!pfn_vkGetPhysicalDeviceFeatures2)
			return false;
		pfn_vkGetPhysicalDeviceProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(loadFunc("vkGetPhysicalDeviceProperties2"));
		if (!pfn_vkGetPhysicalDeviceProperties2)
			return false;
		pfn_vkGetPhysicalDeviceFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2>(loadFunc("vkGetPhysicalDeviceFormatProperties2"));
		if (!pfn_vkGetPhysicalDeviceFormatProperties2)
			return false;
		pfn_vkGetPhysicalDeviceImageFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2>(loadFunc("vkGetPhysicalDeviceImageFormatProperties2"));
		if (!pfn_vkGetPhysicalDeviceImageFormatProperties2)
			return false;
		pfn_vkGetPhysicalDeviceQueueFamilyProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2>(loadFunc("vkGetPhysicalDeviceQueueFamilyProperties2"));
		if (!pfn_vkGetPhysicalDeviceQueueFamilyProperties2)
			return false;
		pfn_vkGetPhysicalDeviceMemoryProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2>(loadFunc("vkGetPhysicalDeviceMemoryProperties2"));
		if (!pfn_vkGetPhysicalDeviceMemoryProperties2)
			return false;
		pfn_vkGetPhysicalDeviceSparseImageFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2>(loadFunc("vkGetPhysicalDeviceSparseImageFormatProperties2"));
		if (!pfn_vkGetPhysicalDeviceSparseImageFormatProperties2)
			return false;
		pfn_vkTrimCommandPool = reinterpret_cast<PFN_vkTrimCommandPool>(loadFunc("vkTrimCommandPool"));
		if (!pfn_vkTrimCommandPool)
			return false;
		pfn_vkGetDeviceQueue2 = reinterpret_cast<PFN_vkGetDeviceQueue2>(loadFunc("vkGetDeviceQueue2"));
		if (!pfn_vkGetDeviceQueue2)
			return false;
		pfn_vkCreateSamplerYcbcrConversion = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversion>(loadFunc("vkCreateSamplerYcbcrConversion"));
		if (!pfn_vkCreateSamplerYcbcrConversion)
			return false;
		pfn_vkDestroySamplerYcbcrConversion = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversion>(loadFunc("vkDestroySamplerYcbcrConversion"));
		if (!pfn_vkDestroySamplerYcbcrConversion)
			return false;
		pfn_vkCreateDescriptorUpdateTemplate = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplate>(loadFunc("vkCreateDescriptorUpdateTemplate"));
		if (!pfn_vkCreateDescriptorUpdateTemplate)
			return false;
		pfn_vkDestroyDescriptorUpdateTemplate = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplate>(loadFunc("vkDestroyDescriptorUpdateTemplate"));
		if (!pfn_vkDestroyDescriptorUpdateTemplate)
			return false;
		pfn_vkUpdateDescriptorSetWithTemplate = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplate>(loadFunc("vkUpdateDescriptorSetWithTemplate"));
		if (!pfn_vkUpdateDescriptorSetWithTemplate)
			return false;
		pfn_vkGetPhysicalDeviceExternalBufferProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferProperties>(loadFunc("vkGetPhysicalDeviceExternalBufferProperties"));
		if (!pfn_vkGetPhysicalDeviceExternalBufferProperties)
			return false;
		pfn_vkGetPhysicalDeviceExternalFenceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFenceProperties>(loadFunc("vkGetPhysicalDeviceExternalFenceProperties"));
		if (!pfn_vkGetPhysicalDeviceExternalFenceProperties)
			return false;
		pfn_vkGetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphoreProperties>(loadFunc("vkGetPhysicalDeviceExternalSemaphoreProperties"));
		if (!pfn_vkGetPhysicalDeviceExternalSemaphoreProperties)
			return false;
		pfn_vkGetDescriptorSetLayoutSupport = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupport>(loadFunc("vkGetDescriptorSetLayoutSupport"));
		if (!pfn_vkGetDescriptorSetLayoutSupport)
			return false;
		return true;
	}

	bool Vulkan::loadVulkan_1_2() {
		pfn_vkCmdDrawIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndirectCount>(loadFunc("vkCmdDrawIndirectCount"));
		if (!pfn_vkCmdDrawIndirectCount)
			return false;
		pfn_vkCmdDrawIndexedIndirectCount = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCount>(loadFunc("vkCmdDrawIndexedIndirectCount"));
		if (!pfn_vkCmdDrawIndexedIndirectCount)
			return false;
		pfn_vkCreateRenderPass2 = reinterpret_cast<PFN_vkCreateRenderPass2>(loadFunc("vkCreateRenderPass2"));
		if (!pfn_vkCreateRenderPass2)
			return false;
		pfn_vkCmdBeginRenderPass2 = reinterpret_cast<PFN_vkCmdBeginRenderPass2>(loadFunc("vkCmdBeginRenderPass2"));
		if (!pfn_vkCmdBeginRenderPass2)
			return false;
		pfn_vkCmdNextSubpass2 = reinterpret_cast<PFN_vkCmdNextSubpass2>(loadFunc("vkCmdNextSubpass2"));
		if (!pfn_vkCmdNextSubpass2)
			return false;
		pfn_vkCmdEndRenderPass2 = reinterpret_cast<PFN_vkCmdEndRenderPass2>(loadFunc("vkCmdEndRenderPass2"));
		if (!pfn_vkCmdEndRenderPass2)
			return false;
		pfn_vkResetQueryPool = reinterpret_cast<PFN_vkResetQueryPool>(loadFunc("vkResetQueryPool"));
		if (!pfn_vkResetQueryPool)
			return false;
		pfn_vkGetSemaphoreCounterValue = reinterpret_cast<PFN_vkGetSemaphoreCounterValue>(loadFunc("vkGetSemaphoreCounterValue"));
		if (!pfn_vkGetSemaphoreCounterValue)
			return false;
		pfn_vkWaitSemaphores = reinterpret_cast<PFN_vkWaitSemaphores>(loadFunc("vkWaitSemaphores"));
		if (!pfn_vkWaitSemaphores)
			return false;
		pfn_vkSignalSemaphore = reinterpret_cast<PFN_vkSignalSemaphore>(loadFunc("vkSignalSemaphore"));
		if (!pfn_vkSignalSemaphore)
			return false;
		pfn_vkGetBufferDeviceAddress = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(loadFunc("vkGetBufferDeviceAddress"));
		if (!pfn_vkGetBufferDeviceAddress)
			return false;
		pfn_vkGetBufferOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddress>(loadFunc("vkGetBufferOpaqueCaptureAddress"));
		if (!pfn_vkGetBufferOpaqueCaptureAddress)
			return false;
		pfn_vkGetDeviceMemoryOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddress>(loadFunc("vkGetDeviceMemoryOpaqueCaptureAddress"));
		if (!pfn_vkGetDeviceMemoryOpaqueCaptureAddress)
			return false;
		return true;
	}

	bool Vulkan::loadVulkan_1_3() {
		pfn_vkGetPhysicalDeviceToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolProperties>(loadFunc("vkGetPhysicalDeviceToolProperties"));
		if (!pfn_vkGetPhysicalDeviceToolProperties)
			return false;
		pfn_vkCreatePrivateDataSlot = reinterpret_cast<PFN_vkCreatePrivateDataSlot>(loadFunc("vkCreatePrivateDataSlot"));
		if (!pfn_vkCreatePrivateDataSlot)
			return false;
		pfn_vkDestroyPrivateDataSlot = reinterpret_cast<PFN_vkDestroyPrivateDataSlot>(loadFunc("vkDestroyPrivateDataSlot"));
		if (!pfn_vkDestroyPrivateDataSlot)
			return false;
		pfn_vkSetPrivateData = reinterpret_cast<PFN_vkSetPrivateData>(loadFunc("vkSetPrivateData"));
		if (!pfn_vkSetPrivateData)
			return false;
		pfn_vkGetPrivateData = reinterpret_cast<PFN_vkGetPrivateData>(loadFunc("vkGetPrivateData"));
		if (!pfn_vkGetPrivateData)
			return false;
		pfn_vkCmdSetEvent2 = reinterpret_cast<PFN_vkCmdSetEvent2>(loadFunc("vkCmdSetEvent2"));
		if (!pfn_vkCmdSetEvent2)
			return false;
		pfn_vkCmdResetEvent2 = reinterpret_cast<PFN_vkCmdResetEvent2>(loadFunc("vkCmdResetEvent2"));
		if (!pfn_vkCmdResetEvent2)
			return false;
		pfn_vkCmdWaitEvents2 = reinterpret_cast<PFN_vkCmdWaitEvents2>(loadFunc("vkCmdWaitEvents2"));
		if (!pfn_vkCmdWaitEvents2)
			return false;
		pfn_vkCmdPipelineBarrier2 = reinterpret_cast<PFN_vkCmdPipelineBarrier2>(loadFunc("vkCmdPipelineBarrier2"));
		if (!pfn_vkCmdPipelineBarrier2)
			return false;
		pfn_vkCmdWriteTimestamp2 = reinterpret_cast<PFN_vkCmdWriteTimestamp2>(loadFunc("vkCmdWriteTimestamp2"));
		if (!pfn_vkCmdWriteTimestamp2)
			return false;
		pfn_vkQueueSubmit2 = reinterpret_cast<PFN_vkQueueSubmit2>(loadFunc("vkQueueSubmit2"));
		if (!pfn_vkQueueSubmit2)
			return false;
		pfn_vkCmdCopyBuffer2 = reinterpret_cast<PFN_vkCmdCopyBuffer2>(loadFunc("vkCmdCopyBuffer2"));
		if (!pfn_vkCmdCopyBuffer2)
			return false;
		pfn_vkCmdCopyImage2 = reinterpret_cast<PFN_vkCmdCopyImage2>(loadFunc("vkCmdCopyImage2"));
		if (!pfn_vkCmdCopyImage2)
			return false;
		pfn_vkCmdCopyBufferToImage2 = reinterpret_cast<PFN_vkCmdCopyBufferToImage2>(loadFunc("vkCmdCopyBufferToImage2"));
		if (!pfn_vkCmdCopyBufferToImage2)
			return false;
		pfn_vkCmdCopyImageToBuffer2 = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2>(loadFunc("vkCmdCopyImageToBuffer2"));
		if (!pfn_vkCmdCopyImageToBuffer2)
			return false;
		pfn_vkCmdBlitImage2 = reinterpret_cast<PFN_vkCmdBlitImage2>(loadFunc("vkCmdBlitImage2"));
		if (!pfn_vkCmdBlitImage2)
			return false;
		pfn_vkCmdResolveImage2 = reinterpret_cast<PFN_vkCmdResolveImage2>(loadFunc("vkCmdResolveImage2"));
		if (!pfn_vkCmdResolveImage2)
			return false;
		pfn_vkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(loadFunc("vkCmdBeginRendering"));
		if (!pfn_vkCmdBeginRendering)
			return false;
		pfn_vkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(loadFunc("vkCmdEndRendering"));
		if (!pfn_vkCmdEndRendering)
			return false;
		pfn_vkCmdSetCullMode = reinterpret_cast<PFN_vkCmdSetCullMode>(loadFunc("vkCmdSetCullMode"));
		if (!pfn_vkCmdSetCullMode)
			return false;
		pfn_vkCmdSetFrontFace = reinterpret_cast<PFN_vkCmdSetFrontFace>(loadFunc("vkCmdSetFrontFace"));
		if (!pfn_vkCmdSetFrontFace)
			return false;
		pfn_vkCmdSetPrimitiveTopology = reinterpret_cast<PFN_vkCmdSetPrimitiveTopology>(loadFunc("vkCmdSetPrimitiveTopology"));
		if (!pfn_vkCmdSetPrimitiveTopology)
			return false;
		pfn_vkCmdSetViewportWithCount = reinterpret_cast<PFN_vkCmdSetViewportWithCount>(loadFunc("vkCmdSetViewportWithCount"));
		if (!pfn_vkCmdSetViewportWithCount)
			return false;
		pfn_vkCmdSetScissorWithCount = reinterpret_cast<PFN_vkCmdSetScissorWithCount>(loadFunc("vkCmdSetScissorWithCount"));
		if (!pfn_vkCmdSetScissorWithCount)
			return false;
		pfn_vkCmdBindVertexBuffers2 = reinterpret_cast<PFN_vkCmdBindVertexBuffers2>(loadFunc("vkCmdBindVertexBuffers2"));
		if (!pfn_vkCmdBindVertexBuffers2)
			return false;
		pfn_vkCmdSetDepthTestEnable = reinterpret_cast<PFN_vkCmdSetDepthTestEnable>(loadFunc("vkCmdSetDepthTestEnable"));
		if (!pfn_vkCmdSetDepthTestEnable)
			return false;
		pfn_vkCmdSetDepthWriteEnable = reinterpret_cast<PFN_vkCmdSetDepthWriteEnable>(loadFunc("vkCmdSetDepthWriteEnable"));
		if (!pfn_vkCmdSetDepthWriteEnable)
			return false;
		pfn_vkCmdSetDepthCompareOp = reinterpret_cast<PFN_vkCmdSetDepthCompareOp>(loadFunc("vkCmdSetDepthCompareOp"));
		if (!pfn_vkCmdSetDepthCompareOp)
			return false;
		pfn_vkCmdSetDepthBoundsTestEnable = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnable>(loadFunc("vkCmdSetDepthBoundsTestEnable"));
		if (!pfn_vkCmdSetDepthBoundsTestEnable)
			return false;
		pfn_vkCmdSetStencilTestEnable = reinterpret_cast<PFN_vkCmdSetStencilTestEnable>(loadFunc("vkCmdSetStencilTestEnable"));
		if (!pfn_vkCmdSetStencilTestEnable)
			return false;
		pfn_vkCmdSetStencilOp = reinterpret_cast<PFN_vkCmdSetStencilOp>(loadFunc("vkCmdSetStencilOp"));
		if (!pfn_vkCmdSetStencilOp)
			return false;
		pfn_vkCmdSetRasterizerDiscardEnable = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnable>(loadFunc("vkCmdSetRasterizerDiscardEnable"));
		if (!pfn_vkCmdSetRasterizerDiscardEnable)
			return false;
		pfn_vkCmdSetDepthBiasEnable = reinterpret_cast<PFN_vkCmdSetDepthBiasEnable>(loadFunc("vkCmdSetDepthBiasEnable"));
		if (!pfn_vkCmdSetDepthBiasEnable)
			return false;
		pfn_vkCmdSetPrimitiveRestartEnable = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnable>(loadFunc("vkCmdSetPrimitiveRestartEnable"));
		if (!pfn_vkCmdSetPrimitiveRestartEnable)
			return false;
		pfn_vkGetDeviceBufferMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirements>(loadFunc("vkGetDeviceBufferMemoryRequirements"));
		if (!pfn_vkGetDeviceBufferMemoryRequirements)
			return false;
		pfn_vkGetDeviceImageMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirements>(loadFunc("vkGetDeviceImageMemoryRequirements"));
		if (!pfn_vkGetDeviceImageMemoryRequirements)
			return false;
		pfn_vkGetDeviceImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirements>(loadFunc("vkGetDeviceImageSparseMemoryRequirements"));
		if (!pfn_vkGetDeviceImageSparseMemoryRequirements)
			return false;
		return true;
	}

	/* bool Vulkan::loadVulkan_1_4() {
		pfn_vkCmdSetLineStipple = reinterpret_cast<PFN_vkCmdSetLineStipple>(loadFunc("vkCmdSetLineStipple"));
		if (!pfn_vkCmdSetLineStipple)
			return false;
		pfn_vkMapMemory2 = reinterpret_cast<PFN_vkMapMemory2>(loadFunc("vkMapMemory2"));
		if (!pfn_vkMapMemory2)
			return false;
		pfn_vkUnmapMemory2 = reinterpret_cast<PFN_vkUnmapMemory2>(loadFunc("vkUnmapMemory2"));
		if (!pfn_vkUnmapMemory2)
			return false;
		pfn_vkCmdBindIndexBuffer2 = reinterpret_cast<PFN_vkCmdBindIndexBuffer2>(loadFunc("vkCmdBindIndexBuffer2"));
		if (!pfn_vkCmdBindIndexBuffer2)
			return false;
		pfn_vkGetRenderingAreaGranularity = reinterpret_cast<PFN_vkGetRenderingAreaGranularity>(loadFunc("vkCmdSetLineStipple"));
		if (!pfn_vkGetRenderingAreaGranularity)
			return false;
		pfn_vkGetDeviceImageSubresourceLayout = reinterpret_cast<PFN_vkGetDeviceImageSubresourceLayout>(loadFunc("vkGetDeviceImageSubresourceLayout"));
		if (!pfn_vkGetDeviceImageSubresourceLayout)
			return false;
		pfn_vkGetImageSubresourceLayout2 = reinterpret_cast<PFN_vkGetImageSubresourceLayout2>(loadFunc("vkGetImageSubresourceLayout2"));
		if (!pfn_vkGetImageSubresourceLayout2)
			return false;
		pfn_vkCmdPushDescriptorSet = reinterpret_cast<PFN_vkCmdPushDescriptorSet>(loadFunc("vkCmdPushDescriptorSet"));
		if (!pfn_vkCmdPushDescriptorSet)
			return false;
		pfn_vkCmdPushDescriptorSetWithTemplate = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplate>(loadFunc("vkCmdPushDescriptorSetWithTemplate"));
		if (!pfn_vkCmdPushDescriptorSetWithTemplate)
			return false;
		pfn_vkCmdSetRenderingAttachmentLocations = reinterpret_cast<PFN_vkCmdSetRenderingAttachmentLocations>(loadFunc("vkCmdSetRenderingAttachmentLocations"));
		if (!pfn_vkCmdSetRenderingAttachmentLocations)
			return false;
		pfn_vkCmdSetRenderingInputAttachmentIndices = reinterpret_cast<PFN_vkCmdSetRenderingInputAttachmentIndices>(loadFunc("vkCmdSetRenderingInputAttachmentIndices"));
		if (!pfn_vkCmdSetRenderingInputAttachmentIndices)
			return false;
		pfn_vkCmdBindDescriptorSets2 = reinterpret_cast<PFN_vkCmdBindDescriptorSets2>(loadFunc("vkCmdBindDescriptorSets2"));
		if (!pfn_vkCmdBindDescriptorSets2)
			return false;
		pfn_vkCmdPushConstants2 = reinterpret_cast<PFN_vkCmdPushConstants2>(loadFunc("vkCmdPushConstants2"));
		if (!pfn_vkCmdPushConstants2)
			return false;
		pfn_vkCmdPushDescriptorSet2 = reinterpret_cast<PFN_vkCmdPushDescriptorSet2>(loadFunc("vkCmdPushDescriptorSet2"));
		if (!pfn_vkCmdPushDescriptorSet2)
			return false;
		pfn_vkCmdPushDescriptorSetWithTemplate2 = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplate2>(loadFunc("vkCmdPushDescriptorSetWithTemplate2"));
		if (!pfn_vkCmdPushDescriptorSetWithTemplate2)
			return false;
		pfn_vkCopyMemoryToImage = reinterpret_cast<PFN_vkCopyMemoryToImage>(loadFunc("vkCopyMemoryToImage"));
		if (!pfn_vkCopyMemoryToImage)
			return false;
		pfn_vkCopyImageToMemory = reinterpret_cast<PFN_vkCopyImageToMemory>(loadFunc("vkCopyImageToMemory"));
		if (!pfn_vkCopyImageToMemory)
			return false;
		pfn_vkCopyImageToImage = reinterpret_cast<PFN_vkCopyImageToImage>(loadFunc("vkCopyImageToImage"));
		if (!pfn_vkCopyImageToImage)
			return false;
		pfn_vkTransitionImageLayout = reinterpret_cast<PFN_vkTransitionImageLayout>(loadFunc("vkTransitionImageLayout"));
		if (!pfn_vkTransitionImageLayout)
			return false;
		return true;
	} */
	
	Vulkan::Vulkan() : valid(false) {
		debugMessenger = VK_NULL_HANDLE;
		internalInstance = VK_NULL_HANDLE;
		internalPhysicalDevice = VK_NULL_HANDLE;
		internalPhysicalDeviceProperties = {};
		internalPhysicalDeviceFeatures = {};

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
		if (Vulkan::instance) {
			RE_ERROR("A new object of the Vulkan class has been constructed. Only one can exist at a time");
			return;
		}
		Vulkan::instance = this;
#ifdef RE_OS_WINDOWS
		hVulkan = LoadLibraryW(L"vulkan-1.dll");
		if (!hVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return;
		}
		pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(hVulkan, "vkGetInstanceProcAddr"));
#elif defined RE_OS_LINUX
		libVulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
		if (!libVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return;
		}
		pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(libVulkan, "vkGetInstanceProcAddr"));
#endif
		if (!pfn_vkGetInstanceProcAddr) {
			RE_FATAL_ERROR("Failed loading the Vulkan function \"vkGetInstanceProcAddr\" with OS-API function");
			return;
		}
		if (!createInstance()) {
			RE_FATAL_ERROR("Failed creating a Vulkan instance");
			return;
		}
		if (!setupValidationLayers()) {
			RE_FATAL_ERROR("Failed setting Vulkan validation layers up");
			return;
		}
		if (!loadVulkan_1_0()) {
			RE_FATAL_ERROR("Failed loading Vulkan 1.0 functions");
			return;
		}
		if (!loadVulkan_1_1()) {
			RE_FATAL_ERROR("Failed loading Vulkan 1.1 functions");
			return;
		}
		if (!loadVulkan_1_2()) {
			RE_FATAL_ERROR("Failed loading Vulkan 1.2 functions");
			return;
		}
		if (!loadVulkan_1_3()) {
			RE_FATAL_ERROR("Failed loading Vulkan 1.3 functions");
			return;
		}
		/* if (!loadVulkan_1_4()) {
			RE_ERROR("Failed loading Vulkan 1.4 functions");
			return;
		} */
		if (!pickPhysicalDevice())
			return;
		valid = true;
	}

	Vulkan::~Vulkan() {
		if (Vulkan::instance != this)
			return;
		Vulkan::instance = nullptr;
		if (internalInstance != VK_NULL_HANDLE) {
			if (debugMessenger != VK_NULL_HANDLE) {
				PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(loadFunc("vkDestroyDebugUtilsMessengerEXT"));
				if (pfn_vkDestroyDebugUtilsMessengerEXT)
					pfn_vkDestroyDebugUtilsMessengerEXT(internalInstance, debugMessenger, nullptr);
				else
					RE_ERROR("Failed loading the function for destroying the debug messenger used for Vulkan validation layers");
			}
			if (!pfn_vkDestroyInstance) {
				RE_NOTE("Attempting to reload function for destroying Vulkan instances");
				pfn_vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(loadFunc("vkDestroyInstance"));
				if (!pfn_vkDestroyInstance)
					RE_ERROR("Failed reloading the function for destroying the Vulkan instance");
				else
					pfn_vkDestroyInstance(internalInstance, nullptr);
			} else
				pfn_vkDestroyInstance(internalInstance, nullptr);
		}
#ifdef RE_OS_WINDOWS
		FreeLibrary(hVulkan);
#elif defined RE_OS_LINUX
		dlclose(libVulkan);
#endif
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severityFlagBits, VkDebugUtilsMessageTypeFlagsEXT msgTypeBits, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
		if (severityFlagBits == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			RE_WARNING("Vulkan's validation layers were triggered");
		else if (severityFlagBits == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			RE_ERROR("Vulkan's validation layers were triggered");
		else
			RE_NOTE("Vulkan's validation layers were triggered. The severity couldn't be determined");
		println(callbackData->pMessage);
		return VK_FALSE;
	}

	void* Vulkan::loadFuncInstance(VkInstance instance, const char* funcName) {
		void* funcPtr = reinterpret_cast<void*>(pfn_vkGetInstanceProcAddr(instance, funcName));
		if (!funcPtr)
			RE_ERROR(appendStrings("Failed loading the Vulkan function \"", funcName, "\""));
		return funcPtr;
	}

	void* Vulkan::loadFunc(const char* funcName) {
		return loadFuncInstance(internalInstance, funcName);
	}

	bool Vulkan::createInstance() {
		pfn_vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(loadFuncInstance(nullptr, "vkCreateInstance"));
		if (!pfn_vkCreateInstance) {
			RE_NOTE("Attempted to load the Vulkan function mentioned before for creating a Vulkan instance");
			return false;
		}
		pfn_vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(loadFuncInstance(nullptr, "vkEnumerateInstanceExtensionProperties"));
		if (!pfn_vkEnumerateInstanceExtensionProperties) {
			RE_NOTE("Attempted to load the Vulkan function mentioned before for creating a Vulkan instance");
			return false;
		}
		pfn_vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(loadFuncInstance(nullptr, "vkEnumerateInstanceLayerProperties"));
		if (!pfn_vkEnumerateInstanceLayerProperties) {
			RE_NOTE("Attempted to load the Vulkan function mentioned before for creating a Vulkan instance");
			return false;
		}

		constexpr uint32_t extensionsToLoadCount = 1;
		const char** extensionsToLoad = new const char*[extensionsToLoadCount] {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
		uint32_t availableExtensionsCount = 0;
		pfn_vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);
		VkExtensionProperties* availableExtensions = new VkExtensionProperties[availableExtensionsCount];
		pfn_vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions);
		println("Available Vulkan instance extensions:");
		for (uint32_t i = 0; i < availableExtensionsCount; i++)
			println(appendStrings("\t", availableExtensions[i].extensionName, ": ", availableExtensions[i].specVersion));
		bool extensionsMissing = false;
		for (uint32_t extToLoadIndex = 0; extToLoadIndex < extensionsToLoadCount; extToLoadIndex++) {
			bool found = false;
			for (uint32_t availableExtsIndex = 0; availableExtsIndex < availableExtensionsCount; availableExtsIndex++)
				if (strcmp(extensionsToLoad[extToLoadIndex], availableExtensions[availableExtsIndex].extensionName) == 0) {
					found = true;
					break;
				}
			if (!found) {
				RE_FATAL_ERROR(appendStrings("The requested Vulkan instance extension \"", extensionsToLoad[extToLoadIndex], "\" does not exist on this computer"));
				extensionsMissing = true;
			}
		}

		constexpr uint32_t layersToLoadCount = 1;
		const char** layersToLoad = new const char*[layersToLoadCount] {"VK_LAYER_KHRONOS_validation"};
		uint32_t availableLayersCount = 0;
		pfn_vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr);
		VkLayerProperties* availableLayers = new VkLayerProperties[availableLayersCount];
		pfn_vkEnumerateInstanceLayerProperties(&availableLayersCount, availableLayers);
		println("Available Vulkan instance layers:");
		for (uint32_t i = 0; i < availableLayersCount; i++)
			println(appendStrings("\t", availableLayers[i].layerName, " (", availableLayers[i].specVersion, ".", availableLayers[i].implementationVersion, "): ", availableLayers[i].description));
		bool layersMissing = false;
		for (uint32_t layersToLoadIndex = 0; layersToLoadIndex < layersToLoadCount; layersToLoadIndex++) {
			bool found = false;
			for (uint32_t availableLayersIndex = 0; availableLayersIndex < availableLayersCount; availableLayersIndex++)
				if (strcmp(layersToLoad[layersToLoadIndex], availableLayers[availableLayersIndex].layerName) == 0) {
					found = true;
					break;
				}
			if (!found) {
				RE_FATAL_ERROR(appendStrings("The requested Vulkan instance layer \"", layersToLoad[layersToLoadIndex], "\" does not exist on this computer"));
				layersMissing = true;
			}
		}

		const bool instanceCreationAllowed = !extensionsMissing && !layersMissing;
		bool instanceCreationSuccessful = false;
		if (instanceCreationAllowed) {
			VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
			std::string appName = getAppName();
			appInfo.pApplicationName = appName.c_str();
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "R-Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_3;
			VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
			createInfo.pApplicationInfo = &appInfo;
			createInfo.enabledExtensionCount = extensionsToLoadCount;
			createInfo.ppEnabledExtensionNames = extensionsToLoad;
			createInfo.enabledLayerCount = layersToLoadCount;
			createInfo.ppEnabledLayerNames = layersToLoad;
			VkResult successResult = pfn_vkCreateInstance(&createInfo, nullptr, &internalInstance);
			if (!checkVulkanResult(successResult))
				RE_FATAL_ERROR("Failed creating Vulkan instance");
			instanceCreationSuccessful = successResult == VK_SUCCESS;
		}
		delete[] extensionsToLoad;
		delete[] availableExtensions;
		delete[] layersToLoad;
		delete[] availableLayers;
		return instanceCreationSuccessful && instanceCreationAllowed;
	}

	bool Vulkan::setupValidationLayers() {
		PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(loadFunc("vkCreateDebugUtilsMessengerEXT"));
		if (!pfn_vkCreateDebugUtilsMessengerEXT) {
			RE_NOTE("Attempted to load the Vulkan function mentioned before for setting validation layers up");
			return false;
		}
		VkDebugUtilsMessengerCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
		if (!checkVulkanResult(pfn_vkCreateDebugUtilsMessengerEXT(internalInstance, &createInfo, nullptr, &debugMessenger))) {
			RE_FATAL_ERROR("Failed creating Vulkan debug messenger for validation layers");
			return false;
		}
		return true;
	}

	bool Vulkan::isPhysicalDeviceSuitable(VkPhysicalDevice device) {
		uint32_t queueFamiliesCount = 0;
		Vulkan::instance->pfn_vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, nullptr);
		if (!queueFamiliesCount)
			return false;
		/* VkPhysicalDeviceProperties deviceProperties;
		pfn_vkGetPhysicalDeviceProperties(device, &deviceProperties);
		VkPhysicalDeviceFeatures deviceFeatures;
		pfn_vkGetPhysicalDeviceFeatures(device, &deviceFeatures); */
		VkQueueFamilyProperties* queueFamilies = new VkQueueFamilyProperties[queueFamiliesCount];
		Vulkan::instance->pfn_vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, queueFamilies);
		bool queuesSuitable = false;
		for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamiliesCount; queueFamilyIndex++)
			if (queueFamilies[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				queuesSuitable = true;
				break;
			}
		delete[] queueFamilies;
		return queuesSuitable;
	}

	bool Vulkan::pickPhysicalDevice() {
		uint32_t physicalDeviceCount = 0;
		pfn_vkEnumeratePhysicalDevices(internalInstance, &physicalDeviceCount, nullptr);
		if (!physicalDeviceCount) {
			RE_FATAL_ERROR("There aren't any physical devices with Vulkan support");
			return false;
		}
		VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
		pfn_vkEnumeratePhysicalDevices(internalInstance, &physicalDeviceCount, physicalDevices);
		if (physicalDeviceCount == 1) {
			if (!isPhysicalDeviceSuitable(physicalDevices[0])) {
				RE_FATAL_ERROR("The computer has only one physical device supporting Vulkan, but is not suitable for rendering");
				delete[] physicalDevices;
				return false;
			}
			internalPhysicalDevice = physicalDevices[0];
		} else {
			REushort currentPhysicalDeviceScore = 0;
			for (uint32_t physicalDeviceIndex = 0; physicalDeviceIndex < physicalDeviceCount; physicalDeviceIndex++) {
				// Fetch data about GPU
				VkPhysicalDevice physicalDevice = physicalDevices[physicalDeviceIndex];
				VkPhysicalDeviceProperties physicalDeviceProperties;
				pfn_vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
				VkPhysicalDeviceFeatures physicalDeviceFeatures;
				pfn_vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

				// Inspecting, whether GPU is suitable
				if (!isPhysicalDeviceSuitable(physicalDevice))
					continue;

				// Rating GPU
				REushort physicalDeviceScore = 0;
				switch (physicalDeviceProperties.deviceType) {
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						physicalDeviceScore += 50;
						break;
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						physicalDeviceScore += 25;
						break;
					default:
						break;
				}

				// Deciding, whether GPU is better and has to be chosen
				if (physicalDeviceScore > currentPhysicalDeviceScore || internalPhysicalDevice == VK_NULL_HANDLE) {
					internalPhysicalDevice = physicalDevice;
					currentPhysicalDeviceScore = physicalDeviceScore;
				}
			}
			if (internalPhysicalDevice == VK_NULL_HANDLE) {
				RE_FATAL_ERROR("Failed finding a suitable device with Vulkan support");
				delete[] physicalDevices;
				return false;
			}
		}
		delete[] physicalDevices;
		pfn_vkGetPhysicalDeviceProperties(internalPhysicalDevice, &internalPhysicalDeviceProperties);
		pfn_vkGetPhysicalDeviceFeatures(internalPhysicalDevice, &internalPhysicalDeviceFeatures);
		uint32_t queueFamiliesCount = 0;
		pfn_vkGetPhysicalDeviceQueueFamilyProperties(internalPhysicalDevice, &queueFamiliesCount, nullptr);
		VkQueueFamilyProperties* queueFamilies = new VkQueueFamilyProperties[queueFamiliesCount];
		pfn_vkGetPhysicalDeviceQueueFamilyProperties(internalPhysicalDevice, &queueFamiliesCount, queueFamilies);
		for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamiliesCount; queueFamilyIndex++) {
			if (queueFamilies[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				internalQueueIndices.graphicsFamily = queueFamilyIndex;
				break;
			}
		}
		println(appendStrings("Selected GPU for rendering: ", internalPhysicalDeviceProperties.deviceName));
		delete[] queueFamilies;
		return true;
	}

	bool Vulkan::isValid() {
		return valid;
	}

	bool Vulkan::checkVulkanResult(VkResult result) {
		const char* errorString = "unknown result";
		const char* errorName = errorString;
		switch (result) {
			case VK_SUCCESS:
				return true;

			// Success codes, but treated as errors
			case VK_NOT_READY:
				errorName = "VK_NOT_READY";
				errorString = "Not ready (a fence or query has not yet completed)";
				break;
			case VK_TIMEOUT:
				errorName = "VK_TIMEOUT";
				errorString = "Timeout (a wait operation has not completed in the specified time)";
				break;
			case VK_EVENT_SET:
				errorName = "VK_EVENT_SET";
				errorString = "Event signaled";
				break;
			case VK_EVENT_RESET:
				errorName = "VK_EVENT_RESET";
				errorString = "Event unsignaled";
				break;
			case VK_INCOMPLETE:
				errorName = "VK_INCOMPLETE";
				errorString = "Incomplete (a return array was too small for the result)";
				break;
			case VK_SUBOPTIMAL_KHR:
				errorName = "VK_SUBOPTIMAL_KHR";
				errorString = "A swapchain no longer matches the surface properties exactly";
				break;
			case VK_THREAD_IDLE_KHR:
				errorName = "VK_THREAD_IDLE_KHR";
				errorString = "A deferred operation is not complete, but there's currently no work for this thread";
				break;
			case VK_THREAD_DONE_KHR:
				errorName = "VK_THREAD_DONE_KHR";
				errorString = "A deferred operation is not complete, but there's no work remaining";
				break;
			case VK_OPERATION_DEFERRED_KHR:
				errorName = "VK_OPERATION_DEFERRED_KHR";
				errorString = "A deferred operation was requested and some of the work has been deferred";
				break;
			case VK_OPERATION_NOT_DEFERRED_KHR:
				errorName = "VK_OPERATION_NOT_DEFERRED_KHR";
				errorString = "A deferred operation was requested and no operations were deferred";
				break;
			case VK_PIPELINE_COMPILE_REQUIRED:
				errorName = "VK_PIPELINE_COMPILE_REQUIRED";
				errorString = "Pipeline compilation required, but not done by the application";
				break;
			/* case VK_PIPELINE_BINARY_MISSING_KHR:
				errorName = "VK_PIPELINE_BINARY_MISSING_KHR";
				errorString = "Attempted to create a pipeline binary by querying an internal cache, but the internal cache entry did not exist";
				break;
			case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
				errorName = "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
				errorString = "The binary shader code is incompatible with the device";
				break; */

			// Errors
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				errorName = "VK_ERROR_OUT_OF_HOST_MEMORY";
				errorString = "Out of host memory";
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				errorName = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
				errorString = "Out of device memory";
				break;
			case VK_ERROR_INITIALIZATION_FAILED:
				errorName = "VK_ERROR_INITIALIZATION_FAILED";
				errorString = "Initialization failed";
				break;
			case VK_ERROR_DEVICE_LOST:
				errorName = "VK_ERROR_DEVICE_LOST";
				errorString = "Device lost";
				break;
			case VK_ERROR_MEMORY_MAP_FAILED:
				errorName = "VK_ERROR_MEMORY_MAP_FAILED";
				errorString = "Memory map failed";
				break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				errorName = "VK_ERROR_LAYER_NOT_PRESENT";
				errorString = "Layer not present";
				break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				errorName = "VK_ERROR_EXTENSION_NOT_PRESENT";
				errorString = "Extension not present";
				break;
			case VK_ERROR_FEATURE_NOT_PRESENT:
				errorName = "VK_ERROR_FEATURE_NOT_PRESENT";
				errorString = "Feature not present";
				break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				errorName = "VK_ERROR_INCOMPATIBLE_DRIVER";
				errorString = "Incompatible driver";
				break;
			case VK_ERROR_TOO_MANY_OBJECTS:
				errorName = "VK_ERROR_TOO_MANY_OBJECTS";
				errorString = "Too many objects";
				break;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				errorName = "VK_ERROR_FORMAT_NOT_SUPPORTED";
				errorString = "Format not supported";
				break;
			case VK_ERROR_FRAGMENTED_POOL:
				errorName = "VK_ERROR_FRAGMENTED_POOL";
				errorString = "Fragmented pool";
				break;
			case VK_ERROR_SURFACE_LOST_KHR:
				errorName = "VK_ERROR_SURFACE_LOST_KHR";
				errorString = "Surface has been lost and is no longer available";
				break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				errorName = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
				errorString = "The window is already in use by any API";
				break;
			case VK_ERROR_OUT_OF_DATE_KHR:
				errorName = "VK_ERROR_OUT_OF_DATE_KHR";
				errorString = "A surface has changed in a way that it's no longer compatible with the swapchain and further operations will fail";
				break;
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				errorName = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
				errorString = "The display used by a swapchain doesn't use the same presentable image layout or is incompatible";
				break;
			case VK_ERROR_INVALID_SHADER_NV:
				errorName = "VK_ERROR_INVALID_SHADER_NV";
				errorString = "One or more shaders failed to compile or link";
				break;
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				errorName = "VK_ERROR_OUT_OF_POOL_MEMORY";
				errorString = "Out of pool memory";
				break;
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:
				errorName = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
				errorString = "Invalid external handle";
				break;
			case VK_ERROR_FRAGMENTATION:
				errorName = "VK_ERROR_FRAGMENTATION";
				errorString = "Fragmentation";
				break;
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
				errorName = "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
				errorString = "The memory address is not available";
				break;
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
				errorName = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
				errorString = "Operation on swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT has failed, because the application didn't have exclusive fullscreen access (outside the application's control)";
				break;
			case VK_ERROR_VALIDATION_FAILED_EXT:
				errorName = "VK_ERROR_VALIDATION_FAILED_EXT";
				errorString = "Invalid usage detected";
				break;
			case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
				errorName = "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
				errorString = "Image creation failed for internal resources required for compression are exhausted";
				break;
			case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
				errorName = "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
				errorString = "Requested VkImageUsageFlags aren't supported";
				break;
			case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
				errorName = "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
				errorString = "Requested video picture layout not supported";
				break;
			case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
				errorName = "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
				errorString = "Video profile operation not supported";
				break;
			case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
				errorName = "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
				errorString = "Video profile format parameters not supported";
				break;
			case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
				errorName = "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
				errorString = "Video profile codec not supported";
				break;
			case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
				errorName = "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
				errorString = "Specified video STD header version is not supported";
				break;
			/* case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
				errorName = "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
				errorString = "Invalid video STD parameters";
				break; */
			case VK_ERROR_NOT_PERMITTED_KHR:
				errorName = "VK_ERROR_NOT_PERMITTED_KHR";
				errorString = "Action is not permitted to be executed due to the application's missing privileges";
				break;
			/* case VK_ERROR_NOT_ENOUGH_SPACE:
				errorName = "VK_ERROR_NOT_ENOUGH_SPACE";
				errorString = "Application didn't provide enough space to return the data";
				break; */
			case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
				errorName = "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
				errorString = "";
				break;
			case VK_ERROR_UNKNOWN:
				errorName = "VK_ERROR_UNKNOWN";
				errorString = "Unknown error";
				break;
			case VK_RESULT_MAX_ENUM:
				break;
		}
		RE_ERROR(appendStrings("The recently called Vulkan function threw an error: (", errorName, ") ", errorString));
		return false;
	}

}
