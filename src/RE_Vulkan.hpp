#ifndef __RE_VULKAN_H__
#define __RE_VULKAN_H__

#include "RE_Internal Header.hpp"
#ifdef RE_OS_WINDOWS
# define VK_USE_PLATFORM_WIN32_KHR
#elif defined RE_OS_LINUX
# define VK_USE_PLATFORM_XLIB_KHR
#endif
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#define VK_LAYER_KHR_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

namespace RE {
	
	class Vulkan {
		private:
			bool bValid;
#ifdef RE_OS_WINDOWS
			HMODULE hLibVulkan;
#elif defined RE_OS_LINUX
			void* hLibVulkan;
#endif
			VkInstance vk_hInstance;
			VkDebugUtilsMessengerEXT vk_hDebugMessenger;

			static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT vk_severityFlagBits, VkDebugUtilsMessageTypeFlagsEXT vk_MsgTypeBits, const VkDebugUtilsMessengerCallbackDataEXT* ck_CallbackData, void* vk_UserData);
			void* load_func_with_instance(VkInstance vk_hInstance, const char* pFuncName);
			void* load_func(const char* pFuncName);
			bool create_instance();
			bool load_vulkan_1_0();
			bool load_vulkan_1_1();
			bool load_vulkan_1_2();
			bool load_vulkan_1_3();
			// bool load_vulkan_1_4();
			bool load_extension_funcs();
			bool setup_validation_layers();

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

			// Debug Messages
			PFN_vkSetDebugUtilsObjectNameEXT pfn_vkSetDebugUtilsObjectNameEXT;
			PFN_vkSetDebugUtilsObjectTagEXT pfn_vkSetDebugUtilsObjectTagEXT;
			PFN_vkQueueBeginDebugUtilsLabelEXT pfn_vkQueueBeginDebugUtilsLabelEXT;
			PFN_vkQueueEndDebugUtilsLabelEXT pfn_vkQueueEndDebugUtilsLabelEXT;
			PFN_vkQueueInsertDebugUtilsLabelEXT pfn_vkQueueInsertDebugUtilsLabelEXT;
			PFN_vkCmdBeginDebugUtilsLabelEXT pfn_vkCmdBeginDebugUtilsLabelEXT;
			PFN_vkCmdEndDebugUtilsLabelEXT pfn_vkCmdEndDebugUtilsLabelEXT;
			PFN_vkCmdInsertDebugUtilsLabelEXT pfn_vkCmdInsertDebugUtilsLabelEXT;
			PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT;
			PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT;
			PFN_vkSubmitDebugUtilsMessageEXT pfn_vkSubmitDebugUtilsMessageEXT;

			// Surface
			PFN_vkDestroySurfaceKHR pfn_vkDestroySurfaceKHR;
			PFN_vkGetPhysicalDeviceSurfaceSupportKHR pfn_vkGetPhysicalDeviceSurfaceSupportKHR;
			PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
			PFN_vkGetPhysicalDeviceSurfaceFormatsKHR pfn_vkGetPhysicalDeviceSurfaceFormatsKHR;
			PFN_vkGetPhysicalDeviceSurfacePresentModesKHR pfn_vkGetPhysicalDeviceSurfacePresentModesKHR;

			// Swapchain
			PFN_vkCreateSwapchainKHR pfn_vkCreateSwapchainKHR;
			PFN_vkDestroySwapchainKHR pfn_vkDestroySwapchainKHR;
			PFN_vkGetSwapchainImagesKHR pfn_vkGetSwapchainImagesKHR;
			PFN_vkAcquireNextImageKHR pfn_vkAcquireNextImageKHR;
			PFN_vkQueuePresentKHR pfn_vkQueuePresentKHR;
			PFN_vkGetDeviceGroupPresentCapabilitiesKHR pfn_vkGetDeviceGroupPresentCapabilitiesKHR;
			PFN_vkGetDeviceGroupSurfacePresentModesKHR pfn_vkGetDeviceGroupSurfacePresentModesKHR;
			PFN_vkGetPhysicalDevicePresentRectanglesKHR pfn_vkGetPhysicalDevicePresentRectanglesKHR;
			PFN_vkAcquireNextImage2KHR pfn_vkAcquireNextImage2KHR;

#ifdef RE_OS_WINDOWS
			// Win32-Surface
			PFN_vkCreateWin32SurfaceKHR pfn_vkCreateWin32SurfaceKHR;
			PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR;
#elif defined RE_OS_LINUX
			// X11-Surface
			PFN_vkCreateXlibSurfaceKHR pfn_vkCreateXlibSurfaceKHR;
			PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

			static Vulkan* pInstance;

			Vulkan();
			~Vulkan();
			VkInstance get_instance();
			bool is_valid();
	};

	bool check_vulkan_result(VkResult vk_eResult, const char* pcFile, const char* pcFunc, REuint u32Line);
#define CHECK_VK_RESULT(T) ([&](const char *const pcActualFunc) -> bool { \
		add_to_stack_trace(__FILE__, pcActualFunc, __LINE__, "\0"); \
		const bool bResult = check_vulkan_result(T, __FILE__, pcActualFunc, __LINE__); \
		remove_from_stack_trace(); \
		return bResult; \
		})(__func__)

// Vulkan 1.0
#define vkCreateInstance Vulkan::pInstance->pfn_vkCreateInstance
#define vkDestroyInstance Vulkan::pInstance->pfn_vkDestroyInstance
#define vkEnumeratePhysicalDevices Vulkan::pInstance->pfn_vkEnumeratePhysicalDevices
#define vkGetPhysicalDeviceFeatures Vulkan::pInstance->pfn_vkGetPhysicalDeviceFeatures
#define vkGetPhysicalDeviceFormatProperties Vulkan::pInstance->pfn_vkGetPhysicalDeviceFormatProperties
#define vkGetPhysicalDeviceImageFormatProperties Vulkan::pInstance->pfn_vkGetPhysicalDeviceImageFormatProperties
#define vkGetPhysicalDeviceProperties Vulkan::pInstance->pfn_vkGetPhysicalDeviceProperties
#define vkGetPhysicalDeviceQueueFamilyProperties Vulkan::pInstance->pfn_vkGetPhysicalDeviceQueueFamilyProperties
#define vkGetPhysicalDeviceMemoryProperties Vulkan::pInstance->pfn_vkGetPhysicalDeviceMemoryProperties
#define vkGetInstanceProcAddr Vulkan::pInstance->pfn_vkGetInstanceProcAddr
#define vkGetDeviceProcAddr Vulkan::pInstance->pfn_vkGetDeviceProcAddr
#define vkCreateDevice Vulkan::pInstance->pfn_vkCreateDevice
#define vkDestroyDevice Vulkan::pInstance->pfn_vkDestroyDevice
#define vkEnumerateInstanceExtensionProperties Vulkan::pInstance->pfn_vkEnumerateInstanceExtensionProperties
#define vkEnumerateDeviceExtensionProperties Vulkan::pInstance->pfn_vkEnumerateDeviceExtensionProperties
#define vkEnumerateInstanceLayerProperties Vulkan::pInstance->pfn_vkEnumerateInstanceLayerProperties
#define vkEnumerateDeviceLayerProperties Vulkan::pInstance->pfn_vkEnumerateDeviceLayerProperties
#define vkGetDeviceQueue Vulkan::pInstance->pfn_vkGetDeviceQueue
#define vkQueueSubmit Vulkan::pInstance->pfn_vkQueueSubmit
#define vkQueueWaitIdle Vulkan::pInstance->pfn_vkQueueWaitIdle
#define vkDeviceWaitIdle Vulkan::pInstance->pfn_vkDeviceWaitIdle
#define vkAllocateMemory Vulkan::pInstance->pfn_vkAllocateMemory
#define vkFreeMemory Vulkan::pInstance->pfn_vkFreeMemory
#define vkMapMemory Vulkan::pInstance->pfn_vkMapMemory
#define vkUnmapMemory Vulkan::pInstance->pfn_vkUnmapMemory
#define vkFlushMappedMemoryRanges Vulkan::pInstance->pfn_vkFlushMappedMemoryRanges
#define vkInvalidateMappedMemoryRanges Vulkan::pInstance->pfn_vkInvalidateMappedMemoryRanges
#define vkGetDeviceMemoryCommitment Vulkan::pInstance->pfn_vkGetDeviceMemoryCommitment
#define vkBindBufferMemory Vulkan::pInstance->pfn_vkBindBufferMemory
#define vkBindImageMemory Vulkan::pInstance->pfn_vkBindImageMemory
#define vkGetBufferMemoryRequirements Vulkan::pInstance->pfn_vkGetBufferMemoryRequirements
#define vkGetImageMemoryRequirements Vulkan::pInstance->pfn_vkGetImageMemoryRequirements
#define vkGetImageSparseMemoryRequirements Vulkan::pInstance->pfn_vkGetImageSparseMemoryRequirements
#define vkGetPhysicalDeviceSparseImageFormatProperties Vulkan::pInstance->pfn_vkGetPhysicalDeviceSparseImageFormatProperties
#define vkQueueBindSparse Vulkan::pInstance->pfn_vkQueueBindSparse
#define vkCreateFence Vulkan::pInstance->pfn_vkCreateFence
#define vkDestroyFence Vulkan::pInstance->pfn_vkDestroyFence
#define vkResetFences Vulkan::pInstance->pfn_vkResetFences
#define vkGetFenceStatus Vulkan::pInstance->pfn_vkGetFenceStatus
#define vkWaitForFences Vulkan::pInstance->pfn_vkWaitForFences
#define vkCreateSemaphore Vulkan::pInstance->pfn_vkCreateSemaphore
#define vkDestroySemaphore Vulkan::pInstance->pfn_vkDestroySemaphore
#define vkCreateEvent Vulkan::pInstance->pfn_vkCreateEvent
#define vkDestroyEvent Vulkan::pInstance->pfn_vkDestroyEvent
#define vkGetEventStatus Vulkan::pInstance->pfn_vkGetEventStatus
#define vkSetEvent Vulkan::pInstance->pfn_vkSetEvent
#define vkResetEvent Vulkan::pInstance->pfn_vkResetEvent
#define vkCreateQueryPool Vulkan::pInstance->pfn_vkCreateQueryPool
#define vkDestroyQueryPool Vulkan::pInstance->pfn_vkDestroyQueryPool
#define vkGetQueryPoolResults Vulkan::pInstance->pfn_vkGetQueryPoolResults
#define vkCreateBuffer Vulkan::pInstance->pfn_vkCreateBuffer
#define vkDestroyBuffer Vulkan::pInstance->pfn_vkDestroyBuffer
#define vkCreateBufferView Vulkan::pInstance->pfn_vkCreateBufferView
#define vkDestroyBufferView Vulkan::pInstance->pfn_vkDestroyBufferView
#define vkCreateImage Vulkan::pInstance->pfn_vkCreateImage
#define vkDestroyImage Vulkan::pInstance->pfn_vkDestroyImage
#define vkGetImageSubresourceLayout Vulkan::pInstance->pfn_vkGetImageSubresourceLayout
#define vkCreateImageView Vulkan::pInstance->pfn_vkCreateImageView
#define vkDestroyImageView Vulkan::pInstance->pfn_vkDestroyImageView
#define vkCreateShaderModule Vulkan::pInstance->pfn_vkCreateShaderModule
#define vkDestroyShaderModule Vulkan::pInstance->pfn_vkDestroyShaderModule
#define vkCreatePipelineCache Vulkan::pInstance->pfn_vkCreatePipelineCache
#define vkDestroyPipelineCache Vulkan::pInstance->pfn_vkDestroyPipelineCache
#define vkGetPipelineCacheData Vulkan::pInstance->pfn_vkGetPipelineCacheData
#define vkMergePipelineCaches Vulkan::pInstance->pfn_vkMergePipelineCaches
#define vkCreateGraphicsPipelines Vulkan::pInstance->pfn_vkCreateGraphicsPipelines
#define vkCreateComputePipelines Vulkan::pInstance->pfn_vkCreateComputePipelines
#define vkDestroyPipeline Vulkan::pInstance->pfn_vkDestroyPipeline
#define vkCreatePipelineLayout Vulkan::pInstance->pfn_vkCreatePipelineLayout
#define vkDestroyPipelineLayout Vulkan::pInstance->pfn_vkDestroyPipelineLayout
#define vkCreateSampler Vulkan::pInstance->pfn_vkCreateSampler
#define vkDestroySampler Vulkan::pInstance->pfn_vkDestroySampler
#define vkCreateDescriptorSetLayout Vulkan::pInstance->pfn_vkCreateDescriptorSetLayout
#define vkDestroyDescriptorSetLayout Vulkan::pInstance->pfn_vkDestroyDescriptorSetLayout
#define vkCreateDescriptorPool Vulkan::pInstance->pfn_vkCreateDescriptorPool
#define vkDestroyDescriptorPool Vulkan::pInstance->pfn_vkDestroyDescriptorPool
#define vkResetDescriptorPool Vulkan::pInstance->pfn_vkResetDescriptorPool
#define vkAllocateDescriptorSets Vulkan::pInstance->pfn_vkAllocateDescriptorSets
#define vkFreeDescriptorSets Vulkan::pInstance->pfn_vkFreeDescriptorSets
#define vkUpdateDescriptorSets Vulkan::pInstance->pfn_vkUpdateDescriptorSets
#define vkCreateFramebuffer Vulkan::pInstance->pfn_vkCreateFramebuffer
#define vkDestroyFramebuffer Vulkan::pInstance->pfn_vkDestroyFramebuffer
#define vkCreateRenderPass Vulkan::pInstance->pfn_vkCreateRenderPass
#define vkDestroyRenderPass Vulkan::pInstance->pfn_vkDestroyRenderPass
#define vkGetRenderAreaGranularity Vulkan::pInstance->pfn_vkGetRenderAreaGranularity
#define vkCreateCommandPool Vulkan::pInstance->pfn_vkCreateCommandPool
#define vkDestroyCommandPool Vulkan::pInstance->pfn_vkDestroyCommandPool
#define vkResetCommandPool Vulkan::pInstance->pfn_vkResetCommandPool
#define vkAllocateCommandBuffers Vulkan::pInstance->pfn_vkAllocateCommandBuffers
#define vkFreeCommandBuffers Vulkan::pInstance->pfn_vkFreeCommandBuffers
#define vkBeginCommandBuffer Vulkan::pInstance->pfn_vkBeginCommandBuffer
#define vkEndCommandBuffer Vulkan::pInstance->pfn_vkEndCommandBuffer
#define vkResetCommandBuffer Vulkan::pInstance->pfn_vkResetCommandBuffer
#define vkCmdBindPipeline Vulkan::pInstance->pfn_vkCmdBindPipeline
#define vkCmdSetViewport Vulkan::pInstance->pfn_vkCmdSetViewport
#define vkCmdSetScissor Vulkan::pInstance->pfn_vkCmdSetScissor
#define vkCmdSetLineWidth Vulkan::pInstance->pfn_vkCmdSetLineWidth
#define vkCmdSetDepthBias Vulkan::pInstance->pfn_vkCmdSetDepthBias
#define vkCmdSetBlendConstants Vulkan::pInstance->pfn_vkCmdSetBlendConstants
#define vkCmdSetDepthBounds Vulkan::pInstance->pfn_vkCmdSetDepthBounds
#define vkCmdSetStencilCompareMask Vulkan::pInstance->pfn_vkCmdSetStencilCompareMask
#define vkCmdSetStencilWriteMask Vulkan::pInstance->pfn_vkCmdSetStencilWriteMask
#define vkCmdSetStencilReference Vulkan::pInstance->pfn_vkCmdSetStencilReference
#define vkCmdBindDescriptorSets Vulkan::pInstance->pfn_vkCmdBindDescriptorSets
#define vkCmdBindIndexBuffer Vulkan::pInstance->pfn_vkCmdBindIndexBuffer
#define vkCmdBindVertexBuffers Vulkan::pInstance->pfn_vkCmdBindVertexBuffers
#define vkCmdDraw Vulkan::pInstance->pfn_vkCmdDraw
#define vkCmdDrawIndexed Vulkan::pInstance->pfn_vkCmdDrawIndexed
#define vkCmdDrawIndirect Vulkan::pInstance->pfn_vkCmdDrawIndirect
#define vkCmdDrawIndexedIndirect Vulkan::pInstance->pfn_vkCmdDrawIndexedIndirect
#define vkCmdDispatch Vulkan::pInstance->pfn_vkCmdDispatch
#define vkCmdDispatchIndirect Vulkan::pInstance->pfn_vkCmdDispatchIndirect
#define vkCmdCopyBuffer Vulkan::pInstance->pfn_vkCmdCopyBuffer
#define vkCmdCopyImage Vulkan::pInstance->pfn_vkCmdCopyImage
#define vkCmdBlitImage Vulkan::pInstance->pfn_vkCmdBlitImage
#define vkCmdCopyBufferToImage Vulkan::pInstance->pfn_vkCmdCopyBufferToImage
#define vkCmdCopyImageToBuffer Vulkan::pInstance->pfn_vkCmdCopyImageToBuffer
#define vkCmdUpdateBuffer Vulkan::pInstance->pfn_vkCmdUpdateBuffer
#define vkCmdFillBuffer Vulkan::pInstance->pfn_vkCmdFillBuffer
#define vkCmdClearColorImage Vulkan::pInstance->pfn_vkCmdClearColorImage
#define vkCmdClearDepthStencilImage Vulkan::pInstance->pfn_vkCmdClearDepthStencilImage
#define vkCmdClearAttachments Vulkan::pInstance->pfn_vkCmdClearAttachments
#define vkCmdResolveImage Vulkan::pInstance->pfn_vkCmdResolveImage
#define vkCmdSetEvent Vulkan::pInstance->pfn_vkCmdSetEvent
#define vkCmdResetEvent Vulkan::pInstance->pfn_vkCmdResetEvent
#define vkCmdWaitEvents Vulkan::pInstance->pfn_vkCmdWaitEvents
#define vkCmdPipelineBarrier Vulkan::pInstance->pfn_vkCmdPipelineBarrier
#define vkCmdBeginQuery Vulkan::pInstance->pfn_vkCmdBeginQuery
#define vkCmdEndQuery Vulkan::pInstance->pfn_vkCmdEndQuery
#define vkCmdResetQueryPool Vulkan::pInstance->pfn_vkCmdResetQueryPool
#define vkCmdWriteTimestamp Vulkan::pInstance->pfn_vkCmdWriteTimestamp
#define vkCmdCopyQueryPoolResults Vulkan::pInstance->pfn_vkCmdCopyQueryPoolResults
#define vkCmdPushConstants Vulkan::pInstance->pfn_vkCmdPushConstants
#define vkCmdBeginRenderPass Vulkan::pInstance->pfn_vkCmdBeginRenderPass
#define vkCmdNextSubpass Vulkan::pInstance->pfn_vkCmdNextSubpass
#define vkCmdEndRenderPass Vulkan::pInstance->pfn_vkCmdEndRenderPass
#define vkCmdExecuteCommands Vulkan::pInstance->pfn_vkCmdExecuteCommands

// Vulkan 1.1
#define vkEnumerateInstanceVersion Vulkan::pInstance->pfn_vkEnumerateInstanceVersion
#define vkBindBufferMemory2 Vulkan::pInstance->pfn_vkBindBufferMemory2
#define vkBindImageMemory2 Vulkan::pInstance->pfn_vkBindImageMemory2
#define vkGetDeviceGroupPeerMemoryFeatures Vulkan::pInstance->pfn_vkGetDeviceGroupPeerMemoryFeatures
#define vkCmdSetDeviceMask Vulkan::pInstance->pfn_vkCmdSetDeviceMask
#define vkCmdDispatchBase Vulkan::pInstance->pfn_vkCmdDispatchBase
#define vkEnumeratePhysicalDeviceGroups Vulkan::pInstance->pfn_vkEnumeratePhysicalDeviceGroups
#define vkGetImageMemoryRequirements2 Vulkan::pInstance->pfn_vkGetImageMemoryRequirements2
#define vkGetBufferMemoryRequirements2 Vulkan::pInstance->pfn_vkGetBufferMemoryRequirements2
#define vkGetImageSparseMemoryRequirements2 Vulkan::pInstance->pfn_vkGetImageSparseMemoryRequirements2
#define vkGetPhysicalDeviceFeatures2 Vulkan::pInstance->pfn_vkGetPhysicalDeviceFeatures2
#define vkGetPhysicalDeviceProperties2 Vulkan::pInstance->pfn_vkGetPhysicalDeviceProperties2
#define vkGetPhysicalDeviceFormatProperties2 Vulkan::pInstance->pfn_vkGetPhysicalDeviceFormatProperties2
#define vkGetPhysicalDeviceImageFormatProperties2 Vulkan::pInstance->pfn_vkGetPhysicalDeviceImageFormatProperties2
#define vkGetPhysicalDeviceQueueFamilyProperties2 Vulkan::pInstance->pfn_vkGetPhysicalDeviceQueueFamilyProperties2
#define vkGetPhysicalDeviceMemoryProperties2 Vulkan::pInstance->pfn_vkGetPhysicalDeviceMemoryProperties2
#define vkGetPhysicalDeviceSparseImageFormatProperties2 Vulkan::pInstance->pfn_vkGetPhysicalDeviceSparseImageFormatProperties2
#define vkTrimCommandPool Vulkan::pInstance->pfn_vkTrimCommandPool
#define vkGetDeviceQueue2 Vulkan::pInstance->pfn_vkGetDeviceQueue2
#define vkCreateSamplerYcbcrConversion Vulkan::pInstance->pfn_vkCreateSamplerYcbcrConversion
#define vkDestroySamplerYcbcrConversion Vulkan::pInstance->pfn_vkDestroySamplerYcbcrConversion
#define vkCreateDescriptorUpdateTemplate Vulkan::pInstance->pfn_vkCreateDescriptorUpdateTemplate
#define vkDestroyDescriptorUpdateTemplate Vulkan::pInstance->pfn_vkDestroyDescriptorUpdateTemplate
#define vkUpdateDescriptorSetWithTemplate Vulkan::pInstance->pfn_vkUpdateDescriptorSetWithTemplate
#define vkGetPhysicalDeviceExternalBufferProperties Vulkan::pInstance->pfn_vkGetPhysicalDeviceExternalBufferProperties
#define vkGetPhysicalDeviceExternalFenceProperties Vulkan::pInstance->pfn_vkGetPhysicalDeviceExternalFenceProperties
#define vkGetPhysicalDeviceExternalSemaphoreProperties Vulkan::pInstance->pfn_vkGetPhysicalDeviceExternalSemaphoreProperties
#define vkGetDescriptorSetLayoutSupport Vulkan::pInstance->pfn_vkGetDescriptorSetLayoutSupport

// Vulkan 1.2
#define vkCmdDrawIndirectCount Vulkan::pInstance->pfn_vkCmdDrawIndirectCount
#define vkCmdDrawIndexedIndirectCount Vulkan::pInstance->pfn_vkCmdDrawIndexedIndirectCount
#define vkCreateRenderPass2 Vulkan::pInstance->pfn_vkCreateRenderPass2
#define vkCmdBeginRenderPass2 Vulkan::pInstance->pfn_vkCmdBeginRenderPass2
#define vkCmdNextSubpass2 Vulkan::pInstance->pfn_vkCmdNextSubpass2
#define vkCmdEndRenderPass2 Vulkan::pInstance->pfn_vkCmdEndRenderPass2
#define vkResetQueryPool Vulkan::pInstance->pfn_vkResetQueryPool
#define vkGetSemaphoreCounterValue Vulkan::pInstance->pfn_vkGetSemaphoreCounterValue
#define vkWaitSemaphores Vulkan::pInstance->pfn_vkWaitSemaphores
#define vkSignalSemaphore Vulkan::pInstance->pfn_vkSignalSemaphore
#define vkGetBufferDeviceAddress Vulkan::pInstance->pfn_vkGetBufferDeviceAddress
#define vkGetBufferOpaqueCaptureAddress Vulkan::pInstance->pfn_vkGetBufferOpaqueCaptureAddress
#define vkGetDeviceMemoryOpaqueCaptureAddress Vulkan::pInstance->pfn_vkGetDeviceMemoryOpaqueCaptureAddress

// Vulkan 1.3
#define vkGetPhysicalDeviceToolProperties Vulkan::pInstance->pfn_vkGetPhysicalDeviceToolProperties
#define vkCreatePrivateDataSlot Vulkan::pInstance->pfn_vkCreatePrivateDataSlot
#define vkDestroyPrivateDataSlot Vulkan::pInstance->pfn_vkDestroyPrivateDataSlot
#define vkSetPrivateData Vulkan::pInstance->pfn_vkSetPrivateData
#define vkGetPrivateData Vulkan::pInstance->pfn_vkGetPrivateData
#define vkCmdSetEvent2 Vulkan::pInstance->pfn_vkCmdSetEvent2
#define vkCmdResetEvent2 Vulkan::pInstance->pfn_vkCmdResetEvent2
#define vkCmdWaitEvents2 Vulkan::pInstance->pfn_vkCmdWaitEvents2
#define vkCmdPipelineBarrier2 Vulkan::pInstance->pfn_vkCmdPipelineBarrier2
#define vkCmdWriteTimestamp2 Vulkan::pInstance->pfn_vkCmdWriteTimestamp2
#define vkQueueSubmit2 Vulkan::pInstance->pfn_vkQueueSubmit2
#define vkCmdCopyBuffer2 Vulkan::pInstance->pfn_vkCmdCopyBuffer2
#define vkCmdCopyImage2 Vulkan::pInstance->pfn_vkCmdCopyImage2
#define vkCmdCopyBufferToImage2 Vulkan::pInstance->pfn_vkCmdCopyBufferToImage2
#define vkCmdCopyImageToBuffer2 Vulkan::pInstance->pfn_vkCmdCopyImageToBuffer2
#define vkCmdBlitImage2 Vulkan::pInstance->pfn_vkCmdBlitImage2
#define vkCmdResolveImage2 Vulkan::pInstance->pfn_vkCmdResolveImage2
#define vkCmdBeginRendering Vulkan::pInstance->pfn_vkCmdBeginRendering
#define vkCmdEndRendering Vulkan::pInstance->pfn_vkCmdEndRendering
#define vkCmdSetCullMode Vulkan::pInstance->pfn_vkCmdSetCullMode
#define vkCmdSetFrontFace Vulkan::pInstance->pfn_vkCmdSetFrontFace
#define vkCmdSetPrimitiveTopology Vulkan::pInstance->pfn_vkCmdSetPrimitiveTopology
#define vkCmdSetViewportWithCount Vulkan::pInstance->pfn_vkCmdSetViewportWithCount
#define vkCmdSetScissorWithCount Vulkan::pInstance->pfn_vkCmdSetScissorWithCount
#define vkCmdBindVertexBuffers2 Vulkan::pInstance->pfn_vkCmdBindVertexBuffers2
#define vkCmdSetDepthTestEnable Vulkan::pInstance->pfn_vkCmdSetDepthTestEnable
#define vkCmdSetDepthWriteEnable Vulkan::pInstance->pfn_vkCmdSetDepthWriteEnable
#define vkCmdSetDepthCompareOp Vulkan::pInstance->pfn_vkCmdSetDepthCompareOp
#define vkCmdSetDepthBoundsTestEnable Vulkan::pInstance->pfn_vkCmdSetDepthBoundsTestEnable
#define vkCmdSetStencilTestEnable Vulkan::pInstance->pfn_vkCmdSetStencilTestEnable
#define vkCmdSetStencilOp Vulkan::pInstance->pfn_vkCmdSetStencilOp
#define vkCmdSetRasterizerDiscardEnable Vulkan::pInstance->pfn_vkCmdSetRasterizerDiscardEnable
#define vkCmdSetDepthBiasEnable Vulkan::pInstance->pfn_vkCmdSetDepthBiasEnable
#define vkCmdSetPrimitiveRestartEnable Vulkan::pInstance->pfn_vkCmdSetPrimitiveRestartEnable
#define vkGetDeviceBufferMemoryRequirements Vulkan::pInstance->pfn_vkGetDeviceBufferMemoryRequirements
#define vkGetDeviceImageMemoryRequirements Vulkan::pInstance->pfn_vkGetDeviceImageMemoryRequirements
#define vkGetDeviceImageSparseMemoryRequirements Vulkan::pInstance->pfn_vkGetDeviceImageSparseMemoryRequirements

// Vulkan 1.4
/* #define vkCmdSetLineStipple Vulkan::pInstance->pfn_vkCmdSetLineStipple
#define vkMapMemory2 Vulkan::pInstance->pfn_vkMapMemory2
#define vkUnmapMemory2 Vulkan::pInstance->pfn_vkUnmapMemory2
#define vkCmdBindIndexBuffer2 Vulkan::pInstance->pfn_vkCmdBindIndexBuffer2
#define vkGetRenderingAreaGranularity Vulkan::pInstance->pfn_vkGetRenderingAreaGranularity
#define vkGetDeviceImageSubresourceLayout Vulkan::pInstance->pfn_vkGetDeviceImageSubresourceLayout
#define vkGetImageSubresourceLayout2 Vulkan::pInstance->pfn_vkGetImageSubresourceLayout2
#define vkCmdPushDescriptorSet Vulkan::pInstance->pfn_vkCmdPushDescriptorSet
#define vkCmdPushDescriptorSetWithTemplate Vulkan::pInstance->pfn_vkCmdPushDescriptorSetWithTemplate
#define vkCmdSetRenderingAttachmentLocations Vulkan::pInstance->pfn_vkCmdSetRenderingAttachmentLocations
#define vkCmdSetRenderingInputAttachmentIndices Vulkan::pInstance->pfn_vkCmdSetRenderingInputAttachmentIndices
#define vkCmdBindDescriptorSets2 Vulkan::pInstance->pfn_vkCmdBindDescriptorSets2
#define vkCmdPushConstants2 Vulkan::pInstance->pfn_vkCmdPushConstants2
#define vkCmdPushDescriptorSet2 Vulkan::pInstance->pfn_vkCmdPushDescriptorSet2
#define vkCmdPushDescriptorSetWithTemplate2 Vulkan::pInstance->pfn_vkCmdPushDescriptorSetWithTemplate2
#define vkCopyMemoryToImage Vulkan::pInstance->pfn_vkCopyMemoryToImage
#define vkCopyImageToMemory Vulkan::pInstance->pfn_vkCopyImageToMemory
#define vkCopyImageToImage Vulkan::pInstance->pfn_vkCopyImageToImage
#define vkTransitionImageLayout Vulkan::pInstance->pfn_vkTransitionImageLayout */

// Debug Messages
#define vkSetDebugUtilsObjectNameEXT Vulkan::pInstance->pfn_vkSetDebugUtilsObjectNameEXT;
#define vkSetDebugUtilsObjectTagEXT Vulkan::pInstance->pfn_vkSetDebugUtilsObjectTagEXT;
#define vkQueueBeginDebugUtilsLabelEXT Vulkan::pInstance->pfn_vkQueueBeginDebugUtilsLabelEXT;
#define vkQueueEndDebugUtilsLabelEXT Vulkan::pInstance->pfn_vkQueueEndDebugUtilsLabelEXT;
#define vkQueueInsertDebugUtilsLabelEXT Vulkan::pInstance->pfn_vkQueueInsertDebugUtilsLabelEXT;
#define vkCmdBeginDebugUtilsLabelEXT Vulkan::pInstance->pfn_vkCmdBeginDebugUtilsLabelEXT;
#define vkCmdEndDebugUtilsLabelEXT Vulkan::pInstance->pfn_vkCmdEndDebugUtilsLabelEXT;
#define vkCmdInsertDebugUtilsLabelEXT Vulkan::pInstance->pfn_vkCmdInsertDebugUtilsLabelEXT;
#define vkCreateDebugUtilsMessengerEXT Vulkan::pInstance->pfn_vkCreateDebugUtilsMessengerEXT;
#define vkDestroyDebugUtilsMessengerEXT Vulkan::pInstance->pfn_vkDestroyDebugUtilsMessengerEXT;
#define vkSubmitDebugUtilsMessageEXT Vulkan::pInstance->pfn_vkSubmitDebugUtilsMessageEXT;

// Surface
#define vkDestroySurfaceKHR Vulkan::pInstance->pfn_vkDestroySurfaceKHR
#define vkGetPhysicalDeviceSurfaceSupportKHR Vulkan::pInstance->pfn_vkGetPhysicalDeviceSurfaceSupportKHR
#define vkGetPhysicalDeviceSurfaceCapabilitiesKHR Vulkan::pInstance->pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
#define vkGetPhysicalDeviceSurfaceFormatsKHR Vulkan::pInstance->pfn_vkGetPhysicalDeviceSurfaceFormatsKHR
#define vkGetPhysicalDeviceSurfacePresentModesKHR Vulkan::pInstance->pfn_vkGetPhysicalDeviceSurfacePresentModesKHR

// Swapchain
#define vkCreateSwapchainKHR Vulkan::pInstance->pfn_vkCreateSwapchainKHR
#define vkDestroySwapchainKHR Vulkan::pInstance->pfn_vkDestroySwapchainKHR
#define vkGetSwapchainImagesKHR Vulkan::pInstance->pfn_vkGetSwapchainImagesKHR
#define vkAcquireNextImageKHR Vulkan::pInstance->pfn_vkAcquireNextImageKHR
#define vkQueuePresentKHR Vulkan::pInstance->pfn_vkQueuePresentKHR
#define vkGetDeviceGroupPresentCapabilitiesKHR Vulkan::pInstance->pfn_vkGetDeviceGroupPresentCapabilitiesKHR
#define vkGetDeviceGroupSurfacePresentModesKHR Vulkan::pInstance->pfn_vkGetDeviceGroupSurfacePresentModesKHR
#define vkGetPhysicalDevicePresentRectanglesKHR Vulkan::pInstance->pfn_vkGetPhysicalDevicePresentRectanglesKHR
#define vkAcquireNextImage2KHR Vulkan::pInstance->pfn_vkAcquireNextImage2KHR

#ifdef RE_OS_WINDOWS
# define vkCreateWin32SurfaceKHR Vulkan::pInstance->pfn_vkCreateWin32SurfaceKHR
# define vkGetPhysicalDeviceWin32PresentationSupportKHR Vulkan::pInstance->pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR
#elif defined RE_OS_LINUX
# define vkCreateXlibSurfaceKHR Vulkan::pInstance->pfn_vkCreateXlibSurfaceKHR
# define vkGetPhysicalDeviceXlibPresentationSupportKHR Vulkan::pInstance->pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

#define RE_VK_INSTANCE Vulkan::pInstance->get_instance()

#define VK_KHR_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"

}

#endif /* __RE_VULKAN_H__ */
