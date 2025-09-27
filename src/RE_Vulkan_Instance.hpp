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
#ifndef VK_VERSION_1_3
# error The Vulkan header has to support 1.3 or later to compile successfully
#endif

#define RE_VK_API_VERSION VK_API_VERSION_1_3

#define IS_VULKAN_VERSION_SUPPORTED(VK_VERSION_REQUIRED) (VK_API_VERSION_MAJOR(RE_VK_API_VERSION) == VK_API_VERSION_MAJOR(VK_VERSION_REQUIRED) \
		&& VK_API_VERSION_MINOR(RE_VK_API_VERSION) >= VK_API_VERSION_MINOR(VK_VERSION_REQUIRED) \
		&& VK_API_VERSION_PATCH(RE_VK_API_VERSION) >= VK_API_VERSION_PATCH(VK_VERSION_REQUIRED))

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
	extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties pfn_vkGetPhysicalDeviceSparseImageFormatProperties;

	// Vulkan 1.1
	extern PFN_vkEnumerateInstanceVersion pfn_vkEnumerateInstanceVersion;
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
	extern PFN_vkGetPhysicalDeviceExternalSemaphoreProperties pfn_vkGetPhysicalDeviceExternalSemaphoreProperties;

	// Vulkan 1.3
	extern PFN_vkGetPhysicalDeviceToolProperties pfn_vkGetPhysicalDeviceToolProperties;

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

// Vulkan 1.0
#define vkEnumeratePhysicalDevices(...) pfn_vkEnumeratePhysicalDevices(__VA_ARGS__)
#define vkGetPhysicalDeviceFeatures(...) pfn_vkGetPhysicalDeviceFeatures(__VA_ARGS__)
#define vkGetPhysicalDeviceFormatProperties(...) pfn_vkGetPhysicalDeviceFormatProperties(__VA_ARGS__)
#define vkGetPhysicalDeviceImageFormatProperties(...) pfn_vkGetPhysicalDeviceImageFormatProperties(__VA_ARGS__)
#define vkGetPhysicalDeviceProperties(...) pfn_vkGetPhysicalDeviceProperties(__VA_ARGS__)
#define vkGetPhysicalDeviceQueueFamilyProperties(...) pfn_vkGetPhysicalDeviceQueueFamilyProperties(__VA_ARGS__)
#define vkGetPhysicalDeviceMemoryProperties(...) pfn_vkGetPhysicalDeviceMemoryProperties(__VA_ARGS__)
#define vkGetInstanceProcAddr(...) pfn_vkGetInstanceProcAddr(__VA_ARGS__)
#define vkGetDeviceProcAddr(...) pfn_vkGetDeviceProcAddr(__VA_ARGS__)
#define vkCreateDevice(...) pfn_vkCreateDevice(__VA_ARGS__)
#define vkDestroyDevice(...) pfn_vkDestroyDevice(__VA_ARGS__)
#define vkEnumerateInstanceExtensionProperties(...) pfn_vkEnumerateInstanceExtensionProperties(__VA_ARGS__)
#define vkEnumerateDeviceExtensionProperties(...) pfn_vkEnumerateDeviceExtensionProperties(__VA_ARGS__)
#define vkEnumerateInstanceLayerProperties(...) pfn_vkEnumerateInstanceLayerProperties(__VA_ARGS__)
#define vkEnumerateDeviceLayerProperties(...) pfn_vkEnumerateDeviceLayerProperties(__VA_ARGS__)
#define vkGetPhysicalDeviceSparseImageFormatProperties(...) pfn_vkGetPhysicalDeviceSparseImageFormatProperties(__VA_ARGS__)

// Vulkan 1.1
#define vkEnumerateInstanceVersion(...) pfn_vkEnumerateInstanceVersion(__VA_ARGS__)
#define vkEnumeratePhysicalDeviceGroups(...) pfn_vkEnumeratePhysicalDeviceGroups(__VA_ARGS__)
#define vkGetPhysicalDeviceFeatures2(...) pfn_vkGetPhysicalDeviceFeatures2(__VA_ARGS__)
#define vkGetPhysicalDeviceProperties2(...) pfn_vkGetPhysicalDeviceProperties2(__VA_ARGS__)
#define vkGetPhysicalDeviceFormatProperties2(...) pfn_vkGetPhysicalDeviceFormatProperties2(__VA_ARGS__)
#define vkGetPhysicalDeviceImageFormatProperties2(...) pfn_vkGetPhysicalDeviceImageFormatProperties2(__VA_ARGS__)
#define vkGetPhysicalDeviceQueueFamilyProperties2(...) pfn_vkGetPhysicalDeviceQueueFamilyProperties2(__VA_ARGS__)
#define vkGetPhysicalDeviceMemoryProperties2(...) pfn_vkGetPhysicalDeviceMemoryProperties2(__VA_ARGS__)
#define vkGetPhysicalDeviceSparseImageFormatProperties2(...) pfn_vkGetPhysicalDeviceSparseImageFormatProperties2(__VA_ARGS__)
#define vkGetPhysicalDeviceExternalBufferProperties(...) pfn_vkGetPhysicalDeviceExternalBufferProperties(__VA_ARGS__)
#define vkGetPhysicalDeviceExternalFenceProperties(...) pfn_vkGetPhysicalDeviceExternalFenceProperties(__VA_ARGS__)
#define vkGetPhysicalDeviceExternalSemaphoreProperties(...) pfn_vkGetPhysicalDeviceExternalSemaphoreProperties(__VA_ARGS__)

// Vulkan 1.3
#define vkGetPhysicalDeviceToolProperties(...) pfn_vkGetPhysicalDeviceToolProperties(__VA_ARGS__)

// Debug extension
#define vkSetDebugUtilsObjectNameEXT(...) pfn_vkSetDebugUtilsObjectNameEXT(__VA_ARGS__)
#define vkSetDebugUtilsObjectTagEXT(...) pfn_vkSetDebugUtilsObjectTagEXT(__VA_ARGS__)
#define vkCreateDebugUtilsMessengerEXT(...) pfn_vkCreateDebugUtilsMessengerEXT(__VA_ARGS__)
#define vkDestroyDebugUtilsMessengerEXT(...) pfn_vkDestroyDebugUtilsMessengerEXT(__VA_ARGS__)
#define vkSubmitDebugUtilsMessageEXT(...) pfn_vkSubmitDebugUtilsMessageEXT(__VA_ARGS__)
// Surface extension
#define vkDestroySurfaceKHR(...) pfn_vkDestroySurfaceKHR(__VA_ARGS__)
#define vkGetPhysicalDeviceSurfaceSupportKHR(...) pfn_vkGetPhysicalDeviceSurfaceSupportKHR(__VA_ARGS__)
#define vkGetPhysicalDeviceSurfaceCapabilitiesKHR(...) pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(__VA_ARGS__)
#define vkGetPhysicalDeviceSurfaceFormatsKHR(...) pfn_vkGetPhysicalDeviceSurfaceFormatsKHR(__VA_ARGS__)
#define vkGetPhysicalDeviceSurfacePresentModesKHR(...) pfn_vkGetPhysicalDeviceSurfacePresentModesKHR(__VA_ARGS__)
// Swapchain extensions
#define vkGetPhysicalDevicePresentRectanglesKHR(...) pfn_vkGetPhysicalDevicePresentRectanglesKHR(__VA_ARGS__)

#ifdef RE_OS_WINDOWS
# define vkCreateWin32SurfaceKHR(...) pfn_vkCreateWin32SurfaceKHR(__VA_ARGS__)
# define vkGetPhysicalDeviceWin32PresentationSupportKHR(...) pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR(__VA_ARGS__)
#elif defined RE_OS_LINUX
# define vkCreateWaylandSurfaceKHR(...) pfn_vkCreateWaylandSurfaceKHR(__VA_ARGS__)
# define vkGetPhysicalDeviceWaylandPresentationSupportKHR(...) pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR(__VA_ARGS__)
# define vkCreateXlibSurfaceKHR(...) pfn_vkCreateXlibSurfaceKHR(__VA_ARGS__)
# define vkGetPhysicalDeviceXlibPresentationSupportKHR(...) pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR(__VA_ARGS__)
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

#define VK_KHR_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"

}

#endif /* __RE_VULKAN_INSTANCE_H__ */
