#include "RE_Vulkan_Instance.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"

#ifndef VK_VERSION_1_3
# error The Vulkan header should support 1.3 or later
#endif

#include <queue>

namespace RE {

#ifdef RE_OS_WINDOWS
	HMODULE hLibVulkan = nullptr;
#elif defined RE_OS_LINUX
	void *hLibVulkan = nullptr;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	VkInstance vk_hInstance = VK_NULL_HANDLE;

	VkDebugUtilsMessengerEXT vk_hDebugMessenger = VK_NULL_HANDLE;
	uint16_t u16VulkanDebugFocusCount = 0;

	// Vulkan 1.0
	PFN_vkCreateInstance pfn_vkCreateInstance = nullptr;
	PFN_vkDestroyInstance pfn_vkDestroyInstance = nullptr;
	PFN_vkEnumeratePhysicalDevices pfn_vkEnumeratePhysicalDevices = nullptr;
	[[deprecated]] PFN_vkGetPhysicalDeviceFeatures pfn_vkGetPhysicalDeviceFeatures = nullptr;
	[[deprecated]] PFN_vkGetPhysicalDeviceFormatProperties pfn_vkGetPhysicalDeviceFormatProperties = nullptr;
	[[deprecated]] PFN_vkGetPhysicalDeviceImageFormatProperties pfn_vkGetPhysicalDeviceImageFormatProperties = nullptr;
	[[deprecated]] PFN_vkGetPhysicalDeviceProperties pfn_vkGetPhysicalDeviceProperties = nullptr;
	[[deprecated]] PFN_vkGetPhysicalDeviceQueueFamilyProperties pfn_vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
	[[deprecated]] PFN_vkGetPhysicalDeviceMemoryProperties pfn_vkGetPhysicalDeviceMemoryProperties = nullptr;
	PFN_vkGetInstanceProcAddr pfn_vkGetInstanceProcAddr = nullptr;
	PFN_vkGetDeviceProcAddr pfn_vkGetDeviceProcAddr = nullptr;
	PFN_vkCreateDevice pfn_vkCreateDevice = nullptr;
	PFN_vkDestroyDevice pfn_vkDestroyDevice = nullptr;
	PFN_vkEnumerateInstanceExtensionProperties pfn_vkEnumerateInstanceExtensionProperties = nullptr;
	PFN_vkEnumerateDeviceExtensionProperties pfn_vkEnumerateDeviceExtensionProperties = nullptr;
	PFN_vkEnumerateInstanceLayerProperties pfn_vkEnumerateInstanceLayerProperties = nullptr;
	PFN_vkEnumerateDeviceLayerProperties pfn_vkEnumerateDeviceLayerProperties = nullptr;
	[[deprecated]] PFN_vkGetPhysicalDeviceSparseImageFormatProperties pfn_vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;

	// Vulkan 1.1
	PFN_vkEnumerateInstanceVersion pfn_vkEnumerateInstanceVersion = nullptr;
	PFN_vkEnumeratePhysicalDeviceGroups pfn_vkEnumeratePhysicalDeviceGroups = nullptr;
	PFN_vkGetPhysicalDeviceFeatures2 pfn_vkGetPhysicalDeviceFeatures2 = nullptr;
	PFN_vkGetPhysicalDeviceProperties2 pfn_vkGetPhysicalDeviceProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceFormatProperties2 pfn_vkGetPhysicalDeviceFormatProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceImageFormatProperties2 pfn_vkGetPhysicalDeviceImageFormatProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties2 pfn_vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceMemoryProperties2 pfn_vkGetPhysicalDeviceMemoryProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 pfn_vkGetPhysicalDeviceSparseImageFormatProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceExternalBufferProperties pfn_vkGetPhysicalDeviceExternalBufferProperties = nullptr;
	PFN_vkGetPhysicalDeviceExternalFenceProperties pfn_vkGetPhysicalDeviceExternalFenceProperties = nullptr;
	PFN_vkGetPhysicalDeviceExternalSemaphoreProperties pfn_vkGetPhysicalDeviceExternalSemaphoreProperties = nullptr;

	// Vulkan 1.3
	PFN_vkGetPhysicalDeviceToolProperties pfn_vkGetPhysicalDeviceToolProperties = nullptr;

	// Debug Messages
	PFN_vkSetDebugUtilsObjectNameEXT pfn_vkSetDebugUtilsObjectNameEXT = nullptr;
	PFN_vkSetDebugUtilsObjectTagEXT pfn_vkSetDebugUtilsObjectTagEXT = nullptr;
	PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT = nullptr;
	PFN_vkSubmitDebugUtilsMessageEXT pfn_vkSubmitDebugUtilsMessageEXT = nullptr;
	// Surface
	PFN_vkDestroySurfaceKHR pfn_vkDestroySurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR pfn_vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR pfn_vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR pfn_vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
	// Swapchain
	PFN_vkGetPhysicalDevicePresentRectanglesKHR pfn_vkGetPhysicalDevicePresentRectanglesKHR = nullptr;

#ifdef RE_OS_WINDOWS
	// Win32-Surface
	PFN_vkCreateWin32SurfaceKHR pfn_vkCreateWin32SurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR = nullptr;
#elif defined RE_OS_LINUX
	// Wayland-Surface
	PFN_vkCreateWaylandSurfaceKHR pfn_vkCreateWaylandSurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
	// X11-Surface
	PFN_vkCreateXlibSurfaceKHR pfn_vkCreateXlibSurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

	static PFN_vkVoidFunction load_func_with_instance(const VkInstance vk_hInstance, const char *const pacFuncName) {
		PFN_vkVoidFunction pFunc;
		PUSH_TO_CALLSTACKTRACE(pFunc = pfn_vkGetInstanceProcAddr(vk_hInstance, pacFuncName));
		if (!pFunc)
			RE_FATAL_ERROR(append_to_string("Failed loading the Vulkan instance-level function \"", pacFuncName, "\""));
		return pFunc;
	}

	static PFN_vkVoidFunction load_func(const char *const pacFuncName) {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(load_func_with_instance(vk_hInstance, pacFuncName), PFN_vkVoidFunction);
	}

	static bool create_vulkan_instance() {
		PUSH_TO_CALLSTACKTRACE(pfn_vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(load_func_with_instance(VK_NULL_HANDLE, "vkCreateInstance")));
		PUSH_TO_CALLSTACKTRACE(pfn_vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(load_func_with_instance(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties")));
		PUSH_TO_CALLSTACKTRACE(pfn_vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(load_func_with_instance(VK_NULL_HANDLE, "vkEnumerateInstanceLayerProperties")));
		if (!pfn_vkCreateInstance || !pfn_vkEnumerateInstanceExtensionProperties || !pfn_vkEnumerateInstanceLayerProperties) {
			RE_FATAL_ERROR("Failed loading Vulkan functions, which are essential for creating a Vulkan instance");
			return false;
		}

		bool bFailure = false;
		
		uint32_t u32AvailableExtensionsCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, nullptr);
		VkExtensionProperties *const vk_pAvailableExtensions = new VkExtensionProperties[u32AvailableExtensionsCount];
		vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, vk_pAvailableExtensions);
		constexpr uint32_t u32RequiredVulkanExtensionCount = 3;
		std::array<const char*, u32RequiredVulkanExtensionCount> apacRequiredExtensions = {{VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, "\0"}};
		apacRequiredExtensions[u32RequiredVulkanExtensionCount - 1] = get_vulkan_required_surface_extension_name();
		std::array<bool, u32RequiredVulkanExtensionCount> abExtensionsPresent = {};
		std::queue<const char*> missingExtensions;
		if (is_verbose_behaviour_enabled())
			PRINT_LN("Available Vulkan instance extensions:");
		for (uint32_t u32ExtensionIndex = 0; u32ExtensionIndex < u32AvailableExtensionsCount; u32ExtensionIndex++) {
			if (is_verbose_behaviour_enabled())
				println(append_to_string("\t", vk_pAvailableExtensions[u32ExtensionIndex].extensionName, " (", VK_API_VERSION_MAJOR(vk_pAvailableExtensions[u32ExtensionIndex].specVersion), ".",VK_API_VERSION_MINOR(vk_pAvailableExtensions[u32ExtensionIndex].specVersion), ".", VK_API_VERSION_PATCH(vk_pAvailableExtensions[u32ExtensionIndex].specVersion), ")"));
			for (uint8_t u8RequiredExtensionIndex = 0; u8RequiredExtensionIndex < u32RequiredVulkanExtensionCount; u8RequiredExtensionIndex++)
				if (are_string_contents_equal(vk_pAvailableExtensions[u32ExtensionIndex].extensionName, apacRequiredExtensions[u8RequiredExtensionIndex]))
					abExtensionsPresent[u8RequiredExtensionIndex] = true;
		}
		delete[] vk_pAvailableExtensions;
		for (uint8_t u8RequiredExtensionIndex = 0; u8RequiredExtensionIndex < u32RequiredVulkanExtensionCount; u8RequiredExtensionIndex++)
			if (!abExtensionsPresent[u8RequiredExtensionIndex])
				missingExtensions.push(apacRequiredExtensions[u8RequiredExtensionIndex]);
		if (!missingExtensions.empty()) {
			bFailure = true;
			println_colored("\tThe following Vulkan instance extensions are missing on this computer:", RE_TERMINAL_COLOR_RED, false, false);
			do {
				println_colored(append_to_string("\t - ", missingExtensions.front()).c_str(), RE_TERMINAL_COLOR_RED, false, false);
				missingExtensions.pop();
			} while (!missingExtensions.empty());
		}

		uint32_t u32AvailableLayersCount = 0;
		vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, nullptr);
		VkLayerProperties *const vk_pAvailableLayers = new VkLayerProperties[u32AvailableLayersCount];
		vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, vk_pAvailableLayers);
		constexpr uint32_t u32RequiredVulkanLayerCount = 1;
		const std::array<const char*, u32RequiredVulkanLayerCount> apacRequiredLayers = {{VK_KHR_VALIDATION_LAYER_NAME}};
		std::array<bool, u32RequiredVulkanLayerCount> abRequiredLayersPresent = {};
		std::queue<const char*> missingLayers;
		if (is_verbose_behaviour_enabled())
			PRINT_LN("Available Vulkan instance layers:");
		for (uint32_t u32LayerIndex = 0; u32LayerIndex < u32AvailableLayersCount; u32LayerIndex++) {
			if (is_verbose_behaviour_enabled())
				println(append_to_string("\t", vk_pAvailableLayers[u32LayerIndex].layerName, " (", VK_API_VERSION_MAJOR(vk_pAvailableLayers[u32LayerIndex].specVersion), ".", VK_API_VERSION_MINOR(vk_pAvailableLayers[u32LayerIndex].specVersion), ".",VK_API_VERSION_PATCH(vk_pAvailableLayers[u32LayerIndex].specVersion), " - ", vk_pAvailableLayers[u32LayerIndex].implementationVersion, "): ", vk_pAvailableLayers[u32LayerIndex].description));
			for (uint8_t u8RequiredLayerIndex = 0; u8RequiredLayerIndex < u32RequiredVulkanLayerCount; u8RequiredLayerIndex++)
				if (are_string_contents_equal(vk_pAvailableLayers[u32LayerIndex].layerName, apacRequiredLayers[u8RequiredLayerIndex]))
					abRequiredLayersPresent[u8RequiredLayerIndex] = true;
		}
		delete[] vk_pAvailableLayers;
		for (uint8_t u8RequiredLayerIndex = 0; u8RequiredLayerIndex < u32RequiredVulkanLayerCount; u8RequiredLayerIndex++)
			if (!abRequiredLayersPresent[u8RequiredLayerIndex])
				missingLayers.push(apacRequiredLayers[u8RequiredLayerIndex]);
		if (!missingLayers.empty()) {
			bFailure = true;
			println_colored("\tThe following Vulkan instance layers are missing on this computer:", RE_TERMINAL_COLOR_RED, false, false);
			do {
				println_colored(append_to_string("\t - ", missingLayers.front()).c_str(), RE_TERMINAL_COLOR_RED, false, false);
				missingLayers.pop();
			} while (!missingLayers.empty());
		}
		if (bFailure)
			return false;

		const std::string sAppName = get_app_name();
		const VkApplicationInfo vk_appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = sAppName.c_str(),
			.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
			.pEngineName = "R-Engine",
			.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
			.apiVersion = RE_VK_API_VERSION
		};
		const VkInstanceCreateInfo vk_instanceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &vk_appInfo,
			.enabledLayerCount = u32RequiredVulkanLayerCount,
			.ppEnabledLayerNames = apacRequiredLayers.data(),
			.enabledExtensionCount = u32RequiredVulkanExtensionCount,
			.ppEnabledExtensionNames = apacRequiredExtensions.data()
		};
		if (CHECK_VK_RESULT(pfn_vkCreateInstance(&vk_instanceCreateInfo, nullptr, &vk_hInstance)) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating Vulkan instance");
			return false;
		}
		return true;
	}

	static bool load_vulkan_1_0_with_instance() {
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
		// Skipped initialization of "pfn_vkGetInstanceProcAddr", because it's already loaded
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
		pfn_vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties>(load_func("vkGetPhysicalDeviceSparseImageFormatProperties"));
		if (!pfn_vkGetPhysicalDeviceSparseImageFormatProperties)
			return false;
		return true;
	}

	static bool load_vulkan_1_1_with_instance() {
		pfn_vkEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(load_func_with_instance(nullptr, "vkEnumerateInstanceVersion"));
		if (!pfn_vkEnumerateInstanceVersion)
			return false;
		pfn_vkEnumeratePhysicalDeviceGroups = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroups>(load_func("vkEnumeratePhysicalDeviceGroups"));
		if (!pfn_vkEnumeratePhysicalDeviceGroups)
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
		pfn_vkGetPhysicalDeviceExternalBufferProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferProperties>(load_func("vkGetPhysicalDeviceExternalBufferProperties"));
		if (!pfn_vkGetPhysicalDeviceExternalBufferProperties)
			return false;
		pfn_vkGetPhysicalDeviceExternalFenceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFenceProperties>(load_func("vkGetPhysicalDeviceExternalFenceProperties"));
		if (!pfn_vkGetPhysicalDeviceExternalFenceProperties)
			return false;
		pfn_vkGetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphoreProperties>(load_func("vkGetPhysicalDeviceExternalSemaphoreProperties"));
		if (!pfn_vkGetPhysicalDeviceExternalSemaphoreProperties)
			return false;
		return true;
	}

	static bool load_vulkan_1_3_with_instance() {
		pfn_vkGetPhysicalDeviceToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolProperties>(load_func("vkGetPhysicalDeviceToolProperties"));
		if (!pfn_vkGetPhysicalDeviceToolProperties)
			return false;
		return true;
	}

	static bool load_extension_funcs_with_instance() {
		pfn_vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(load_func("vkSetDebugUtilsObjectNameEXT"));
		if (!pfn_vkSetDebugUtilsObjectNameEXT)
			return false;
		pfn_vkSetDebugUtilsObjectTagEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectTagEXT>(load_func("vkSetDebugUtilsObjectTagEXT"));
		if (!pfn_vkSetDebugUtilsObjectTagEXT)
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
		pfn_vkGetPhysicalDevicePresentRectanglesKHR = reinterpret_cast<PFN_vkGetPhysicalDevicePresentRectanglesKHR>(load_func("vkGetPhysicalDevicePresentRectanglesKHR"));
		if (!pfn_vkGetPhysicalDevicePresentRectanglesKHR)
			return false;
#ifdef RE_OS_WINDOWS
		pfn_vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(load_func("vkCreateWin32SurfaceKHR"));
		if (!pfn_vkCreateWin32SurfaceKHR)
			return false;
		pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(load_func("vkGetPhysicalDeviceWin32PresentationSupportKHR"));
		if (!pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR)
			return false;
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LinuxWindowType::X11:
				pfn_vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(load_func("vkCreateXlibSurfaceKHR"));
				if (!pfn_vkCreateXlibSurfaceKHR)
					return false;
				pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(load_func("vkGetPhysicalDeviceXlibPresentationSupportKHR"));
				if (!pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR)
					return false;
				break;
			case LinuxWindowType::Wayland:
				pfn_vkCreateWaylandSurfaceKHR = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(load_func("vkCreateWaylandSurfaceKHR"));
				if (!pfn_vkCreateWaylandSurfaceKHR)
					return false;
				pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>(load_func("vkGetPhysicalDeviceWaylandPresentationSupportKHR"));
				if (!pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR)
					return false;
				break;
			default:
				return false;
		}
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		return true;
	}

	// Avoids dangling pointers (don't remove!)
	static void unload_all_vulkan_functions_of_instance() {
		// Vulkan 1.0
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
		pfn_vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;

		// Vulkan 1.1
		pfn_vkEnumerateInstanceVersion = nullptr;
		pfn_vkEnumeratePhysicalDeviceGroups = nullptr;
		pfn_vkGetPhysicalDeviceFeatures2 = nullptr;
		pfn_vkGetPhysicalDeviceProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceFormatProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceImageFormatProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceMemoryProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceSparseImageFormatProperties2 = nullptr;
		pfn_vkGetPhysicalDeviceExternalBufferProperties = nullptr;
		pfn_vkGetPhysicalDeviceExternalFenceProperties = nullptr;
		pfn_vkGetPhysicalDeviceExternalSemaphoreProperties = nullptr;

		// Vulkan 1.3
		pfn_vkGetPhysicalDeviceToolProperties = nullptr;

		// Debug Messages
		pfn_vkSetDebugUtilsObjectNameEXT = nullptr;
		pfn_vkSetDebugUtilsObjectTagEXT = nullptr;
		pfn_vkCreateDebugUtilsMessengerEXT = nullptr;
		pfn_vkDestroyDebugUtilsMessengerEXT = nullptr;
		pfn_vkSubmitDebugUtilsMessageEXT = nullptr;
		// Surface
		pfn_vkDestroySurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
		// Swapchain
		pfn_vkGetPhysicalDevicePresentRectanglesKHR = nullptr;
#ifdef RE_OS_WINDOWS
		// Win32-Surface
		pfn_vkCreateWin32SurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR = nullptr;
#elif defined RE_OS_LINUX
		// Wayland-Surface
		pfn_vkCreateWaylandSurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
		// X11-Surface
		pfn_vkCreateXlibSurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(const VkDebugUtilsMessageSeverityFlagBitsEXT vk_eSeverityFlagBits, const VkDebugUtilsMessageTypeFlagsEXT vk_eMsgTypeBits, const VkDebugUtilsMessengerCallbackDataEXT *const vk_pCallbackData, void *const vk_pUserData) {
		TerminalColor eConsoleColor = RE_TERMINAL_COLOR_WHITE;
		switch (vk_eSeverityFlagBits) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_BRIGHT_RED;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_BRIGHT_YELLOW;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_BRIGHT_BLACK;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_BLACK;
				break;
			default:
				return VK_FALSE;
		}
		if (!u16VulkanDebugFocusCount) {
			println_colored("The Vulkan validation layers were triggered", RE_TERMINAL_COLOR_WHITE, false, false);
			print_call_stack_trace();
		}
		u16VulkanDebugFocusCount++;
		print("\t");
		println_colored(append_to_string("[", vk_pCallbackData->pMessageIdName, "]").c_str(), eConsoleColor, false, false);
		const std::string_view msgView(vk_pCallbackData->pMessage);
		const size_t linebreakIndex = msgView.find("\n");
		print("\t\t");
		println_colored(msgView.substr(0, linebreakIndex).data(), RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
		print("\t\t");
		println_colored(msgView.substr(linebreakIndex + 1, -1).data(), RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
		return VK_FALSE;
	}

	static bool setup_validation_layers() {
		VkDebugUtilsMessengerCreateInfoEXT vk_debugCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = debug_callback,
			.pUserData = nullptr
		};
		if (is_verbose_behaviour_enabled())
			vk_debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
		if (vkCreateDebugUtilsMessengerEXT(vk_hInstance, &vk_debugCreateInfo, nullptr, &vk_hDebugMessenger) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating Vulkan debug messenger for validation layers");
			return false;
		}
		return true;
	}

	bool init_vulkan_instance() {
#ifdef RE_OS_WINDOWS
		PUSH_TO_CALLSTACKTRACE(hLibVulkan = LoadLibraryW(L"vulkan-1.dll"));
		if (!hLibVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return false;
		}
		PUSH_TO_CALLSTACKTRACE(pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(hLibVulkan, "vkGetInstanceProcAddr")));
#elif defined RE_OS_LINUX
		PUSH_TO_CALLSTACKTRACE(hLibVulkan = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL));
		if (!hLibVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return false;
		}
		PUSH_TO_CALLSTACKTRACE(pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(hLibVulkan, "vkGetInstanceProcAddr")));
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		if (!pfn_vkGetInstanceProcAddr) {
			RE_FATAL_ERROR("Failed loading the Vulkan function \"vkGetInstanceProcAddr\" with the OS-specific API");
			return false;
		}
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_instance(), bool)) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(load_vulkan_1_0_with_instance() && load_vulkan_1_1_with_instance() && load_vulkan_1_3_with_instance() && load_extension_funcs_with_instance() && setup_validation_layers(), bool))
				return true;
			unload_all_vulkan_functions_of_instance();
			PUSH_TO_CALLSTACKTRACE(pfn_vkDestroyInstance(vk_hInstance, nullptr));
			vk_hInstance = VK_NULL_HANDLE;
		}
		return false;
	}
	
	void destroy_vulkan_instance() {
		vkDestroyDebugUtilsMessengerEXT(vk_hInstance, vk_hDebugMessenger, nullptr);
		vk_hDebugMessenger = VK_NULL_HANDLE;
		PUSH_TO_CALLSTACKTRACE(pfn_vkDestroyInstance(vk_hInstance, nullptr));
		vk_hInstance = VK_NULL_HANDLE;
		unload_all_vulkan_functions_of_instance();
#ifdef RE_OS_WINDOWS
		FreeLibrary(hLibVulkan);
#elif defined RE_OS_LINUX
		dlclose(hLibVulkan);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		hLibVulkan = nullptr;
	}

	void reset_vulkan_validation_layer_report_count() {
		u16VulkanDebugFocusCount = 0;
	}

	bool check_vulkan_result(const VkResult vk_eResult, const char *const pacFile, const char *const pacFunc, const uint32_t u32Line) {
		const char* pacErrName = "Unknown Vulkan result";
		const char* pacErrDetail = "Unknown Vulkan Result enumeration value";
		bool bCrash = false;
		switch (vk_eResult) {
			case VK_SUCCESS:
				return true;

			// Success codes, but treated as errors
			case VK_NOT_READY:
				pacErrName = "VK_NOT_READY";
				pacErrDetail = "A fence or query has not completed yet";
				break;
			case VK_TIMEOUT:
				pacErrName = "VK_TIMEOUT";
				pacErrDetail = "A wait operation has not completed in the specified time";
				break;
			case VK_EVENT_SET:
				pacErrName = "VK_EVENT_SET";
				pacErrDetail = "Event signaled";
				break;
			case VK_EVENT_RESET:
				pacErrName = "VK_EVENT_RESET";
				pacErrDetail = "Event unsignaled";
				break;
			case VK_INCOMPLETE:
				pacErrName = "VK_INCOMPLETE";
				pacErrDetail = "A return array was too small for the result";
				break;
			case VK_SUBOPTIMAL_KHR:
				pacErrName = "VK_SUBOPTIMAL_KHR";
				pacErrDetail = "A swapchain no longer matches the surface properties exactly";
				break;
			case VK_THREAD_IDLE_KHR:
				pacErrName = "VK_THREAD_IDLE_KHR";
				pacErrDetail = "A deferred operation is not complete, but there's currently no work for this thread";
				break;
			case VK_THREAD_DONE_KHR:
				pacErrName = "VK_THREAD_DONE_KHR";
				pacErrDetail = "A deferred operation is not complete, but there's no work remaining";
				break;
			case VK_OPERATION_DEFERRED_KHR:
				pacErrName = "VK_OPERATION_DEFERRED_KHR";
				pacErrDetail = "A deferred operation was requested and some of the work has been deferred";
				break;
			case VK_OPERATION_NOT_DEFERRED_KHR:
				pacErrName = "VK_OPERATION_NOT_DEFERRED_KHR";
				pacErrDetail = "A deferred operation was requested and no operations were deferred";
				break;
			case VK_PIPELINE_COMPILE_REQUIRED:
				pacErrName = "VK_PIPELINE_COMPILE_REQUIRED";
				pacErrDetail = "Pipeline compilation required, but not done by the application";
				break;
#ifdef RE_OS_WINDOWS
			case VK_PIPELINE_BINARY_MISSING_KHR:
				pacErrName = "VK_PIPELINE_BINARY_MISSING_KHR";
				pacErrDetail = "Attempted to create a pipeline binary by querying an internal cache, but the internal cache entry did not exist";
				break;
			case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
				pacErrName = "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
				pacErrDetail = "The binary shader code is incompatible with the device";
				break;
#endif

			// Errors
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				pacErrName = "VK_ERROR_OUT_OF_HOST_MEMORY";
				pacErrDetail = "PC ran out of memory";
				bCrash = true;
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				pacErrName = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
				pacErrDetail = "GPU ran out of memory";
				bCrash = true;
				break;
			case VK_ERROR_INITIALIZATION_FAILED:
				pacErrName = "VK_ERROR_INITIALIZATION_FAILED";
				pacErrDetail = "Initialization failed";
				break;
			case VK_ERROR_DEVICE_LOST:
				pacErrName = "VK_ERROR_DEVICE_LOST";
				pacErrDetail = "Fatal error on GPU, which caused termination for segmentation violation in shaders, floating-point exception or other. Aborting...";
				bCrash = true;
				break;
			case VK_ERROR_MEMORY_MAP_FAILED:
				pacErrName = "VK_ERROR_MEMORY_MAP_FAILED";
				pacErrDetail = "Mapping memory failed";
				break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				pacErrName = "VK_ERROR_LAYER_NOT_PRESENT";
				pacErrDetail = "Requested Vulkan layer not present";
				break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				pacErrName = "VK_ERROR_EXTENSION_NOT_PRESENT";
				pacErrDetail = "Requested Vulkan extension not present";
				break;
			case VK_ERROR_FEATURE_NOT_PRESENT:
				pacErrName = "VK_ERROR_FEATURE_NOT_PRESENT";
				pacErrDetail = "Requested GPU-feature not present";
				break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				pacErrName = "VK_ERROR_INCOMPATIBLE_DRIVER";
				pacErrDetail = "Incompatible driver";
				break;
			case VK_ERROR_TOO_MANY_OBJECTS:
				pacErrName = "VK_ERROR_TOO_MANY_OBJECTS";
				pacErrDetail = "Too many objects";
				break;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				pacErrName = "VK_ERROR_FORMAT_NOT_SUPPORTED";
				pacErrDetail = "Format not supported on this GPU";
				break;
			case VK_ERROR_FRAGMENTED_POOL:
				pacErrName = "VK_ERROR_FRAGMENTED_POOL";
				pacErrDetail = "Memory fragmentation in pool";
				break;
			case VK_ERROR_SURFACE_LOST_KHR:
				pacErrName = "VK_ERROR_SURFACE_LOST_KHR";
				pacErrDetail = "Surface has been lost and is no longer available";
				break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				pacErrName = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
				pacErrDetail = "The window is already in use by any API";
				break;
			case VK_ERROR_OUT_OF_DATE_KHR:
				pacErrName = "VK_ERROR_OUT_OF_DATE_KHR";
				pacErrDetail = "A surface has changed in a way that it's no longer compatible with the swapchain and further operations will fail";
				break;
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				pacErrName = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
				pacErrDetail = "The display used by a swapchain doesn't use the same presentable image layout or is incompatible";
				break;
			case VK_ERROR_INVALID_SHADER_NV:
				pacErrName = "VK_ERROR_INVALID_SHADER_NV";
				pacErrDetail = "One or more shaders failed to compile or link";
				break;
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				pacErrName = "VK_ERROR_OUT_OF_POOL_MEMORY";
				pacErrDetail = "Out of pool memory";
				break;
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:
				pacErrName = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
				pacErrDetail = "Invalid external handle";
				break;
			case VK_ERROR_FRAGMENTATION:
				pacErrName = "VK_ERROR_FRAGMENTATION";
				pacErrDetail = "Fragmentation";
				break;
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
				pacErrName = "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
				pacErrDetail = "The memory address is not available";
				break;
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
				pacErrName = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
				pacErrDetail = "Operation on swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT has failed, because the application didn't have exclusive fullscreen access (outside the application's control)";
				break;
			case VK_ERROR_VALIDATION_FAILED_EXT:
				pacErrName = "VK_ERROR_VALIDATION_FAILED_EXT";
				pacErrDetail = "Invalid usage detected";
				break;
			case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
				pacErrName = "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
				pacErrDetail = "Image creation failed for internal resources required for compression are exhausted";
				break;
			case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
				pacErrName = "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
				pacErrDetail = "Requested VkImageUsageFlags aren't supported";
				break;
			case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
				pacErrName = "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
				pacErrDetail = "Requested video picture layout not supported";
				break;
			case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
				pacErrName = "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
				pacErrDetail = "Video profile operation not supported";
				break;
			case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
				pacErrName = "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
				pacErrDetail = "Video profile format parameters not supported";
				break;
			case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
				pacErrName = "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
				pacErrDetail = "Video profile codec not supported";
				break;
			case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
				pacErrName = "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
				pacErrDetail = "Specified video STD header version is not supported";
				break;
#ifdef RE_OS_WINDOWS
			case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
				pacErrName = "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
				pacErrDetail = "Invalid video STD parameters";
				break;
#endif
			case VK_ERROR_NOT_PERMITTED_KHR:
				pacErrName = "VK_ERROR_NOT_PERMITTED_KHR";
				pacErrDetail = "Action is not permitted to be executed due to the application's missing privileges";
				break;
#ifdef RE_OS_WINDOWS
			case VK_ERROR_NOT_ENOUGH_SPACE_KHR:
				pacErrName = "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
				pacErrDetail = "Application didn't provide enough space to return the data";
				break;
#endif
			case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
				pacErrName = "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
				pacErrDetail = "";
				break;
			case VK_ERROR_UNKNOWN:
				pacErrName = "VK_ERROR_UNKNOWN";
				pacErrDetail = "Unknown error";
				break;
			case VK_RESULT_MAX_ENUM:
			default:
				break;
		}
		print(pacFile, " (line ", u32Line, "): ");
		println_colored(append_to_string(pacErrName, ": ", pacErrDetail).c_str(), RE_TERMINAL_COLOR_BRIGHT_RED, false, false);
		if (bCrash)
			std::abort();
		return false;
	}

}
