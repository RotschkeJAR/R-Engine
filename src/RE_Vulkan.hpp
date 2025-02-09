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
		uint32_t presentationFamily;
	};
	
	class Vulkan {
		private:
			bool valid;
#ifdef RE_OS_WINDOWS
			HMODULE hVulkan;
#elif defined RE_OS_LINUX
			void* libVulkan;
#endif
			uint32_t internalSurfaceFormatsCount;
			uint32_t internalPresentModesCount;
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
			bool loadExtensionFuncs();
			bool pickPhysicalDevice();
			bool createLogicalDevice();
			bool createWindowSurface();

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
			// Windows-specific
			PFN_vkCreateWin32SurfaceKHR pfn_vkCreateWin32SurfaceKHR;
			PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR;
#elif defined RE_OS_LINUX
			// X11-specific
			PFN_vkCreateXlibSurfaceKHR pfn_vkCreateXlibSurfaceKHR;
			PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

			static Vulkan* instance;
			VkInstance internalInstance;
			VkPhysicalDevice internalPhysicalDevice;
			VkPhysicalDeviceProperties internalPhysicalDeviceProperties;
			VkPhysicalDeviceFeatures internalPhysicalDeviceFeatures;
			VulkanQueueIndices internalQueueIndices;
			VkDevice internalLogicalDevice;
			VkQueue internalGraphicsQueue;
			VkQueue internalPresentationQueue;
			VkSurfaceKHR internalSurface;
			VkSurfaceCapabilitiesKHR internalSurfaceCapabilities;
			VkSurfaceFormatKHR* internalSurfaceFormats;
			VkPresentModeKHR* internalPresentModes;

			Vulkan();
			~Vulkan();
			bool isValid();
			uint32_t getSurfaceFormatsCount();
			uint32_t getPresentModesCount();
			bool checkVulkanResult(VkResult result);
	};

// Vulkan 1.0
#define vkCreateInstance Vulkan::instance->pfn_vkCreateInstance
#define vkDestroyInstance Vulkan::instance->pfn_vkDestroyInstance
#define vkEnumeratePhysicalDevices Vulkan::instance->pfn_vkEnumeratePhysicalDevices
#define vkGetPhysicalDeviceFeatures Vulkan::instance->pfn_vkGetPhysicalDeviceFeatures
#define vkGetPhysicalDeviceFormatProperties Vulkan::instance->pfn_vkGetPhysicalDeviceFormatProperties
#define vkGetPhysicalDeviceImageFormatProperties Vulkan::instance->pfn_vkGetPhysicalDeviceImageFormatProperties
#define vkGetPhysicalDeviceProperties Vulkan::instance->pfn_vkGetPhysicalDeviceProperties
#define vkGetPhysicalDeviceQueueFamilyProperties Vulkan::instance->pfn_vkGetPhysicalDeviceQueueFamilyProperties
#define vkGetPhysicalDeviceMemoryProperties Vulkan::instance->pfn_vkGetPhysicalDeviceMemoryProperties
#define vkGetInstanceProcAddr Vulkan::instance->pfn_vkGetInstanceProcAddr
#define vkGetDeviceProcAddr Vulkan::instance->pfn_vkGetDeviceProcAddr
#define vkCreateDevice Vulkan::instance->pfn_vkCreateDevice
#define vkDestroyDevice Vulkan::instance->pfn_vkDestroyDevice
#define vkEnumerateInstanceExtensionProperties Vulkan::instance->pfn_vkEnumerateInstanceExtensionProperties
#define vkEnumerateDeviceExtensionProperties Vulkan::instance->pfn_vkEnumerateDeviceExtensionProperties
#define vkEnumerateInstanceLayerProperties Vulkan::instance->pfn_vkEnumerateInstanceLayerProperties
#define vkEnumerateDeviceLayerProperties Vulkan::instance->pfn_vkEnumerateDeviceLayerProperties
#define vkGetDeviceQueue Vulkan::instance->pfn_vkGetDeviceQueue
#define vkQueueSubmit Vulkan::instance->pfn_vkQueueSubmit
#define vkQueueWaitIdle Vulkan::instance->pfn_vkQueueWaitIdle
#define vkDeviceWaitIdle Vulkan::instance->pfn_vkDeviceWaitIdle
#define vkAllocateMemory Vulkan::instance->pfn_vkAllocateMemory
#define vkFreeMemory Vulkan::instance->pfn_vkFreeMemory
#define vkMapMemory Vulkan::instance->pfn_vkMapMemory
#define vkUnmapMemory Vulkan::instance->pfn_vkUnmapMemory
#define vkFlushMappedMemoryRanges Vulkan::instance->pfn_vkFlushMappedMemoryRanges
#define vkInvalidateMappedMemoryRanges Vulkan::instance->pfn_vkInvalidateMappedMemoryRanges
#define vkGetDeviceMemoryCommitment Vulkan::instance->pfn_vkGetDeviceMemoryCommitment
#define vkBindBufferMemory Vulkan::instance->pfn_vkBindBufferMemory
#define vkBindImageMemory Vulkan::instance->pfn_vkBindImageMemory
#define vkGetBufferMemoryRequirements Vulkan::instance->pfn_vkGetBufferMemoryRequirements
#define vkGetImageMemoryRequirements Vulkan::instance->pfn_vkGetImageMemoryRequirements
#define vkGetImageSparseMemoryRequirements Vulkan::instance->pfn_vkGetImageSparseMemoryRequirements
#define vkGetPhysicalDeviceSparseImageFormatProperties Vulkan::instance->pfn_vkGetPhysicalDeviceSparseImageFormatProperties
#define vkQueueBindSparse Vulkan::instance->pfn_vkQueueBindSparse
#define vkCreateFence Vulkan::instance->pfn_vkCreateFence
#define vkDestroyFence Vulkan::instance->pfn_vkDestroyFence
#define vkResetFences Vulkan::instance->pfn_vkResetFences
#define vkGetFenceStatus Vulkan::instance->pfn_vkGetFenceStatus
#define vkWaitForFences Vulkan::instance->pfn_vkWaitForFences
#define vkCreateSemaphore Vulkan::instance->pfn_vkCreateSemaphore
#define vkDestroySemaphore Vulkan::instance->pfn_vkDestroySemaphore
#define vkCreateEvent Vulkan::instance->pfn_vkCreateEvent
#define vkDestroyEvent Vulkan::instance->pfn_vkDestroyEvent
#define vkGetEventStatus Vulkan::instance->pfn_vkGetEventStatus
#define vkSetEvent Vulkan::instance->pfn_vkSetEvent
#define vkResetEvent Vulkan::instance->pfn_vkResetEvent
#define vkCreateQueryPool Vulkan::instance->pfn_vkCreateQueryPool
#define vkDestroyQueryPool Vulkan::instance->pfn_vkDestroyQueryPool
#define vkGetQueryPoolResults Vulkan::instance->pfn_vkGetQueryPoolResults
#define vkCreateBuffer Vulkan::instance->pfn_vkCreateBuffer
#define vkDestroyBuffer Vulkan::instance->pfn_vkDestroyBuffer
#define vkCreateBufferView Vulkan::instance->pfn_vkCreateBufferView
#define vkDestroyBufferView Vulkan::instance->pfn_vkDestroyBufferView
#define vkCreateImage Vulkan::instance->pfn_vkCreateImage
#define vkDestroyImage Vulkan::instance->pfn_vkDestroyImage
#define vkGetImageSubresourceLayout Vulkan::instance->pfn_vkGetImageSubresourceLayout
#define vkCreateImageView Vulkan::instance->pfn_vkCreateImageView
#define vkDestroyImageView Vulkan::instance->pfn_vkDestroyImageView
#define vkCreateShaderModule Vulkan::instance->pfn_vkCreateShaderModule
#define vkDestroyShaderModule Vulkan::instance->pfn_vkDestroyShaderModule
#define vkCreatePipelineCache Vulkan::instance->pfn_vkCreatePipelineCache
#define vkDestroyPipelineCache Vulkan::instance->pfn_vkDestroyPipelineCache
#define vkGetPipelineCacheData Vulkan::instance->pfn_vkGetPipelineCacheData
#define vkMergePipelineCaches Vulkan::instance->pfn_vkMergePipelineCaches
#define vkCreateGraphicsPipelines Vulkan::instance->pfn_vkCreateGraphicsPipelines
#define vkCreateComputePipelines Vulkan::instance->pfn_vkCreateComputePipelines
#define vkDestroyPipeline Vulkan::instance->pfn_vkDestroyPipeline
#define vkCreatePipelineLayout Vulkan::instance->pfn_vkCreatePipelineLayout
#define vkDestroyPipelineLayout Vulkan::instance->pfn_vkDestroyPipelineLayout
#define vkCreateSampler Vulkan::instance->pfn_vkCreateSampler
#define vkDestroySampler Vulkan::instance->pfn_vkDestroySampler
#define vkCreateDescriptorSetLayout Vulkan::instance->pfn_vkCreateDescriptorSetLayout
#define vkDestroyDescriptorSetLayout Vulkan::instance->pfn_vkDestroyDescriptorSetLayout
#define vkCreateDescriptorPool Vulkan::instance->pfn_vkCreateDescriptorPool
#define vkDestroyDescriptorPool Vulkan::instance->pfn_vkDestroyDescriptorPool
#define vkResetDescriptorPool Vulkan::instance->pfn_vkResetDescriptorPool
#define vkAllocateDescriptorSets Vulkan::instance->pfn_vkAllocateDescriptorSets
#define vkFreeDescriptorSets Vulkan::instance->pfn_vkFreeDescriptorSets
#define vkUpdateDescriptorSets Vulkan::instance->pfn_vkUpdateDescriptorSets
#define vkCreateFramebuffer Vulkan::instance->pfn_vkCreateFramebuffer
#define vkDestroyFramebuffer Vulkan::instance->pfn_vkDestroyFramebuffer
#define vkCreateRenderPass Vulkan::instance->pfn_vkCreateRenderPass
#define vkDestroyRenderPass Vulkan::instance->pfn_vkDestroyRenderPass
#define vkGetRenderAreaGranularity Vulkan::instance->pfn_vkGetRenderAreaGranularity
#define vkCreateCommandPool Vulkan::instance->pfn_vkCreateCommandPool
#define vkDestroyCommandPool Vulkan::instance->pfn_vkDestroyCommandPool
#define vkResetCommandPool Vulkan::instance->pfn_vkResetCommandPool
#define vkAllocateCommandBuffers Vulkan::instance->pfn_vkAllocateCommandBuffers
#define vkFreeCommandBuffers Vulkan::instance->pfn_vkFreeCommandBuffers
#define vkBeginCommandBuffer Vulkan::instance->pfn_vkBeginCommandBuffer
#define vkEndCommandBuffer Vulkan::instance->pfn_vkEndCommandBuffer
#define vkResetCommandBuffer Vulkan::instance->pfn_vkResetCommandBuffer
#define vkCmdBindPipeline Vulkan::instance->pfn_vkCmdBindPipeline
#define vkCmdSetViewport Vulkan::instance->pfn_vkCmdSetViewport
#define vkCmdSetScissor Vulkan::instance->pfn_vkCmdSetScissor
#define vkCmdSetLineWidth Vulkan::instance->pfn_vkCmdSetLineWidth
#define vkCmdSetDepthBias Vulkan::instance->pfn_vkCmdSetDepthBias
#define vkCmdSetBlendConstants Vulkan::instance->pfn_vkCmdSetBlendConstants
#define vkCmdSetDepthBounds Vulkan::instance->pfn_vkCmdSetDepthBounds
#define vkCmdSetStencilCompareMask Vulkan::instance->pfn_vkCmdSetStencilCompareMask
#define vkCmdSetStencilWriteMask Vulkan::instance->pfn_vkCmdSetStencilWriteMask
#define vkCmdSetStencilReference Vulkan::instance->pfn_vkCmdSetStencilReference
#define vkCmdBindDescriptorSets Vulkan::instance->pfn_vkCmdBindDescriptorSets
#define vkCmdBindIndexBuffer Vulkan::instance->pfn_vkCmdBindIndexBuffer
#define vkCmdBindVertexBuffers Vulkan::instance->pfn_vkCmdBindVertexBuffers
#define vkCmdDraw Vulkan::instance->pfn_vkCmdDraw
#define vkCmdDrawIndexed Vulkan::instance->pfn_vkCmdDrawIndexed
#define vkCmdDrawIndirect Vulkan::instance->pfn_vkCmdDrawIndirect
#define vkCmdDrawIndexedIndirect Vulkan::instance->pfn_vkCmdDrawIndexedIndirect
#define vkCmdDispatch Vulkan::instance->pfn_vkCmdDispatch
#define vkCmdDispatchIndirect Vulkan::instance->pfn_vkCmdDispatchIndirect
#define vkCmdCopyBuffer Vulkan::instance->pfn_vkCmdCopyBuffer
#define vkCmdCopyImage Vulkan::instance->pfn_vkCmdCopyImage
#define vkCmdBlitImage Vulkan::instance->pfn_vkCmdBlitImage
#define vkCmdCopyBufferToImage Vulkan::instance->pfn_vkCmdCopyBufferToImage
#define vkCmdCopyImageToBuffer Vulkan::instance->pfn_vkCmdCopyImageToBuffer
#define vkCmdUpdateBuffer Vulkan::instance->pfn_vkCmdUpdateBuffer
#define vkCmdFillBuffer Vulkan::instance->pfn_vkCmdFillBuffer
#define vkCmdClearColorImage Vulkan::instance->pfn_vkCmdClearColorImage
#define vkCmdClearDepthStencilImage Vulkan::instance->pfn_vkCmdClearDepthStencilImage
#define vkCmdClearAttachments Vulkan::instance->pfn_vkCmdClearAttachments
#define vkCmdResolveImage Vulkan::instance->pfn_vkCmdResolveImage
#define vkCmdSetEvent Vulkan::instance->pfn_vkCmdSetEvent
#define vkCmdResetEvent Vulkan::instance->pfn_vkCmdResetEvent
#define vkCmdWaitEvents Vulkan::instance->pfn_vkCmdWaitEvents
#define vkCmdPipelineBarrier Vulkan::instance->pfn_vkCmdPipelineBarrier
#define vkCmdBeginQuery Vulkan::instance->pfn_vkCmdBeginQuery
#define vkCmdEndQuery Vulkan::instance->pfn_vkCmdEndQuery
#define vkCmdResetQueryPool Vulkan::instance->pfn_vkCmdResetQueryPool
#define vkCmdWriteTimestamp Vulkan::instance->pfn_vkCmdWriteTimestamp
#define vkCmdCopyQueryPoolResults Vulkan::instance->pfn_vkCmdCopyQueryPoolResults
#define vkCmdPushConstants Vulkan::instance->pfn_vkCmdPushConstants
#define vkCmdBeginRenderPass Vulkan::instance->pfn_vkCmdBeginRenderPass
#define vkCmdNextSubpass Vulkan::instance->pfn_vkCmdNextSubpass
#define vkCmdEndRenderPass Vulkan::instance->pfn_vkCmdEndRenderPass
#define vkCmdExecuteCommands Vulkan::instance->pfn_vkCmdExecuteCommands

// Vulkan 1.1
#define vkEnumerateInstanceVersion Vulkan::instance->pfn_vkEnumerateInstanceVersion
#define vkBindBufferMemory2 Vulkan::instance->pfn_vkBindBufferMemory2
#define vkBindImageMemory2 Vulkan::instance->pfn_vkBindImageMemory2
#define vkGetDeviceGroupPeerMemoryFeatures Vulkan::instance->pfn_vkGetDeviceGroupPeerMemoryFeatures
#define vkCmdSetDeviceMask Vulkan::instance->pfn_vkCmdSetDeviceMask
#define vkCmdDispatchBase Vulkan::instance->pfn_vkCmdDispatchBase
#define vkEnumeratePhysicalDeviceGroups Vulkan::instance->pfn_vkEnumeratePhysicalDeviceGroups
#define vkGetImageMemoryRequirements2 Vulkan::instance->pfn_vkGetImageMemoryRequirements2
#define vkGetBufferMemoryRequirements2 Vulkan::instance->pfn_vkGetBufferMemoryRequirements2
#define vkGetImageSparseMemoryRequirements2 Vulkan::instance->pfn_vkGetImageSparseMemoryRequirements2
#define vkGetPhysicalDeviceFeatures2 Vulkan::instance->pfn_vkGetPhysicalDeviceFeatures2
#define vkGetPhysicalDeviceProperties2 Vulkan::instance->pfn_vkGetPhysicalDeviceProperties2
#define vkGetPhysicalDeviceFormatProperties2 Vulkan::instance->pfn_vkGetPhysicalDeviceFormatProperties2
#define vkGetPhysicalDeviceImageFormatProperties2 Vulkan::instance->pfn_vkGetPhysicalDeviceImageFormatProperties2
#define vkGetPhysicalDeviceQueueFamilyProperties2 Vulkan::instance->pfn_vkGetPhysicalDeviceQueueFamilyProperties2
#define vkGetPhysicalDeviceMemoryProperties2 Vulkan::instance->pfn_vkGetPhysicalDeviceMemoryProperties2
#define vkGetPhysicalDeviceSparseImageFormatProperties2 Vulkan::instance->pfn_vkGetPhysicalDeviceSparseImageFormatProperties2
#define vkTrimCommandPool Vulkan::instance->pfn_vkTrimCommandPool
#define vkGetDeviceQueue2 Vulkan::instance->pfn_vkGetDeviceQueue2
#define vkCreateSamplerYcbcrConversion Vulkan::instance->pfn_vkCreateSamplerYcbcrConversion
#define vkDestroySamplerYcbcrConversion Vulkan::instance->pfn_vkDestroySamplerYcbcrConversion
#define vkCreateDescriptorUpdateTemplate Vulkan::instance->pfn_vkCreateDescriptorUpdateTemplate
#define vkDestroyDescriptorUpdateTemplate Vulkan::instance->pfn_vkDestroyDescriptorUpdateTemplate
#define vkUpdateDescriptorSetWithTemplate Vulkan::instance->pfn_vkUpdateDescriptorSetWithTemplate
#define vkGetPhysicalDeviceExternalBufferProperties Vulkan::instance->pfn_vkGetPhysicalDeviceExternalBufferProperties
#define vkGetPhysicalDeviceExternalFenceProperties Vulkan::instance->pfn_vkGetPhysicalDeviceExternalFenceProperties
#define vkGetPhysicalDeviceExternalSemaphoreProperties Vulkan::instance->pfn_vkGetPhysicalDeviceExternalSemaphoreProperties
#define vkGetDescriptorSetLayoutSupport Vulkan::instance->pfn_vkGetDescriptorSetLayoutSupport

// Vulkan 1.2
#define vkCmdDrawIndirectCount Vulkan::instance->pfn_vkCmdDrawIndirectCount
#define vkCmdDrawIndexedIndirectCount Vulkan::instance->pfn_vkCmdDrawIndexedIndirectCount
#define vkCreateRenderPass2 Vulkan::instance->pfn_vkCreateRenderPass2
#define vkCmdBeginRenderPass2 Vulkan::instance->pfn_vkCmdBeginRenderPass2
#define vkCmdNextSubpass2 Vulkan::instance->pfn_vkCmdNextSubpass2
#define vkCmdEndRenderPass2 Vulkan::instance->pfn_vkCmdEndRenderPass2
#define vkResetQueryPool Vulkan::instance->pfn_vkResetQueryPool
#define vkGetSemaphoreCounterValue Vulkan::instance->pfn_vkGetSemaphoreCounterValue
#define vkWaitSemaphores Vulkan::instance->pfn_vkWaitSemaphores
#define vkSignalSemaphore Vulkan::instance->pfn_vkSignalSemaphore
#define vkGetBufferDeviceAddress Vulkan::instance->pfn_vkGetBufferDeviceAddress
#define vkGetBufferOpaqueCaptureAddress Vulkan::instance->pfn_vkGetBufferOpaqueCaptureAddress
#define vkGetDeviceMemoryOpaqueCaptureAddress Vulkan::instance->pfn_vkGetDeviceMemoryOpaqueCaptureAddress

// Vulkan 1.3
#define vkGetPhysicalDeviceToolProperties Vulkan::instance->pfn_vkGetPhysicalDeviceToolProperties
#define vkCreatePrivateDataSlot Vulkan::instance->pfn_vkCreatePrivateDataSlot
#define vkDestroyPrivateDataSlot Vulkan::instance->pfn_vkDestroyPrivateDataSlot
#define vkSetPrivateData Vulkan::instance->pfn_vkSetPrivateData
#define vkGetPrivateData Vulkan::instance->pfn_vkGetPrivateData
#define vkCmdSetEvent2 Vulkan::instance->pfn_vkCmdSetEvent2
#define vkCmdResetEvent2 Vulkan::instance->pfn_vkCmdResetEvent2
#define vkCmdWaitEvents2 Vulkan::instance->pfn_vkCmdWaitEvents2
#define vkCmdPipelineBarrier2 Vulkan::instance->pfn_vkCmdPipelineBarrier2
#define vkCmdWriteTimestamp2 Vulkan::instance->pfn_vkCmdWriteTimestamp2
#define vkQueueSubmit2 Vulkan::instance->pfn_vkQueueSubmit2
#define vkCmdCopyBuffer2 Vulkan::instance->pfn_vkCmdCopyBuffer2
#define vkCmdCopyImage2 Vulkan::instance->pfn_vkCmdCopyImage2
#define vkCmdCopyBufferToImage2 Vulkan::instance->pfn_vkCmdCopyBufferToImage2
#define vkCmdCopyImageToBuffer2 Vulkan::instance->pfn_vkCmdCopyImageToBuffer2
#define vkCmdBlitImage2 Vulkan::instance->pfn_vkCmdBlitImage2
#define vkCmdResolveImage2 Vulkan::instance->pfn_vkCmdResolveImage2
#define vkCmdBeginRendering Vulkan::instance->pfn_vkCmdBeginRendering
#define vkCmdEndRendering Vulkan::instance->pfn_vkCmdEndRendering
#define vkCmdSetCullMode Vulkan::instance->pfn_vkCmdSetCullMode
#define vkCmdSetFrontFace Vulkan::instance->pfn_vkCmdSetFrontFace
#define vkCmdSetPrimitiveTopology Vulkan::instance->pfn_vkCmdSetPrimitiveTopology
#define vkCmdSetViewportWithCount Vulkan::instance->pfn_vkCmdSetViewportWithCount
#define vkCmdSetScissorWithCount Vulkan::instance->pfn_vkCmdSetScissorWithCount
#define vkCmdBindVertexBuffers2 Vulkan::instance->pfn_vkCmdBindVertexBuffers2
#define vkCmdSetDepthTestEnable Vulkan::instance->pfn_vkCmdSetDepthTestEnable
#define vkCmdSetDepthWriteEnable Vulkan::instance->pfn_vkCmdSetDepthWriteEnable
#define vkCmdSetDepthCompareOp Vulkan::instance->pfn_vkCmdSetDepthCompareOp
#define vkCmdSetDepthBoundsTestEnable Vulkan::instance->pfn_vkCmdSetDepthBoundsTestEnable
#define vkCmdSetStencilTestEnable Vulkan::instance->pfn_vkCmdSetStencilTestEnable
#define vkCmdSetStencilOp Vulkan::instance->pfn_vkCmdSetStencilOp
#define vkCmdSetRasterizerDiscardEnable Vulkan::instance->pfn_vkCmdSetRasterizerDiscardEnable
#define vkCmdSetDepthBiasEnable Vulkan::instance->pfn_vkCmdSetDepthBiasEnable
#define vkCmdSetPrimitiveRestartEnable Vulkan::instance->pfn_vkCmdSetPrimitiveRestartEnable
#define vkGetDeviceBufferMemoryRequirements Vulkan::instance->pfn_vkGetDeviceBufferMemoryRequirements
#define vkGetDeviceImageMemoryRequirements Vulkan::instance->pfn_vkGetDeviceImageMemoryRequirements
#define vkGetDeviceImageSparseMemoryRequirements Vulkan::instance->pfn_vkGetDeviceImageSparseMemoryRequirements

// Vulkan 1.4
/* #define vkCmdSetLineStipple Vulkan::instance->pfn_vkCmdSetLineStipple
#define vkMapMemory2 Vulkan::instance->pfn_vkMapMemory2
#define vkUnmapMemory2 Vulkan::instance->pfn_vkUnmapMemory2
#define vkCmdBindIndexBuffer2 Vulkan::instance->pfn_vkCmdBindIndexBuffer2
#define vkGetRenderingAreaGranularity Vulkan::instance->pfn_vkGetRenderingAreaGranularity
#define vkGetDeviceImageSubresourceLayout Vulkan::instance->pfn_vkGetDeviceImageSubresourceLayout
#define vkGetImageSubresourceLayout2 Vulkan::instance->pfn_vkGetImageSubresourceLayout2
#define vkCmdPushDescriptorSet Vulkan::instance->pfn_vkCmdPushDescriptorSet
#define vkCmdPushDescriptorSetWithTemplate Vulkan::instance->pfn_vkCmdPushDescriptorSetWithTemplate
#define vkCmdSetRenderingAttachmentLocations Vulkan::instance->pfn_vkCmdSetRenderingAttachmentLocations
#define vkCmdSetRenderingInputAttachmentIndices Vulkan::instance->pfn_vkCmdSetRenderingInputAttachmentIndices
#define vkCmdBindDescriptorSets2 Vulkan::instance->pfn_vkCmdBindDescriptorSets2
#define vkCmdPushConstants2 Vulkan::instance->pfn_vkCmdPushConstants2
#define vkCmdPushDescriptorSet2 Vulkan::instance->pfn_vkCmdPushDescriptorSet2
#define vkCmdPushDescriptorSetWithTemplate2 Vulkan::instance->pfn_vkCmdPushDescriptorSetWithTemplate2
#define vkCopyMemoryToImage Vulkan::instance->pfn_vkCopyMemoryToImage
#define vkCopyImageToMemory Vulkan::instance->pfn_vkCopyImageToMemory
#define vkCopyImageToImage Vulkan::instance->pfn_vkCopyImageToImage
#define vkTransitionImageLayout Vulkan::instance->pfn_vkTransitionImageLayout */

// Surface
#define vkDestroySurfaceKHR Vulkan::instance->pfn_vkDestroySurfaceKHR
#define vkGetPhysicalDeviceSurfaceSupportKHR Vulkan::instance->pfn_vkGetPhysicalDeviceSurfaceSupportKHR
#define vkGetPhysicalDeviceSurfaceCapabilitiesKHR Vulkan::instance->pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
#define vkGetPhysicalDeviceSurfaceFormatsKHR Vulkan::instance->pfn_vkGetPhysicalDeviceSurfaceFormatsKHR
#define vkGetPhysicalDeviceSurfacePresentModesKHR Vulkan::instance->pfn_vkGetPhysicalDeviceSurfacePresentModesKHR

// Swapchain
#define vkCreateSwapchainKHR Vulkan::instance->pfn_vkCreateSwapchainKHR
#define vkDestroySwapchainKHR Vulkan::instance->pfn_vkDestroySwapchainKHR
#define vkGetSwapchainImagesKHR Vulkan::instance->pfn_vkGetSwapchainImagesKHR
#define vkAcquireNextImageKHR Vulkan::instance->pfn_vkAcquireNextImageKHR
#define vkQueuePresentKHR Vulkan::instance->pfn_vkQueuePresentKHR
#define vkGetDeviceGroupPresentCapabilitiesKHR Vulkan::instance->pfn_vkGetDeviceGroupPresentCapabilitiesKHR
#define vkGetDeviceGroupSurfacePresentModesKHR Vulkan::instance->pfn_vkGetDeviceGroupSurfacePresentModesKHR
#define vkGetPhysicalDevicePresentRectanglesKHR Vulkan::instance->pfn_vkGetPhysicalDevicePresentRectanglesKHR
#define vkAcquireNextImage2KHR Vulkan::instance->pfn_vkAcquireNextImage2KHR

#ifdef RE_OS_WINDOWS
# define vkCreateWin32SurfaceKHR Vulkan::instance->pfn_vkCreateWin32SurfaceKHR
# define vkGetPhysicalDeviceWin32PresentationSupportKHR Vulkan::instance->pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR
#elif defined RE_OS_LINUX
# define vkCreateXlibSurfaceKHR Vulkan::instance->pfn_vkCreateXlibSurfaceKHR
# define vkGetPhysicalDeviceXlibPresentationSupportKHR Vulkan::instance->pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

#define vkInstance Vulkan::instance->internalInstance
#define vkPhysicalDevice Vulkan::instance->internalPhysicalDevice
#define vkPhysicalDeviceProperties Vulkan::instance->internalPhysicalDeviceProperties
#define vkPhysicalDeviceFeatures Vulkan::instance->internalPhysicalDeviceFeatures
#define vkQueueIndices Vulkan::instance->internalQueueIndices
#define vkDevice Vulkan::instance->internalLogicalDevice
#define vkGraphicsQueue Vulkan::instance->internalGraphicsQueue
#define vkPresentQueue Vulkan::instance->internalPresentationQueue
#define vkSurface Vulkan::instance->internalSurface
#define vkSurfaceCapabilities Vulkan::instance->internalSurfaceCapabilities
#define vkSurfaceFormats Vulkan::instance->internalSurfaceFormats
#define vkSurfaceFormatsCount Vulkan::instance->getSurfaceFormatsCount()
#define vkPresentModes Vulkan::instance->internalPresentModes
#define vkPresentModesCount Vulkan::instance->getPresentModesCount()

#define CHECK_VK_RESULT(T) Vulkan::instance->checkVulkanResult(T)

#define VK_KHR_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"

}

#endif /* __RE_VULKAN_H__ */
