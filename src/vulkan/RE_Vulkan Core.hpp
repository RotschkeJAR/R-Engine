#ifndef __RE_VULKAN_CORE_H__
#define __RE_VULKAN_CORE_H__

#include "../RE_Ext Header.hpp"
#ifdef RE_OS_WINDOWS
# define VK_USE_PLATFORM_WIN32_KHR
#elif defined RE_OS_LINUX
# define VK_USE_PLATFORM_XLIB_KHR
#endif
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace RE {
	
	class VulkanCore {
		private:
			bool valid;
#ifdef RE_OS_WINDOWS
			HMODULE hVulkan;
#elif defined RE_OS_LINUX
			void* libVulkan;
#endif

			void* loadFuncInstance(VkInstance instance, const char* funcName);
			void* loadFunc(const char* funcName);
			bool createInstance(std::vector<std::string>& extensionsToLoad, bool enableDebug);
			bool loadVulkan_1_0();
			bool loadVulkan_1_1();
			bool loadVulkan_1_2();
			bool loadVulkan_1_3();
			// bool loadVulkan_1_4();

		public:
			// Vulkan 1.0
			PFN_vkCreateInstance pfn_vkCreateInstance;
			PFN_vkDestroyInstance pfn_vkDestroyInstance;
			PFN_vkEnumeratePhysicalDevices pfn_vkEnumeratePhysicalDevices;
			PFN_vkGetPhysicalDeviceFeatures pfn_vkGetPhysicalDeviceFeatures;
			PFN_vkGetPhysicalDeviceFormatProperties pfn_vkGetPhysicalDeviceFormatProperties;
			PFN_vkGetPhysicalDeviceImageFormatProperties pfn_vkGetPhysicalDeviceImageFormatProperties;
			PFN_vkGetPhysicalDeviceProperties pfn_vkGetPhysicalDeviceProperties;
			PFN_vkGetPhysicalDeviceQueueFamilyProperties pfn_vkGetPhysicalDeviceQueueFamilyProperties;
			PFN_vkGetPhysicalDeviceMemoryProperties pfn_vkGetPhysicalDeviceMemoryProperties;
			PFN_vkGetInstanceProcAddr pfn_vkGetInstanceProcAddr;
			PFN_vkGetDeviceProcAddr pfn_vkGetDeviceProcAddr;
			PFN_vkCreateDevice pfn_vkCreateDevice;
			PFN_vkDestroyDevice pfn_vkDestroyDevice;
			PFN_vkEnumerateInstanceExtensionProperties pfn_vkEnumerateInstanceExtensionProperties;
			PFN_vkEnumerateDeviceExtensionProperties pfn_vkEnumerateDeviceExtensionProperties;
			PFN_vkEnumerateInstanceLayerProperties pfn_vkEnumerateInstanceLayerProperties;
			PFN_vkEnumerateDeviceLayerProperties pfn_vkEnumerateDeviceLayerProperties;
			PFN_vkGetDeviceQueue pfn_vkGetDeviceQueue;
			PFN_vkQueueSubmit pfn_vkQueueSubmit;
			PFN_vkQueueWaitIdle pfn_vkQueueWaitIdle;
			PFN_vkDeviceWaitIdle pfn_vkDeviceWaitIdle;
			PFN_vkAllocateMemory pfn_vkAllocateMemory;
			PFN_vkFreeMemory pfn_vkFreeMemory;
			PFN_vkMapMemory pfn_vkMapMemory;
			PFN_vkUnmapMemory pfn_vkUnmapMemory;
			PFN_vkFlushMappedMemoryRanges pfn_vkFlushMappedMemoryRanges;
			PFN_vkInvalidateMappedMemoryRanges pfn_vkInvalidateMappedMemoryRanges;
			PFN_vkGetDeviceMemoryCommitment pfn_vkGetDeviceMemoryCommitment;
			PFN_vkBindBufferMemory pfn_vkBindBufferMemory;
			PFN_vkBindImageMemory pfn_vkBindImageMemory;
			PFN_vkGetBufferMemoryRequirements pfn_vkGetBufferMemoryRequirements;
			PFN_vkGetImageMemoryRequirements pfn_vkGetImageMemoryRequirements;
			PFN_vkGetImageSparseMemoryRequirements pfn_vkGetImageSparseMemoryRequirements;
			PFN_vkGetPhysicalDeviceSparseImageFormatProperties pfn_vkGetPhysicalDeviceSparseImageFormatProperties;
			PFN_vkQueueBindSparse pfn_vkQueueBindSparse;
			PFN_vkCreateFence pfn_vkCreateFence;
			PFN_vkDestroyFence pfn_vkDestroyFence;
			PFN_vkResetFences pfn_vkResetFences;
			PFN_vkGetFenceStatus pfn_vkGetFenceStatus;
			PFN_vkWaitForFences pfn_vkWaitForFences;
			PFN_vkCreateSemaphore pfn_vkCreateSemaphore;
			PFN_vkDestroySemaphore pfn_vkDestroySemaphore;
			PFN_vkCreateEvent pfn_vkCreateEvent;
			PFN_vkDestroyEvent pfn_vkDestroyEvent;
			PFN_vkGetEventStatus pfn_vkGetEventStatus;
			PFN_vkSetEvent pfn_vkSetEvent;
			PFN_vkResetEvent pfn_vkResetEvent;
			PFN_vkCreateQueryPool pfn_vkCreateQueryPool;
			PFN_vkDestroyQueryPool pfn_vkDestroyQueryPool;
			PFN_vkGetQueryPoolResults pfn_vkGetQueryPoolResults;
			PFN_vkCreateBuffer pfn_vkCreateBuffer;
			PFN_vkDestroyBuffer pfn_vkDestroyBuffer;
			PFN_vkCreateBufferView pfn_vkCreateBufferView;
			PFN_vkDestroyBufferView pfn_vkDestroyBufferView;	
			PFN_vkCreateImage pfn_vkCreateImage;
			PFN_vkDestroyImage pfn_vkDestroyImage;
			PFN_vkGetImageSubresourceLayout pfn_vkGetImageSubresourceLayout;
			PFN_vkCreateImageView pfn_vkCreateImageView;
			PFN_vkDestroyImageView pfn_vkDestroyImageView;
			PFN_vkCreateShaderModule pfn_vkCreateShaderModule;
			PFN_vkDestroyShaderModule pfn_vkDestroyShaderModule;
			PFN_vkCreatePipelineCache pfn_vkCreatePipelineCache;
			PFN_vkDestroyPipelineCache pfn_vkDestroyPipelineCache;
			PFN_vkGetPipelineCacheData pfn_vkGetPipelineCacheData;
			PFN_vkMergePipelineCaches pfn_vkMergePipelineCaches;
			PFN_vkCreateGraphicsPipelines pfn_vkCreateGraphicsPipelines;
			PFN_vkCreateComputePipelines pfn_vkCreateComputePipelines;
			PFN_vkDestroyPipeline pfn_vkDestroyPipeline;
			PFN_vkCreatePipelineLayout pfn_vkCreatePipelineLayout;
			PFN_vkDestroyPipelineLayout pfn_vkDestroyPipelineLayout;
			PFN_vkCreateSampler pfn_vkCreateSampler;
			PFN_vkDestroySampler pfn_vkDestroySampler;
			PFN_vkCreateDescriptorSetLayout pfn_vkCreateDescriptorSetLayout;
			PFN_vkDestroyDescriptorSetLayout pfn_vkDestroyDescriptorSetLayout;
			PFN_vkCreateDescriptorPool pfn_vkCreateDescriptorPool;
			PFN_vkDestroyDescriptorPool pfn_vkDestroyDescriptorPool;
			PFN_vkResetDescriptorPool pfn_vkResetDescriptorPool;
			PFN_vkAllocateDescriptorSets pfn_vkAllocateDescriptorSets;
			PFN_vkFreeDescriptorSets pfn_vkFreeDescriptorSets;
			PFN_vkUpdateDescriptorSets pfn_vkUpdateDescriptorSets;
			PFN_vkCreateFramebuffer pfn_vkCreateFramebuffer;
			PFN_vkDestroyFramebuffer pfn_vkDestroyFramebuffer;
			PFN_vkCreateRenderPass pfn_vkCreateRenderPass;
			PFN_vkDestroyRenderPass pfn_vkDestroyRenderPass;
			PFN_vkGetRenderAreaGranularity pfn_vkGetRenderAreaGranularity;
			PFN_vkCreateCommandPool pfn_vkCreateCommandPool;
			PFN_vkDestroyCommandPool pfn_vkDestroyCommandPool;
			PFN_vkResetCommandPool pfn_vkResetCommandPool;
			PFN_vkAllocateCommandBuffers pfn_vkAllocateCommandBuffers;
			PFN_vkFreeCommandBuffers pfn_vkFreeCommandBuffers;
			PFN_vkBeginCommandBuffer pfn_vkBeginCommandBuffer;
			PFN_vkEndCommandBuffer pfn_vkEndCommandBuffer;
			PFN_vkResetCommandBuffer pfn_vkResetCommandBuffer;
			PFN_vkCmdBindPipeline pfn_vkCmdBindPipeline;
			PFN_vkCmdSetViewport pfn_vkCmdSetViewport;
			PFN_vkCmdSetScissor pfn_vkCmdSetScissor;
			PFN_vkCmdSetLineWidth pfn_vkCmdSetLineWidth;
			PFN_vkCmdSetDepthBias pfn_vkCmdSetDepthBias;
			PFN_vkCmdSetBlendConstants pfn_vkCmdSetBlendConstants;
			PFN_vkCmdSetDepthBounds pfn_vkCmdSetDepthBounds;
			PFN_vkCmdSetStencilCompareMask pfn_vkCmdSetStencilCompareMask;
			PFN_vkCmdSetStencilWriteMask pfn_vkCmdSetStencilWriteMask;
			PFN_vkCmdSetStencilReference pfn_vkCmdSetStencilReference;
			PFN_vkCmdBindDescriptorSets pfn_vkCmdBindDescriptorSets;
			PFN_vkCmdBindIndexBuffer pfn_vkCmdBindIndexBuffer;
			PFN_vkCmdBindVertexBuffers pfn_vkCmdBindVertexBuffers;
			PFN_vkCmdDraw pfn_vkCmdDraw;
			PFN_vkCmdDrawIndexed pfn_vkCmdDrawIndexed;
			PFN_vkCmdDrawIndirect pfn_vkCmdDrawIndirect;
			PFN_vkCmdDrawIndexedIndirect pfn_vkCmdDrawIndexedIndirect;
			PFN_vkCmdDispatch pfn_vkCmdDispatch;
			PFN_vkCmdDispatchIndirect pfn_vkCmdDispatchIndirect;
			PFN_vkCmdCopyBuffer pfn_vkCmdCopyBuffer;
			PFN_vkCmdCopyImage pfn_vkCmdCopyImage;
			PFN_vkCmdBlitImage pfn_vkCmdBlitImage;
			PFN_vkCmdCopyBufferToImage pfn_vkCmdCopyBufferToImage;
			PFN_vkCmdCopyImageToBuffer pfn_vkCmdCopyImageToBuffer;
			PFN_vkCmdUpdateBuffer pfn_vkCmdUpdateBuffer;
			PFN_vkCmdFillBuffer pfn_vkCmdFillBuffer;
			PFN_vkCmdClearColorImage pfn_vkCmdClearColorImage;
			PFN_vkCmdClearDepthStencilImage pfn_vkCmdClearDepthStencilImage;
			PFN_vkCmdClearAttachments pfn_vkCmdClearAttachments;
			PFN_vkCmdResolveImage pfn_vkCmdResolveImage;
			PFN_vkCmdSetEvent pfn_vkCmdSetEvent;
			PFN_vkCmdResetEvent pfn_vkCmdResetEvent;
			PFN_vkCmdWaitEvents pfn_vkCmdWaitEvents;
			PFN_vkCmdPipelineBarrier pfn_vkCmdPipelineBarrier;
			PFN_vkCmdBeginQuery pfn_vkCmdBeginQuery;
			PFN_vkCmdEndQuery pfn_vkCmdEndQuery;
			PFN_vkCmdResetQueryPool pfn_vkCmdResetQueryPool;
			PFN_vkCmdWriteTimestamp pfn_vkCmdWriteTimestamp;
			PFN_vkCmdCopyQueryPoolResults pfn_vkCmdCopyQueryPoolResults;
			PFN_vkCmdPushConstants pfn_vkCmdPushConstants;
			PFN_vkCmdBeginRenderPass pfn_vkCmdBeginRenderPass;
			PFN_vkCmdNextSubpass pfn_vkCmdNextSubpass;
			PFN_vkCmdEndRenderPass pfn_vkCmdEndRenderPass;
			PFN_vkCmdExecuteCommands pfn_vkCmdExecuteCommands;

			// Vulkan 1.1
			PFN_vkEnumerateInstanceVersion pfn_vkEnumerateInstanceVersion;
			PFN_vkBindBufferMemory2 pfn_vkBindBufferMemory2;
			PFN_vkBindImageMemory2 pfn_vkBindImageMemory2;
			PFN_vkGetDeviceGroupPeerMemoryFeatures pfn_vkGetDeviceGroupPeerMemoryFeatures;
			PFN_vkCmdSetDeviceMask pfn_vkCmdSetDeviceMask;
			PFN_vkCmdDispatchBase pfn_vkCmdDispatchBase;
			PFN_vkEnumeratePhysicalDeviceGroups pfn_vkEnumeratePhysicalDeviceGroups;
			PFN_vkGetImageMemoryRequirements2 pfn_vkGetImageMemoryRequirements2;
			PFN_vkGetBufferMemoryRequirements2 pfn_vkGetBufferMemoryRequirements2;
			PFN_vkGetImageSparseMemoryRequirements2 pfn_vkGetImageSparseMemoryRequirements2;
			PFN_vkGetPhysicalDeviceFeatures2 pfn_vkGetPhysicalDeviceFeatures2;
			PFN_vkGetPhysicalDeviceProperties2 pfn_vkGetPhysicalDeviceProperties2;
			PFN_vkGetPhysicalDeviceFormatProperties2 pfn_vkGetPhysicalDeviceFormatProperties2;
			PFN_vkGetPhysicalDeviceImageFormatProperties2 pfn_vkGetPhysicalDeviceImageFormatProperties2;
			PFN_vkGetPhysicalDeviceQueueFamilyProperties2 pfn_vkGetPhysicalDeviceQueueFamilyProperties2;
			PFN_vkGetPhysicalDeviceMemoryProperties2 pfn_vkGetPhysicalDeviceMemoryProperties2;
			PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 pfn_vkGetPhysicalDeviceSparseImageFormatProperties2;
			PFN_vkTrimCommandPool pfn_vkTrimCommandPool;
			PFN_vkGetDeviceQueue2 pfn_vkGetDeviceQueue2;
			PFN_vkCreateSamplerYcbcrConversion pfn_vkCreateSamplerYcbcrConversion;
			PFN_vkDestroySamplerYcbcrConversion pfn_vkDestroySamplerYcbcrConversion;
			PFN_vkCreateDescriptorUpdateTemplate pfn_vkCreateDescriptorUpdateTemplate;
			PFN_vkDestroyDescriptorUpdateTemplate pfn_vkDestroyDescriptorUpdateTemplate;
			PFN_vkUpdateDescriptorSetWithTemplate pfn_vkUpdateDescriptorSetWithTemplate;
			PFN_vkGetPhysicalDeviceExternalBufferProperties pfn_vkGetPhysicalDeviceExternalBufferProperties;
			PFN_vkGetPhysicalDeviceExternalFenceProperties pfn_vkGetPhysicalDeviceExternalFenceProperties;
			PFN_vkGetPhysicalDeviceExternalSemaphoreProperties pfn_vkGetPhysicalDeviceExternalSemaphoreProperties;
			PFN_vkGetDescriptorSetLayoutSupport pfn_vkGetDescriptorSetLayoutSupport;

			// Vulkan 1.2
			PFN_vkCmdDrawIndirectCount pfn_vkCmdDrawIndirectCount;
			PFN_vkCmdDrawIndexedIndirectCount pfn_vkCmdDrawIndexedIndirectCount;
			PFN_vkCreateRenderPass2 pfn_vkCreateRenderPass2;
			PFN_vkCmdBeginRenderPass2 pfn_vkCmdBeginRenderPass2;
			PFN_vkCmdNextSubpass2 pfn_vkCmdNextSubpass2;
			PFN_vkCmdEndRenderPass2 pfn_vkCmdEndRenderPass2;
			PFN_vkResetQueryPool pfn_vkResetQueryPool;
			PFN_vkGetSemaphoreCounterValue pfn_vkGetSemaphoreCounterValue;
			PFN_vkWaitSemaphores pfn_vkWaitSemaphores;
			PFN_vkSignalSemaphore pfn_vkSignalSemaphore;
			PFN_vkGetBufferDeviceAddress pfn_vkGetBufferDeviceAddress;
			PFN_vkGetBufferOpaqueCaptureAddress pfn_vkGetBufferOpaqueCaptureAddress;
			PFN_vkGetDeviceMemoryOpaqueCaptureAddress pfn_vkGetDeviceMemoryOpaqueCaptureAddress;

			// Vulkan 1.3
			PFN_vkGetPhysicalDeviceToolProperties pfn_vkGetPhysicalDeviceToolProperties;
			PFN_vkCreatePrivateDataSlot pfn_vkCreatePrivateDataSlot;
			PFN_vkDestroyPrivateDataSlot pfn_vkDestroyPrivateDataSlot;
			PFN_vkSetPrivateData pfn_vkSetPrivateData;
			PFN_vkGetPrivateData pfn_vkGetPrivateData;
			PFN_vkCmdSetEvent2 pfn_vkCmdSetEvent2;
			PFN_vkCmdResetEvent2 pfn_vkCmdResetEvent2;
			PFN_vkCmdWaitEvents2 pfn_vkCmdWaitEvents2;
			PFN_vkCmdPipelineBarrier2 pfn_vkCmdPipelineBarrier2;
			PFN_vkCmdWriteTimestamp2 pfn_vkCmdWriteTimestamp2;
			PFN_vkQueueSubmit2 pfn_vkQueueSubmit2;
			PFN_vkCmdCopyBuffer2 pfn_vkCmdCopyBuffer2;
			PFN_vkCmdCopyImage2 pfn_vkCmdCopyImage2;
			PFN_vkCmdCopyBufferToImage2 pfn_vkCmdCopyBufferToImage2;
			PFN_vkCmdCopyImageToBuffer2 pfn_vkCmdCopyImageToBuffer2;
			PFN_vkCmdBlitImage2 pfn_vkCmdBlitImage2;
			PFN_vkCmdResolveImage2 pfn_vkCmdResolveImage2;
			PFN_vkCmdBeginRendering pfn_vkCmdBeginRendering;
			PFN_vkCmdEndRendering pfn_vkCmdEndRendering;
			PFN_vkCmdSetCullMode pfn_vkCmdSetCullMode;
			PFN_vkCmdSetFrontFace pfn_vkCmdSetFrontFace;
			PFN_vkCmdSetPrimitiveTopology pfn_vkCmdSetPrimitiveTopology;
			PFN_vkCmdSetViewportWithCount pfn_vkCmdSetViewportWithCount;
			PFN_vkCmdSetScissorWithCount pfn_vkCmdSetScissorWithCount;
			PFN_vkCmdBindVertexBuffers2 pfn_vkCmdBindVertexBuffers2;
			PFN_vkCmdSetDepthTestEnable pfn_vkCmdSetDepthTestEnable;
			PFN_vkCmdSetDepthWriteEnable pfn_vkCmdSetDepthWriteEnable;
			PFN_vkCmdSetDepthCompareOp pfn_vkCmdSetDepthCompareOp;
			PFN_vkCmdSetDepthBoundsTestEnable pfn_vkCmdSetDepthBoundsTestEnable;
			PFN_vkCmdSetStencilTestEnable pfn_vkCmdSetStencilTestEnable;
			PFN_vkCmdSetStencilOp pfn_vkCmdSetStencilOp;
			PFN_vkCmdSetRasterizerDiscardEnable pfn_vkCmdSetRasterizerDiscardEnable;
			PFN_vkCmdSetDepthBiasEnable pfn_vkCmdSetDepthBiasEnable;
			PFN_vkCmdSetPrimitiveRestartEnable pfn_vkCmdSetPrimitiveRestartEnable;
			PFN_vkGetDeviceBufferMemoryRequirements pfn_vkGetDeviceBufferMemoryRequirements;
			PFN_vkGetDeviceImageMemoryRequirements pfn_vkGetDeviceImageMemoryRequirements;
			PFN_vkGetDeviceImageSparseMemoryRequirements pfn_vkGetDeviceImageSparseMemoryRequirements;

			// Vulkan 1.4
			/* PFN_vkCmdSetLineStipple pfn_vkCmdSetLineStipple;
			PFN_vkMapMemory2 pfn_vkMapMemory2;
			PFN_vkUnmapMemory2 pfn_vkUnmapMemory2;
			PFN_vkCmdBindIndexBuffer2 pfn_vkCmdBindIndexBuffer2;
			PFN_vkGetRenderingAreaGranularity pfn_vkGetRenderingAreaGranularity;
			PFN_vkGetDeviceImageSubresourceLayout pfn_vkGetDeviceImageSubresourceLayout;
			PFN_vkGetImageSubresourceLayout2 pfn_vkGetImageSubresourceLayout2;
			PFN_vkCmdPushDescriptorSet pfn_vkCmdPushDescriptorSet;
			PFN_vkCmdPushDescriptorSetWithTemplate pfn_vkCmdPushDescriptorSetWithTemplate;
			PFN_vkCmdSetRenderingAttachmentLocations pfn_vkCmdSetRenderingAttachmentLocations;
			PFN_vkCmdSetRenderingInputAttachmentIndices pfn_vkCmdSetRenderingInputAttachmentIndices;
			PFN_vkCmdBindDescriptorSets2 pfn_vkCmdBindDescriptorSets2;
			PFN_vkCmdPushConstants2 pfn_vkCmdPushConstants2;
			PFN_vkCmdPushDescriptorSet2 pfn_vkCmdPushDescriptorSet2;
			PFN_vkCmdPushDescriptorSetWithTemplate2 pfn_vkCmdPushDescriptorSetWithTemplate2;
			PFN_vkCopyMemoryToImage pfn_vkCopyMemoryToImage;
			PFN_vkCopyImageToMemory pfn_vkCopyImageToMemory;
			PFN_vkCopyImageToImage pfn_vkCopyImageToImage;
			PFN_vkTransitionImageLayout pfn_vkTransitionImageLayout; */

			static VulkanCore* instance;
			VkInstance internalInstance;

			VulkanCore() = delete;
			VulkanCore(std::vector<std::string>& extensionsToLoad, bool enableDebug);
			~VulkanCore();
			bool isValid();
			bool checkVulkanResult(VkResult result);
	};

// Vulkan 1.0
#define vkCreateInstance(T) VulkanCore::instance->pfn_vkCreateInstance(T)
#define vkDestroyInstance(T) VulkanCore::instance->pfn_vkDestroyInstance(T)
#define vkEnumeratePhysicalDevices(T) VulkanCore::instance->pfn_vkEnumeratePhysicalDevices(T)
#define vkGetPhysicalDeviceFeatures(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceFeatures(T)
#define vkGetPhysicalDeviceFormatProperties(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceFormatProperties(T)
#define vkGetPhysicalDeviceImageFormatProperties(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceImageFormatProperties(T)
#define vkGetPhysicalDeviceProperties(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceProperties(T)
#define vkGetPhysicalDeviceQueueFamilyProperties(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceQueueFamilyProperties(T)
#define vkGetPhysicalDeviceMemoryProperties(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceMemoryProperties(T)
#define vkGetInstanceProcAddr(T) VulkanCore::instance->pfn_vkGetInstanceProcAddr(T)
#define vkGetDeviceProcAddr(T) VulkanCore::instance->pfn_vkGetDeviceProcAddr(T)
#define vkCreateDevice(T) VulkanCore::instance->pfn_vkCreateDevice(T)
#define vkDestroyDevice(T) VulkanCore::instance->pfn_vkDestroyDevice(T)
#define vkEnumerateInstanceExtensionProperties(T) VulkanCore::instance->pfn_vkEnumerateInstanceExtensionProperties(T)
#define vkEnumerateDeviceExtensionProperties(T) VulkanCore::instance->pfn_vkEnumerateDeviceExtensionProperties(T)
#define vkEnumerateInstanceLayerProperties(T) VulkanCore::instance->pfn_vkEnumerateInstanceLayerProperties(T)
#define vkEnumerateDeviceLayerProperties(T) VulkanCore::instance->pfn_vkEnumerateDeviceLayerProperties(T)
#define vkGetDeviceQueue(T) VulkanCore::instance->pfn_vkGetDeviceQueue(T)
#define vkQueueSubmit(T) VulkanCore::instance->pfn_vkQueueSubmit(T)
#define vkQueueWaitIdle(T) VulkanCore::instance->pfn_vkQueueWaitIdle(T)
#define vkDeviceWaitIdle(T) VulkanCore::instance->pfn_vkDeviceWaitIdle(T)
#define vkAllocateMemory(T) VulkanCore::instance->pfn_vkAllocateMemory(T)
#define vkFreeMemory(T) VulkanCore::instance->pfn_vkFreeMemory(T)
#define vkMapMemory(T) VulkanCore::instance->pfn_vkMapMemory(T)
#define vkUnmapMemory(T) VulkanCore::instance->pfn_vkUnmapMemory(T)
#define vkFlushMappedMemoryRanges(T) VulkanCore::instance->pfn_vkFlushMappedMemoryRanges(T)
#define vkInvalidateMappedMemoryRanges(T) VulkanCore::instance->pfn_vkInvalidateMappedMemoryRanges(T)
#define vkGetDeviceMemoryCommitment(T) VulkanCore::instance->pfn_vkGetDeviceMemoryCommitment(T)
#define vkBindBufferMemory(T) VulkanCore::instance->pfn_vkBindBufferMemory(T)
#define vkBindImageMemory(T) VulkanCore::instance->pfn_vkBindImageMemory(T)
#define vkGetBufferMemoryRequirements(T) VulkanCore::instance->pfn_vkGetBufferMemoryRequirements(T)
#define vkGetImageMemoryRequirements(T) VulkanCore::instance->pfn_vkGetImageMemoryRequirements(T)
#define vkGetImageSparseMemoryRequirements(T) VulkanCore::instance->pfn_vkGetImageSparseMemoryRequirements(T)
#define vkGetPhysicalDeviceSparseImageFormatProperties(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceSparseImageFormatProperties(T)
#define vkQueueBindSparse(T) VulkanCore::instance->pfn_vkQueueBindSparse(T)
#define vkCreateFence(T) VulkanCore::instance->pfn_vkCreateFence(T)
#define vkDestroyFence(T) VulkanCore::instance->pfn_vkDestroyFence(T)
#define vkResetFences(T) VulkanCore::instance->pfn_vkResetFences(T)
#define vkGetFenceStatus(T) VulkanCore::instance->pfn_vkGetFenceStatus(T)
#define vkWaitForFences(T) VulkanCore::instance->pfn_vkWaitForFences(T)
#define vkCreateSemaphore(T) VulkanCore::instance->pfn_vkCreateSemaphore(T)
#define vkDestroySemaphore(T) VulkanCore::instance->pfn_vkDestroySemaphore(T)
#define vkCreateEvent(T) VulkanCore::instance->pfn_vkCreateEvent(T)
#define vkDestroyEvent(T) VulkanCore::instance->pfn_vkDestroyEvent(T)
#define vkGetEventStatus(T) VulkanCore::instance->pfn_vkGetEventStatus(T)
#define vkSetEvent(T) VulkanCore::instance->pfn_vkSetEvent(T)
#define vkResetEvent(T) VulkanCore::instance->pfn_vkResetEvent(T)
#define vkCreateQueryPool(T) VulkanCore::instance->pfn_vkCreateQueryPool(T)
#define vkDestroyQueryPool(T) VulkanCore::instance->pfn_vkDestroyQueryPool(T)
#define vkGetQueryPoolResults(T) VulkanCore::instance->pfn_vkGetQueryPoolResults(T)
#define vkCreateBuffer(T) VulkanCore::instance->pfn_vkCreateBuffer(T)
#define vkDestroyBuffer(T) VulkanCore::instance->pfn_vkDestroyBuffer(T)
#define vkCreateBufferView(T) VulkanCore::instance->pfn_vkCreateBufferView(T)
#define vkDestroyBufferView(T) VulkanCore::instance->pfn_vkDestroyBufferView(T)
#define vkCreateImage(T) VulkanCore::instance->pfn_vkCreateImage(T)
#define vkDestroyImage(T) VulkanCore::instance->pfn_vkDestroyImage(T)
#define vkGetImageSubresourceLayout(T) VulkanCore::instance->pfn_vkGetImageSubresourceLayout(T)
#define vkCreateImageView(T) VulkanCore::instance->pfn_vkCreateImageView(T)
#define vkDestroyImageView(T) VulkanCore::instance->pfn_vkDestroyImageView(T)
#define vkCreateShaderModule(T) VulkanCore::instance->pfn_vkCreateShaderModule(T)
#define vkDestroyShaderModule(T) VulkanCore::instance->pfn_vkDestroyShaderModule(T)
#define vkCreatePipelineCache(T) VulkanCore::instance->pfn_vkCreatePipelineCache(T)
#define vkDestroyPipelineCache(T) VulkanCore::instance->pfn_vkDestroyPipelineCache(T)
#define vkGetPipelineCacheData(T) VulkanCore::instance->pfn_vkGetPipelineCacheData(T)
#define vkMergePipelineCaches(T) VulkanCore::instance->pfn_vkMergePipelineCaches(T)
#define vkCreateGraphicsPipelines(T) VulkanCore::instance->pfn_vkCreateGraphicsPipelines(T)
#define vkCreateComputePipelines(T) VulkanCore::instance->pfn_vkCreateComputePipelines(T)
#define vkDestroyPipeline(T) VulkanCore::instance->pfn_vkDestroyPipeline(T)
#define vkCreatePipelineLayout(T) VulkanCore::instance->pfn_vkCreatePipelineLayout(T)
#define vkDestroyPipelineLayout(T) VulkanCore::instance->pfn_vkDestroyPipelineLayout(T)
#define vkCreateSampler(T) VulkanCore::instance->pfn_vkCreateSampler(T)
#define vkDestroySampler(T) VulkanCore::instance->pfn_vkDestroySampler(T)
#define vkCreateDescriptorSetLayout(T) VulkanCore::instance->pfn_vkCreateDescriptorSetLayout(T)
#define vkDestroyDescriptorSetLayout(T) VulkanCore::instance->pfn_vkDestroyDescriptorSetLayout(T)
#define vkCreateDescriptorPool(T) VulkanCore::instance->pfn_vkCreateDescriptorPool(T)
#define vkDestroyDescriptorPool(T) VulkanCore::instance->pfn_vkDestroyDescriptorPool(T)
#define vkResetDescriptorPool(T) VulkanCore::instance->pfn_vkResetDescriptorPool(T)
#define vkAllocateDescriptorSets(T) VulkanCore::instance->pfn_vkAllocateDescriptorSets(T)
#define vkFreeDescriptorSets(T) VulkanCore::instance->pfn_vkFreeDescriptorSets(T)
#define vkUpdateDescriptorSets(T) VulkanCore::instance->pfn_vkUpdateDescriptorSets(T)
#define vkCreateFramebuffer(T) VulkanCore::instance->pfn_vkCreateFramebuffer(T)
#define vkDestroyFramebuffer(T) VulkanCore::instance->pfn_vkDestroyFramebuffer(T)
#define vkCreateRenderPass(T) VulkanCore::instance->pfn_vkCreateRenderPass(T)
#define vkDestroyRenderPass(T) VulkanCore::instance->pfn_vkDestroyRenderPass(T)
#define vkGetRenderAreaGranularity(T) VulkanCore::instance->pfn_vkGetRenderAreaGranularity(T)
#define vkCreateCommandPool(T) VulkanCore::instance->pfn_vkCreateCommandPool(T)
#define vkDestroyCommandPool(T) VulkanCore::instance->pfn_vkDestroyCommandPool(T)
#define vkResetCommandPool(T) VulkanCore::instance->pfn_vkResetCommandPool(T)
#define vkAllocateCommandBuffers(T) VulkanCore::instance->pfn_vkAllocateCommandBuffers(T)
#define vkFreeCommandBuffers(T) VulkanCore::instance->pfn_vkFreeCommandBuffers(T)
#define vkBeginCommandBuffer(T) VulkanCore::instance->pfn_vkBeginCommandBuffer(T)
#define vkEndCommandBuffer(T) VulkanCore::instance->pfn_vkEndCommandBuffer(T)
#define vkResetCommandBuffer(T) VulkanCore::instance->pfn_vkResetCommandBuffer(T)
#define vkCmdBindPipeline(T) VulkanCore::instance->pfn_vkCmdBindPipeline(T)
#define vkCmdSetViewport(T) VulkanCore::instance->pfn_vkCmdSetViewport(T)
#define vkCmdSetScissor(T) VulkanCore::instance->pfn_vkCmdSetScissor(T)
#define vkCmdSetLineWidth(T) VulkanCore::instance->pfn_vkCmdSetLineWidth(T)
#define vkCmdSetDepthBias(T) VulkanCore::instance->pfn_vkCmdSetDepthBias(T)
#define vkCmdSetBlendConstants(T) VulkanCore::instance->pfn_vkCmdSetBlendConstants(T)
#define vkCmdSetDepthBounds(T) VulkanCore::instance->pfn_vkCmdSetDepthBounds(T)
#define vkCmdSetStencilCompareMask(T) VulkanCore::instance->pfn_vkCmdSetStencilCompareMask(T)
#define vkCmdSetStencilWriteMask(T) VulkanCore::instance->pfn_vkCmdSetStencilWriteMask(T)
#define vkCmdSetStencilReference(T) VulkanCore::instance->pfn_vkCmdSetStencilReference(T)
#define vkCmdBindDescriptorSets(T) VulkanCore::instance->pfn_vkCmdBindDescriptorSets(T)
#define vkCmdBindIndexBuffer(T) VulkanCore::instance->pfn_vkCmdBindIndexBuffer(T)
#define vkCmdBindVertexBuffers(T) VulkanCore::instance->pfn_vkCmdBindVertexBuffers(T)
#define vkCmdDraw(T) VulkanCore::instance->pfn_vkCmdDraw(T)
#define vkCmdDrawIndexed(T) VulkanCore::instance->pfn_vkCmdDrawIndexed(T)
#define vkCmdDrawIndirect(T) VulkanCore::instance->pfn_vkCmdDrawIndirect(T)
#define vkCmdDrawIndexedIndirect(T) VulkanCore::instance->pfn_vkCmdDrawIndexedIndirect(T)
#define vkCmdDispatch(T) VulkanCore::instance->pfn_vkCmdDispatch(T)
#define vkCmdDispatchIndirect(T) VulkanCore::instance->pfn_vkCmdDispatchIndirect(T)
#define vkCmdCopyBuffer(T) VulkanCore::instance->pfn_vkCmdCopyBuffer(T)
#define vkCmdCopyImage(T) VulkanCore::instance->pfn_vkCmdCopyImage(T)
#define vkCmdBlitImage(T) VulkanCore::instance->pfn_vkCmdBlitImage(T)
#define vkCmdCopyBufferToImage(T) VulkanCore::instance->pfn_vkCmdCopyBufferToImage(T)
#define vkCmdCopyImageToBuffer(T) VulkanCore::instance->pfn_vkCmdCopyImageToBuffer(T)
#define vkCmdUpdateBuffer(T) VulkanCore::instance->pfn_vkCmdUpdateBuffer(T)
#define vkCmdFillBuffer(T) VulkanCore::instance->pfn_vkCmdFillBuffer(T)
#define vkCmdClearColorImage(T) VulkanCore::instance->pfn_vkCmdClearColorImage(T)
#define vkCmdClearDepthStencilImage(T) VulkanCore::instance->pfn_vkCmdClearDepthStencilImage(T)
#define vkCmdClearAttachments(T) VulkanCore::instance->pfn_vkCmdClearAttachments(T)
#define vkCmdResolveImage(T) VulkanCore::instance->pfn_vkCmdResolveImage(T)
#define vkCmdSetEvent(T) VulkanCore::instance->pfn_vkCmdSetEvent(T)
#define vkCmdResetEvent(T) VulkanCore::instance->pfn_vkCmdResetEvent(T)
#define vkCmdWaitEvents(T) VulkanCore::instance->pfn_vkCmdWaitEvents(T)
#define vkCmdPipelineBarrier(T) VulkanCore::instance->pfn_vkCmdPipelineBarrier(T)
#define vkCmdBeginQuery(T) VulkanCore::instance->pfn_vkCmdBeginQuery(T)
#define vkCmdEndQuery(T) VulkanCore::instance->pfn_vkCmdEndQuery(T)
#define vkCmdResetQueryPool(T) VulkanCore::instance->pfn_vkCmdResetQueryPool(T)
#define vkCmdWriteTimestamp(T) VulkanCore::instance->pfn_vkCmdWriteTimestamp(T)
#define vkCmdCopyQueryPoolResults(T) VulkanCore::instance->pfn_vkCmdCopyQueryPoolResults(T)
#define vkCmdPushConstants(T) VulkanCore::instance->pfn_vkCmdPushConstants(T)
#define vkCmdBeginRenderPass(T) VulkanCore::instance->pfn_vkCmdBeginRenderPass(T)
#define vkCmdNextSubpass(T) VulkanCore::instance->pfn_vkCmdNextSubpass(T)
#define vkCmdEndRenderPass(T) VulkanCore::instance->pfn_vkCmdEndRenderPass(T)
#define vkCmdExecuteCommands(T) VulkanCore::instance->pfn_vkCmdExecuteCommands(T)

// Vulkan 1.1
#define vkEnumerateInstanceVersion(T) VulkanCore::instance->pfn_vkEnumerateInstanceVersion(T)
#define vkBindBufferMemory2(T) VulkanCore::instance->pfn_vkBindBufferMemory2(T)
#define vkBindImageMemory2(T) VulkanCore::instance->pfn_vkBindImageMemory2(T)
#define vkGetDeviceGroupPeerMemoryFeatures(T) VulkanCore::instance->pfn_vkGetDeviceGroupPeerMemoryFeatures(T)
#define vkCmdSetDeviceMask(T) VulkanCore::instance->pfn_vkCmdSetDeviceMask(T)
#define vkCmdDispatchBase(T) VulkanCore::instance->pfn_vkCmdDispatchBase(T)
#define vkEnumeratePhysicalDeviceGroups(T) VulkanCore::instance->pfn_vkEnumeratePhysicalDeviceGroups(T)
#define vkGetImageMemoryRequirements2(T) VulkanCore::instance->pfn_vkGetImageMemoryRequirements2(T)
#define vkGetBufferMemoryRequirements2(T) VulkanCore::instance->pfn_vkGetBufferMemoryRequirements2(T)
#define vkGetImageSparseMemoryRequirements2(T) VulkanCore::instance->pfn_vkGetImageSparseMemoryRequirements2(T)
#define vkGetPhysicalDeviceFeatures2(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceFeatures2(T)
#define vkGetPhysicalDeviceProperties2(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceProperties2(T)
#define vkGetPhysicalDeviceFormatProperties2(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceFormatProperties2(T)
#define vkGetPhysicalDeviceImageFormatProperties2(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceImageFormatProperties2(T)
#define vkGetPhysicalDeviceQueueFamilyProperties2(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceQueueFamilyProperties2(T)
#define vkGetPhysicalDeviceMemoryProperties2(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceMemoryProperties2(T)
#define vkGetPhysicalDeviceSparseImageFormatProperties2(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceSparseImageFormatProperties2(T)
#define vkTrimCommandPool(T) VulkanCore::instance->pfn_vkTrimCommandPool(T)
#define vkGetDeviceQueue2(T) VulkanCore::instance->pfn_vkGetDeviceQueue2(T)
#define vkCreateSamplerYcbcrConversion(T) VulkanCore::instance->pfn_vkCreateSamplerYcbcrConversion(T)
#define vkDestroySamplerYcbcrConversion(T) VulkanCore::instance->pfn_vkDestroySamplerYcbcrConversion(T)
#define vkCreateDescriptorUpdateTemplate(T) VulkanCore::instance->pfn_vkCreateDescriptorUpdateTemplate(T)
#define vkDestroyDescriptorUpdateTemplate(T) VulkanCore::instance->pfn_vkDestroyDescriptorUpdateTemplate(T)
#define vkUpdateDescriptorSetWithTemplate(T) VulkanCore::instance->pfn_vkUpdateDescriptorSetWithTemplate(T)
#define vkGetPhysicalDeviceExternalBufferProperties(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceExternalBufferProperties(T)
#define vkGetPhysicalDeviceExternalFenceProperties(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceExternalFenceProperties(T)
#define vkGetPhysicalDeviceExternalSemaphoreProperties(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceExternalSemaphoreProperties(T)
#define vkGetDescriptorSetLayoutSupport(T) VulkanCore::instance->pfn_vkGetDescriptorSetLayoutSupport(T)

// Vulkan 1.2
#define vkCmdDrawIndirectCount(T) VulkanCore::instance->pfn_vkCmdDrawIndirectCount(T)
#define vkCmdDrawIndexedIndirectCount(T) VulkanCore::instance->pfn_vkCmdDrawIndexedIndirectCount(T)
#define vkCreateRenderPass2(T) VulkanCore::instance->pfn_vkCreateRenderPass2(T)
#define vkCmdBeginRenderPass2(T) VulkanCore::instance->pfn_vkCmdBeginRenderPass2(T)
#define vkCmdNextSubpass2(T) VulkanCore::instance->pfn_vkCmdNextSubpass2(T)
#define vkCmdEndRenderPass2(T) VulkanCore::instance->pfn_vkCmdEndRenderPass2(T)
#define vkResetQueryPool(T) VulkanCore::instance->pfn_vkResetQueryPool(T)
#define vkGetSemaphoreCounterValue(T) VulkanCore::instance->pfn_vkGetSemaphoreCounterValue(T)
#define vkWaitSemaphores(T) VulkanCore::instance->pfn_vkWaitSemaphores(T)
#define vkSignalSemaphore(T) VulkanCore::instance->pfn_vkSignalSemaphore(T)
#define vkGetBufferDeviceAddress(T) VulkanCore::instance->pfn_vkGetBufferDeviceAddress(T)
#define vkGetBufferOpaqueCaptureAddress(T) VulkanCore::instance->pfn_vkGetBufferOpaqueCaptureAddress(T)
#define vkGetDeviceMemoryOpaqueCaptureAddress(T) VulkanCore::instance->pfn_vkGetDeviceMemoryOpaqueCaptureAddress(T)

// Vulkan 1.3
#define vkGetPhysicalDeviceToolProperties(T) VulkanCore::instance->pfn_vkGetPhysicalDeviceToolProperties(T)
#define vkCreatePrivateDataSlot(T) VulkanCore::instance->pfn_vkCreatePrivateDataSlot(T)
#define vkDestroyPrivateDataSlot(T) VulkanCore::instance->pfn_vkDestroyPrivateDataSlot(T)
#define vkSetPrivateData(T) VulkanCore::instance->pfn_vkSetPrivateData(T)
#define vkGetPrivateData(T) VulkanCore::instance->pfn_vkGetPrivateData(T)
#define vkCmdSetEvent2(T) VulkanCore::instance->pfn_vkCmdSetEvent2(T)
#define vkCmdResetEvent2(T) VulkanCore::instance->pfn_vkCmdResetEvent2(T)
#define vkCmdWaitEvents2(T) VulkanCore::instance->pfn_vkCmdWaitEvents2(T)
#define vkCmdPipelineBarrier2(T) VulkanCore::instance->pfn_vkCmdPipelineBarrier2(T)
#define vkCmdWriteTimestamp2(T) VulkanCore::instance->pfn_vkCmdWriteTimestamp2(T)
#define vkQueueSubmit2(T) VulkanCore::instance->pfn_vkQueueSubmit2(T)
#define vkCmdCopyBuffer2(T) VulkanCore::instance->pfn_vkCmdCopyBuffer2(T)
#define vkCmdCopyImage2(T) VulkanCore::instance->pfn_vkCmdCopyImage2(T)
#define vkCmdCopyBufferToImage2(T) VulkanCore::instance->pfn_vkCmdCopyBufferToImage2(T)
#define vkCmdCopyImageToBuffer2(T) VulkanCore::instance->pfn_vkCmdCopyImageToBuffer2(T)
#define vkCmdBlitImage2(T) VulkanCore::instance->pfn_vkCmdBlitImage2(T)
#define vkCmdResolveImage2(T) VulkanCore::instance->pfn_vkCmdResolveImage2(T)
#define vkCmdBeginRendering(T) VulkanCore::instance->pfn_vkCmdBeginRendering(T)
#define vkCmdEndRendering(T) VulkanCore::instance->pfn_vkCmdEndRendering(T)
#define vkCmdSetCullMode(T) VulkanCore::instance->pfn_vkCmdSetCullMode(T)
#define vkCmdSetFrontFace(T) VulkanCore::instance->pfn_vkCmdSetFrontFace(T)
#define vkCmdSetPrimitiveTopology(T) VulkanCore::instance->pfn_vkCmdSetPrimitiveTopology(T)
#define vkCmdSetViewportWithCount(T) VulkanCore::instance->pfn_vkCmdSetViewportWithCount(T)
#define vkCmdSetScissorWithCount(T) VulkanCore::instance->pfn_vkCmdSetScissorWithCount(T)
#define vkCmdBindVertexBuffers2(T) VulkanCore::instance->pfn_vkCmdBindVertexBuffers2(T)
#define vkCmdSetDepthTestEnable(T) VulkanCore::instance->pfn_vkCmdSetDepthTestEnable(T)
#define vkCmdSetDepthWriteEnable(T) VulkanCore::instance->pfn_vkCmdSetDepthWriteEnable(T)
#define vkCmdSetDepthCompareOp(T) VulkanCore::instance->pfn_vkCmdSetDepthCompareOp(T)
#define vkCmdSetDepthBoundsTestEnable(T) VulkanCore::instance->pfn_vkCmdSetDepthBoundsTestEnable(T)
#define vkCmdSetStencilTestEnable(T) VulkanCore::instance->pfn_vkCmdSetStencilTestEnable(T)
#define vkCmdSetStencilOp(T) VulkanCore::instance->pfn_vkCmdSetStencilOp(T)
#define vkCmdSetRasterizerDiscardEnable(T) VulkanCore::instance->pfn_vkCmdSetRasterizerDiscardEnable(T)
#define vkCmdSetDepthBiasEnable(T) VulkanCore::instance->pfn_vkCmdSetDepthBiasEnable(T)
#define vkCmdSetPrimitiveRestartEnable(T) VulkanCore::instance->pfn_vkCmdSetPrimitiveRestartEnable(T)
#define vkGetDeviceBufferMemoryRequirements(T) VulkanCore::instance->pfn_vkGetDeviceBufferMemoryRequirements(T)
#define vkGetDeviceImageMemoryRequirements(T) VulkanCore::instance->pfn_vkGetDeviceImageMemoryRequirements(T)
#define vkGetDeviceImageSparseMemoryRequirements(T) VulkanCore::instance->pfn_vkGetDeviceImageSparseMemoryRequirements(T)

// Vulkan 1.4
/* #define vkCmdSetLineStipple(T) VulkanCore::instance->pfn_vkCmdSetLineStipple(T)
#define vkMapMemory2(T) VulkanCore::instance->pfn_vkMapMemory2(T)
#define vkUnmapMemory2(T) VulkanCore::instance->pfn_vkUnmapMemory2(T)
#define vkCmdBindIndexBuffer2(T) VulkanCore::instance->pfn_vkCmdBindIndexBuffer2(T)
#define vkGetRenderingAreaGranularity(T) VulkanCore::instance->pfn_vkGetRenderingAreaGranularity(T)
#define vkGetDeviceImageSubresourceLayout(T) VulkanCore::instance->pfn_vkGetDeviceImageSubresourceLayout(T)
#define vkGetImageSubresourceLayout2(T) VulkanCore::instance->pfn_vkGetImageSubresourceLayout2(T)
#define vkCmdPushDescriptorSet(T) VulkanCore::instance->pfn_vkCmdPushDescriptorSet(T)
#define vkCmdPushDescriptorSetWithTemplate(T) VulkanCore::instance->pfn_vkCmdPushDescriptorSetWithTemplate(T)
#define vkCmdSetRenderingAttachmentLocations(T) VulkanCore::instance->pfn_vkCmdSetRenderingAttachmentLocations(T)
#define vkCmdSetRenderingInputAttachmentIndices(T) VulkanCore::instance->pfn_vkCmdSetRenderingInputAttachmentIndices(T)
#define vkCmdBindDescriptorSets2(T) VulkanCore::instance->pfn_vkCmdBindDescriptorSets2(T)
#define vkCmdPushConstants2(T) VulkanCore::instance->pfn_vkCmdPushConstants2(T)
#define vkCmdPushDescriptorSet2(T) VulkanCore::instance->pfn_vkCmdPushDescriptorSet2(T)
#define vkCmdPushDescriptorSetWithTemplate2(T) VulkanCore::instance->pfn_vkCmdPushDescriptorSetWithTemplate2(T)
#define vkCopyMemoryToImage(T) VulkanCore::instance->pfn_vkCopyMemoryToImage(T)
#define vkCopyImageToMemory(T) VulkanCore::instance->pfn_vkCopyImageToMemory(T)
#define vkCopyImageToImage(T) VulkanCore::instance->pfn_vkCopyImageToImage(T)
#define vkTransitionImageLayout(T) VulkanCore::instance->pfn_vkTransitionImageLayout(T) */

#define vkInstance VulkanCore::instance->internalInstance

}

#endif /* __RE_VULKAN_CORE_H__ */
