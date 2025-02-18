#include "RE_Vulkan.hpp"
#include "RE_Window_Win64.hpp"
#include "RE_Window_X11.hpp"

namespace RE {

	Vulkan* Vulkan::pInstance = nullptr;

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

	bool Vulkan::loadExtensionFuncs() {
		pfn_vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(loadFunc("vkDestroySurfaceKHR"));
		if (!pfn_vkDestroySurfaceKHR)
			return false;
		pfn_vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(loadFunc("vkGetPhysicalDeviceSurfaceSupportKHR"));
		if (!pfn_vkGetPhysicalDeviceSurfaceSupportKHR)
			return false;
		pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(loadFunc("vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
		if (!pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
			return false;
		pfn_vkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(loadFunc("vkGetPhysicalDeviceSurfaceFormatsKHR"));
		if (!pfn_vkGetPhysicalDeviceSurfaceFormatsKHR)
			return false;
		pfn_vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(loadFunc("vkGetPhysicalDeviceSurfacePresentModesKHR"));
		if (!pfn_vkGetPhysicalDeviceSurfacePresentModesKHR)
			return false;
		pfn_vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(loadFunc("vkCreateSwapchainKHR"));
		if (!pfn_vkCreateSwapchainKHR)
			return false;
		pfn_vkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(loadFunc("vkDestroySwapchainKHR"));
		if (!pfn_vkDestroySwapchainKHR)
			return false;
		pfn_vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(loadFunc("vkGetSwapchainImagesKHR"));
		if (!pfn_vkGetSwapchainImagesKHR)
			return false;
		pfn_vkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(loadFunc("vkAcquireNextImageKHR"));
		if (!pfn_vkAcquireNextImageKHR)
			return false;
		pfn_vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(loadFunc("vkQueuePresentKHR"));
		if (!pfn_vkQueuePresentKHR)
			return false;
		pfn_vkGetDeviceGroupPresentCapabilitiesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPresentCapabilitiesKHR>(loadFunc("vkGetDeviceGroupPresentCapabilitiesKHR"));
		if (!pfn_vkGetDeviceGroupPresentCapabilitiesKHR)
			return false;
		pfn_vkGetDeviceGroupSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModesKHR>(loadFunc("vkGetDeviceGroupSurfacePresentModesKHR"));
		if (!pfn_vkGetDeviceGroupSurfacePresentModesKHR)
			return false;
		pfn_vkGetPhysicalDevicePresentRectanglesKHR = reinterpret_cast<PFN_vkGetPhysicalDevicePresentRectanglesKHR>(loadFunc("vkGetPhysicalDevicePresentRectanglesKHR"));
		if (!pfn_vkGetPhysicalDevicePresentRectanglesKHR)
			return false;
		pfn_vkAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(loadFunc("vkAcquireNextImage2KHR"));
		if (!pfn_vkAcquireNextImage2KHR)
			return false;
#ifdef RE_OS_WINDOWS
		pfn_vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(loadFunc("vkCreateWin32SurfaceKHR"));
		if (!pfn_vkCreateWin32SurfaceKHR)
			return false;
		pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(loadFunc("vkGetPhysicalDeviceWin32PresentationSupportKHR"));
		if (!pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR)
			return false;
#elif defined RE_OS_LINUX
		pfn_vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(loadFunc("vkCreateXlibSurfaceKHR"));
		if (!pfn_vkCreateXlibSurfaceKHR)
			return false;
		pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(loadFunc("vkGetPhysicalDeviceXlibPresentationSupportKHR"));
		if (!pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR)
			return false;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		return true;
	}
	
	Vulkan::Vulkan() : bValid(false) {
		if (Vulkan::pInstance) {
			RE_ERROR("A new object of the Vulkan class has been constructed. Only one can exist at a time");
			return;
		}
		Vulkan::pInstance = this;

		vk_hDebugMessenger = VK_NULL_HANDLE;
		vk_hInternalInstance = VK_NULL_HANDLE;
		vk_hInternalPhysicalDevice = VK_NULL_HANDLE;
		vk_internalPhysicalDeviceProperties = {};
		vk_internalPhysicalDeviceFeatures = {};
		internalQueueIndices = {};
		vk_hInternalDevice = VK_NULL_HANDLE;
		vk_hInternalGraphicsQueue = VK_NULL_HANDLE;
		vk_hInternalPresentationQueue = VK_NULL_HANDLE;
		vk_hInternalSurface = VK_NULL_HANDLE;
		vk_internalSurfaceCapabilities = {};
		vk_pInternalSurfaceFormats = nullptr;
		vk_pInternalPresentModes = nullptr;

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
		win_hVulkan = LoadLibraryW(L"vulkan-1.dll");
		if (!win_hVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return;
		}
		pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(win_hVulkan, "vkGetInstanceProcAddr"));
#elif defined RE_OS_LINUX
		pfn_vkCreateXlibSurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
		linux_libVulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
		if (!linux_libVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return;
		}
		pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(linux_libVulkan, "vkGetInstanceProcAddr"));
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
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
		if (!loadExtensionFuncs()) {
			RE_FATAL_ERROR("Failed loading Vulkan extension functions");
			return;
		}
		if (!createWindowSurface())
			return;
		if (!pickPhysicalDevice())
			return;
		if (!createLogicalDevice())
			return;
		bValid = true;
	}

	Vulkan::~Vulkan() {
		if (Vulkan::pInstance != this)
			return;
		Vulkan::pInstance = nullptr;
		if (vk_hInternalInstance != VK_NULL_HANDLE) {
			if (vk_hInternalSurface != VK_NULL_HANDLE)
				pfn_vkDestroySurfaceKHR(vk_hInternalInstance, vk_hInternalSurface, nullptr);
			if (vk_hInternalDevice != VK_NULL_HANDLE)
				pfn_vkDestroyDevice(vk_hInternalDevice, nullptr);
			if (vk_hDebugMessenger != VK_NULL_HANDLE) {
				PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(loadFunc("vkDestroyDebugUtilsMessengerEXT"));
				if (pfn_vkDestroyDebugUtilsMessengerEXT)
					pfn_vkDestroyDebugUtilsMessengerEXT(vk_hInternalInstance, vk_hDebugMessenger, nullptr);
				else
					RE_ERROR("Failed loading the function for destroying the debug messenger used for Vulkan validation layers");
			}
			if (!pfn_vkDestroyInstance) {
				RE_NOTE("Attempting to reload function for destroying Vulkan instances");
				pfn_vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(loadFuncInstance(VK_NULL_HANDLE, "vkDestroyInstance"));
				if (!pfn_vkDestroyInstance)
					RE_ERROR("Failed reloading the function for destroying the Vulkan instance");
				else
					pfn_vkDestroyInstance(vk_hInternalInstance, nullptr);
			} else
				pfn_vkDestroyInstance(vk_hInternalInstance, nullptr);
		}
		if (vk_pInternalSurfaceFormats)
			delete[] vk_pInternalSurfaceFormats;
		if (vk_pInternalPresentModes)
			delete[] vk_pInternalPresentModes;
#ifdef RE_OS_WINDOWS
		FreeLibrary(win_hVulkan);
#elif defined RE_OS_LINUX
		dlclose(linux_libVulkan);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT vk_eSeverityFlagBits, VkDebugUtilsMessageTypeFlagsEXT vk_eMsgTypeBits, const VkDebugUtilsMessengerCallbackDataEXT* vk_pCallbackData, void* vk_pUserData) {
		if (vk_eSeverityFlagBits == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			RE_WARNING("Vulkan's validation layers were triggered");
		else if (vk_eSeverityFlagBits == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			RE_ERROR("Vulkan's validation layers were triggered");
		else
			RE_NOTE("Vulkan's validation layers were triggered. The severity couldn't be determined");
		println(vk_pCallbackData->pMessage);
		return VK_FALSE;
	}

	void* Vulkan::loadFuncInstance(VkInstance vk_hInstance, const char* pFuncName) {
		void* pFuncPtr = reinterpret_cast<void*>(pfn_vkGetInstanceProcAddr(vk_hInstance, pFuncName));
		if (!pFuncPtr)
			RE_ERROR(appendStrings("Failed loading the Vulkan function \"", pFuncName, "\""));
		return pFuncPtr;
	}

	void* Vulkan::loadFunc(const char* pFuncName) {
		return loadFuncInstance(vk_hInternalInstance, pFuncName);
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

		constexpr uint32_t u32ExtensionsToLoadCount = 3;
		const char** ppcExtensionsToLoad = new const char*[u32ExtensionsToLoadCount] {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME};
#ifdef RE_OS_WINDOWS
		ppcExtensionsToLoad[u32ExtensionsToLoadCount - 1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined RE_OS_LINUX
		ppcExtensionsToLoad[u32ExtensionsToLoadCount - 1] = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		uint32_t u32AvailableExtensionsCount = 0;
		pfn_vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, nullptr);
		VkExtensionProperties* vk_pAvailableExtensions = new VkExtensionProperties[u32AvailableExtensionsCount];
		pfn_vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, vk_pAvailableExtensions);
		println("Available Vulkan instance extensions:");
		for (uint32_t i = 0; i < u32AvailableExtensionsCount; i++)
			println(appendStrings("\t", vk_pAvailableExtensions[i].extensionName, " (", VK_API_VERSION_MAJOR(vk_pAvailableExtensions[i].specVersion), ".", VK_API_VERSION_MINOR(vk_pAvailableExtensions[i].specVersion), ".", VK_API_VERSION_PATCH(vk_pAvailableExtensions[i].specVersion), ")"));
		bool bExtensionsMissing = false;
		for (uint32_t uiExtToLoadIndex = 0; uiExtToLoadIndex < u32ExtensionsToLoadCount; uiExtToLoadIndex++) {
			bool bFound = false;
			for (uint32_t uiAvailableExtsIndex = 0; uiAvailableExtsIndex < u32AvailableExtensionsCount; uiAvailableExtsIndex++)
				if (strcmp(ppcExtensionsToLoad[uiExtToLoadIndex], vk_pAvailableExtensions[uiAvailableExtsIndex].extensionName) == 0) {
					bFound = true;
					break;
				}
			if (!bFound) {
				RE_FATAL_ERROR(appendStrings("The requested Vulkan instance extension \"", ppcExtensionsToLoad[uiExtToLoadIndex], "\" does not exist on this computer"));
				bExtensionsMissing = true;
			}
		}

		constexpr uint32_t u32LayersToLoadCount = 1;
		const char** ppcLayersToLoad = new const char*[u32LayersToLoadCount] {VK_KHR_VALIDATION_LAYER_NAME};
		uint32_t u32AvailableLayersCount = 0;
		pfn_vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, nullptr);
		VkLayerProperties* vk_pAvailableLayers = new VkLayerProperties[u32AvailableLayersCount];
		pfn_vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, vk_pAvailableLayers);
		println("Available Vulkan instance layers:");
		for (uint32_t i = 0; i < u32AvailableLayersCount; i++)
			println(appendStrings("\t", vk_pAvailableLayers[i].layerName, " (", VK_API_VERSION_MAJOR(vk_pAvailableLayers[i].specVersion), ".", VK_API_VERSION_MINOR(vk_pAvailableLayers[i].specVersion), ".",VK_API_VERSION_PATCH(vk_pAvailableLayers[i].specVersion), " - ", vk_pAvailableLayers[i].implementationVersion, "): ", vk_pAvailableLayers[i].description));
		bool bLayersMissing = false;
		for (uint32_t uiLayersToLoadIndex = 0; uiLayersToLoadIndex < u32LayersToLoadCount; uiLayersToLoadIndex++) {
			bool bFound = false;
			for (uint32_t uiAvailableLayersIndex = 0; uiAvailableLayersIndex < u32AvailableLayersCount; uiAvailableLayersIndex++)
				if (strcmp(ppcLayersToLoad[uiLayersToLoadIndex], vk_pAvailableLayers[uiAvailableLayersIndex].layerName) == 0) {
					bFound = true;
					break;
				}
			if (!bFound) {
				RE_FATAL_ERROR(appendStrings("The requested Vulkan instance layer \"", ppcLayersToLoad[uiLayersToLoadIndex], "\" does not exist on this computer"));
				bLayersMissing = true;
			}
		}

		const bool bInstanceCreationAllowed = !bExtensionsMissing && !bLayersMissing;
		bool bInstanceCreationSuccessful = false;
		if (bInstanceCreationAllowed) {
			VkApplicationInfo vk_appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
			std::string strAppName = getAppName();
			vk_appInfo.pApplicationName = strAppName.c_str();
			vk_appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			vk_appInfo.pEngineName = "R-Engine";
			vk_appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			vk_appInfo.apiVersion = VK_API_VERSION_1_3;
			VkInstanceCreateInfo vk_instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
			vk_instanceCreateInfo.pApplicationInfo = &vk_appInfo;
			vk_instanceCreateInfo.enabledExtensionCount = u32ExtensionsToLoadCount;
			vk_instanceCreateInfo.ppEnabledExtensionNames = ppcExtensionsToLoad;
			vk_instanceCreateInfo.enabledLayerCount = u32LayersToLoadCount;
			vk_instanceCreateInfo.ppEnabledLayerNames = ppcLayersToLoad;
			VkResult vk_eSuccessResult = pfn_vkCreateInstance(&vk_instanceCreateInfo, nullptr, &vk_hInternalInstance);
			if (!checkVulkanResult(vk_eSuccessResult))
				RE_FATAL_ERROR("Failed creating Vulkan instance");
			bInstanceCreationSuccessful = vk_eSuccessResult == VK_SUCCESS;
		}
		delete[] ppcExtensionsToLoad;
		delete[] vk_pAvailableExtensions;
		delete[] ppcLayersToLoad;
		delete[] vk_pAvailableLayers;
		return bInstanceCreationSuccessful && bInstanceCreationAllowed;
	}

	bool Vulkan::setupValidationLayers() {
		PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(loadFunc("vkCreateDebugUtilsMessengerEXT"));
		if (!pfn_vkCreateDebugUtilsMessengerEXT) {
			RE_NOTE("Attempted to load the Vulkan function mentioned before for setting validation layers up");
			return false;
		}
		VkDebugUtilsMessengerCreateInfoEXT vk_debugCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		vk_debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		vk_debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		vk_debugCreateInfo.pfnUserCallback = debugCallback;
		vk_debugCreateInfo.pUserData = nullptr;
		if (!checkVulkanResult(pfn_vkCreateDebugUtilsMessengerEXT(vk_hInternalInstance, &vk_debugCreateInfo, nullptr, &vk_hDebugMessenger))) {
			RE_FATAL_ERROR("Failed creating Vulkan debug messenger for validation layers");
			return false;
		}
		return true;
	}

	bool Vulkan::isPhysicalDeviceSuitable(VkPhysicalDevice vk_hPhysicalDevice) {
		uint32_t u32QueueFamiliesCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32QueueFamiliesCount, nullptr);
		if (!u32QueueFamiliesCount)
			return false;
		uint32_t u32ExtensionsCount = 0;
		vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32ExtensionsCount, nullptr);
		if (!u32ExtensionsCount)
			return false;
		/* VkPhysicalDeviceProperties deviceProperties;
		pfn_vkGetPhysicalDeviceProperties(vk_hPhysicalDevice, &deviceProperties);
		VkPhysicalDeviceFeatures deviceFeatures;
		pfn_vkGetPhysicalDeviceFeatures(vk_hPhysicalDevice, &deviceFeatures); */
		VkQueueFamilyProperties* vk_pQueueFamilies = new VkQueueFamilyProperties[u32QueueFamiliesCount];
		vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32QueueFamiliesCount, vk_pQueueFamilies);
		bool bGraphicsSupport = false, bPresentSupport = false;
		for (uint32_t u32QueueFamilyIndex = 0; u32QueueFamilyIndex < u32QueueFamiliesCount; u32QueueFamilyIndex++) {
			if (!bGraphicsSupport && vk_pQueueFamilies[u32QueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				bGraphicsSupport = true;
			if (!bPresentSupport) {
				VkBool32 vk_presentQueueSupport = 0;
				vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDevice, u32QueueFamilyIndex, Vulkan::pInstance->vk_hInternalSurface, &vk_presentQueueSupport);
				bPresentSupport = static_cast<bool>(vk_presentQueueSupport);
			}
		}
		delete[] vk_pQueueFamilies;
		if (!bGraphicsSupport || !bPresentSupport)
			return false;
		VkExtensionProperties* vk_pExtensions = new VkExtensionProperties[u32ExtensionsCount];
		vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32ExtensionsCount, vk_pExtensions);
		bool bSwapchainSupport = false;
		for (uint32_t uiExtensionsIndex = 0; uiExtensionsIndex < u32ExtensionsCount; uiExtensionsIndex++) {
			if (strcmp(vk_pExtensions[uiExtensionsIndex].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
				uint32_t u32Count = 0;
				vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDevice, Vulkan::pInstance->vk_hInternalSurface, &u32Count, nullptr);
				if (!u32Count)
					continue;
				vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDevice, Vulkan::pInstance->vk_hInternalSurface, &u32Count, nullptr);
				if (!u32Count)
					continue;
				bSwapchainSupport = true;
				break;
			}
		}
		delete[] vk_pExtensions;
		return bSwapchainSupport;
	}

	bool Vulkan::pickPhysicalDevice() {
		uint32_t u32PhysicalDeviceCount = 0;
		pfn_vkEnumeratePhysicalDevices(vk_hInternalInstance, &u32PhysicalDeviceCount, nullptr);
		if (!u32PhysicalDeviceCount) {
			RE_FATAL_ERROR("There aren't any physical devices with Vulkan support");
			return false;
		}
		VkPhysicalDevice* vk_pPhysicalDevices = new VkPhysicalDevice[u32PhysicalDeviceCount];
		pfn_vkEnumeratePhysicalDevices(vk_hInternalInstance, &u32PhysicalDeviceCount, vk_pPhysicalDevices);
		REushort u16CurrentPhysicalDeviceScore = 0;
		for (uint32_t uiPhysicalDeviceIndex = 0; uiPhysicalDeviceIndex < u32PhysicalDeviceCount; uiPhysicalDeviceIndex++) {
			VkPhysicalDevice vk_hPhysicalDevice = vk_pPhysicalDevices[uiPhysicalDeviceIndex];

			// Inspecting, whether GPU is suitable
			if (!isPhysicalDeviceSuitable(vk_hPhysicalDevice))
				continue;

			// Fetch data about GPU
			VkPhysicalDeviceProperties vk_hPhysicalDeviceProperties;
			pfn_vkGetPhysicalDeviceProperties(vk_hPhysicalDevice, &vk_hPhysicalDeviceProperties);
			VkPhysicalDeviceFeatures vk_hPhysicalDeviceFeatures;
			pfn_vkGetPhysicalDeviceFeatures(vk_hPhysicalDevice, &vk_hPhysicalDeviceFeatures);

			// Rating GPU
			REushort u16PhysicalDeviceScore = 0;
			switch (vk_hPhysicalDeviceProperties.deviceType) {
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					u16PhysicalDeviceScore += 1000;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					u16PhysicalDeviceScore += 500;
					break;
				default:
					break;
			}

			// Deciding, whether GPU is better and has to be chosen
			if (u16PhysicalDeviceScore > u16CurrentPhysicalDeviceScore || vk_hInternalPhysicalDevice == VK_NULL_HANDLE) {
				vk_hInternalPhysicalDevice = vk_hPhysicalDevice;
				u16CurrentPhysicalDeviceScore = u16PhysicalDeviceScore;
			}
		}
		delete[] vk_pPhysicalDevices;
		if (vk_hInternalPhysicalDevice == VK_NULL_HANDLE) {
			RE_FATAL_ERROR("Failed finding a suitable device with Vulkan support");
			return false;
		}
		pfn_vkGetPhysicalDeviceProperties(vk_hInternalPhysicalDevice, &vk_internalPhysicalDeviceProperties);
		pfn_vkGetPhysicalDeviceFeatures(vk_hInternalPhysicalDevice, &vk_internalPhysicalDeviceFeatures);
		uint32_t u32QueueFamiliesCount = 0;
		pfn_vkGetPhysicalDeviceQueueFamilyProperties(vk_hInternalPhysicalDevice, &u32QueueFamiliesCount, nullptr);
		VkQueueFamilyProperties* vk_pQueueFamilies = new VkQueueFamilyProperties[u32QueueFamiliesCount];
		pfn_vkGetPhysicalDeviceQueueFamilyProperties(vk_hInternalPhysicalDevice, &u32QueueFamiliesCount, vk_pQueueFamilies);
		std::optional<uint32_t> graphicsIndex;
		std::optional<uint32_t> presentIndex;
		for (uint32_t u32QueueFamilyIndex = 0; u32QueueFamilyIndex < u32QueueFamiliesCount; u32QueueFamilyIndex++) {
			if (!graphicsIndex.has_value() && vk_pQueueFamilies[u32QueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				graphicsIndex = u32QueueFamilyIndex;
			if (!presentIndex.has_value()) {
				VkBool32 vk_presentQueueSupport = 0;
				pfn_vkGetPhysicalDeviceSurfaceSupportKHR(vk_hInternalPhysicalDevice, u32QueueFamilyIndex, vk_hInternalSurface, &vk_presentQueueSupport);
				if (vk_presentQueueSupport)
					presentIndex = u32QueueFamilyIndex;
			}
		}
		internalQueueIndices.u32GraphicsFamily = graphicsIndex.value();
		internalQueueIndices.u32PresentationFamily = presentIndex.value();
		pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hInternalPhysicalDevice, vk_hInternalSurface, &vk_internalSurfaceCapabilities);
		pfn_vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hInternalPhysicalDevice, vk_hInternalSurface, &u32InternalSurfaceFormatsCount, nullptr);
		vk_pInternalSurfaceFormats = new VkSurfaceFormatKHR[u32InternalSurfaceFormatsCount];
		pfn_vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hInternalPhysicalDevice, vk_hInternalSurface, &u32InternalSurfaceFormatsCount, vk_pInternalSurfaceFormats);
		pfn_vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hInternalPhysicalDevice, vk_hInternalSurface, &u32InternalPresentModesCount, nullptr);
		vk_pInternalPresentModes = new VkPresentModeKHR[u32InternalPresentModesCount];
		pfn_vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hInternalPhysicalDevice, vk_hInternalSurface, &u32InternalPresentModesCount, vk_pInternalPresentModes);
		println(appendStrings("Selected GPU for rendering: ", vk_internalPhysicalDeviceProperties.deviceName));
		delete[] vk_pQueueFamilies;
		return true;
	}

	bool Vulkan::createLogicalDevice() {
		std::vector<uint32_t> queueIndices;
		for (REubyte u8FamilyIndex = 0; u8FamilyIndex < 2; u8FamilyIndex++) {
			uint32_t u32Index = 0;
			switch (u8FamilyIndex) {
				case 0:
					u32Index = internalQueueIndices.u32GraphicsFamily;
					break;
				case 1:
					u32Index = internalQueueIndices.u32PresentationFamily;
					break;
			}
			if (std::find(queueIndices.begin(), queueIndices.end(), u32Index) == queueIndices.end())
				queueIndices.push_back(u32Index);
		}
		VkDeviceQueueCreateInfo* vk_pDeviceQueueCreateInfos = new VkDeviceQueueCreateInfo[queueIndices.size()];
		const float fQueuePriority = 1.0f;
		for (uint32_t u32DeviceQueueCreateInfosIndex = 0; u32DeviceQueueCreateInfosIndex < queueIndices.size(); u32DeviceQueueCreateInfosIndex++) {
			vk_pDeviceQueueCreateInfos[u32DeviceQueueCreateInfosIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			vk_pDeviceQueueCreateInfos[u32DeviceQueueCreateInfosIndex].pNext = nullptr;
			vk_pDeviceQueueCreateInfos[u32DeviceQueueCreateInfosIndex].flags = 0;
			vk_pDeviceQueueCreateInfos[u32DeviceQueueCreateInfosIndex].queueFamilyIndex = queueIndices.at(u32DeviceQueueCreateInfosIndex);
			vk_pDeviceQueueCreateInfos[u32DeviceQueueCreateInfosIndex].queueCount = 1;
			vk_pDeviceQueueCreateInfos[u32DeviceQueueCreateInfosIndex].pQueuePriorities = &fQueuePriority;
		}
		constexpr uint32_t u32ExtensionsToLoadCount = 1;
		const char** ppcExtensionsToLoad = new const char*[u32ExtensionsToLoadCount] {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		constexpr uint32_t u32LayersToLoadCount = 1;
		const char** ppcLayersToLoad = new const char*[u32LayersToLoadCount] {VK_KHR_VALIDATION_LAYER_NAME};
		VkPhysicalDeviceFeatures vk_hPhysicalDeviceFeatures = {};
		VkDeviceCreateInfo vk_deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		vk_deviceCreateInfo.pQueueCreateInfos = vk_pDeviceQueueCreateInfos;
		vk_deviceCreateInfo.queueCreateInfoCount = queueIndices.size();
		vk_deviceCreateInfo.pEnabledFeatures = &vk_hPhysicalDeviceFeatures;
		vk_deviceCreateInfo.ppEnabledExtensionNames = ppcExtensionsToLoad;
		vk_deviceCreateInfo.enabledExtensionCount = u32ExtensionsToLoadCount;
		vk_deviceCreateInfo.ppEnabledLayerNames = ppcLayersToLoad;
		vk_deviceCreateInfo.enabledLayerCount = u32LayersToLoadCount;
		VkResult vk_eSuccessResult = pfn_vkCreateDevice(vk_hInternalPhysicalDevice, &vk_deviceCreateInfo, nullptr, &vk_hInternalDevice);
		if (!checkVulkanResult(vk_eSuccessResult))
			RE_FATAL_ERROR("Failed creating a logical Vulkan device");
		else {
			pfn_vkGetDeviceQueue(vk_hInternalDevice, internalQueueIndices.u32GraphicsFamily, 0, &vk_hInternalGraphicsQueue);
			pfn_vkGetDeviceQueue(vk_hInternalDevice, internalQueueIndices.u32PresentationFamily, 0, &vk_hInternalPresentationQueue);
		}
		delete[] vk_pDeviceQueueCreateInfos;
		delete[] ppcExtensionsToLoad;
		delete[] ppcLayersToLoad;
		return vk_eSuccessResult == VK_SUCCESS;
	}

	bool Vulkan::createWindowSurface() {
		VkResult vk_eSuccessResult = VK_RESULT_MAX_ENUM;
#ifdef RE_OS_WINDOWS
		Window_Win64* pWindowWin64 = static_cast<Window_Win64*>(Window::pInstance);
		VkWin32SurfaceCreateInfoKHR vk_win64SurfaceCreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
		vk_win64SurfaceCreateInfo.hinstance = pWindowWin64->win_hInstance;
		vk_win64SurfaceCreateInfo.hwnd = pWindowWin64->win_hWindow;
		vk_eSuccessResult = pfn_vkCreateWin32SurfaceKHR(vk_hInternalInstance, &vk_win64SurfaceCreateInfo, nullptr, &vk_hInternalSurface);
#elif defined RE_OS_LINUX
		Window_X11* pWindowX11 = static_cast<Window_X11*>(Window::pInstance);
		VkXlibSurfaceCreateInfoKHR vk_x11SurfaceCreateInfo = { VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR };
		vk_x11SurfaceCreateInfo.dpy = pWindowX11->xDisplay;
		vk_x11SurfaceCreateInfo.window = pWindowX11->xWindow;
		vk_eSuccessResult = pfn_vkCreateXlibSurfaceKHR(vk_hInternalInstance, &vk_x11SurfaceCreateInfo, nullptr, &vk_hInternalSurface);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		if (!checkVulkanResult(vk_eSuccessResult)) {
			RE_FATAL_ERROR("Failed creating a surface for the window and linking to Vulkan");
			return false;
		}
		return true;
	}

	bool Vulkan::isValid() {
		return bValid;
	}

	uint32_t Vulkan::getSurfaceFormatsCount() {
		return u32InternalSurfaceFormatsCount;
	}

	uint32_t Vulkan::getPresentModesCount() {
		return u32InternalPresentModesCount;
	}

	bool Vulkan::checkVulkanResult(VkResult vk_eResult) {
		const char* pcErrName = "unknown enumeration value";
		const char* pcErrDetail = pcErrName;
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
				break;
		}
		RE_ERROR(appendStrings("The recently called Vulkan function threw an error: (", pcErrName, ") ", pcErrDetail));
		return false;
	}

}
