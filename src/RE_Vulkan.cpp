#include "RE_Vulkan.hpp"
#include "RE_Window_Win64.hpp"
#include "RE_Window_X11.hpp"
#include "RE_Main.hpp"

#undef vk_hInstance

namespace RE {

	Vulkan* Vulkan::pInstance = nullptr;

	bool Vulkan::load_vulkan_1_0() {
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

	bool Vulkan::load_vulkan_1_1() {
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

	bool Vulkan::load_vulkan_1_2() {
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

	bool Vulkan::load_vulkan_1_3() {
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

	/* bool Vulkan::load_vulkan_1_4() {
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

	bool Vulkan::load_extension_funcs() {
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
		switch (eUsingWindowingSystem) {
			case RE_WINDOWING_SYSTEM_WAYLAND:
				pfn_vkCreateWaylandSurfaceKHR = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(load_func("vkCreateWaylandSurfaceKHR"));
				if (!pfn_vkCreateWaylandSurfaceKHR)
					return false;
				pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>(load_func("vkGetPhysicalDeviceWaylandPresentationSupportKHR"));
				if (!pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR)
					return false;
				break;
			case RE_WINDOWING_SYSTEM_X11:
				pfn_vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(load_func("vkCreateXlibSurfaceKHR"));
				if (!pfn_vkCreateXlibSurfaceKHR)
					return false;
				pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(load_func("vkGetPhysicalDeviceXlibPresentationSupportKHR"));
				if (!pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR)
					return false;
				break;
			default:
				return false;
		}
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		return true;
	}
	
	Vulkan::Vulkan() : bValid(false), hLibVulkan(nullptr), vk_hInstance(VK_NULL_HANDLE), vk_hDebugMessenger(VK_NULL_HANDLE) {
		if (Vulkan::pInstance) {
			RE_FATAL_ERROR("A new object of the Vulkan class has been constructed. Only one can exist at a time");
			return;
		}
		Vulkan::pInstance = this;
		DEFINE_SIGNAL_GUARD(sigGuardVulkanConstructor);

		// Avoiding wild pointers (don't remove!)
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
		CATCH_SIGNAL(hLibVulkan = LoadLibraryW(L"vulkan-1.dll"));
		if (!hLibVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return;
		}
		CATCH_SIGNAL(pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(hLibVulkan, "vkGetInstanceProcAddr")));
#elif defined RE_OS_LINUX
		pfn_vkCreateXlibSurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
		CATCH_SIGNAL(hLibVulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL));
		if (!hLibVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return;
		}
		CATCH_SIGNAL(pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(hLibVulkan, "vkGetInstanceProcAddr")));
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		if (!pfn_vkGetInstanceProcAddr) {
			RE_FATAL_ERROR("Failed loading the Vulkan function \"vkGetInstanceProcAddr\" with the OS-specific API");
			return;
		}

		bool bRecentSuccess;
		CATCH_SIGNAL(bRecentSuccess = create_instance());
		if (!bRecentSuccess) {
			RE_FATAL_ERROR("Failed creating a Vulkan instance");
			return;
		}
		CATCH_SIGNAL(bRecentSuccess = load_vulkan_1_0());
		if (!bRecentSuccess) {
			RE_FATAL_ERROR("Failed loading Vulkan 1.0 functions");
			return;
		}
		CATCH_SIGNAL(bRecentSuccess = load_vulkan_1_1());
		if (!bRecentSuccess) {
			RE_FATAL_ERROR("Failed loading Vulkan 1.1 functions");
			return;
		}
		CATCH_SIGNAL(bRecentSuccess = load_vulkan_1_2());
		if (!bRecentSuccess) {
			RE_FATAL_ERROR("Failed loading Vulkan 1.2 functions");
			return;
		}
		CATCH_SIGNAL(bRecentSuccess = load_vulkan_1_3());
		if (!bRecentSuccess) {
			RE_FATAL_ERROR("Failed loading Vulkan 1.3 functions");
			return;
		}
		/* CATCH_SIGNAL(bRecentSuccess = load_vulkan_1_4());
		if (!bRecentSuccess) {
			RE_ERROR("Failed loading Vulkan 1.4 functions");
			return;
		} */
		CATCH_SIGNAL(bRecentSuccess = load_extension_funcs());
		if (!bRecentSuccess) {
			RE_FATAL_ERROR("Failed loading Vulkan extension functions");
			return;
		}
		CATCH_SIGNAL(bRecentSuccess = setup_validation_layers());
		if (!bRecentSuccess) {
			RE_FATAL_ERROR("Failed setting validation layers up");
			return;
		}
		bValid = true;
	}

	Vulkan::~Vulkan() {
		if (Vulkan::pInstance != this)
			return;
		Vulkan::pInstance = nullptr;
		if (vk_hInstance != VK_NULL_HANDLE) {
			if (vk_hDebugMessenger != VK_NULL_HANDLE)
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
#ifdef RE_OS_WINDOWS
		FreeLibrary(hLibVulkan);
#elif defined RE_OS_LINUX
		dlclose(hLibVulkan);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT vk_eSeverityFlagBits, VkDebugUtilsMessageTypeFlagsEXT vk_eMsgTypeBits, const VkDebugUtilsMessengerCallbackDataEXT* vk_pCallbackData, void* vk_pUserData) {
		if (vk_eSeverityFlagBits == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			RE_WARNING(append_to_string("Vulkan's validation layers were triggered\n", vk_pCallbackData->pMessage));
		else if (vk_eSeverityFlagBits == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			RE_ERROR(append_to_string("Vulkan's validation layers were triggered\n", vk_pCallbackData->pMessage));
		else
			RE_NOTE(append_to_string("Vulkan's validation layers were triggered. The severity couldn't be determined\n", vk_pCallbackData->pMessage));
		return VK_FALSE;
	}

	void* Vulkan::load_func_with_instance(VkInstance vk_hInstance, const char* pFuncName) {
		void* pFuncPtr;
		CATCH_SIGNAL(pFuncPtr = reinterpret_cast<void*>(pfn_vkGetInstanceProcAddr(vk_hInstance, pFuncName)));
		if (!pFuncPtr)
			RE_ERROR(append_to_string("Failed loading the Vulkan function \"", pFuncName, "\""));
		return pFuncPtr;
	}

	void* Vulkan::load_func(const char* pFuncName) {
		void* pFuncPtr;
		CATCH_SIGNAL(pFuncPtr = load_func_with_instance(vk_hInstance, pFuncName));
		return pFuncPtr;
	}

	bool Vulkan::create_instance() {
		CATCH_SIGNAL(pfn_vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(load_func_with_instance(VK_NULL_HANDLE, "vkCreateInstance")));
		if (!pfn_vkCreateInstance) {
			RE_NOTE("Attempted to load the Vulkan function mentioned before for creating a Vulkan instance");
			return false;
		}
		CATCH_SIGNAL(pfn_vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(load_func_with_instance(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties")));
		if (!pfn_vkEnumerateInstanceExtensionProperties) {
			RE_NOTE("Attempted to load the Vulkan function mentioned before for creating a Vulkan instance");
			return false;
		}
		CATCH_SIGNAL(pfn_vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(load_func_with_instance(VK_NULL_HANDLE, "vkEnumerateInstanceLayerProperties")));
		if (!pfn_vkEnumerateInstanceLayerProperties) {
			RE_NOTE("Attempted to load the Vulkan function mentioned before for creating a Vulkan instance");
			return false;
		}

		constexpr uint32_t u32ExtensionsToLoadCount = 3U;
		const char** ppcExtensionsToLoad = new const char*[u32ExtensionsToLoadCount] {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME};
#ifdef RE_OS_WINDOWS
		ppcExtensionsToLoad[u32ExtensionsToLoadCount - 1U] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined RE_OS_LINUX
		switch (eUsingWindowingSystem) {
			case RE_WINDOWING_SYSTEM_WAYLAND:
				ppcExtensionsToLoad[u32ExtensionsToLoadCount - 1U] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
				break;
			case RE_WINDOWING_SYSTEM_X11:
				ppcExtensionsToLoad[u32ExtensionsToLoadCount - 1U] = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
				break;
			default:
				RE_ERROR("Unknown windowing system on Linux selected");
				break;
		}
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		uint32_t u32AvailableExtensionsCount = 0U;
		CATCH_SIGNAL(pfn_vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, nullptr));
		VkExtensionProperties* vk_pAvailableExtensions = new VkExtensionProperties[u32AvailableExtensionsCount];
		CATCH_SIGNAL(pfn_vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, vk_pAvailableExtensions));
		PRINT_LN("Available Vulkan instance extensions:");
		for (uint32_t i = 0U; i < u32AvailableExtensionsCount; i++)
			println(append_to_string("\t", vk_pAvailableExtensions[i].extensionName, " (", VK_API_VERSION_MAJOR(vk_pAvailableExtensions[i].specVersion), ".", VK_API_VERSION_MINOR(vk_pAvailableExtensions[i].specVersion), ".", VK_API_VERSION_PATCH(vk_pAvailableExtensions[i].specVersion), ")"));
		bool bExtensionsMissing = false;
		for (uint32_t u32ExtToLoadIndex = 0U; u32ExtToLoadIndex < u32ExtensionsToLoadCount; u32ExtToLoadIndex++) {
			bool bFound = false;
			for (uint32_t uiAvailableExtsIndex = 0U; uiAvailableExtsIndex < u32AvailableExtensionsCount; uiAvailableExtsIndex++)
				if (std::strcmp(ppcExtensionsToLoad[u32ExtToLoadIndex], vk_pAvailableExtensions[uiAvailableExtsIndex].extensionName) == 0) {
					bFound = true;
					break;
				}
			if (!bFound) {
				RE_FATAL_ERROR(append_to_string("The requested Vulkan instance extension \"", ppcExtensionsToLoad[u32ExtToLoadIndex], "\" does not exist on this computer"));
				bExtensionsMissing = true;
			}
		}

		constexpr uint32_t u32LayersToLoadCount = 1U;
		const char** ppcLayersToLoad = new const char*[u32LayersToLoadCount] {VK_KHR_VALIDATION_LAYER_NAME};
		uint32_t u32AvailableLayersCount = 0U;
		CATCH_SIGNAL(pfn_vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, nullptr));
		VkLayerProperties* vk_pAvailableLayers = new VkLayerProperties[u32AvailableLayersCount];
		CATCH_SIGNAL(pfn_vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, vk_pAvailableLayers));
		PRINT_LN("Available Vulkan instance layers:");
		for (uint32_t u32LayerIndex = 0; u32LayerIndex < u32AvailableLayersCount; u32LayerIndex++)
			println(append_to_string("\t", vk_pAvailableLayers[u32LayerIndex].layerName, " (", VK_API_VERSION_MAJOR(vk_pAvailableLayers[u32LayerIndex].specVersion), ".", VK_API_VERSION_MINOR(vk_pAvailableLayers[u32LayerIndex].specVersion), ".",VK_API_VERSION_PATCH(vk_pAvailableLayers[u32LayerIndex].specVersion), " - ", vk_pAvailableLayers[u32LayerIndex].implementationVersion, "): ", vk_pAvailableLayers[u32LayerIndex].description));
		bool bLayersMissing = false;
		for (uint32_t u32LayersToLoadIndex = 0U; u32LayersToLoadIndex < u32LayersToLoadCount; u32LayersToLoadIndex++) {
			bool bFound = false;
			for (uint32_t u32AvailableLayersIndex = 0U; u32AvailableLayersIndex < u32AvailableLayersCount; u32AvailableLayersIndex++)
				if (std::strcmp(ppcLayersToLoad[u32LayersToLoadIndex], vk_pAvailableLayers[u32AvailableLayersIndex].layerName) == 0) {
					bFound = true;
					break;
				}
			if (!bFound) {
				RE_FATAL_ERROR(append_to_string("The requested Vulkan instance layer \"", ppcLayersToLoad[u32LayersToLoadIndex], "\" does not exist on this computer"));
				bLayersMissing = true;
			}
		}

		bool bInstanceCreationSuccessful = false;
		if (!bExtensionsMissing && !bLayersMissing) {
			VkApplicationInfo vk_appInfo = {};
			vk_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			std::string strAppName = get_app_name();
			vk_appInfo.pApplicationName = strAppName.c_str();
			vk_appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			vk_appInfo.pEngineName = "R-Engine";
			vk_appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			vk_appInfo.apiVersion = VK_API_VERSION_1_3;
			VkInstanceCreateInfo vk_instanceCreateInfo = {};
			vk_instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			vk_instanceCreateInfo.pApplicationInfo = &vk_appInfo;
			vk_instanceCreateInfo.enabledExtensionCount = u32ExtensionsToLoadCount;
			vk_instanceCreateInfo.ppEnabledExtensionNames = ppcExtensionsToLoad;
			vk_instanceCreateInfo.enabledLayerCount = u32LayersToLoadCount;
			vk_instanceCreateInfo.ppEnabledLayerNames = ppcLayersToLoad;
			VkResult vk_eSuccessResult;
			CATCH_SIGNAL(vk_eSuccessResult = pfn_vkCreateInstance(&vk_instanceCreateInfo, nullptr, &vk_hInstance));
			if (!CHECK_VK_RESULT(vk_eSuccessResult))
				RE_FATAL_ERROR("Failed creating Vulkan instance");
			bInstanceCreationSuccessful = vk_eSuccessResult == VK_SUCCESS;
		}
		CATCH_SIGNAL(delete[] ppcExtensionsToLoad);
		CATCH_SIGNAL(delete[] vk_pAvailableExtensions);
		CATCH_SIGNAL(delete[] ppcLayersToLoad);
		CATCH_SIGNAL(delete[] vk_pAvailableLayers);
		return bInstanceCreationSuccessful;
	}

	bool Vulkan::setup_validation_layers() {
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

	VkInstance Vulkan::get_instance() {
		return pInstance ? pInstance->vk_hInstance : nullptr;
	}

	bool Vulkan::is_valid() {
		return bValid;
	}

	bool check_vulkan_result(VkResult vk_eResult, const char* pcFile, const char* pcFunc, REuint u32Line) {
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
		error(pcFile, pcFunc, u32Line, append_to_string("The recently called Vulkan function threw an error: ", pcErrName, "\n", pcErrDetail).c_str(), true);
		return false;
	}

}
