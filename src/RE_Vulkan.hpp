#ifndef __RE_VULKAN_H__
#define __RE_VULKAN_H__

#include "RE_Ext Header.hpp"
#ifdef RE_OS_WINDOWS
# define VK_USE_PLATFORM_WIN32_KHR
#elif defined RE_OS_LINUX
# define VK_USE_PLATFORM_XLIB_KHR
#endif
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#define VK_LAYER_KHR_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

namespace RE {

	struct VulkanQueueIndices {
		uint32_t graphicsFamily;
	};
	
	class Vulkan {
		private:
			bool valid;
#ifdef RE_OS_WINDOWS
			HMODULE hVulkan;
#elif defined RE_OS_LINUX
			void* libVulkan;
#endif
			VkDebugUtilsMessengerEXT debugMessenger;

			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severityFlagBits, VkDebugUtilsMessageTypeFlagsEXT msgTypeBits, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);
			static bool isPhysicalDeviceSuitable(VkPhysicalDevice device);
			bool setupValidationLayers();
			void* loadFuncInstance(VkInstance instance, const char* funcName);
			void* loadFunc(const char* funcName);
			bool createInstance();
			bool loadVulkan_1_0();
			bool loadVulkan_1_1();
			bool loadVulkan_1_2();
			bool loadVulkan_1_3();
			// bool loadVulkan_1_4();
			bool pickPhysicalDevice();

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

			static Vulkan* instance;
			VkInstance internalInstance;
			VkPhysicalDevice internalPhysicalDevice;
			VkPhysicalDeviceProperties internalPhysicalDeviceProperties;
			VkPhysicalDeviceFeatures internalPhysicalDeviceFeatures;
			VulkanQueueIndices internalQueueIndices;

			Vulkan();
			~Vulkan();
			bool isValid();
			bool checkVulkanResult(VkResult result);
	};

// Vulkan 1.0
#define vkCreateInstance(T) Vulkan::instance->pfn_vkCreateInstance(T)
#define vkDestroyInstance(T) Vulkan::instance->pfn_vkDestroyInstance(T)
#define vkEnumeratePhysicalDevices(T) Vulkan::instance->pfn_vkEnumeratePhysicalDevices(T)
#define vkGetPhysicalDeviceFeatures(T) Vulkan::instance->pfn_vkGetPhysicalDeviceFeatures(T)
#define vkGetPhysicalDeviceFormatProperties(T) Vulkan::instance->pfn_vkGetPhysicalDeviceFormatProperties(T)
#define vkGetPhysicalDeviceImageFormatProperties(T) Vulkan::instance->pfn_vkGetPhysicalDeviceImageFormatProperties(T)
#define vkGetPhysicalDeviceProperties(T) Vulkan::instance->pfn_vkGetPhysicalDeviceProperties(T)
#define vkGetPhysicalDeviceQueueFamilyProperties(T) Vulkan::instance->pfn_vkGetPhysicalDeviceQueueFamilyProperties(T)
#define vkGetPhysicalDeviceMemoryProperties(T) Vulkan::instance->pfn_vkGetPhysicalDeviceMemoryProperties(T)
#define vkGetInstanceProcAddr(T) Vulkan::instance->pfn_vkGetInstanceProcAddr(T)
#define vkGetDeviceProcAddr(T) Vulkan::instance->pfn_vkGetDeviceProcAddr(T)
#define vkCreateDevice(T) Vulkan::instance->pfn_vkCreateDevice(T)
#define vkDestroyDevice(T) Vulkan::instance->pfn_vkDestroyDevice(T)
#define vkEnumerateInstanceExtensionProperties(T) Vulkan::instance->pfn_vkEnumerateInstanceExtensionProperties(T)
#define vkEnumerateDeviceExtensionProperties(T) Vulkan::instance->pfn_vkEnumerateDeviceExtensionProperties(T)
#define vkEnumerateInstanceLayerProperties(T) Vulkan::instance->pfn_vkEnumerateInstanceLayerProperties(T)
#define vkEnumerateDeviceLayerProperties(T) Vulkan::instance->pfn_vkEnumerateDeviceLayerProperties(T)
#define vkGetDeviceQueue(T) Vulkan::instance->pfn_vkGetDeviceQueue(T)
#define vkQueueSubmit(T) Vulkan::instance->pfn_vkQueueSubmit(T)
#define vkQueueWaitIdle(T) Vulkan::instance->pfn_vkQueueWaitIdle(T)
#define vkDeviceWaitIdle(T) Vulkan::instance->pfn_vkDeviceWaitIdle(T)
#define vkAllocateMemory(T) Vulkan::instance->pfn_vkAllocateMemory(T)
#define vkFreeMemory(T) Vulkan::instance->pfn_vkFreeMemory(T)
#define vkMapMemory(T) Vulkan::instance->pfn_vkMapMemory(T)
#define vkUnmapMemory(T) Vulkan::instance->pfn_vkUnmapMemory(T)
#define vkFlushMappedMemoryRanges(T) Vulkan::instance->pfn_vkFlushMappedMemoryRanges(T)
#define vkInvalidateMappedMemoryRanges(T) Vulkan::instance->pfn_vkInvalidateMappedMemoryRanges(T)
#define vkGetDeviceMemoryCommitment(T) Vulkan::instance->pfn_vkGetDeviceMemoryCommitment(T)
#define vkBindBufferMemory(T) Vulkan::instance->pfn_vkBindBufferMemory(T)
#define vkBindImageMemory(T) Vulkan::instance->pfn_vkBindImageMemory(T)
#define vkGetBufferMemoryRequirements(T) Vulkan::instance->pfn_vkGetBufferMemoryRequirements(T)
#define vkGetImageMemoryRequirements(T) Vulkan::instance->pfn_vkGetImageMemoryRequirements(T)
#define vkGetImageSparseMemoryRequirements(T) Vulkan::instance->pfn_vkGetImageSparseMemoryRequirements(T)
#define vkGetPhysicalDeviceSparseImageFormatProperties(T) Vulkan::instance->pfn_vkGetPhysicalDeviceSparseImageFormatProperties(T)
#define vkQueueBindSparse(T) Vulkan::instance->pfn_vkQueueBindSparse(T)
#define vkCreateFence(T) Vulkan::instance->pfn_vkCreateFence(T)
#define vkDestroyFence(T) Vulkan::instance->pfn_vkDestroyFence(T)
#define vkResetFences(T) Vulkan::instance->pfn_vkResetFences(T)
#define vkGetFenceStatus(T) Vulkan::instance->pfn_vkGetFenceStatus(T)
#define vkWaitForFences(T) Vulkan::instance->pfn_vkWaitForFences(T)
#define vkCreateSemaphore(T) Vulkan::instance->pfn_vkCreateSemaphore(T)
#define vkDestroySemaphore(T) Vulkan::instance->pfn_vkDestroySemaphore(T)
#define vkCreateEvent(T) Vulkan::instance->pfn_vkCreateEvent(T)
#define vkDestroyEvent(T) Vulkan::instance->pfn_vkDestroyEvent(T)
#define vkGetEventStatus(T) Vulkan::instance->pfn_vkGetEventStatus(T)
#define vkSetEvent(T) Vulkan::instance->pfn_vkSetEvent(T)
#define vkResetEvent(T) Vulkan::instance->pfn_vkResetEvent(T)
#define vkCreateQueryPool(T) Vulkan::instance->pfn_vkCreateQueryPool(T)
#define vkDestroyQueryPool(T) Vulkan::instance->pfn_vkDestroyQueryPool(T)
#define vkGetQueryPoolResults(T) Vulkan::instance->pfn_vkGetQueryPoolResults(T)
#define vkCreateBuffer(T) Vulkan::instance->pfn_vkCreateBuffer(T)
#define vkDestroyBuffer(T) Vulkan::instance->pfn_vkDestroyBuffer(T)
#define vkCreateBufferView(T) Vulkan::instance->pfn_vkCreateBufferView(T)
#define vkDestroyBufferView(T) Vulkan::instance->pfn_vkDestroyBufferView(T)
#define vkCreateImage(T) Vulkan::instance->pfn_vkCreateImage(T)
#define vkDestroyImage(T) Vulkan::instance->pfn_vkDestroyImage(T)
#define vkGetImageSubresourceLayout(T) Vulkan::instance->pfn_vkGetImageSubresourceLayout(T)
#define vkCreateImageView(T) Vulkan::instance->pfn_vkCreateImageView(T)
#define vkDestroyImageView(T) Vulkan::instance->pfn_vkDestroyImageView(T)
#define vkCreateShaderModule(T) Vulkan::instance->pfn_vkCreateShaderModule(T)
#define vkDestroyShaderModule(T) Vulkan::instance->pfn_vkDestroyShaderModule(T)
#define vkCreatePipelineCache(T) Vulkan::instance->pfn_vkCreatePipelineCache(T)
#define vkDestroyPipelineCache(T) Vulkan::instance->pfn_vkDestroyPipelineCache(T)
#define vkGetPipelineCacheData(T) Vulkan::instance->pfn_vkGetPipelineCacheData(T)
#define vkMergePipelineCaches(T) Vulkan::instance->pfn_vkMergePipelineCaches(T)
#define vkCreateGraphicsPipelines(T) Vulkan::instance->pfn_vkCreateGraphicsPipelines(T)
#define vkCreateComputePipelines(T) Vulkan::instance->pfn_vkCreateComputePipelines(T)
#define vkDestroyPipeline(T) Vulkan::instance->pfn_vkDestroyPipeline(T)
#define vkCreatePipelineLayout(T) Vulkan::instance->pfn_vkCreatePipelineLayout(T)
#define vkDestroyPipelineLayout(T) Vulkan::instance->pfn_vkDestroyPipelineLayout(T)
#define vkCreateSampler(T) Vulkan::instance->pfn_vkCreateSampler(T)
#define vkDestroySampler(T) Vulkan::instance->pfn_vkDestroySampler(T)
#define vkCreateDescriptorSetLayout(T) Vulkan::instance->pfn_vkCreateDescriptorSetLayout(T)
#define vkDestroyDescriptorSetLayout(T) Vulkan::instance->pfn_vkDestroyDescriptorSetLayout(T)
#define vkCreateDescriptorPool(T) Vulkan::instance->pfn_vkCreateDescriptorPool(T)
#define vkDestroyDescriptorPool(T) Vulkan::instance->pfn_vkDestroyDescriptorPool(T)
#define vkResetDescriptorPool(T) Vulkan::instance->pfn_vkResetDescriptorPool(T)
#define vkAllocateDescriptorSets(T) Vulkan::instance->pfn_vkAllocateDescriptorSets(T)
#define vkFreeDescriptorSets(T) Vulkan::instance->pfn_vkFreeDescriptorSets(T)
#define vkUpdateDescriptorSets(T) Vulkan::instance->pfn_vkUpdateDescriptorSets(T)
#define vkCreateFramebuffer(T) Vulkan::instance->pfn_vkCreateFramebuffer(T)
#define vkDestroyFramebuffer(T) Vulkan::instance->pfn_vkDestroyFramebuffer(T)
#define vkCreateRenderPass(T) Vulkan::instance->pfn_vkCreateRenderPass(T)
#define vkDestroyRenderPass(T) Vulkan::instance->pfn_vkDestroyRenderPass(T)
#define vkGetRenderAreaGranularity(T) Vulkan::instance->pfn_vkGetRenderAreaGranularity(T)
#define vkCreateCommandPool(T) Vulkan::instance->pfn_vkCreateCommandPool(T)
#define vkDestroyCommandPool(T) Vulkan::instance->pfn_vkDestroyCommandPool(T)
#define vkResetCommandPool(T) Vulkan::instance->pfn_vkResetCommandPool(T)
#define vkAllocateCommandBuffers(T) Vulkan::instance->pfn_vkAllocateCommandBuffers(T)
#define vkFreeCommandBuffers(T) Vulkan::instance->pfn_vkFreeCommandBuffers(T)
#define vkBeginCommandBuffer(T) Vulkan::instance->pfn_vkBeginCommandBuffer(T)
#define vkEndCommandBuffer(T) Vulkan::instance->pfn_vkEndCommandBuffer(T)
#define vkResetCommandBuffer(T) Vulkan::instance->pfn_vkResetCommandBuffer(T)
#define vkCmdBindPipeline(T) Vulkan::instance->pfn_vkCmdBindPipeline(T)
#define vkCmdSetViewport(T) Vulkan::instance->pfn_vkCmdSetViewport(T)
#define vkCmdSetScissor(T) Vulkan::instance->pfn_vkCmdSetScissor(T)
#define vkCmdSetLineWidth(T) Vulkan::instance->pfn_vkCmdSetLineWidth(T)
#define vkCmdSetDepthBias(T) Vulkan::instance->pfn_vkCmdSetDepthBias(T)
#define vkCmdSetBlendConstants(T) Vulkan::instance->pfn_vkCmdSetBlendConstants(T)
#define vkCmdSetDepthBounds(T) Vulkan::instance->pfn_vkCmdSetDepthBounds(T)
#define vkCmdSetStencilCompareMask(T) Vulkan::instance->pfn_vkCmdSetStencilCompareMask(T)
#define vkCmdSetStencilWriteMask(T) Vulkan::instance->pfn_vkCmdSetStencilWriteMask(T)
#define vkCmdSetStencilReference(T) Vulkan::instance->pfn_vkCmdSetStencilReference(T)
#define vkCmdBindDescriptorSets(T) Vulkan::instance->pfn_vkCmdBindDescriptorSets(T)
#define vkCmdBindIndexBuffer(T) Vulkan::instance->pfn_vkCmdBindIndexBuffer(T)
#define vkCmdBindVertexBuffers(T) Vulkan::instance->pfn_vkCmdBindVertexBuffers(T)
#define vkCmdDraw(T) Vulkan::instance->pfn_vkCmdDraw(T)
#define vkCmdDrawIndexed(T) Vulkan::instance->pfn_vkCmdDrawIndexed(T)
#define vkCmdDrawIndirect(T) Vulkan::instance->pfn_vkCmdDrawIndirect(T)
#define vkCmdDrawIndexedIndirect(T) Vulkan::instance->pfn_vkCmdDrawIndexedIndirect(T)
#define vkCmdDispatch(T) Vulkan::instance->pfn_vkCmdDispatch(T)
#define vkCmdDispatchIndirect(T) Vulkan::instance->pfn_vkCmdDispatchIndirect(T)
#define vkCmdCopyBuffer(T) Vulkan::instance->pfn_vkCmdCopyBuffer(T)
#define vkCmdCopyImage(T) Vulkan::instance->pfn_vkCmdCopyImage(T)
#define vkCmdBlitImage(T) Vulkan::instance->pfn_vkCmdBlitImage(T)
#define vkCmdCopyBufferToImage(T) Vulkan::instance->pfn_vkCmdCopyBufferToImage(T)
#define vkCmdCopyImageToBuffer(T) Vulkan::instance->pfn_vkCmdCopyImageToBuffer(T)
#define vkCmdUpdateBuffer(T) Vulkan::instance->pfn_vkCmdUpdateBuffer(T)
#define vkCmdFillBuffer(T) Vulkan::instance->pfn_vkCmdFillBuffer(T)
#define vkCmdClearColorImage(T) Vulkan::instance->pfn_vkCmdClearColorImage(T)
#define vkCmdClearDepthStencilImage(T) Vulkan::instance->pfn_vkCmdClearDepthStencilImage(T)
#define vkCmdClearAttachments(T) Vulkan::instance->pfn_vkCmdClearAttachments(T)
#define vkCmdResolveImage(T) Vulkan::instance->pfn_vkCmdResolveImage(T)
#define vkCmdSetEvent(T) Vulkan::instance->pfn_vkCmdSetEvent(T)
#define vkCmdResetEvent(T) Vulkan::instance->pfn_vkCmdResetEvent(T)
#define vkCmdWaitEvents(T) Vulkan::instance->pfn_vkCmdWaitEvents(T)
#define vkCmdPipelineBarrier(T) Vulkan::instance->pfn_vkCmdPipelineBarrier(T)
#define vkCmdBeginQuery(T) Vulkan::instance->pfn_vkCmdBeginQuery(T)
#define vkCmdEndQuery(T) Vulkan::instance->pfn_vkCmdEndQuery(T)
#define vkCmdResetQueryPool(T) Vulkan::instance->pfn_vkCmdResetQueryPool(T)
#define vkCmdWriteTimestamp(T) Vulkan::instance->pfn_vkCmdWriteTimestamp(T)
#define vkCmdCopyQueryPoolResults(T) Vulkan::instance->pfn_vkCmdCopyQueryPoolResults(T)
#define vkCmdPushConstants(T) Vulkan::instance->pfn_vkCmdPushConstants(T)
#define vkCmdBeginRenderPass(T) Vulkan::instance->pfn_vkCmdBeginRenderPass(T)
#define vkCmdNextSubpass(T) Vulkan::instance->pfn_vkCmdNextSubpass(T)
#define vkCmdEndRenderPass(T) Vulkan::instance->pfn_vkCmdEndRenderPass(T)
#define vkCmdExecuteCommands(T) Vulkan::instance->pfn_vkCmdExecuteCommands(T)

// Vulkan 1.1
#define vkEnumerateInstanceVersion(T) Vulkan::instance->pfn_vkEnumerateInstanceVersion(T)
#define vkBindBufferMemory2(T) Vulkan::instance->pfn_vkBindBufferMemory2(T)
#define vkBindImageMemory2(T) Vulkan::instance->pfn_vkBindImageMemory2(T)
#define vkGetDeviceGroupPeerMemoryFeatures(T) Vulkan::instance->pfn_vkGetDeviceGroupPeerMemoryFeatures(T)
#define vkCmdSetDeviceMask(T) Vulkan::instance->pfn_vkCmdSetDeviceMask(T)
#define vkCmdDispatchBase(T) Vulkan::instance->pfn_vkCmdDispatchBase(T)
#define vkEnumeratePhysicalDeviceGroups(T) Vulkan::instance->pfn_vkEnumeratePhysicalDeviceGroups(T)
#define vkGetImageMemoryRequirements2(T) Vulkan::instance->pfn_vkGetImageMemoryRequirements2(T)
#define vkGetBufferMemoryRequirements2(T) Vulkan::instance->pfn_vkGetBufferMemoryRequirements2(T)
#define vkGetImageSparseMemoryRequirements2(T) Vulkan::instance->pfn_vkGetImageSparseMemoryRequirements2(T)
#define vkGetPhysicalDeviceFeatures2(T) Vulkan::instance->pfn_vkGetPhysicalDeviceFeatures2(T)
#define vkGetPhysicalDeviceProperties2(T) Vulkan::instance->pfn_vkGetPhysicalDeviceProperties2(T)
#define vkGetPhysicalDeviceFormatProperties2(T) Vulkan::instance->pfn_vkGetPhysicalDeviceFormatProperties2(T)
#define vkGetPhysicalDeviceImageFormatProperties2(T) Vulkan::instance->pfn_vkGetPhysicalDeviceImageFormatProperties2(T)
#define vkGetPhysicalDeviceQueueFamilyProperties2(T) Vulkan::instance->pfn_vkGetPhysicalDeviceQueueFamilyProperties2(T)
#define vkGetPhysicalDeviceMemoryProperties2(T) Vulkan::instance->pfn_vkGetPhysicalDeviceMemoryProperties2(T)
#define vkGetPhysicalDeviceSparseImageFormatProperties2(T) Vulkan::instance->pfn_vkGetPhysicalDeviceSparseImageFormatProperties2(T)
#define vkTrimCommandPool(T) Vulkan::instance->pfn_vkTrimCommandPool(T)
#define vkGetDeviceQueue2(T) Vulkan::instance->pfn_vkGetDeviceQueue2(T)
#define vkCreateSamplerYcbcrConversion(T) Vulkan::instance->pfn_vkCreateSamplerYcbcrConversion(T)
#define vkDestroySamplerYcbcrConversion(T) Vulkan::instance->pfn_vkDestroySamplerYcbcrConversion(T)
#define vkCreateDescriptorUpdateTemplate(T) Vulkan::instance->pfn_vkCreateDescriptorUpdateTemplate(T)
#define vkDestroyDescriptorUpdateTemplate(T) Vulkan::instance->pfn_vkDestroyDescriptorUpdateTemplate(T)
#define vkUpdateDescriptorSetWithTemplate(T) Vulkan::instance->pfn_vkUpdateDescriptorSetWithTemplate(T)
#define vkGetPhysicalDeviceExternalBufferProperties(T) Vulkan::instance->pfn_vkGetPhysicalDeviceExternalBufferProperties(T)
#define vkGetPhysicalDeviceExternalFenceProperties(T) Vulkan::instance->pfn_vkGetPhysicalDeviceExternalFenceProperties(T)
#define vkGetPhysicalDeviceExternalSemaphoreProperties(T) Vulkan::instance->pfn_vkGetPhysicalDeviceExternalSemaphoreProperties(T)
#define vkGetDescriptorSetLayoutSupport(T) Vulkan::instance->pfn_vkGetDescriptorSetLayoutSupport(T)

// Vulkan 1.2
#define vkCmdDrawIndirectCount(T) Vulkan::instance->pfn_vkCmdDrawIndirectCount(T)
#define vkCmdDrawIndexedIndirectCount(T) Vulkan::instance->pfn_vkCmdDrawIndexedIndirectCount(T)
#define vkCreateRenderPass2(T) Vulkan::instance->pfn_vkCreateRenderPass2(T)
#define vkCmdBeginRenderPass2(T) Vulkan::instance->pfn_vkCmdBeginRenderPass2(T)
#define vkCmdNextSubpass2(T) Vulkan::instance->pfn_vkCmdNextSubpass2(T)
#define vkCmdEndRenderPass2(T) Vulkan::instance->pfn_vkCmdEndRenderPass2(T)
#define vkResetQueryPool(T) Vulkan::instance->pfn_vkResetQueryPool(T)
#define vkGetSemaphoreCounterValue(T) Vulkan::instance->pfn_vkGetSemaphoreCounterValue(T)
#define vkWaitSemaphores(T) Vulkan::instance->pfn_vkWaitSemaphores(T)
#define vkSignalSemaphore(T) Vulkan::instance->pfn_vkSignalSemaphore(T)
#define vkGetBufferDeviceAddress(T) Vulkan::instance->pfn_vkGetBufferDeviceAddress(T)
#define vkGetBufferOpaqueCaptureAddress(T) Vulkan::instance->pfn_vkGetBufferOpaqueCaptureAddress(T)
#define vkGetDeviceMemoryOpaqueCaptureAddress(T) Vulkan::instance->pfn_vkGetDeviceMemoryOpaqueCaptureAddress(T)

// Vulkan 1.3
#define vkGetPhysicalDeviceToolProperties(T) Vulkan::instance->pfn_vkGetPhysicalDeviceToolProperties(T)
#define vkCreatePrivateDataSlot(T) Vulkan::instance->pfn_vkCreatePrivateDataSlot(T)
#define vkDestroyPrivateDataSlot(T) Vulkan::instance->pfn_vkDestroyPrivateDataSlot(T)
#define vkSetPrivateData(T) Vulkan::instance->pfn_vkSetPrivateData(T)
#define vkGetPrivateData(T) Vulkan::instance->pfn_vkGetPrivateData(T)
#define vkCmdSetEvent2(T) Vulkan::instance->pfn_vkCmdSetEvent2(T)
#define vkCmdResetEvent2(T) Vulkan::instance->pfn_vkCmdResetEvent2(T)
#define vkCmdWaitEvents2(T) Vulkan::instance->pfn_vkCmdWaitEvents2(T)
#define vkCmdPipelineBarrier2(T) Vulkan::instance->pfn_vkCmdPipelineBarrier2(T)
#define vkCmdWriteTimestamp2(T) Vulkan::instance->pfn_vkCmdWriteTimestamp2(T)
#define vkQueueSubmit2(T) Vulkan::instance->pfn_vkQueueSubmit2(T)
#define vkCmdCopyBuffer2(T) Vulkan::instance->pfn_vkCmdCopyBuffer2(T)
#define vkCmdCopyImage2(T) Vulkan::instance->pfn_vkCmdCopyImage2(T)
#define vkCmdCopyBufferToImage2(T) Vulkan::instance->pfn_vkCmdCopyBufferToImage2(T)
#define vkCmdCopyImageToBuffer2(T) Vulkan::instance->pfn_vkCmdCopyImageToBuffer2(T)
#define vkCmdBlitImage2(T) Vulkan::instance->pfn_vkCmdBlitImage2(T)
#define vkCmdResolveImage2(T) Vulkan::instance->pfn_vkCmdResolveImage2(T)
#define vkCmdBeginRendering(T) Vulkan::instance->pfn_vkCmdBeginRendering(T)
#define vkCmdEndRendering(T) Vulkan::instance->pfn_vkCmdEndRendering(T)
#define vkCmdSetCullMode(T) Vulkan::instance->pfn_vkCmdSetCullMode(T)
#define vkCmdSetFrontFace(T) Vulkan::instance->pfn_vkCmdSetFrontFace(T)
#define vkCmdSetPrimitiveTopology(T) Vulkan::instance->pfn_vkCmdSetPrimitiveTopology(T)
#define vkCmdSetViewportWithCount(T) Vulkan::instance->pfn_vkCmdSetViewportWithCount(T)
#define vkCmdSetScissorWithCount(T) Vulkan::instance->pfn_vkCmdSetScissorWithCount(T)
#define vkCmdBindVertexBuffers2(T) Vulkan::instance->pfn_vkCmdBindVertexBuffers2(T)
#define vkCmdSetDepthTestEnable(T) Vulkan::instance->pfn_vkCmdSetDepthTestEnable(T)
#define vkCmdSetDepthWriteEnable(T) Vulkan::instance->pfn_vkCmdSetDepthWriteEnable(T)
#define vkCmdSetDepthCompareOp(T) Vulkan::instance->pfn_vkCmdSetDepthCompareOp(T)
#define vkCmdSetDepthBoundsTestEnable(T) Vulkan::instance->pfn_vkCmdSetDepthBoundsTestEnable(T)
#define vkCmdSetStencilTestEnable(T) Vulkan::instance->pfn_vkCmdSetStencilTestEnable(T)
#define vkCmdSetStencilOp(T) Vulkan::instance->pfn_vkCmdSetStencilOp(T)
#define vkCmdSetRasterizerDiscardEnable(T) Vulkan::instance->pfn_vkCmdSetRasterizerDiscardEnable(T)
#define vkCmdSetDepthBiasEnable(T) Vulkan::instance->pfn_vkCmdSetDepthBiasEnable(T)
#define vkCmdSetPrimitiveRestartEnable(T) Vulkan::instance->pfn_vkCmdSetPrimitiveRestartEnable(T)
#define vkGetDeviceBufferMemoryRequirements(T) Vulkan::instance->pfn_vkGetDeviceBufferMemoryRequirements(T)
#define vkGetDeviceImageMemoryRequirements(T) Vulkan::instance->pfn_vkGetDeviceImageMemoryRequirements(T)
#define vkGetDeviceImageSparseMemoryRequirements(T) Vulkan::instance->pfn_vkGetDeviceImageSparseMemoryRequirements(T)

// Vulkan 1.4
/* #define vkCmdSetLineStipple(T) Vulkan::instance->pfn_vkCmdSetLineStipple(T)
#define vkMapMemory2(T) Vulkan::instance->pfn_vkMapMemory2(T)
#define vkUnmapMemory2(T) Vulkan::instance->pfn_vkUnmapMemory2(T)
#define vkCmdBindIndexBuffer2(T) Vulkan::instance->pfn_vkCmdBindIndexBuffer2(T)
#define vkGetRenderingAreaGranularity(T) Vulkan::instance->pfn_vkGetRenderingAreaGranularity(T)
#define vkGetDeviceImageSubresourceLayout(T) Vulkan::instance->pfn_vkGetDeviceImageSubresourceLayout(T)
#define vkGetImageSubresourceLayout2(T) Vulkan::instance->pfn_vkGetImageSubresourceLayout2(T)
#define vkCmdPushDescriptorSet(T) Vulkan::instance->pfn_vkCmdPushDescriptorSet(T)
#define vkCmdPushDescriptorSetWithTemplate(T) Vulkan::instance->pfn_vkCmdPushDescriptorSetWithTemplate(T)
#define vkCmdSetRenderingAttachmentLocations(T) Vulkan::instance->pfn_vkCmdSetRenderingAttachmentLocations(T)
#define vkCmdSetRenderingInputAttachmentIndices(T) Vulkan::instance->pfn_vkCmdSetRenderingInputAttachmentIndices(T)
#define vkCmdBindDescriptorSets2(T) Vulkan::instance->pfn_vkCmdBindDescriptorSets2(T)
#define vkCmdPushConstants2(T) Vulkan::instance->pfn_vkCmdPushConstants2(T)
#define vkCmdPushDescriptorSet2(T) Vulkan::instance->pfn_vkCmdPushDescriptorSet2(T)
#define vkCmdPushDescriptorSetWithTemplate2(T) Vulkan::instance->pfn_vkCmdPushDescriptorSetWithTemplate2(T)
#define vkCopyMemoryToImage(T) Vulkan::instance->pfn_vkCopyMemoryToImage(T)
#define vkCopyImageToMemory(T) Vulkan::instance->pfn_vkCopyImageToMemory(T)
#define vkCopyImageToImage(T) Vulkan::instance->pfn_vkCopyImageToImage(T)
#define vkTransitionImageLayout(T) Vulkan::instance->pfn_vkTransitionImageLayout(T) */

#define vkInstance Vulkan::instance->internalInstance
#define vkPhysicalDevice Vulkan::instance->internalPhysicalDevice
#define vkPhysicalDeviceProperties Vulkan::instance->internalPhysicalDeviceProperties
#define vkPhysicalDeviceFeatures Vulkan::instance->internalPhysicalDeviceFeatures
#define vkQueueIndices Vulkan::instance->internalQueueIndices

}

#endif /* __RE_VULKAN_H__ */
