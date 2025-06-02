#include "RE_Vulkan_Instance.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"

#include <queue>

namespace RE {

#ifdef RE_OS_WINDOWS
	HMODULE hLibVulkan = nullptr;
#elif defined RE_OS_LINUX
	void *hLibVulkan = nullptr;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	VkInstance vk_hInstance = VK_NULL_HANDLE;

	const char *pcVulkanDebugFocusOnFile = nullptr, *pcVulkanDebugFocusOnFunc = nullptr;
	uint32_t u32VulkanDebugFocusOnLine = 0U, u32VulkanDebugFocusCount = 0U;
	VkDebugUtilsMessengerEXT vk_hDebugMessenger = VK_NULL_HANDLE;

	// Vulkan 1.0
	PFN_vkCreateInstance pfn_vkCreateInstance = nullptr;
	PFN_vkDestroyInstance pfn_vkDestroyInstance = nullptr;
	PFN_vkEnumeratePhysicalDevices pfn_vkEnumeratePhysicalDevices = nullptr;
	PFN_vkGetPhysicalDeviceFeatures pfn_vkGetPhysicalDeviceFeatures = nullptr;
	PFN_vkGetPhysicalDeviceFormatProperties pfn_vkGetPhysicalDeviceFormatProperties = nullptr;
	PFN_vkGetPhysicalDeviceImageFormatProperties pfn_vkGetPhysicalDeviceImageFormatProperties = nullptr;
	PFN_vkGetPhysicalDeviceProperties pfn_vkGetPhysicalDeviceProperties = nullptr;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties pfn_vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
	PFN_vkGetPhysicalDeviceMemoryProperties pfn_vkGetPhysicalDeviceMemoryProperties = nullptr;
	PFN_vkGetInstanceProcAddr pfn_vkGetInstanceProcAddr = nullptr;
	PFN_vkGetDeviceProcAddr pfn_vkGetDeviceProcAddr = nullptr;
	PFN_vkCreateDevice pfn_vkCreateDevice = nullptr;
	PFN_vkDestroyDevice pfn_vkDestroyDevice = nullptr;
	PFN_vkEnumerateInstanceExtensionProperties pfn_vkEnumerateInstanceExtensionProperties = nullptr;
	PFN_vkEnumerateDeviceExtensionProperties pfn_vkEnumerateDeviceExtensionProperties = nullptr;
	PFN_vkEnumerateInstanceLayerProperties pfn_vkEnumerateInstanceLayerProperties = nullptr;
	PFN_vkEnumerateDeviceLayerProperties pfn_vkEnumerateDeviceLayerProperties = nullptr;

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

	static PFN_vkVoidFunction load_func_with_instance(VkInstance vk_hInstance, const char* pcFuncName) {
		PFN_vkVoidFunction pFunc;
		CATCH_SIGNAL(pFunc = pfn_vkGetInstanceProcAddr(vk_hInstance, pcFuncName));
		if (!pFunc)
			RE_FATAL_ERROR(append_to_string("Failed loading the Vulkan instance-level function \"", pcFuncName, "\""));
		return pFunc;
	}

	static PFN_vkVoidFunction load_func(const char* pcFuncName) {
		return CATCH_SIGNAL_AND_RETURN(load_func_with_instance(vk_hInstance, pcFuncName), PFN_vkVoidFunction);
	}

#define RE_VK_REQUIRED_EXTENSIONS_COUNT 3U
	const char *pcRequiredExtensions[RE_VK_REQUIRED_EXTENSIONS_COUNT] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, "\0"};
#define RE_VK_REQUIRED_LAYERS_COUNT 1U
	constexpr const char *pcRequiredLayers[RE_VK_REQUIRED_LAYERS_COUNT] = {VK_KHR_VALIDATION_LAYER_NAME};

	static bool create_vulkan_instance() {
		CATCH_SIGNAL(pfn_vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(load_func_with_instance(VK_NULL_HANDLE, "vkCreateInstance")));
		CATCH_SIGNAL(pfn_vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(load_func_with_instance(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties")));
		CATCH_SIGNAL(pfn_vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(load_func_with_instance(VK_NULL_HANDLE, "vkEnumerateInstanceLayerProperties")));
		if (!pfn_vkCreateInstance || !pfn_vkEnumerateInstanceExtensionProperties || !pfn_vkEnumerateInstanceLayerProperties) {
			RE_FATAL_ERROR("Failed loading Vulkan functions, which are essential for creating a Vulkan instance");
			return false;
		}

		bool bFailure = false;
		pcRequiredExtensions[RE_VK_REQUIRED_EXTENSIONS_COUNT - 1U] = Window::pInstance->get_vulkan_required_surface_extension_name();
		uint32_t u32AvailableExtensionsCount = 0U;
		vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, nullptr);
		VkExtensionProperties* vk_pAvailableExtensions = new VkExtensionProperties[u32AvailableExtensionsCount];
		vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, vk_pAvailableExtensions);
		bool bExtensionsPresent[RE_VK_REQUIRED_EXTENSIONS_COUNT] = {};
		std::queue<const char*> missingExtensions;
		PRINT_LN("Available Vulkan instance extensions:");
		for (uint32_t u32ExtensionIndex = 0U; u32ExtensionIndex < u32AvailableExtensionsCount; u32ExtensionIndex++) {
			println(append_to_string("\t", vk_pAvailableExtensions[u32ExtensionIndex].extensionName, " (", VK_API_VERSION_MAJOR(vk_pAvailableExtensions[u32ExtensionIndex].specVersion), ".",VK_API_VERSION_MINOR(vk_pAvailableExtensions[u32ExtensionIndex].specVersion), ".", VK_API_VERSION_PATCH(vk_pAvailableExtensions[u32ExtensionIndex].specVersion), ")"));
			for (uint8_t u8RequiredExtensionIndex = 0U; u8RequiredExtensionIndex < RE_VK_REQUIRED_EXTENSIONS_COUNT; u8RequiredExtensionIndex++)
				if (are_string_contents_equal(vk_pAvailableExtensions[u32ExtensionIndex].extensionName, pcRequiredExtensions[u8RequiredExtensionIndex]))
					bExtensionsPresent[u8RequiredExtensionIndex] = true;
		}
		delete[] vk_pAvailableExtensions;
		for (uint8_t u8RequiredExtensionIndex = 0U; u8RequiredExtensionIndex < RE_VK_REQUIRED_EXTENSIONS_COUNT; u8RequiredExtensionIndex++)
			if (!bExtensionsPresent[u8RequiredExtensionIndex])
				missingExtensions.push(pcRequiredExtensions[u8RequiredExtensionIndex]);
		if (!missingExtensions.empty()) {
			bFailure = true;
			println_colored("\tThe following Vulkan instance extensions are missing on this computer:", RE_TERMINAL_COLOR_RED, false, false);
			do {
				println_colored(append_to_string("\t - ", missingExtensions.front()).c_str(), RE_TERMINAL_COLOR_RED, false, false);
				missingExtensions.pop();
			} while (!missingExtensions.empty());
		}

		uint32_t u32AvailableLayersCount = 0U;
		vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, nullptr);
		VkLayerProperties* vk_pAvailableLayers = new VkLayerProperties[u32AvailableLayersCount];
		vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, vk_pAvailableLayers);
		bool bRequiredLayersPresent[RE_VK_REQUIRED_LAYERS_COUNT] = {};
		std::queue<const char*> missingLayers;
		PRINT_LN("Available Vulkan instance layers:");
		for (uint32_t u32LayerIndex = 0; u32LayerIndex < u32AvailableLayersCount; u32LayerIndex++) {
			println(append_to_string("\t", vk_pAvailableLayers[u32LayerIndex].layerName, " (", VK_API_VERSION_MAJOR(vk_pAvailableLayers[u32LayerIndex].specVersion), ".", VK_API_VERSION_MINOR(vk_pAvailableLayers[u32LayerIndex].specVersion), ".",VK_API_VERSION_PATCH(vk_pAvailableLayers[u32LayerIndex].specVersion), " - ", vk_pAvailableLayers[u32LayerIndex].implementationVersion, "): ", vk_pAvailableLayers[u32LayerIndex].description));
			for (uint8_t u8RequiredLayerIndex = 0U; u8RequiredLayerIndex < RE_VK_REQUIRED_LAYERS_COUNT; u8RequiredLayerIndex++)
				if (are_string_contents_equal(vk_pAvailableLayers[u32LayerIndex].layerName, pcRequiredLayers[u8RequiredLayerIndex]))
					bRequiredLayersPresent[u8RequiredLayerIndex] = true;
		}
		delete[] vk_pAvailableLayers;
		for (uint8_t u8RequiredLayerIndex = 0U; u8RequiredLayerIndex < RE_VK_REQUIRED_LAYERS_COUNT; u8RequiredLayerIndex++)
			if (!bRequiredLayersPresent[u8RequiredLayerIndex])
				missingLayers.push(pcRequiredLayers[u8RequiredLayerIndex]);
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
		std::string strAppName = get_app_name();
		VkApplicationInfo vk_appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = strAppName.c_str(),
			.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
			.pEngineName = "R-Engine",
			.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
			.apiVersion = VK_API_VERSION_1_2
		};
		VkInstanceCreateInfo vk_instanceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &vk_appInfo,
			.enabledLayerCount = RE_VK_REQUIRED_LAYERS_COUNT,
			.ppEnabledLayerNames = pcRequiredLayers,
			.enabledExtensionCount = RE_VK_REQUIRED_EXTENSIONS_COUNT,
			.ppEnabledExtensionNames = pcRequiredExtensions
		};
		if (!CHECK_VK_RESULT(pfn_vkCreateInstance(&vk_instanceCreateInfo, nullptr, &vk_hInstance))) {
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

	/* static bool load_vulkan_1_3_with_instance() {
		pfn_vkGetPhysicalDeviceToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolProperties>(load_func("vkGetPhysicalDeviceToolProperties"));
		if (!pfn_vkGetPhysicalDeviceToolProperties)
			return false;
		return true;
	} */

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
		if (std::strcmp(Window::pInstance->get_vulkan_required_surface_extension_name(), VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME) == 0) {
			pfn_vkCreateWaylandSurfaceKHR = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(load_func("vkCreateWaylandSurfaceKHR"));
			if (!pfn_vkCreateWaylandSurfaceKHR)
				return false;
			pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>(load_func("vkGetPhysicalDeviceWaylandPresentationSupportKHR"));
			if (!pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR)
				return false;
		} else if (std::strcmp(Window::pInstance->get_vulkan_required_surface_extension_name(), VK_KHR_XLIB_SURFACE_EXTENSION_NAME) == 0) {
			pfn_vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(load_func("vkCreateXlibSurfaceKHR"));
			if (!pfn_vkCreateXlibSurfaceKHR)
				return false;
			pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(load_func("vkGetPhysicalDeviceXlibPresentationSupportKHR"));
			if (!pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR)
				return false;
		} else
			return false;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		return true;
	}

	// Avoids dangling pointers (don't remove!)
	static void unload_all_vulkan_functions_of_instance() {
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
		pfn_vkGetPhysicalDeviceToolProperties = nullptr;
		pfn_vkSetDebugUtilsObjectNameEXT = nullptr;
		pfn_vkSetDebugUtilsObjectTagEXT = nullptr;
		pfn_vkCreateDebugUtilsMessengerEXT = nullptr;
		pfn_vkDestroyDebugUtilsMessengerEXT = nullptr;
		pfn_vkSubmitDebugUtilsMessageEXT = nullptr;
		pfn_vkDestroySurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
		pfn_vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
		pfn_vkGetPhysicalDevicePresentRectanglesKHR = nullptr;
#ifdef RE_OS_WINDOWS
		pfn_vkCreateWin32SurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceWin32PresentationSupportKHR = nullptr;
#elif defined RE_OS_LINUX
		pfn_vkCreateWaylandSurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
		pfn_vkCreateXlibSurfaceKHR = nullptr;
		pfn_vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT vk_eSeverityFlagBits, VkDebugUtilsMessageTypeFlagsEXT vk_eMsgTypeBits, const VkDebugUtilsMessengerCallbackDataEXT* vk_pCallbackData, void* vk_pUserData) {
		TerminalColor eConsoleColor = RE_TERMINAL_COLOR_WHITE;
		switch (vk_eSeverityFlagBits) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_RED;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_YELLOW;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_BRIGHT_BLACK;
				break;
			default:
				return VK_FALSE;
		}
		if (!u32VulkanDebugFocusCount) {
			if (pcVulkanDebugFocusOnFile && pcVulkanDebugFocusOnFunc && u32VulkanDebugFocusOnLine)
				println_colored(append_to_string("Vulkan validation layers triggered in ", pcVulkanDebugFocusOnFile, ", in function \"", pcVulkanDebugFocusOnFunc, "\", at line ", u32VulkanDebugFocusOnLine, ":").c_str(), eConsoleColor, false, false);
			else
				println_colored("Vulkan validation layers triggered", eConsoleColor, false, false);
		}
		u32VulkanDebugFocusCount++;
		println_colored(append_to_string("\t[", vk_pCallbackData->pMessageIdName, "] ", vk_pCallbackData->pMessage).c_str(), eConsoleColor, false, false);
		return VK_FALSE;
	}

	static bool setup_validation_layers() {
		VkDebugUtilsMessengerCreateInfoEXT vk_debugCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		vk_debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		vk_debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		vk_debugCreateInfo.pfnUserCallback = debug_callback;
		vk_debugCreateInfo.pUserData = nullptr;
		if (!vkCreateDebugUtilsMessengerEXT(vk_hInstance, &vk_debugCreateInfo, nullptr, &vk_hDebugMessenger)) {
			RE_FATAL_ERROR("Failed creating Vulkan debug messenger for validation layers");
			return false;
		}
		return true;
	}

	bool init_vulkan_instance() {
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(hLibVulkan = LoadLibraryW(L"vulkan-1.dll"));
		if (!hLibVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return false;
		}
		CATCH_SIGNAL(pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(hLibVulkan, "vkGetInstanceProcAddr")));
#elif defined RE_OS_LINUX
		CATCH_SIGNAL(hLibVulkan = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL));
		if (!hLibVulkan) {
			RE_FATAL_ERROR("Failed loading Vulkan library");
			return false;
		}
		CATCH_SIGNAL(pfn_vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(hLibVulkan, "vkGetInstanceProcAddr")));
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		if (!pfn_vkGetInstanceProcAddr) {
			RE_FATAL_ERROR("Failed loading the Vulkan function \"vkGetInstanceProcAddr\" with the OS-specific API");
			return false;
		}
		uint32_t u32ErrorLevel;
#define JUMP_TO_ERR(NUM) do {u32ErrorLevel = NUM; goto RE_VK_INSTANCE_INIT_ERR_LABEL;} while(false)
		if (!CATCH_SIGNAL_AND_RETURN(create_vulkan_instance(), bool)) {
			RE_FATAL_ERROR("Failed creating Vulkan instance");
			JUMP_TO_ERR(0U);
		}
		if (!CATCH_SIGNAL_AND_RETURN(load_vulkan_1_0_with_instance() && load_vulkan_1_1_with_instance() /* && load_vulkan_1_3_with_instance() */ && load_extension_funcs_with_instance() && setup_validation_layers(), bool)) {
			RE_FATAL_ERROR("Failed loading function pointers to Vulkan");
			JUMP_TO_ERR(1U);
		}
		return true;
#undef JUMP_TO_ERR

		RE_VK_INSTANCE_INIT_ERR_LABEL:
		switch (u32ErrorLevel) {
			case 1U:
				CATCH_SIGNAL(pfn_vkDestroyInstance(vk_hInstance, nullptr));
				vk_hInstance = VK_NULL_HANDLE;
			case 0U:
				unload_all_vulkan_functions_of_instance();
			default:
				return false;
		}
	}
	
	void destroy_vulkan_instance() {
		vkDestroyDebugUtilsMessengerEXT(vk_hInstance, vk_hDebugMessenger, nullptr);
		vk_hDebugMessenger = VK_NULL_HANDLE;
		CATCH_SIGNAL(pfn_vkDestroyInstance(vk_hInstance, nullptr));
		vk_hInstance = VK_NULL_HANDLE;
		unload_all_vulkan_functions_of_instance();
#ifdef RE_OS_WINDOWS
		FreeLibrary(hLibVulkan);
#elif defined RE_OS_LINUX
		dlclose(hLibVulkan);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		hLibVulkan = nullptr;
	}

	void focus_vulkan_debug_on(const char *const pcFile, const char *const pcFunc, const uint32_t u32Line) {
		pcVulkanDebugFocusOnFile = pcFile;
		pcVulkanDebugFocusOnFunc = pcFunc;
		u32VulkanDebugFocusOnLine = u32Line;
		u32VulkanDebugFocusCount = 0U;
	}

	void unfocus_vulkan_debug() {
		pcVulkanDebugFocusOnFile = nullptr;
		pcVulkanDebugFocusOnFunc = nullptr;
		u32VulkanDebugFocusOnLine = 0U;
		u32VulkanDebugFocusCount = 0U;
	}

	bool check_vulkan_result(const VkResult vk_eResult, const char *const pcFile, const char *const pcFunc, const uint32_t u32Line) {
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
		error(pcFile, pcFunc, u32Line, append_to_string("The recently called Vulkan function threw an error: ", pcErrName, "\n", pcErrDetail).c_str(), false);
		return false;
	}

}
