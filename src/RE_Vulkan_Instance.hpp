#ifndef __RE_VULKAN_INSTANCE_H__
#define __RE_VULKAN_INSTANCE_H__

#include "RE_Internal Header.hpp"
#ifdef RE_OS_WINDOWS
# define VK_USE_PLATFORM_WIN32_KHR
#elif defined RE_OS_LINUX
# define VK_USE_PLATFORM_WAYLAND_KHR
# define VK_USE_PLATFORM_XLIB_KHR
#endif
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#ifndef VK_API_VERSION_1_2
# error The Vulkan header should support at least all versions up to 1.2
#endif

namespace RE {
	
	extern VkInstance vk_hInstance;
	
	// Vulkan 1.0
	extern PFN_vkEnumeratePhysicalDevices pfn_vkEnumeratePhysicalDevices;
	extern PFN_vkGetPhysicalDeviceFeatures pfn_vkGetPhysicalDeviceFeatures;
	extern PFN_vkGetPhysicalDeviceFormatProperties pfn_vkGetPhysicalDeviceFormatProperties;
	extern PFN_vkGetPhysicalDeviceImageFormatProperties pfn_vkGetPhysicalDeviceImageFormatProperties;
	extern PFN_vkGetPhysicalDeviceProperties pfn_vkGetPhysicalDeviceProperties;
	extern PFN_vkGetPhysicalDeviceQueueFamilyProperties pfn_vkGetPhysicalDeviceQueueFamilyProperties;
	extern PFN_vkGetPhysicalDeviceMemoryProperties pfn_vkGetPhysicalDeviceMemoryProperties;
	extern PFN_vkGetInstanceProcAddr pfn_vkGetInstanceProcAddr;
	extern PFN_vkGetDeviceProcAddr pfn_vkGetDeviceProcAddr;
	extern PFN_vkCreateDevice pfn_vkCreateDevice;
	extern PFN_vkDestroyDevice pfn_vkDestroyDevice;
	extern PFN_vkEnumerateInstanceExtensionProperties pfn_vkEnumerateInstanceExtensionProperties;
	extern PFN_vkEnumerateDeviceExtensionProperties pfn_vkEnumerateDeviceExtensionProperties;
	extern PFN_vkEnumerateInstanceLayerProperties pfn_vkEnumerateInstanceLayerProperties;
	extern PFN_vkEnumerateDeviceLayerProperties pfn_vkEnumerateDeviceLayerProperties;

	// Vulkan 1.1
	/* extern PFN_vkEnumerateInstanceVersion pfn_vkEnumerateInstanceVersion;
	extern PFN_vkEnumeratePhysicalDeviceGroups pfn_vkEnumeratePhysicalDeviceGroups;
	extern PFN_vkGetPhysicalDeviceFeatures2 pfn_vkGetPhysicalDeviceFeatures2;
	extern PFN_vkGetPhysicalDeviceProperties2 pfn_vkGetPhysicalDeviceProperties2;
	extern PFN_vkGetPhysicalDeviceFormatProperties2 pfn_vkGetPhysicalDeviceFormatProperties2;
	extern PFN_vkGetPhysicalDeviceImageFormatProperties2 pfn_vkGetPhysicalDeviceImageFormatProperties2;
	extern PFN_vkGetPhysicalDeviceQueueFamilyProperties2 pfn_vkGetPhysicalDeviceQueueFamilyProperties2;
	extern PFN_vkGetPhysicalDeviceMemoryProperties2 pfn_vkGetPhysicalDeviceMemoryProperties2;
	extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 pfn_vkGetPhysicalDeviceSparseImageFormatProperties2;
	extern PFN_vkGetPhysicalDeviceExternalBufferProperties pfn_vkGetPhysicalDeviceExternalBufferProperties;
	extern PFN_vkGetPhysicalDeviceExternalFenceProperties pfn_vkGetPhysicalDeviceExternalFenceProperties;
	extern PFN_vkGetPhysicalDeviceExternalSemaphoreProperties pfn_vkGetPhysicalDeviceExternalSemaphoreProperties; */

	// Vulkan 1.3
	/* extern PFN_vkGetPhysicalDeviceToolProperties pfn_vkGetPhysicalDeviceToolProperties; */

	// Debug Messages
	extern PFN_vkSetDebugUtilsObjectNameEXT pfn_vkSetDebugUtilsObjectNameEXT;
	extern PFN_vkSetDebugUtilsObjectTagEXT pfn_vkSetDebugUtilsObjectTagEXT;
	extern PFN_vkQueueBeginDebugUtilsLabelEXT pfn_vkQueueBeginDebugUtilsLabelEXT;
	extern PFN_vkQueueEndDebugUtilsLabelEXT pfn_vkQueueEndDebugUtilsLabelEXT;
	extern PFN_vkQueueInsertDebugUtilsLabelEXT pfn_vkQueueInsertDebugUtilsLabelEXT;
	extern PFN_vkCmdBeginDebugUtilsLabelEXT pfn_vkCmdBeginDebugUtilsLabelEXT;
	extern PFN_vkCmdEndDebugUtilsLabelEXT pfn_vkCmdEndDebugUtilsLabelEXT;
	extern PFN_vkCmdInsertDebugUtilsLabelEXT pfn_vkCmdInsertDebugUtilsLabelEXT;
	extern PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT;
	extern PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT;
	extern PFN_vkSubmitDebugUtilsMessageEXT pfn_vkSubmitDebugUtilsMessageEXT;

	// Surface
	extern PFN_vkDestroySurfaceKHR pfn_vkDestroySurfaceKHR;
	extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR pfn_vkGetPhysicalDeviceSurfaceSupportKHR;
	extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
	extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR pfn_vkGetPhysicalDeviceSurfaceFormatsKHR;
	extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR pfn_vkGetPhysicalDeviceSurfacePresentModesKHR;

	// Swapchain
	extern PFN_vkCreateSwapchainKHR pfn_vkCreateSwapchainKHR;
	extern PFN_vkDestroySwapchainKHR pfn_vkDestroySwapchainKHR;
	extern PFN_vkGetSwapchainImagesKHR pfn_vkGetSwapchainImagesKHR;
	extern PFN_vkAcquireNextImageKHR pfn_vkAcquireNextImageKHR;
	extern PFN_vkQueuePresentKHR pfn_vkQueuePresentKHR;
	extern PFN_vkGetDeviceGroupPresentCapabilitiesKHR pfn_vkGetDeviceGroupPresentCapabilitiesKHR;
	extern PFN_vkGetDeviceGroupSurfacePresentModesKHR pfn_vkGetDeviceGroupSurfacePresentModesKHR;
	extern PFN_vkGetPhysicalDevicePresentRectanglesKHR pfn_vkGetPhysicalDevicePresentRectanglesKHR;
	extern PFN_vkAcquireNextImage2KHR pfn_vkAcquireNextImage2KHR;

#ifdef RE_OS_WINDOWS
	// Win32-Surface
	extern PFN_vkCreateWin32SurfaceKHR pfn_vkCreateWin32SurfaceKHR;
	extern PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR;
#elif defined RE_OS_LINUX
	// Wayland-Surface
	extern PFN_vkCreateWaylandSurfaceKHR pfn_vkCreateWaylandSurfaceKHR;
	extern PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR;
	// X11-Surface
	extern PFN_vkCreateXlibSurfaceKHR pfn_vkCreateXlibSurfaceKHR;
	extern PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

	bool init_vulkan_instance();
	void destroy_vulkan_instance();

	void focus_vulkan_debug_on(const char *const pcFile, const char *const pcFunc, const uint32_t u32Line);
	void unfocus_vulkan_debug();
#define FOCUS_FOR_VK_DEBUG(CMD) ([&](const char *const pcFile, const char *const pcActualFunc, const uint32_t u32Line) { \
			add_to_stack_trace(pcFile, pcActualFunc, u32Line, "\0"); \
			focus_vulkan_debug_on(pcFile, pcActualFunc, u32Line); \
			CMD; \
			unfocus_vulkan_debug(); \
			remove_from_stack_trace(); \
		}) (__FILE__, __func__, __LINE__)

	bool check_vulkan_result(const VkResult vk_eResult, const char *const pcFile, const char *const pcFunc, const uint32_t u32Line);
#define CHECK_VK_RESULT(RESULT) ([&](const char *const pcFile, const char *const pcActualFunc, const uint32_t u32Line) -> VkResult { \
			add_to_stack_trace(pcFile, pcActualFunc, u32Line, "\0"); \
			focus_vulkan_debug_on(pcFile, pcActualFunc, u32Line); \
			const VkResult vk_eResult = RESULT; \
			check_vulkan_result(vk_eResult, pcFile, pcActualFunc, u32Line); \
			unfocus_vulkan_debug(); \
			remove_from_stack_trace(); \
			return vk_eResult; \
		}) (__FILE__, __func__, __LINE__)
#define FOCUS_FOR_VK_DEBUG_AND_RETURN(CMD, RETURN_TYPE) ([&](const char *const pcFile, const char *const pcActualFunc, const uint32_t u32Line) -> RETURN_TYPE { \
			add_to_stack_trace(pcFile, pcActualFunc, u32Line, "\0"); \
			focus_vulkan_debug_on(pcFile, pcActualFunc, u32Line); \
			const RETURN_TYPE vk_bResult = CMD; \
			unfocus_vulkan_debug(); \
			remove_from_stack_trace(); \
			return vk_bResult; \
		}) (__FILE__, __func__, __LINE__)

// Vulkan 1.0
#define vkEnumeratePhysicalDevices(...) CHECK_VK_RESULT(pfn_vkEnumeratePhysicalDevices(__VA_ARGS__))
#define vkGetPhysicalDeviceFeatures(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceFeatures(__VA_ARGS__))
#define vkGetPhysicalDeviceFormatProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceFormatProperties(__VA_ARGS__))
#define vkGetPhysicalDeviceImageFormatProperties(...) CHECK_VK_RESULT(pfn_vkGetPhysicalDeviceImageFormatProperties(__VA_ARGS__))
#define vkGetPhysicalDeviceProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceProperties(__VA_ARGS__))
#define vkGetPhysicalDeviceQueueFamilyProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceQueueFamilyProperties(__VA_ARGS__))
#define vkGetPhysicalDeviceMemoryProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceMemoryProperties(__VA_ARGS__))
#define vkGetInstanceProcAddr(...) FOCUS_FOR_VK_DEBUG_AND_RETURN(pfn_vkGetInstanceProcAddr(__VA_ARGS__), PFN_vkVoidFunction)
#define vkGetDeviceProcAddr(...) FOCUS_FOR_VK_DEBUG_AND_RETURN(pfn_vkGetDeviceProcAddr(__VA_ARGS__), PFN_vkVoidFunction)
#define vkCreateDevice(...) CHECK_VK_RESULT(pfn_vkCreateDevice(__VA_ARGS__))
#define vkDestroyDevice(...) FOCUS_FOR_VK_DEBUG(pfn_vkDestroyDevice(__VA_ARGS__))
#define vkEnumerateInstanceExtensionProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkEnumerateInstanceExtensionProperties(__VA_ARGS__))
#define vkEnumerateDeviceExtensionProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkEnumerateDeviceExtensionProperties(__VA_ARGS__))
#define vkEnumerateInstanceLayerProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkEnumerateInstanceLayerProperties(__VA_ARGS__))
#define vkEnumerateDeviceLayerProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkEnumerateDeviceLayerProperties(__VA_ARGS__))
#define vkGetPhysicalDeviceSparseImageFormatProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceSparseImageFormatProperties(__VA_ARGS__))

// Vulkan 1.1
#define vkEnumerateInstanceVersion(...) CHECK_VK_RESULT(pfn_vkEnumerateInstanceVersion(__VA_ARGS__))
#define vkEnumeratePhysicalDeviceGroups(...) CHECK_VK_RESULT(pfn_vkEnumeratePhysicalDeviceGroups(__VA_ARGS__))
#define vkGetPhysicalDeviceFeatures2(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceFeatures2(__VA_ARGS__))
#define vkGetPhysicalDeviceProperties2(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceProperties2(__VA_ARGS__))
#define vkGetPhysicalDeviceFormatProperties2(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceFormatProperties2(__VA_ARGS__))
#define vkGetPhysicalDeviceImageFormatProperties2(...) CHECK_VK_RESULT(pfn_vkGetPhysicalDeviceImageFormatProperties2(__VA_ARGS__))
#define vkGetPhysicalDeviceQueueFamilyProperties2(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceQueueFamilyProperties2(__VA_ARGS__))
#define vkGetPhysicalDeviceMemoryProperties2(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceMemoryProperties2(__VA_ARGS__))
#define vkGetPhysicalDeviceSparseImageFormatProperties2(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceSparseImageFormatProperties2(__VA_ARGS__))
#define vkGetPhysicalDeviceExternalBufferProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceExternalBufferProperties(__VA_ARGS__))
#define vkGetPhysicalDeviceExternalFenceProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceExternalFenceProperties(__VA_ARGS__))
#define vkGetPhysicalDeviceExternalSemaphoreProperties(...) FOCUS_FOR_VK_DEBUG(pfn_vkGetPhysicalDeviceExternalSemaphoreProperties(__VA_ARGS__))

// Vulkan 1.3
/* #define vkGetPhysicalDeviceToolProperties(...) CHECK_VK_RESULT(pfn_vkGetPhysicalDeviceToolProperties(__VA_ARGS__)) */

// Debug extension
#define vkSetDebugUtilsObjectNameEXT(...) CHECK_VK_RESULT(pfn_vkSetDebugUtilsObjectNameEXT(__VA_ARGS__))
#define vkSetDebugUtilsObjectTagEXT(...) CHECK_VK_RESULT(pfn_vkSetDebugUtilsObjectTagEXT(__VA_ARGS__))
#define vkCreateDebugUtilsMessengerEXT(...) CHECK_VK_RESULT(pfn_vkCreateDebugUtilsMessengerEXT(__VA_ARGS__))
#define vkDestroyDebugUtilsMessengerEXT(...) FOCUS_FOR_VK_DEBUG(pfn_vkDestroyDebugUtilsMessengerEXT(__VA_ARGS__))
#define vkSubmitDebugUtilsMessageEXT(...) FOCUS_FOR_VK_DEBUG(pfn_vkSubmitDebugUtilsMessageEXT(__VA_ARGS__))
// Surface extension
#define vkDestroySurfaceKHR(...) FOCUS_FOR_VK_DEBUG(pfn_vkDestroySurfaceKHR(__VA_ARGS__))
#define vkGetPhysicalDeviceSurfaceSupportKHR(...) CHECK_VK_RESULT(pfn_vkGetPhysicalDeviceSurfaceSupportKHR(__VA_ARGS__))
#define vkGetPhysicalDeviceSurfaceCapabilitiesKHR(...) CHECK_VK_RESULT(pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(__VA_ARGS__))
#define vkGetPhysicalDeviceSurfaceFormatsKHR(...) CHECK_VK_RESULT(pfn_vkGetPhysicalDeviceSurfaceFormatsKHR(__VA_ARGS__))
#define vkGetPhysicalDeviceSurfacePresentModesKHR(...) CHECK_VK_RESULT(pfn_vkGetPhysicalDeviceSurfacePresentModesKHR(__VA_ARGS__))
// Swapchain extensions
#define vkGetPhysicalDevicePresentRectanglesKHR(...) CHECK_VK_RESULT(pfn_vkGetPhysicalDevicePresentRectanglesKHR(__VA_ARGS__))

#ifdef RE_OS_WINDOWS
# define vkCreateWin32SurfaceKHR(...) CHECK_VK_RESULT(pfn_vkCreateWin32SurfaceKHR(__VA_ARGS__))
# define vkGetPhysicalDeviceWin32PresentationSupportKHR(...) FOCUS_FOR_VK_DEBUG_AND_RETURN(pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR(__VA_ARGS__), VkBool32)
#elif defined RE_OS_LINUX
# define vkCreateWaylandSurfaceKHR(...) CHECK_VK_RESULT(pfn_vkCreateWaylandSurfaceKHR(__VA_ARGS__))
# define vkGetPhysicalDeviceWaylandPresentationSupportKHR(...) FOCUS_FOR_VK_DEBUG_AND_RETURN(pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR(__VA_ARGS__), VkBool32)
# define vkCreateXlibSurfaceKHR(...) CHECK_VK_RESULT(pfn_vkCreateXlibSurfaceKHR(__VA_ARGS__))
# define vkGetPhysicalDeviceXlibPresentationSupportKHR(...) FOCUS_FOR_VK_DEBUG_AND_RETURN(pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR(__VA_ARGS__), VkBool32)
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

#define VK_KHR_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"

}

#endif /* __RE_VULKAN_INSTANCE_H__ */
