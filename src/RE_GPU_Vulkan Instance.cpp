#include "RE_GPU_Internal.hpp"
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

	VkDebugUtilsMessengerEXT vk_hDebugMessenger = VK_NULL_HANDLE;

	// Vulkan 1.0
	PFN_vkCreateInstance vkCreateInstance = nullptr;
	PFN_vkDestroyInstance vkDestroyInstance = nullptr;
	PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;
	PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures = nullptr;
	PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties = nullptr;
	PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties = nullptr;
	PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = nullptr;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
	PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = nullptr;
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;
	PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = nullptr;
	PFN_vkCreateDevice vkCreateDevice = nullptr;
	PFN_vkDestroyDevice vkDestroyDevice = nullptr;
	PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = nullptr;
	PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties = nullptr;
	PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = nullptr;
	PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties = nullptr;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;

	// Vulkan 1.1
	PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = nullptr;
	PFN_vkEnumeratePhysicalDeviceGroups vkEnumeratePhysicalDeviceGroups = nullptr;
	PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2 = nullptr;
	PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2 = nullptr;
	PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties = nullptr;
	PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties = nullptr;
	PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties = nullptr;

	// Vulkan 1.3
	PFN_vkGetPhysicalDeviceToolProperties vkGetPhysicalDeviceToolProperties = nullptr;

	// Debug Messages
	PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = nullptr;
	PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT = nullptr;
	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
	PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT = nullptr;
	// Surface
	PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
	// Swapchain
	PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR = nullptr;

#ifdef RE_OS_WINDOWS
	// Win32-Surface
	PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR = nullptr;
#elif defined RE_OS_LINUX
	// Wayland-Surface
	PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
	// X11-Surface
	PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR = nullptr;
	PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

	static PFN_vkVoidFunction load_vulkan_func_without_instance(const char *const pacFuncName) {
		PRINT_DEBUG("Loading Vulkan instance-level function \"", pacFuncName, "\" without instance");
		const PFN_vkVoidFunction pFunc = vkGetInstanceProcAddr(VK_NULL_HANDLE, pacFuncName);
		if (!pFunc)
			RE_ERROR("Failed loading the Vulkan instance-level function \"", pacFuncName, "\" without instance");
		return pFunc;
	}

	static PFN_vkVoidFunction load_vulkan_func_with_instance(const char *const pacFuncName) {
		PRINT_DEBUG("Loading Vulkan instance-level function \"", pacFuncName, "\" with instance ", vk_hInstance);
		const PFN_vkVoidFunction pFunc = vkGetInstanceProcAddr(vk_hInstance, pacFuncName);
		if (!pFunc)
			RE_ERROR("Failed loading the Vulkan instance-level function \"", pacFuncName, "\" with instance ", vk_hInstance);
		return pFunc;
	}

	static bool create_vulkan_instance() {
		PRINT_DEBUG("Loading Vulkan function \"vkEnumerateInstanceVersion\"");
		vkEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(load_vulkan_func_without_instance("vkEnumerateInstanceVersion"));
		if (!vkEnumerateInstanceVersion) {
			RE_FATAL_ERROR("Failed to get the Vulkan version on this computer. It either failed loading the function or its version is 1.0. The least version required is ", VK_API_VERSION_MAJOR(RE_VK_API_VERSION), ".", VK_API_VERSION_MINOR(RE_VK_API_VERSION));
			return false;
		}

		{ // Checking version of Vulkan
			PRINT_DEBUG("Querying latest Vulkan version");
			uint32_t u32VulkanVersion;
			if (vkEnumerateInstanceVersion(&u32VulkanVersion) != VK_SUCCESS) {
				RE_FATAL_ERROR("Failed to get the Vulkan version on this computer");
				return false;
			}
			if (IS_VULKAN_VERSION_SUPPORTED(u32VulkanVersion)) {
				RE_FATAL_ERROR("The minimum required Vulkan version ", VK_API_VERSION_MAJOR(RE_VK_API_VERSION), ".", VK_API_VERSION_MINOR(RE_VK_API_VERSION), " is not supported on this computer. This computer has Vulkan ", VK_API_VERSION_MAJOR(u32VulkanVersion), ".", VK_API_VERSION_MINOR(u32VulkanVersion), " installed");
				return false;
			}
		}

		PRINT_DEBUG("Loading Vulkan functions \"vkCreateInstance\", \"vkEnumerateInstanceExtensionProperties\", \"vkEnumerateInstanceLayerProperties\"");
		vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(load_vulkan_func_without_instance("vkCreateInstance"));
		vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(load_vulkan_func_without_instance("vkEnumerateInstanceExtensionProperties"));
		vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(load_vulkan_func_without_instance("vkEnumerateInstanceLayerProperties"));
		if (!vkCreateInstance || !vkEnumerateInstanceExtensionProperties || !vkEnumerateInstanceLayerProperties) {
			RE_FATAL_ERROR("Failed loading Vulkan functions, which are essential for creating a Vulkan instance");
			return false;
		}

		bool bFailure = false;
		
		PRINT_DEBUG("Querying supported Vulkan instance extensions");
		uint32_t u32AvailableExtensionsCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(u32AvailableExtensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &u32AvailableExtensionsCount, availableExtensions.data());
		PRINT_DEBUG("Checking if all necessary Vulkan instance extensions are supported");
		constexpr uint32_t u32RequiredVulkanExtensionCount = 3;
		std::array<const char*, u32RequiredVulkanExtensionCount> apacRequiredExtensions = {{VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, "\0"}};
		apacRequiredExtensions[u32RequiredVulkanExtensionCount - 1] = get_vulkan_required_surface_extension_name();
		std::array<bool, u32RequiredVulkanExtensionCount> abExtensionsPresent = {};
		std::queue<const char*> missingExtensions;
		for (uint32_t u32ExtensionIndex = 0; u32ExtensionIndex < u32AvailableExtensionsCount; u32ExtensionIndex++) {
			PRINT_DEBUG("Vulkan instance extension at index ", u32ExtensionIndex, ", called \"", availableExtensions[u32ExtensionIndex].extensionName, "\", is supported");
			for (uint8_t u8RequiredExtensionIndex = 0; u8RequiredExtensionIndex < u32RequiredVulkanExtensionCount; u8RequiredExtensionIndex++) {
				PRINT_DEBUG("Comparing name to required extension at index ", u8RequiredExtensionIndex, " named \"", apacRequiredExtensions[u8RequiredExtensionIndex], "\"");
				if (are_string_contents_equal(availableExtensions[u32ExtensionIndex].extensionName, apacRequiredExtensions[u8RequiredExtensionIndex]))
					abExtensionsPresent[u8RequiredExtensionIndex] = true;
			}
		}
		for (uint8_t u8RequiredExtensionIndex = 0; u8RequiredExtensionIndex < u32RequiredVulkanExtensionCount; u8RequiredExtensionIndex++) {
			PRINT_DEBUG("Checking availability of Vulkan instance extension at index ", u8RequiredExtensionIndex, ", called \"", apacRequiredExtensions[u8RequiredExtensionIndex], "\"");
			if (!abExtensionsPresent[u8RequiredExtensionIndex])
				missingExtensions.push(apacRequiredExtensions[u8RequiredExtensionIndex]);
		}
		if (!missingExtensions.empty()) {
			bFailure = true;
			println_colored("\tThe following Vulkan instance extensions are missing on this computer:", RE_TERMINAL_COLOR_RED, false, false);
			do {
				println_colored(append_to_string("\t - ", missingExtensions.front()).c_str(), RE_TERMINAL_COLOR_RED, false, false);
				missingExtensions.pop();
			} while (!missingExtensions.empty());
		}

		PRINT_DEBUG("Querying supported Vulkan instance layers");
		uint32_t u32AvailableLayersCount = 0;
		vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(u32AvailableLayersCount);
		vkEnumerateInstanceLayerProperties(&u32AvailableLayersCount, availableLayers.data());
		PRINT_DEBUG("Checking if all necessary Vulkan instance layers are supported");
		constexpr uint32_t u32RequiredVulkanLayerCount = 1;
		const std::array<const char*, u32RequiredVulkanLayerCount> apacRequiredLayers = {{VK_KHR_VALIDATION_LAYER_NAME}};
		std::array<bool, u32RequiredVulkanLayerCount> abRequiredLayersPresent = {};
		std::queue<const char*> missingLayers;
		for (uint32_t u32LayerIndex = 0; u32LayerIndex < u32AvailableLayersCount; u32LayerIndex++) {
			PRINT_DEBUG("Vulkan instance layer at index ", u32LayerIndex, ", called \"", availableLayers[u32LayerIndex].layerName, "\", is supported");
			for (uint8_t u8RequiredLayerIndex = 0; u8RequiredLayerIndex < u32RequiredVulkanLayerCount; u8RequiredLayerIndex++) {
				PRINT_DEBUG("Comparing name to required layer at index ", u8RequiredLayerIndex, " named \"", apacRequiredLayers[u8RequiredLayerIndex], "\"");
				if (are_string_contents_equal(availableLayers[u32LayerIndex].layerName, apacRequiredLayers[u8RequiredLayerIndex]))
					abRequiredLayersPresent[u8RequiredLayerIndex] = true;
			}
		}
		for (uint8_t u8RequiredLayerIndex = 0; u8RequiredLayerIndex < u32RequiredVulkanLayerCount; u8RequiredLayerIndex++) {
			if (!abRequiredLayersPresent[u8RequiredLayerIndex])
				missingLayers.push(apacRequiredLayers[u8RequiredLayerIndex]);
		}
		if (!missingLayers.empty()) {
			bFailure = true;
			println_colored("\tThe following Vulkan instance layers are missing on this computer:", RE_TERMINAL_COLOR_RED, false, false);
			do {
				println_colored(append_to_string("\t - ", missingLayers.front()).c_str(), RE_TERMINAL_COLOR_RED, false, false);
				missingLayers.pop();
			} while (!missingLayers.empty());
		}

		if (bFailure) {
			PRINT_DEBUG("Not all Vulkan instance extensions and/or layers are supported");
			return false;
		}

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
		PRINT_DEBUG("Creating Vulkan instance");
		switch (vkCreateInstance(&vk_instanceCreateInfo, nullptr, &vk_hInstance)) {
			case VK_SUCCESS:
				return true;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				RE_FATAL_ERROR("The Vulkan instance couldn't be created, because the driver doesn't support any Vulkan version higher than 1.0");
				return false;
			default:
				RE_FATAL_ERROR("Failed creating Vulkan instance");
				return false;
		}
	}

	static bool load_vulkan_1_0_with_instance() {
		PRINT_DEBUG("Loading all instance-level Vulkan 1.0-functions");
		// Skipped initialization of "vkCreateInstance", because it's already loaded
		vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(load_vulkan_func_with_instance("vkDestroyInstance"));
		if (!vkDestroyInstance)
			return false;
		vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(load_vulkan_func_with_instance("vkEnumeratePhysicalDevices"));
		if (!vkEnumeratePhysicalDevices)
			return false;
		vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(load_vulkan_func_with_instance("vkGetPhysicalDeviceFeatures"));
		if (!vkGetPhysicalDeviceFeatures)
			return false;
		vkGetPhysicalDeviceFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(load_vulkan_func_with_instance("vkGetPhysicalDeviceFormatProperties"));
		if (!vkGetPhysicalDeviceFormatProperties)
			return false;
		vkGetPhysicalDeviceImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties>(load_vulkan_func_with_instance("vkGetPhysicalDeviceImageFormatProperties"));
		if (!vkGetPhysicalDeviceImageFormatProperties)
			return false;
		vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(load_vulkan_func_with_instance("vkGetPhysicalDeviceProperties"));
		if (!vkGetPhysicalDeviceProperties)
			return false;
		vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(load_vulkan_func_with_instance("vkGetPhysicalDeviceQueueFamilyProperties"));
		if (!vkGetPhysicalDeviceQueueFamilyProperties)
			return false;
		vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(load_vulkan_func_with_instance("vkGetPhysicalDeviceMemoryProperties"));
		if (!vkGetPhysicalDeviceMemoryProperties)
			return false;
		// Skipped initialization of "vkGetInstanceProcAddr", because it's already loaded
		vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(load_vulkan_func_with_instance("vkGetDeviceProcAddr"));
		if (!vkGetDeviceProcAddr)
			return false;
		vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(load_vulkan_func_with_instance("vkCreateDevice"));
		if (!vkCreateDevice)
			return false;
		vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(load_vulkan_func_with_instance("vkDestroyDevice"));
		if (!vkDestroyDevice)
			return false;
		// Skipped initialization of "vkEnumerateInstanceExtensionProperties", because it's already loaded
		vkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(load_vulkan_func_with_instance("vkEnumerateDeviceExtensionProperties"));
		if (!vkEnumerateDeviceExtensionProperties)
			return false;
		// Skipped initialization of "vkEnumerateInstanceLayerProperties", because it's already loaded
		vkEnumerateDeviceLayerProperties = reinterpret_cast<PFN_vkEnumerateDeviceLayerProperties>(load_vulkan_func_with_instance("vkEnumerateDeviceLayerProperties"));
		if (!vkEnumerateDeviceLayerProperties)
			return false;
		vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties>(load_vulkan_func_with_instance("vkGetPhysicalDeviceSparseImageFormatProperties"));
		if (!vkGetPhysicalDeviceSparseImageFormatProperties)
			return false;
		return true;
	}

	static bool load_vulkan_1_1_with_instance() {
		PRINT_DEBUG("Loading all instance-level Vulkan 1.1-functions");
		// Skipped initialization of "vkEnumerateInstanceVersion", because it's already loaded
		vkEnumeratePhysicalDeviceGroups = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroups>(load_vulkan_func_with_instance("vkEnumeratePhysicalDeviceGroups"));
		if (!vkEnumeratePhysicalDeviceGroups)
			return false;
		vkGetPhysicalDeviceFeatures2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(load_vulkan_func_with_instance("vkGetPhysicalDeviceFeatures2"));
		if (!vkGetPhysicalDeviceFeatures2)
			return false;
		vkGetPhysicalDeviceProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(load_vulkan_func_with_instance("vkGetPhysicalDeviceProperties2"));
		if (!vkGetPhysicalDeviceProperties2)
			return false;
		vkGetPhysicalDeviceFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2>(load_vulkan_func_with_instance("vkGetPhysicalDeviceFormatProperties2"));
		if (!vkGetPhysicalDeviceFormatProperties2)
			return false;
		vkGetPhysicalDeviceImageFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2>(load_vulkan_func_with_instance("vkGetPhysicalDeviceImageFormatProperties2"));
		if (!vkGetPhysicalDeviceImageFormatProperties2)
			return false;
		vkGetPhysicalDeviceQueueFamilyProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2>(load_vulkan_func_with_instance("vkGetPhysicalDeviceQueueFamilyProperties2"));
		if (!vkGetPhysicalDeviceQueueFamilyProperties2)
			return false;
		vkGetPhysicalDeviceMemoryProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2>(load_vulkan_func_with_instance("vkGetPhysicalDeviceMemoryProperties2"));
		if (!vkGetPhysicalDeviceMemoryProperties2)
			return false;
		vkGetPhysicalDeviceSparseImageFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2>(load_vulkan_func_with_instance("vkGetPhysicalDeviceSparseImageFormatProperties2"));
		if (!vkGetPhysicalDeviceSparseImageFormatProperties2)
			return false;
		vkGetPhysicalDeviceExternalBufferProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferProperties>(load_vulkan_func_with_instance("vkGetPhysicalDeviceExternalBufferProperties"));
		if (!vkGetPhysicalDeviceExternalBufferProperties)
			return false;
		vkGetPhysicalDeviceExternalFenceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFenceProperties>(load_vulkan_func_with_instance("vkGetPhysicalDeviceExternalFenceProperties"));
		if (!vkGetPhysicalDeviceExternalFenceProperties)
			return false;
		vkGetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphoreProperties>(load_vulkan_func_with_instance("vkGetPhysicalDeviceExternalSemaphoreProperties"));
		if (!vkGetPhysicalDeviceExternalSemaphoreProperties)
			return false;
		return true;
	}

	static bool load_vulkan_1_3_with_instance() {
		PRINT_DEBUG("Loading all instance-level Vulkan 1.3-functions");
		vkGetPhysicalDeviceToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolProperties>(load_vulkan_func_with_instance("vkGetPhysicalDeviceToolProperties"));
		if (!vkGetPhysicalDeviceToolProperties)
			return false;
		return true;
	}

	static bool load_extension_funcs() {
		PRINT_DEBUG("Loading all instance-level extension-functions");
		vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(load_vulkan_func_with_instance("vkSetDebugUtilsObjectNameEXT"));
		if (!vkSetDebugUtilsObjectNameEXT)
			return false;
		vkSetDebugUtilsObjectTagEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectTagEXT>(load_vulkan_func_with_instance("vkSetDebugUtilsObjectTagEXT"));
		if (!vkSetDebugUtilsObjectTagEXT)
			return false;
		vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(load_vulkan_func_with_instance("vkCreateDebugUtilsMessengerEXT"));
		if (!vkCreateDebugUtilsMessengerEXT)
			return false;
		vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(load_vulkan_func_with_instance("vkDestroyDebugUtilsMessengerEXT"));
		if (!vkDestroyDebugUtilsMessengerEXT)
			return false;
		vkSubmitDebugUtilsMessageEXT = reinterpret_cast<PFN_vkSubmitDebugUtilsMessageEXT>(load_vulkan_func_with_instance("vkSubmitDebugUtilsMessageEXT"));
		if (!vkSubmitDebugUtilsMessageEXT)
			return false;
		vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(load_vulkan_func_with_instance("vkDestroySurfaceKHR"));
		if (!vkDestroySurfaceKHR)
			return false;
		vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(load_vulkan_func_with_instance("vkGetPhysicalDeviceSurfaceSupportKHR"));
		if (!vkGetPhysicalDeviceSurfaceSupportKHR)
			return false;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(load_vulkan_func_with_instance("vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
		if (!vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
			return false;
		vkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(load_vulkan_func_with_instance("vkGetPhysicalDeviceSurfaceFormatsKHR"));
		if (!vkGetPhysicalDeviceSurfaceFormatsKHR)
			return false;
		vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(load_vulkan_func_with_instance("vkGetPhysicalDeviceSurfacePresentModesKHR"));
		if (!vkGetPhysicalDeviceSurfacePresentModesKHR)
			return false;
		vkGetPhysicalDevicePresentRectanglesKHR = reinterpret_cast<PFN_vkGetPhysicalDevicePresentRectanglesKHR>(load_vulkan_func_with_instance("vkGetPhysicalDevicePresentRectanglesKHR"));
		if (!vkGetPhysicalDevicePresentRectanglesKHR)
			return false;
#ifdef RE_OS_WINDOWS
		vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(load_vulkan_func_with_instance("vkCreateWin32SurfaceKHR"));
		if (!vkCreateWin32SurfaceKHR)
			return false;
		vkGetPhysicalDeviceWin32PresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(load_vulkan_func_with_instance("vkGetPhysicalDeviceWin32PresentationSupportKHR"));
		if (!vkGetPhysicalDeviceWin32PresentationSupportKHR)
			return false;
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case RE_LINUX_WINDOW_TYPE_X11:
				PRINT_DEBUG("Loading X11-related Vulkan functions");
				vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(load_vulkan_func_with_instance("vkCreateXlibSurfaceKHR"));
				if (!vkCreateXlibSurfaceKHR)
					return false;
				vkGetPhysicalDeviceXlibPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(load_vulkan_func_with_instance("vkGetPhysicalDeviceXlibPresentationSupportKHR"));
				if (!vkGetPhysicalDeviceXlibPresentationSupportKHR)
					return false;
				break;
			case RE_LINUX_WINDOW_TYPE_WAYLAND:
				PRINT_DEBUG("Loading Wayland-related Vulkan functions");
				vkCreateWaylandSurfaceKHR = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(load_vulkan_func_with_instance("vkCreateWaylandSurfaceKHR"));
				if (!vkCreateWaylandSurfaceKHR)
					return false;
				vkGetPhysicalDeviceWaylandPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>(load_vulkan_func_with_instance("vkGetPhysicalDeviceWaylandPresentationSupportKHR"));
				if (!vkGetPhysicalDeviceWaylandPresentationSupportKHR)
					return false;
				break;
			default:
				return false;
		}
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		return true;
	}

	// Avoids dangling pointers and corruption (don't remove!)
	static void unload_all_vulkan_functions_of_instance() {
		PRINT_DEBUG("Resetting all instance-level function pointers to Vulkan");
		// Vulkan 1.0
		vkCreateInstance = nullptr;
		vkDestroyInstance = nullptr;
		vkEnumeratePhysicalDevices = nullptr;
		vkGetPhysicalDeviceFeatures = nullptr;
		vkGetPhysicalDeviceFormatProperties = nullptr;
		vkGetPhysicalDeviceImageFormatProperties = nullptr;
		vkGetPhysicalDeviceProperties = nullptr;
		vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
		vkGetPhysicalDeviceMemoryProperties = nullptr;
		vkGetInstanceProcAddr = nullptr;
		vkGetDeviceProcAddr = nullptr;
		vkCreateDevice = nullptr;
		vkDestroyDevice = nullptr;
		vkEnumerateInstanceExtensionProperties = nullptr;
		vkEnumerateDeviceExtensionProperties = nullptr;
		vkEnumerateInstanceLayerProperties = nullptr;
		vkEnumerateDeviceLayerProperties = nullptr;
		vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;

		// Vulkan 1.1
		vkEnumerateInstanceVersion = nullptr;
		vkEnumeratePhysicalDeviceGroups = nullptr;
		vkGetPhysicalDeviceFeatures2 = nullptr;
		vkGetPhysicalDeviceProperties2 = nullptr;
		vkGetPhysicalDeviceFormatProperties2 = nullptr;
		vkGetPhysicalDeviceImageFormatProperties2 = nullptr;
		vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
		vkGetPhysicalDeviceMemoryProperties2 = nullptr;
		vkGetPhysicalDeviceSparseImageFormatProperties2 = nullptr;
		vkGetPhysicalDeviceExternalBufferProperties = nullptr;
		vkGetPhysicalDeviceExternalFenceProperties = nullptr;
		vkGetPhysicalDeviceExternalSemaphoreProperties = nullptr;

		// Vulkan 1.3
		vkGetPhysicalDeviceToolProperties = nullptr;

		// Debug Messages
		vkSetDebugUtilsObjectNameEXT = nullptr;
		vkSetDebugUtilsObjectTagEXT = nullptr;
		vkCreateDebugUtilsMessengerEXT = nullptr;
		vkDestroyDebugUtilsMessengerEXT = nullptr;
		vkSubmitDebugUtilsMessageEXT = nullptr;
		// Surface
		vkDestroySurfaceKHR = nullptr;
		vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
		vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
		vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
		// Swapchain
		vkGetPhysicalDevicePresentRectanglesKHR = nullptr;
#ifdef RE_OS_WINDOWS
		// Win32-Surface
		vkCreateWin32SurfaceKHR = nullptr;
		vkGetPhysicalDeviceWin32PresentationSupportKHR = nullptr;
#elif defined RE_OS_LINUX
		// Wayland-Surface
		vkCreateWaylandSurfaceKHR = nullptr;
		vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
		// X11-Surface
		vkCreateXlibSurfaceKHR = nullptr;
		vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(const VkDebugUtilsMessageSeverityFlagBitsEXT vk_mSeverityFlag, const VkDebugUtilsMessageTypeFlagsEXT vk_mMsgType, const VkDebugUtilsMessengerCallbackDataEXT *const vk_pCallbackData, void *const vk_pUserData) {
		PRINT_DEBUG("Vulkan called debug message-callback");
		TerminalColor eConsoleColor = RE_TERMINAL_COLOR_WHITE;
		switch (vk_mSeverityFlag) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_BRIGHT_RED;
				u32VulkanErrorCount++;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				eConsoleColor = RE_TERMINAL_COLOR_BRIGHT_YELLOW;
				u32VulkanWarningCount++;
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
		println_colored(append_to_string("[", vk_pCallbackData->pMessageIdName, "]").c_str(), eConsoleColor, false, false);
		const std::string_view msgView(vk_pCallbackData->pMessage);
		const size_t linebreakIndex = msgView.find("\n");
		print("\t");
		println_colored(msgView.substr(0, linebreakIndex).data(), RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
		print("\t");
		println_colored(msgView.substr(linebreakIndex + 1, -1).data(), RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
		return VK_FALSE;
	}

	static bool setup_validation_layers() {
		constexpr VkDebugUtilsMessengerCreateInfoEXT vk_debugCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = debug_callback,
			.pUserData = nullptr
		};
		PRINT_DEBUG("Creating Vulkan debug messenger");
		if (vkCreateDebugUtilsMessengerEXT(vk_hInstance, &vk_debugCreateInfo, nullptr, &vk_hDebugMessenger) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating Vulkan debug messenger for validation layers");
			return false;
		}
		return true;
	}

	bool init_vulkan_instance() {
		PRINT_DEBUG("Loading Vulkan's dynamic library");
#ifdef RE_OS_WINDOWS
		hLibVulkan = LoadLibraryW(L"vulkan-1.dll");
#elif defined RE_OS_LINUX
		hLibVulkan = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		if (!hLibVulkan) {
			RE_FATAL_ERROR("Failed loading the Vulkan library. Make sure Vulkan drivers are installed on your computer");
			return false;
		}

		PRINT_DEBUG("Getting function pointer for \"vkGetInstanceProcAddr\"");
#ifdef RE_OS_WINDOWS
		vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(hLibVulkan, "vkGetInstanceProcAddr"));
#elif defined RE_OS_LINUX
		vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(hLibVulkan, "vkGetInstanceProcAddr"));
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		if (!vkGetInstanceProcAddr) {
			RE_FATAL_ERROR("Failed loading the Vulkan function \"vkGetInstanceProcAddr\" with the OS-specific API");
			return false;
		}

		if (create_vulkan_instance()) {
			if (load_vulkan_1_0_with_instance() && load_vulkan_1_1_with_instance() && load_vulkan_1_3_with_instance() && load_extension_funcs() && setup_validation_layers())
				return true;
			PRINT_DEBUG("Destroying Vulkan instance");
			vkDestroyInstance(vk_hInstance, nullptr);
			vk_hInstance = VK_NULL_HANDLE;
			unload_all_vulkan_functions_of_instance();
		}
		PRINT_DEBUG("Unloading Vulkan's dynamic library");
#ifdef RE_OS_WINDOWS
		FreeLibrary(hLibVulkan);
#elif defined RE_OS_LINUX
		dlclose(hLibVulkan);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		return false;
	}
	
	void destroy_vulkan_instance() {
		PRINT_DEBUG("Destroying Vulkan debug messenger");
		vkDestroyDebugUtilsMessengerEXT(vk_hInstance, vk_hDebugMessenger, nullptr);
		vk_hDebugMessenger = VK_NULL_HANDLE;
		PRINT_DEBUG("Destroying Vulkan instance");
		vkDestroyInstance(vk_hInstance, nullptr);
		vk_hInstance = VK_NULL_HANDLE;
		unload_all_vulkan_functions_of_instance();
		PRINT_DEBUG("Unloading Vulkan's dynamic library");
#ifdef RE_OS_WINDOWS
		FreeLibrary(hLibVulkan);
#elif defined RE_OS_LINUX
		dlclose(hLibVulkan);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		hLibVulkan = nullptr;
	}

}
