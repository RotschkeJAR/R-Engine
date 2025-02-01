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
			bool loadVulkan_1_0();
			bool loadVulkan_1_1();
			bool loadVulkan_1_2();
			bool loadVulkan_1_3();
			bool loadVulkan_1_4();
			bool createInstance(std::vector<std::string>& extensionsToLoad, bool enableDebug);

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
			PFN_vkCmdSetLineStipple pfn_vkCmdSetLineStipple;
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
			PFN_vkTransitionImageLayout pfn_vkTransitionImageLayout;

			static VulkanCore* instance;
			VkInstance internalInstance;

			VulkanCore() = delete;
			VulkanCore(std::vector<std::string>& extensionsToLoad, bool enableDebug, bool& initSuccess);
			~VulkanCore();
			bool isValid();
	};

#define vkCreateInstance(T) VulkanCore::instance->pfn_vkCreateInstance(T)

#define vkInstance VulkanCore::instance->internalInstance

}

#endif /* __RE_VULKAN_CORE_H__ */
