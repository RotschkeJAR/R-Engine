#include "RE_Render System.hpp"
#include "RE_Window_Win64.hpp"
#include "RE_Window_X11.hpp"
#include "RE_Window_Wayland.hpp"
#include "RE_Main.hpp"

#include "RE_Input.hpp"

namespace RE {

#define RE_VK_QUEUE_GRAPHICS_INDEX 0U
#define RE_VK_QUEUE_PRESENT_INDEX 1U
#define RE_VK_QUEUE_TRANSFER_INDEX 2U

#define RE_VK_COMMAND_POOL_GRAPHICS_INDEX 0U
#define RE_VK_COMMAND_POOL_TRANSFER_INDEX 1U

#define RE_VK_SEMAPHORE_IMAGE_AVAILABLE_INDEX 0U
#define RE_VK_SEMAPHORE_RENDERING_FINISHED_INDEX 1U
#define RE_VK_SEMAPHORES_PER_FRAME 2U
	
	RenderSystem* RenderSystem::pInstance = nullptr;
	REvertex vertices[] = {1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
						-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
						0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};

	RenderSystem::RenderSystem() : bValid(false), vk_hSwapchain(VK_NULL_HANDLE), bUseOtherFrame(false), bVsyncEnabled(true) {
		if (RenderSystem::pInstance) {
			RE_ERROR("An instance of the render system has already been created. The new one won't be initialized and remains invalid");
			return;
		}
		RenderSystem::pInstance = this;
		DEFINE_SIGNAL_GUARD(sigGuardConstructorRenderSystem);
		if (!create_surface() || !alloc_physical_device_list()) {
			RE_ERROR("Failed initializing the render system");
			return;
		}
		int32_t i32DeviceScoreBest = std::numeric_limits<int32_t>::min();
		for (uint32_t i = 0U; i < u32PhysicalDevicesAvailableCount; i++) {
			VkPhysicalDeviceProperties vk_physicalDeviceProperties;
			CATCH_SIGNAL(vkGetPhysicalDeviceProperties(vk_phPhysicalDevicesAvailable[i], &vk_physicalDeviceProperties));
			int32_t i32CurrentDeviceScore = 0;
			switch (vk_physicalDeviceProperties.deviceType) {
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					i32DeviceScoreBest += 1000;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					i32DeviceScoreBest += 500;
					break;
				default:
					i32DeviceScoreBest -= 1000;
					break;
			}
			i32DeviceScoreBest += vk_physicalDeviceProperties.limits.maxImageDimension2D;
			if (i32DeviceScoreBest < i32CurrentDeviceScore) {
				i32DeviceScoreBest = i32CurrentDeviceScore;
				vk_hPhysicalDeviceSelected = vk_phPhysicalDevicesAvailable[i];
			}
		}
		CATCH_SIGNAL(fetch_surface_infos());
		PRINT_LN(u32SurfaceFormatsAvailableCount);
		for (uint32_t i = 0U; i < u32SurfaceFormatsAvailableCount; i++) {
			PRINT(vk_peSurfaceFormatsAvailable[i].format);
			println(" - ", vk_peSurfaceFormatsAvailable[i].colorSpace);
		}
		vk_surfaceFormatSelected = vk_peSurfaceFormatsAvailable[0];
		if (!create_device() || !create_swapchain() || !create_shaders() || !create_pipeline_layout() || !create_renderpass() || !create_graphics_pipeline() || !create_framebuffers() || !alloc_command_buffers() || !create_vertex_buffer() || !record_command_buffers() || !create_sync_objects()) {
			RE_ERROR("Failed initializing the render system");
			return;
		}
		bValid = true;
	}

	RenderSystem::~RenderSystem() {
		if (RenderSystem::pInstance != this)
			return;
		RenderSystem::pInstance = nullptr;
		CATCH_SIGNAL(destroy_sync_objects());
		CATCH_SIGNAL(destroy_vertex_buffer());
		CATCH_SIGNAL(free_command_buffers());
		CATCH_SIGNAL(destroy_framebuffers());
		CATCH_SIGNAL(destroy_graphics_pipeline());
		CATCH_SIGNAL(destroy_renderpass());
		CATCH_SIGNAL(destroy_pipeline_layout());
		CATCH_SIGNAL(destroy_shaders());
		CATCH_SIGNAL(destroy_swapchain());
		CATCH_SIGNAL(destroy_surface());
		CATCH_SIGNAL(destroy_device());
		CATCH_SIGNAL(free_physical_device_list());
	}

	bool RenderSystem::create_buffer(VkDeviceSize vk_size, VkBufferUsageFlags vk_usage, VkSharingMode vk_sharingMode, uint32_t u32QueueFamilyIndexCount, const uint32_t* pu32QueueFamilyIndices, VkMemoryPropertyFlags vk_memoryProperties, VkBuffer& vk_rBuffer, VkDeviceMemory& vk_rDeviceMemory) {
		VkBufferCreateInfo vk_bufferCreateInfo = {};
		vk_bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vk_bufferCreateInfo.size = vk_size;
		vk_bufferCreateInfo.usage = vk_usage;
		vk_bufferCreateInfo.sharingMode = vk_sharingMode;
		switch (vk_sharingMode) {
			case VK_SHARING_MODE_CONCURRENT:
				vk_bufferCreateInfo.queueFamilyIndexCount = u32QueueFamilyIndexCount;
				vk_bufferCreateInfo.pQueueFamilyIndices = pu32QueueFamilyIndices;
				break;
			case VK_SHARING_MODE_EXCLUSIVE:
			default:
				vk_bufferCreateInfo.queueFamilyIndexCount = 0U;
				vk_bufferCreateInfo.pQueueFamilyIndices = nullptr;
				break;
		}
		if (!CHECK_VK_RESULT(vkCreateBuffer(vk_hDevice, &vk_bufferCreateInfo, nullptr, &vk_rBuffer))) {
			RE_ERROR("Failed creating new buffer in Vulkan");
			return false;
		}
		VkMemoryRequirements vk_bufferMemoryRequirements;
		CATCH_SIGNAL(vkGetBufferMemoryRequirements(vk_hDevice, vk_rBuffer, &vk_bufferMemoryRequirements));
		VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties;
		CATCH_SIGNAL(vkGetPhysicalDeviceMemoryProperties(vk_hPhysicalDeviceSelected, &vk_physicalDeviceMemoryProperties));
		std::optional<uint32_t> memoryTypeSelected;
		for (uint32_t u32MemoryTypeIndex = 0U; u32MemoryTypeIndex < vk_physicalDeviceMemoryProperties.memoryTypeCount; u32MemoryTypeIndex++) {
			if ((vk_bufferMemoryRequirements.memoryTypeBits & (1U << u32MemoryTypeIndex)) && (vk_physicalDeviceMemoryProperties.memoryTypes[u32MemoryTypeIndex].propertyFlags & vk_memoryProperties)) {
				memoryTypeSelected = u32MemoryTypeIndex;
				break;
			}
		}
		if (!memoryTypeSelected.has_value()) {
			RE_ERROR("The physical Vulkan device doesn't support the required memory type for the recently created buffer");
			CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, vk_rBuffer, nullptr));
			return false;
		}
		VkMemoryAllocateInfo vk_bufferMemoryAllocateInfo = {};
		vk_bufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vk_bufferMemoryAllocateInfo.allocationSize = vk_bufferMemoryRequirements.size;
		vk_bufferMemoryAllocateInfo.memoryTypeIndex = memoryTypeSelected.value();
		if (!CHECK_VK_RESULT(vkAllocateMemory(vk_hDevice, &vk_bufferMemoryAllocateInfo, nullptr, &vk_rDeviceMemory))) {
			RE_ERROR("Failed allocating memory for the recently created buffer in Vulkan");
			CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, vk_rBuffer, nullptr));
			return false;
		}
		CATCH_SIGNAL(vkBindBufferMemory(vk_hDevice, vk_rBuffer, vk_rDeviceMemory, 0U));
		return true;
	}

	bool RenderSystem::create_surface() {
		DEFINE_SIGNAL_GUARD(sigGuardCreateSurface);
#ifdef RE_OS_WINDOWS
		VkWin32SurfaceCreateInfoKHR vk_win32SurfaceCreateInfo = {};
		vk_win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		vk_win32SurfaceCreateInfo.hwnd = static_cast<Window_Win64*>(Window::pInstance)->get_hwindow();
		vk_win32SurfaceCreateInfo.hinstance = Window_Win64::win_hInstance;
		return CHECK_VK_RESULT(vkCreateWin32SurfaceKHR(RE_VK_INSTANCE, &vk_win32SurfaceCreateInfo, nullptr, &vk_hSurface));
#elif defined RE_OS_LINUX
		switch (eUsingWindowingSystem) {
			case RE_WINDOWING_SYSTEM_WAYLAND:
				VkWaylandSurfaceCreateInfoKHR vk_waylandSurfaceCreateInfo = {};
				vk_waylandSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
				vk_waylandSurfaceCreateInfo.display = static_cast<Window_Wayland*>(Window::pInstance)->wl_pDisplay;
				vk_waylandSurfaceCreateInfo.surface = static_cast<Window_Wayland*>(Window::pInstance)->get_wl_surface();
				return CHECK_VK_RESULT(vkCreateWaylandSurfaceKHR(RE_VK_INSTANCE, &vk_waylandSurfaceCreateInfo, nullptr, &vk_hSurface));
			case RE_WINDOWING_SYSTEM_X11:
				VkXlibSurfaceCreateInfoKHR vk_x11SurfaceCreateInfo = {};
				vk_x11SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
				vk_x11SurfaceCreateInfo.dpy = static_cast<Window_X11*>(Window::pInstance)->x11_pDisplay;
				vk_x11SurfaceCreateInfo.window = static_cast<Window_X11*>(Window::pInstance)->get_xwindow();
				return CHECK_VK_RESULT(vkCreateXlibSurfaceKHR(RE_VK_INSTANCE, &vk_x11SurfaceCreateInfo, nullptr, &vk_hSurface));
			default:
				return false;
		}
#else
		return false;
#endif
	}
	
	void RenderSystem::destroy_surface() {
		CATCH_SIGNAL(vkDestroySurfaceKHR(RE_VK_INSTANCE, vk_hSurface, nullptr));
		DELETE_ARRAY_SAFELY(vk_peSurfaceFormatsAvailable);
	}

	void RenderSystem::fetch_surface_infos() {
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities));
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32SurfaceFormatsAvailableCount, nullptr));
		vk_peSurfaceFormatsAvailable = new VkSurfaceFormatKHR[u32SurfaceFormatsAvailableCount];
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32SurfaceFormatsAvailableCount, vk_peSurfaceFormatsAvailable));
		uint32_t u32PresentModesCount;
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32PresentModesCount, nullptr));
		VkPresentModeKHR *const vk_peAllPresentModes = new VkPresentModeKHR[u32PresentModesCount];
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32PresentModesCount, vk_peAllPresentModes));
		vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR;
		vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
		for (uint32_t u32PresentModeIndex = 0U; u32PresentModeIndex < u32PresentModesCount; u32PresentModeIndex++) {
			switch (vk_peAllPresentModes[u32PresentModeIndex]) {
				case VK_PRESENT_MODE_IMMEDIATE_KHR:
					vk_ePresentModeNoVsync = VK_PRESENT_MODE_IMMEDIATE_KHR;
					break;
				case VK_PRESENT_MODE_MAILBOX_KHR:
					vk_ePresentModeVsync = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				default:
					break;
			}
		}
		delete[] vk_peAllPresentModes;
	}

	bool RenderSystem::alloc_physical_device_list() {
		DEFINE_SIGNAL_GUARD(sigGuardAllocPhysicalDeviceList);
		uint32_t u32TotalPhysicalDeviceCount;
		CATCH_SIGNAL(vkEnumeratePhysicalDevices(RE_VK_INSTANCE, &u32TotalPhysicalDeviceCount, nullptr));
		if (!u32TotalPhysicalDeviceCount) {
			RE_FATAL_ERROR("There aren't any devices supporting Vulkan");
			return false;
		}
		VkPhysicalDevice *const vk_phTotalPhysicalDevice = new VkPhysicalDevice[u32TotalPhysicalDeviceCount];
		CATCH_SIGNAL(vkEnumeratePhysicalDevices(RE_VK_INSTANCE, &u32TotalPhysicalDeviceCount, vk_phTotalPhysicalDevice));
		std::vector<VkPhysicalDevice> suitablePhysicalDevices;
		for (uint32_t u32PhysicalDeviceIndex = 0U; u32PhysicalDeviceIndex < u32TotalPhysicalDeviceCount; u32PhysicalDeviceIndex++) {
			const VkPhysicalDevice vk_hPhysicalDevice = vk_phTotalPhysicalDevice[u32PhysicalDeviceIndex];

			// Check if there are surface formats defined
			uint32_t u32PhysicalDeviceSurfaceFormatCount;
			CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDevice, vk_hSurface, &u32PhysicalDeviceSurfaceFormatCount, nullptr));
			if (!u32PhysicalDeviceSurfaceFormatCount)
				continue;

			// Check if there are present modes defined
			uint32_t u32PhysicalDevicePresentModeCount;
			CATCH_SIGNAL(vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDevice, vk_hSurface, &u32PhysicalDevicePresentModeCount, nullptr));
			if (!u32PhysicalDevicePresentModeCount)
				continue;

			// Check if the required extensions exist
			uint32_t u32PhysicalDeviceExtensionCount;
			CATCH_SIGNAL(vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32PhysicalDeviceExtensionCount, nullptr));
			if (!u32PhysicalDeviceExtensionCount)
				continue;
			VkExtensionProperties *const vk_pPhysicalDeviceExtensionProperties = new VkExtensionProperties[u32PhysicalDeviceExtensionCount];
			CATCH_SIGNAL(vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32PhysicalDeviceExtensionCount, vk_pPhysicalDeviceExtensionProperties));
			bool bSwapchainExtists = false;
			for (uint32_t u32PhysicalDeviceExtensionIndex = 0U; u32PhysicalDeviceExtensionIndex < u32PhysicalDeviceExtensionCount; u32PhysicalDeviceExtensionIndex++) {
				if (!bSwapchainExtists && std::strcmp(vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
					bSwapchainExtists = true;
				if (bSwapchainExtists)
					break;
			}
			if (!bSwapchainExtists) {
				delete[] vk_pPhysicalDeviceExtensionProperties;
				continue;
			}

			// Check if required queues exist
			uint32_t u32PhysicalDeviceQueueFamilyCount;
			CATCH_SIGNAL(vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, nullptr));
			VkQueueFamilyProperties *const vk_pPhysicalDeviceQueueFamilyProperties = new VkQueueFamilyProperties[u32PhysicalDeviceQueueFamilyCount];
			CATCH_SIGNAL(vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, vk_pPhysicalDeviceQueueFamilyProperties));
			bool bGraphicsQueueExists = false, bPresentQueueExists = false, bTransferQueueExists = false;
			for (uint32_t u32PhysicalDeviceQueueFamilyIndex = 0U; u32PhysicalDeviceQueueFamilyIndex < u32PhysicalDeviceQueueFamilyCount; u32PhysicalDeviceQueueFamilyIndex++) {
				if (!bPresentQueueExists) {
					VkBool32 surfaceSupportExists;
					CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDevice, u32PhysicalDeviceQueueFamilyIndex, vk_hSurface, &surfaceSupportExists));
					if (surfaceSupportExists)
						bPresentQueueExists = true;
				}
				if (!bGraphicsQueueExists && (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT))
					bGraphicsQueueExists = true;
				if (!bTransferQueueExists && (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT))
					bTransferQueueExists = true;
				if (bGraphicsQueueExists && bPresentQueueExists && bTransferQueueExists)
					break;
			}
			if (bGraphicsQueueExists && bPresentQueueExists && bTransferQueueExists)
				suitablePhysicalDevices.push_back(vk_hPhysicalDevice);

			delete[] vk_pPhysicalDeviceExtensionProperties;
			delete[] vk_pPhysicalDeviceQueueFamilyProperties;
		}
		delete[] vk_phTotalPhysicalDevice;
		u32PhysicalDevicesAvailableCount = suitablePhysicalDevices.size();
		if (!u32PhysicalDevicesAvailableCount) {
			RE_FATAL_ERROR("There aren't any physical Vulkan devices supporting the necessary features");
			return false;
		}
		vk_phPhysicalDevicesAvailable = new VkPhysicalDevice[u32PhysicalDevicesAvailableCount];
		REuint u32CurrentIndex = 0U;
		for (VkPhysicalDevice vk_hPhysicalDevice : suitablePhysicalDevices) {
			vk_phPhysicalDevicesAvailable[u32CurrentIndex] = vk_hPhysicalDevice;
			u32CurrentIndex++;
		}
		return true;
	}

	void RenderSystem::free_physical_device_list() {
		DELETE_ARRAY_SAFELY(vk_phPhysicalDevicesAvailable);
	}

	bool RenderSystem::create_device() {
		DEFINE_SIGNAL_GUARD(sigGuardCreateDevice);
		constexpr uint32_t u32PhysicalDeviceExtensionCount = 1U;
		const char *const pcPhysicalDeviceExtensions[u32PhysicalDeviceExtensionCount] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		uint32_t u32PhysicalDeviceQueueFamilyCount;
		CATCH_SIGNAL(vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDeviceSelected, &u32PhysicalDeviceQueueFamilyCount, nullptr));
		VkQueueFamilyProperties *vk_pPhysicalDeviceQueueFamilyProperties = new VkQueueFamilyProperties[u32PhysicalDeviceQueueFamilyCount];
		CATCH_SIGNAL(vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDeviceSelected, &u32PhysicalDeviceQueueFamilyCount, vk_pPhysicalDeviceQueueFamilyProperties));
		std::optional<uint32_t> graphicsQueueIndex, presentQueueIndex, transferQueueIndex;
		std::vector<uint32_t> uniqueQueueIndices;
		for (uint32_t u32PhysicalDeviceQueueFamilyIndex = 0U; u32PhysicalDeviceQueueFamilyIndex < u32PhysicalDeviceQueueFamilyCount; u32PhysicalDeviceQueueFamilyIndex++) {
			bool bQueueUseful = false;
			// Get graphics-queue index
			if (!graphicsQueueIndex.has_value() && (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				graphicsQueueIndex = u32PhysicalDeviceQueueFamilyIndex;
				// Check if graphics queue supports presenting too for better performance
				VkBool32 surfaceSupportExists;
				CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDeviceSelected, u32PhysicalDeviceQueueFamilyIndex, vk_hSurface, &surfaceSupportExists));
				if (surfaceSupportExists)
					presentQueueIndex = u32PhysicalDeviceQueueFamilyIndex;
				bQueueUseful = true;
			}
			// Get presenting-queue index
			if (!presentQueueIndex.has_value()) {
				VkBool32 surfaceSupportExists;
				CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDeviceSelected, u32PhysicalDeviceQueueFamilyIndex, vk_hSurface, &surfaceSupportExists));
				if (surfaceSupportExists)
					presentQueueIndex = u32PhysicalDeviceQueueFamilyIndex;
				bQueueUseful = true;
			}
			// Get transfer-queue index, but avoid being a graphics queue too
			if (!transferQueueIndex.has_value() && (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT) && !((vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)) {
				transferQueueIndex = u32PhysicalDeviceQueueFamilyIndex;
				bQueueUseful = true;
			}
			// Check if current queue index is unique and add it
			if (bQueueUseful && std::find(uniqueQueueIndices.begin(), uniqueQueueIndices.end(), u32PhysicalDeviceQueueFamilyIndex) == uniqueQueueIndices.end())
				uniqueQueueIndices.push_back(u32PhysicalDeviceQueueFamilyIndex);
			if (graphicsQueueIndex.has_value() && presentQueueIndex.has_value() && transferQueueIndex.has_value())
				break;
		}
		// If no transfer-queue found, use graphics-queue instead, which is also a transfer-queue
		if (!transferQueueIndex.has_value())
			transferQueueIndex = graphicsQueueIndex.value();
		const float fQueuePriority = 1.0f;
		const size_t uniqueQueueIndexCount = uniqueQueueIndices.size();
		VkDeviceQueueCreateInfo *vk_pDeviceQueueCreateInfos = new VkDeviceQueueCreateInfo[uniqueQueueIndexCount];
		REuint u32UniqueQueueIndexIndex = 0U;
		for (uint32_t u32UniqueQueueIndex : uniqueQueueIndices) {
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].pNext = nullptr;
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].flags = 0;
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].queueFamilyIndex = u32UniqueQueueIndex;
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].queueCount = 1U;
			vk_pDeviceQueueCreateInfos[u32UniqueQueueIndexIndex].pQueuePriorities = &fQueuePriority;
			u32UniqueQueueIndexIndex++;
		}

		VkPhysicalDeviceFeatures vk_physicalDeviceFeaturesEnabled = {};

		VkDeviceCreateInfo vk_deviceCreateInfo = {};
		vk_deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		vk_deviceCreateInfo.pQueueCreateInfos = vk_pDeviceQueueCreateInfos;
		vk_deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(uniqueQueueIndexCount);
		vk_deviceCreateInfo.enabledExtensionCount = u32PhysicalDeviceExtensionCount;
		vk_deviceCreateInfo.ppEnabledExtensionNames = static_cast<const char *const *>(pcPhysicalDeviceExtensions);
		vk_deviceCreateInfo.pEnabledFeatures = &vk_physicalDeviceFeaturesEnabled;
		const bool bCreatedDeviceSuccessfully = CHECK_VK_RESULT(vkCreateDevice(vk_hPhysicalDeviceSelected, &vk_deviceCreateInfo, nullptr, &vk_hDevice));
		DELETE_ARRAY_SAFELY(vk_pDeviceQueueCreateInfos);
		if (!bCreatedDeviceSuccessfully) {
			RE_FATAL_ERROR("Failed creating logical Vulkan device");
			return false;
		}

		CATCH_SIGNAL(vkGetDeviceQueue(vk_hDevice, graphicsQueueIndex.value(), 0, &vk_hQueues[RE_VK_QUEUE_GRAPHICS_INDEX]));
		u32QueueIndices[RE_VK_QUEUE_GRAPHICS_INDEX] = graphicsQueueIndex.value();
		CATCH_SIGNAL(vkGetDeviceQueue(vk_hDevice, presentQueueIndex.value(), 0, &vk_hQueues[RE_VK_QUEUE_PRESENT_INDEX]));
		u32QueueIndices[RE_VK_QUEUE_PRESENT_INDEX] = presentQueueIndex.value();
		CATCH_SIGNAL(vkGetDeviceQueue(vk_hDevice, transferQueueIndex.value(), 0, &vk_hQueues[RE_VK_QUEUE_TRANSFER_INDEX]));
		u32QueueIndices[RE_VK_QUEUE_TRANSFER_INDEX] = transferQueueIndex.value();
		return true;
	}
	
	void RenderSystem::destroy_device() {
		CATCH_SIGNAL(vkDestroyDevice(vk_hDevice, nullptr));
	}

	bool RenderSystem::create_swapchain() {
		{ // Create actual sweapchain
			vk_eSwapchainImageFormat = vk_surfaceFormatSelected.format;
			const uint32_t u32OldSwapchainImageViewCount = u32SwapchainImageCount;
			const VkSwapchainKHR vk_hOldSwapchain = vk_hSwapchain;
			VkSwapchainCreateInfoKHR vk_swapchainCreateInfo = {};
			vk_swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			vk_swapchainCreateInfo.surface = vk_hSurface;
			vk_swapchainCreateInfo.minImageCount = std::clamp(vk_surfaceCapabilities.minImageCount + 1U, vk_surfaceCapabilities.minImageCount, vk_surfaceCapabilities.maxImageCount > 0U ? vk_surfaceCapabilities.maxImageCount : std::numeric_limits<uint32_t>::max());
			vk_swapchainCreateInfo.imageFormat = vk_eSwapchainImageFormat;
			vk_swapchainCreateInfo.imageColorSpace = vk_surfaceFormatSelected.colorSpace;
			if (vk_surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
				vk_swapchainResolution = vk_surfaceCapabilities.currentExtent;
			else {
				vk_swapchainResolution.width = std::clamp<REushort>(Window::pInstance->get_size()[0], vk_surfaceCapabilities.minImageExtent.width, vk_surfaceCapabilities.maxImageExtent.width);
				vk_swapchainResolution.height = std::clamp<REushort>(Window::pInstance->get_size()[1], vk_surfaceCapabilities.minImageExtent.height, vk_surfaceCapabilities.maxImageExtent.height);
			}
			vk_swapchainCreateInfo.imageExtent = vk_swapchainResolution;
			vk_swapchainCreateInfo.imageArrayLayers = 1U;
			vk_swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			const uint32_t u32SwapchainRelevantQueueIndices[2] = {u32QueueIndices[RE_VK_QUEUE_GRAPHICS_INDEX], u32QueueIndices[RE_VK_QUEUE_PRESENT_INDEX]};
			if (u32QueueIndices[RE_VK_QUEUE_GRAPHICS_INDEX] != u32QueueIndices[RE_VK_QUEUE_PRESENT_INDEX]) {
				vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				vk_swapchainCreateInfo.queueFamilyIndexCount = 2U;
				vk_swapchainCreateInfo.pQueueFamilyIndices = u32SwapchainRelevantQueueIndices;
			} else
				vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vk_swapchainCreateInfo.preTransform = vk_surfaceCapabilities.currentTransform;
			vk_swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			vk_swapchainCreateInfo.presentMode = bVsyncEnabled ? vk_ePresentModeVsync : vk_ePresentModeNoVsync;
			vk_swapchainCreateInfo.clipped = VK_TRUE;
			vk_swapchainCreateInfo.oldSwapchain = vk_hOldSwapchain;
			if (!CHECK_VK_RESULT(vkCreateSwapchainKHR(vk_hDevice, &vk_swapchainCreateInfo, nullptr, &vk_hSwapchain))) {
				RE_FATAL_ERROR("Failed creating Vulkan swapchain");
				return false;
			}
			if (vk_hOldSwapchain != VK_NULL_HANDLE) {
				for (uint32_t u32OldSwapchainImageViewDeleteIndex = 0U; u32OldSwapchainImageViewDeleteIndex < u32OldSwapchainImageViewCount; u32OldSwapchainImageViewDeleteIndex++)
					CATCH_SIGNAL(vkDestroyImageView(vk_hDevice, vk_pSwapchainImageViews[u32OldSwapchainImageViewDeleteIndex], nullptr));
				delete[] vk_pSwapchainImages;
				delete[] vk_pSwapchainImageViews;
				CATCH_SIGNAL(vkDestroySwapchainKHR(vk_hDevice, vk_hOldSwapchain, nullptr));
			}
		} // End of creating actual swapchain

		CATCH_SIGNAL(vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, nullptr));
		vk_pSwapchainImages = new VkImage[u32SwapchainImageCount];
		CATCH_SIGNAL(vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, vk_pSwapchainImages));
		vk_pSwapchainImageViews = new VkImageView[u32SwapchainImageCount];
		uint32_t u32SwapchainImageViewsCreated = 0U;
		while (u32SwapchainImageViewsCreated < u32SwapchainImageCount) {
			VkImageViewCreateInfo vk_swapchainImageViewCreateInfo = {};
			vk_swapchainImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			vk_swapchainImageViewCreateInfo.image = vk_pSwapchainImages[u32SwapchainImageViewsCreated];
			vk_swapchainImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			vk_swapchainImageViewCreateInfo.format = vk_eSwapchainImageFormat;
			vk_swapchainImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			vk_swapchainImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			vk_swapchainImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			vk_swapchainImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			vk_swapchainImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			vk_swapchainImageViewCreateInfo.subresourceRange.baseMipLevel = 0U;
			vk_swapchainImageViewCreateInfo.subresourceRange.levelCount = 1U;
			vk_swapchainImageViewCreateInfo.subresourceRange.baseArrayLayer = 0U;
			vk_swapchainImageViewCreateInfo.subresourceRange.layerCount = 1U;
			if (!CHECK_VK_RESULT(vkCreateImageView(vk_hDevice, &vk_swapchainImageViewCreateInfo, nullptr, &vk_pSwapchainImageViews[u32SwapchainImageViewsCreated]))) {
				RE_FATAL_ERROR(append_to_string("Failed to create Vulkan image view at index ", u32SwapchainImageViewsCreated));
				break;
			}
			u32SwapchainImageViewsCreated++;
		}
		if (u32SwapchainImageViewsCreated != u32SwapchainImageCount) {
			for (uint32_t u32SwapchainImageDeleteIndex = 0U; u32SwapchainImageDeleteIndex < u32SwapchainImageViewsCreated; u32SwapchainImageDeleteIndex++)
				CATCH_SIGNAL(vkDestroyImageView(vk_hDevice, vk_pSwapchainImageViews[u32SwapchainImageDeleteIndex], nullptr));
			DELETE_ARRAY_SAFELY(vk_pSwapchainImages);
			DELETE_ARRAY_SAFELY(vk_pSwapchainImageViews);
			CATCH_SIGNAL(vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr));
			vk_hSwapchain = VK_NULL_HANDLE;
			return false;
		}
		return true;
	}
	
	void RenderSystem::destroy_swapchain() {
		for (uint32_t u32SwapchainImageIndex = 0U; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++)
			CATCH_SIGNAL(vkDestroyImageView(vk_hDevice, vk_pSwapchainImageViews[u32SwapchainImageIndex], nullptr));
		DELETE_ARRAY_SAFELY(vk_pSwapchainImages);
		DELETE_ARRAY_SAFELY(vk_pSwapchainImageViews);
		CATCH_SIGNAL(vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr));
		vk_hSwapchain = VK_NULL_HANDLE;
	}

	void RenderSystem::recreate_swapchain() {
		CATCH_SIGNAL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities));
		CATCH_SIGNAL(wait_for_idle_device());
		CATCH_SIGNAL(destroy_framebuffers());
		if (!CATCH_SIGNAL_AND_RETURN(create_swapchain(), bool) || !CATCH_SIGNAL_AND_RETURN(create_framebuffers(), bool) || !CATCH_SIGNAL_AND_RETURN(record_command_buffers(), bool))
			RE_ERROR("Failed recreating the swapchain");
	}

	bool RenderSystem::create_shaders() {
		std::ifstream vertexBinaryFile("shaders/vertex.spv", std::ios::ate | std::ios::binary);
		if (!vertexBinaryFile.is_open()) {
			RE_FATAL_ERROR("Failed loading the vertex shader binary file");
			return false;
		}
		const size_t vertexBinaryFileSize = vertexBinaryFile.tellg();
		char *const pcVertexShaderBinaries = new char[vertexBinaryFileSize];
		vertexBinaryFile.seekg(0);
		CATCH_SIGNAL(vertexBinaryFile.read(pcVertexShaderBinaries, vertexBinaryFileSize));
		vertexBinaryFile.close();
		VkShaderModuleCreateInfo vk_vertexShaderCreateInfo = {};
		vk_vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vk_vertexShaderCreateInfo.codeSize = static_cast<uint32_t>(vertexBinaryFileSize);
		vk_vertexShaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(pcVertexShaderBinaries);
		const bool bVertexShaderCreatedSuccessfully = CHECK_VK_RESULT(vkCreateShaderModule(vk_hDevice, &vk_vertexShaderCreateInfo, nullptr, &vk_hVertexShader));
		delete[] pcVertexShaderBinaries;
		if (!bVertexShaderCreatedSuccessfully) {
			RE_FATAL_ERROR("Failed creating Vulkan shader module for the vertex shader");
			return false;
		}

		std::ifstream fragmentBinaryFile("shaders/fragment.spv", std::ios::ate | std::ios::binary);
		if (!fragmentBinaryFile.is_open()) {
			RE_FATAL_ERROR("Failed loading the fragment shader binary file");
			CATCH_SIGNAL(vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr));
			return false;
		}
		const size_t fragmentBinaryFileSize = fragmentBinaryFile.tellg();
		char *const pcFragmentShaderBinaries = new char[fragmentBinaryFileSize];
		fragmentBinaryFile.seekg(0);
		CATCH_SIGNAL(fragmentBinaryFile.read(pcFragmentShaderBinaries, fragmentBinaryFileSize));
		fragmentBinaryFile.close();
		VkShaderModuleCreateInfo vk_fragmentShaderCreateInfo = {};
		vk_fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vk_fragmentShaderCreateInfo.codeSize = static_cast<uint32_t>(fragmentBinaryFileSize);
		vk_fragmentShaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(pcFragmentShaderBinaries);
		const bool bFragmentShaderCreatedSuccessfully = CHECK_VK_RESULT(vkCreateShaderModule(vk_hDevice, &vk_fragmentShaderCreateInfo, nullptr, &vk_hFragmentShader));
		delete[] pcFragmentShaderBinaries;
		if (!bFragmentShaderCreatedSuccessfully) {
			RE_FATAL_ERROR("Failed creating Vulkan shader module for the fragment shader");
			CATCH_SIGNAL(vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr));
			return false;
		}
		return true;
	}
	
	void RenderSystem::destroy_shaders() {
		CATCH_SIGNAL(vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr));
		CATCH_SIGNAL(vkDestroyShaderModule(vk_hDevice, vk_hFragmentShader, nullptr));
	}

	bool RenderSystem::create_pipeline_layout() {
		VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo = {};
		vk_pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		vk_pipelineLayoutCreateInfo.setLayoutCount = 0U;
		vk_pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		vk_pipelineLayoutCreateInfo.pushConstantRangeCount = 0U;
		vk_pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
		return CHECK_VK_RESULT(vkCreatePipelineLayout(vk_hDevice, &vk_pipelineLayoutCreateInfo, nullptr, &vk_hPipelineLayout));
	}
	
	void RenderSystem::destroy_pipeline_layout() {
		CATCH_SIGNAL(vkDestroyPipelineLayout(vk_hDevice, vk_hPipelineLayout, nullptr));
	}

	bool RenderSystem::create_renderpass() {
		VkAttachmentDescription vk_colorAttachment = {};
		vk_colorAttachment.format = vk_eSwapchainImageFormat;
		vk_colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		vk_colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		vk_colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		vk_colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		vk_colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vk_colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		vk_colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference vk_attachmentReference = {};
		vk_attachmentReference.attachment = 0U;
		vk_attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription vk_subpassDescription = {};
		vk_subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vk_subpassDescription.colorAttachmentCount = 1U;
		vk_subpassDescription.pColorAttachments = &vk_attachmentReference;

		VkSubpassDependency vk_subpassDependency = {};
		vk_subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		vk_subpassDependency.dstSubpass = 0;
		vk_subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vk_subpassDependency.srcAccessMask = 0U;
		vk_subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vk_subpassDependency.dstAccessMask = 0U;

		VkRenderPassCreateInfo vk_renderPassCreateInfo = {};
		vk_renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		vk_renderPassCreateInfo.attachmentCount = 1U;
		vk_renderPassCreateInfo.pAttachments = &vk_colorAttachment;
		vk_renderPassCreateInfo.subpassCount = 1U;
		vk_renderPassCreateInfo.pSubpasses = &vk_subpassDescription;
		vk_renderPassCreateInfo.dependencyCount = 1U;
		vk_renderPassCreateInfo.pDependencies = &vk_subpassDependency;
		const bool bRenderpassCreatedSuccessfully = CHECK_VK_RESULT(vkCreateRenderPass(vk_hDevice, &vk_renderPassCreateInfo, nullptr, &vk_hRenderPass));
		if (!bRenderpassCreatedSuccessfully)
			RE_FATAL_ERROR("Failed creating Vulkan render pass");
		return bRenderpassCreatedSuccessfully;
	}
	
	void RenderSystem::destroy_renderpass() {
		CATCH_SIGNAL(vkDestroyRenderPass(vk_hDevice, vk_hRenderPass, nullptr));
	}

	bool RenderSystem::create_graphics_pipeline() {
		VkPipelineShaderStageCreateInfo vk_pipelineVertexShaderStateCreateInfo = {};
		vk_pipelineVertexShaderStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vk_pipelineVertexShaderStateCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vk_pipelineVertexShaderStateCreateInfo.module = vk_hVertexShader;
		vk_pipelineVertexShaderStateCreateInfo.pName = "main";
		VkPipelineShaderStageCreateInfo vk_pipelineFragmentShaderStateCreateInfo = {};
		vk_pipelineFragmentShaderStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vk_pipelineFragmentShaderStateCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		vk_pipelineFragmentShaderStateCreateInfo.module = vk_hFragmentShader;
		vk_pipelineFragmentShaderStateCreateInfo.pName = "main";
		VkPipelineShaderStageCreateInfo vk_pipelineShaderStateCreateInfos[2] = {vk_pipelineVertexShaderStateCreateInfo, vk_pipelineFragmentShaderStateCreateInfo};

		VkDynamicState vk_dynamicStates[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo vk_pipelineDynamicStateCreateInfo = {};
		vk_pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		vk_pipelineDynamicStateCreateInfo.dynamicStateCount = sizeof(vk_dynamicStates) / sizeof(VkDynamicState);
		vk_pipelineDynamicStateCreateInfo.pDynamicStates = vk_dynamicStates;

		VkVertexInputBindingDescription vk_vertexInputBindingDescription = {};
		vk_vertexInputBindingDescription.binding = 0;
		vk_vertexInputBindingDescription.stride = RE_VK_VERTEX_TOTAL_SIZE_BYTES;
		vk_vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		VkVertexInputAttributeDescription vk_vertexInputAttributeDescriptions[2];
		vk_vertexInputAttributeDescriptions[0].location = 0U;
		vk_vertexInputAttributeDescriptions[0].binding = 0U;
		vk_vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vk_vertexInputAttributeDescriptions[0].offset = RE_VK_VERTEX_POSITION_OFFSET_BYTES;
		vk_vertexInputAttributeDescriptions[1].location = 1U;
		vk_vertexInputAttributeDescriptions[1].binding = 0U;
		vk_vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vk_vertexInputAttributeDescriptions[1].offset = RE_VK_VERTEX_COLOR_OFFSET_BYTES;
		VkPipelineVertexInputStateCreateInfo vk_pipelineVertexInputStateCreateInfo = {};
		vk_pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vk_pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1U;
		vk_pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vk_vertexInputBindingDescription;
		vk_pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2U;
		vk_pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = (VkVertexInputAttributeDescription*) &vk_vertexInputAttributeDescriptions;

		VkPipelineInputAssemblyStateCreateInfo vk_pipelineInputAssemblyStateCreateInfo = {};
		vk_pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		vk_pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		vk_pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport vk_viewport = {};
		vk_viewport.x = 0.0f;
		vk_viewport.y = 0.0f;
		vk_viewport.width = vk_swapchainResolution.width;
		vk_viewport.height = vk_swapchainResolution.height;
		vk_viewport.minDepth = 0.0f;
		vk_viewport.maxDepth = 1.0f;

		VkRect2D vk_scissor = {};
		vk_scissor.offset.x = 0.0f;
		vk_scissor.offset.y = 0.0f;
		vk_scissor.extent = vk_swapchainResolution;

		VkPipelineViewportStateCreateInfo vk_pipelineViewportStateCreateInfo = {};
		vk_pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vk_pipelineViewportStateCreateInfo.viewportCount = 1U;
		vk_pipelineViewportStateCreateInfo.pViewports = &vk_viewport;
		vk_pipelineViewportStateCreateInfo.scissorCount = 1U;
		vk_pipelineViewportStateCreateInfo.pScissors = &vk_scissor;

		VkPipelineRasterizationStateCreateInfo vk_pipelineRasterizationStateCreateInfo = {};
		vk_pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		vk_pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		vk_pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		vk_pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		vk_pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
		vk_pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		vk_pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		vk_pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		vk_pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		vk_pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		vk_pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo vk_pipelineMultisampleStateCreateInfo = {};
		vk_pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		vk_pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		vk_pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		vk_pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f;
		vk_pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
		vk_pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		vk_pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState vk_pipelineColorBlendAttachmentState = {};
		vk_pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		vk_pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
		vk_pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		vk_pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		vk_pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		vk_pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		vk_pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		vk_pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo vk_pipelineColorBlendStateCreateInfo = {};
		vk_pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		vk_pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		vk_pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		vk_pipelineColorBlendStateCreateInfo.attachmentCount = 1U;
		vk_pipelineColorBlendStateCreateInfo.pAttachments = &vk_pipelineColorBlendAttachmentState;
		vk_pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		vk_pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		vk_pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		vk_pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		VkGraphicsPipelineCreateInfo vk_graphicsPipelineCreateInfo = {};
		vk_graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		vk_graphicsPipelineCreateInfo.stageCount = sizeof(vk_pipelineShaderStateCreateInfos) / sizeof(VkPipelineShaderStageCreateInfo);
		vk_graphicsPipelineCreateInfo.pStages = vk_pipelineShaderStateCreateInfos;
		vk_graphicsPipelineCreateInfo.pVertexInputState = &vk_pipelineVertexInputStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pInputAssemblyState = &vk_pipelineInputAssemblyStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pViewportState = &vk_pipelineViewportStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pRasterizationState = &vk_pipelineRasterizationStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pMultisampleState = &vk_pipelineMultisampleStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
		vk_graphicsPipelineCreateInfo.pColorBlendState = &vk_pipelineColorBlendStateCreateInfo;
		vk_graphicsPipelineCreateInfo.pDynamicState = &vk_pipelineDynamicStateCreateInfo;
		vk_graphicsPipelineCreateInfo.layout = vk_hPipelineLayout;
		vk_graphicsPipelineCreateInfo.renderPass = vk_hRenderPass;
		vk_graphicsPipelineCreateInfo.subpass = 0U;
		vk_graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		vk_graphicsPipelineCreateInfo.basePipelineIndex = -1;
		const bool bGraphicsPipelineCreatedSuccessfully = CHECK_VK_RESULT(vkCreateGraphicsPipelines(vk_hDevice, VK_NULL_HANDLE, 1, &vk_graphicsPipelineCreateInfo, nullptr, &vk_hGraphicsPipeline));
		if (!bGraphicsPipelineCreatedSuccessfully)
			RE_FATAL_ERROR("Failed creating Vulkan graphics pipeline");
		return bGraphicsPipelineCreatedSuccessfully;
	}
	
	void RenderSystem::destroy_graphics_pipeline() {
		CATCH_SIGNAL(vkDestroyPipeline(vk_hDevice, vk_hGraphicsPipeline, nullptr));
	}

	bool RenderSystem::create_framebuffers() {
		vk_phSwapchainFramebuffers = new VkFramebuffer[u32SwapchainImageCount];
		uint32_t u32FramebuffersCreated = 0U;
		while (u32FramebuffersCreated < u32SwapchainImageCount) {
			VkFramebufferCreateInfo vk_framebufferCreateInfo = {};
			vk_framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			vk_framebufferCreateInfo.renderPass = vk_hRenderPass;
			vk_framebufferCreateInfo.attachmentCount = 1U;
			vk_framebufferCreateInfo.pAttachments = &vk_pSwapchainImageViews[u32FramebuffersCreated];
			vk_framebufferCreateInfo.width = vk_swapchainResolution.width;
			vk_framebufferCreateInfo.height = vk_swapchainResolution.height;
			vk_framebufferCreateInfo.layers = 1U;
			if (!CHECK_VK_RESULT(vkCreateFramebuffer(vk_hDevice, &vk_framebufferCreateInfo, nullptr, &vk_phSwapchainFramebuffers[u32FramebuffersCreated]))) {
				RE_FATAL_ERROR(append_to_string("Failed creating Vulkan framebuffer at index ", u32FramebuffersCreated));
				break;
			}
			u32FramebuffersCreated++;
		}
		if (u32FramebuffersCreated != u32SwapchainImageCount) {
			for (uint32_t u32FramebufferDeleteIndex = u32FramebuffersCreated + 1U; u32FramebufferDeleteIndex > 0U; u32FramebufferDeleteIndex--)
				CATCH_SIGNAL(vkDestroyFramebuffer(vk_hDevice, vk_phSwapchainFramebuffers[u32FramebufferDeleteIndex - 1U], nullptr));
			DELETE_ARRAY_SAFELY(vk_phSwapchainFramebuffers);
			return false;
		}
		return true;
	}
	
	void RenderSystem::destroy_framebuffers() {
		for (uint32_t u32FramebufferIndex = 0U; u32FramebufferIndex < u32SwapchainImageCount; u32FramebufferIndex++)
			CATCH_SIGNAL_DETAILED(vkDestroyFramebuffer(vk_hDevice, vk_phSwapchainFramebuffers[u32FramebufferIndex], nullptr), append_to_string("Framebuffer index: ", u32FramebufferIndex).c_str());
		DELETE_ARRAY_SAFELY(vk_phSwapchainFramebuffers);
	}

	bool RenderSystem::create_vertex_buffer() {
		if (!CATCH_SIGNAL_AND_RETURN(create_buffer(sizeof(REvertex) * RE_VK_VERTEX_COUNT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, 0U, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,vk_hVertexStagingBuffer, vk_hVertexStagingBufferMemory), bool)) {
			RE_FATAL_ERROR("Failed creating vertex staging buffer in Vulkan");
			return false;
		}
		return true;
	}
	
	void RenderSystem::destroy_vertex_buffer() {
		CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, vk_hVertexStagingBuffer, nullptr));
		CATCH_SIGNAL(vkFreeMemory(vk_hDevice, vk_hVertexStagingBufferMemory, nullptr));
	}

	bool RenderSystem::alloc_command_buffers() {
		VkCommandPoolCreateInfo vk_commandPoolCreateInfos[RE_VK_COMMAND_POOL_COUNT];
		uint32_t u32CommandPoolCreateIndex = 0U;
		while (u32CommandPoolCreateIndex < RE_VK_COMMAND_POOL_COUNT) {
			vk_commandPoolCreateInfos[u32CommandPoolCreateIndex].sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vk_commandPoolCreateInfos[u32CommandPoolCreateIndex].pNext = nullptr;
			switch (u32CommandPoolCreateIndex) {
				case RE_VK_COMMAND_POOL_GRAPHICS_INDEX:
					vk_commandPoolCreateInfos[u32CommandPoolCreateIndex].flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
					vk_commandPoolCreateInfos[u32CommandPoolCreateIndex].queueFamilyIndex = u32QueueIndices[RE_VK_QUEUE_GRAPHICS_INDEX];
					break;
				case RE_VK_COMMAND_POOL_TRANSFER_INDEX:
					//vk_commandPoolCreateInfos[u32CommandPoolCreateIndex].flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
					vk_commandPoolCreateInfos[u32CommandPoolCreateIndex].flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
					vk_commandPoolCreateInfos[u32CommandPoolCreateIndex].queueFamilyIndex = u32QueueIndices[RE_VK_QUEUE_TRANSFER_INDEX];
					break;
			}
			if (!CHECK_VK_RESULT(vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfos[u32CommandPoolCreateIndex], nullptr, &vk_hCommandPools[u32CommandPoolCreateIndex]))) {
				RE_FATAL_ERROR(append_to_string("Failed creating Vulkan command pool at index ", u32CommandPoolCreateIndex));
				break;
			}
			u32CommandPoolCreateIndex++;
		}
		if (u32CommandPoolCreateIndex != RE_VK_COMMAND_POOL_COUNT) {
			for (uint32_t u32CommandPoolDeleteIndex = u32CommandPoolCreateIndex + 1U; u32CommandPoolDeleteIndex > 0U; u32CommandPoolDeleteIndex--)
				CATCH_SIGNAL(vkDestroyCommandPool(vk_hDevice, vk_hCommandPools[u32CommandPoolDeleteIndex - 1U], nullptr));
			return false;
		}
		
		VkCommandBufferAllocateInfo vk_commandBufferGraphicsAllocInfo = {};
		vk_commandBufferGraphicsAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vk_commandBufferGraphicsAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		vk_commandBufferGraphicsAllocInfo.commandPool = vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX];
		vk_commandBufferGraphicsAllocInfo.commandBufferCount = u32SwapchainImageCount;
		vk_phCommandBuffersGraphics = new VkCommandBuffer[vk_commandBufferGraphicsAllocInfo.commandBufferCount];
		if (!CHECK_VK_RESULT(vkAllocateCommandBuffers(vk_hDevice, &vk_commandBufferGraphicsAllocInfo, vk_phCommandBuffersGraphics))) {
			RE_FATAL_ERROR("Failed allocating Vulkan graphics command buffer(s) for the graphics queue");
			DELETE_ARRAY_SAFELY(vk_phCommandBuffersGraphics);
			for (uint32_t u32CommandPoolDeleteIndex = 0U; u32CommandPoolDeleteIndex < RE_VK_COMMAND_POOL_COUNT; u32CommandPoolDeleteIndex++)
				CATCH_SIGNAL(vkDestroyCommandPool(vk_hDevice, vk_hCommandPools[u32CommandPoolDeleteIndex], nullptr));
			return false;
		}

		VkCommandBufferAllocateInfo vk_commandBufferTransferAllocInfo = {};
		vk_commandBufferTransferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vk_commandBufferTransferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		vk_commandBufferTransferAllocInfo.commandPool = vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_INDEX];
		vk_commandBufferTransferAllocInfo.commandBufferCount = 1U;
		vk_phCommandBuffersTransfer = new VkCommandBuffer[vk_commandBufferTransferAllocInfo.commandBufferCount];
		if (!CHECK_VK_RESULT(vkAllocateCommandBuffers(vk_hDevice, &vk_commandBufferTransferAllocInfo, vk_phCommandBuffersTransfer))) {
			if (u32QueueIndices[RE_VK_QUEUE_GRAPHICS_INDEX] != u32QueueIndices[RE_VK_QUEUE_TRANSFER_INDEX])
				RE_FATAL_ERROR("Failed allocating Vulkan graphics command buffer(s) for the transfer queue");
			else
				RE_FATAL_ERROR("Failed allocating Vulkan transfer command buffer(s) for the graphics queue");
			DELETE_ARRAY_SAFELY(vk_phCommandBuffersGraphics);
			DELETE_ARRAY_SAFELY(vk_phCommandBuffersTransfer);
			for (uint32_t u32CommandPoolDeleteIndex = 0U; u32CommandPoolDeleteIndex < RE_VK_COMMAND_POOL_COUNT; u32CommandPoolDeleteIndex++)
				CATCH_SIGNAL(vkDestroyCommandPool(vk_hDevice, vk_hCommandPools[u32CommandPoolDeleteIndex], nullptr));
			return false;
		}
		return true;
	}
	
	void RenderSystem::free_command_buffers() {
		DELETE_ARRAY_SAFELY(vk_phCommandBuffersGraphics);
		DELETE_ARRAY_SAFELY(vk_phCommandBuffersTransfer);
		CATCH_SIGNAL(vkDestroyCommandPool(vk_hDevice, vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX], nullptr));
		CATCH_SIGNAL(vkDestroyCommandPool(vk_hDevice, vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_INDEX], nullptr));
	}

	bool RenderSystem::record_command_buffers() {
		for (uint32_t u32CommandBufferIndex = 0U; u32CommandBufferIndex < u32SwapchainImageCount; u32CommandBufferIndex++) {
			const VkCommandBuffer vk_hCommandBuffer = vk_phCommandBuffersGraphics[u32CommandBufferIndex];
			CATCH_SIGNAL(vkResetCommandBuffer(vk_hCommandBuffer, 0));
			VkCommandBufferBeginInfo vk_commandBufferBeginInfo = {};
			vk_commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vk_commandBufferBeginInfo.flags = 0U;
			vk_commandBufferBeginInfo.pInheritanceInfo = nullptr;
			if (!CHECK_VK_RESULT(vkBeginCommandBuffer(vk_hCommandBuffer, &vk_commandBufferBeginInfo))) {
				RE_FATAL_ERROR(append_to_string("Failed to begin recording commands for the Vulkan graphics command buffer at index ", u32CommandBufferIndex));
				return false;
			}
			VkClearValue vk_clearValues;
			vk_clearValues.color.float32[0] = 0.0f;
			vk_clearValues.color.float32[1] = 0.0f;
			vk_clearValues.color.float32[2] = 0.0f;
			vk_clearValues.color.float32[3] = 1.0f;
			vk_clearValues.color.int32[0] = 0;
			vk_clearValues.color.int32[1] = 0;
			vk_clearValues.color.int32[2] = 0;
			vk_clearValues.color.int32[3] = 0xFF;
			vk_clearValues.color.uint32[0] = 0U;
			vk_clearValues.color.uint32[1] = 0U;
			vk_clearValues.color.uint32[2] = 0U;
			vk_clearValues.color.uint32[3] = 0xFFU;
			vk_clearValues.depthStencil.depth = 0.0f;
			vk_clearValues.depthStencil.stencil = 0U;
			VkRenderPassBeginInfo vk_commandBufferRenderpassBeginInfo = {};
			vk_commandBufferRenderpassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vk_commandBufferRenderpassBeginInfo.renderPass = vk_hRenderPass;
			CATCH_SIGNAL(vk_commandBufferRenderpassBeginInfo.framebuffer = vk_phSwapchainFramebuffers[u32CommandBufferIndex]);
			vk_commandBufferRenderpassBeginInfo.renderArea.offset.x = 0;
			vk_commandBufferRenderpassBeginInfo.renderArea.offset.y = 0;
			vk_commandBufferRenderpassBeginInfo.renderArea.extent = vk_swapchainResolution;
			vk_commandBufferRenderpassBeginInfo.clearValueCount = 1U;
			vk_commandBufferRenderpassBeginInfo.pClearValues = &vk_clearValues;
			CATCH_SIGNAL(vkCmdBeginRenderPass(vk_hCommandBuffer, &vk_commandBufferRenderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE));
			CATCH_SIGNAL(vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipeline));
			VkBuffer vk_vertexBuffers[1] = {vk_hVertexStagingBuffer};
			VkDeviceSize offsets[1] = {0U};
			CATCH_SIGNAL(vkCmdBindVertexBuffers(vk_hCommandBuffer, 0U, 1U, vk_vertexBuffers, offsets));
			VkViewport vk_viewport = {};
			vk_viewport.x = 0.0f;
			vk_viewport.y = 0.0f;
			vk_viewport.width = vk_swapchainResolution.width;
			vk_viewport.height = vk_swapchainResolution.height;
			vk_viewport.minDepth = 0.0f;
			vk_viewport.maxDepth = 1.0f;
			CATCH_SIGNAL(vkCmdSetViewport(vk_hCommandBuffer, 0U, 1U, &vk_viewport));
			VkRect2D vk_scissor = {};
			vk_scissor.offset.x = 0;
			vk_scissor.offset.y = 0;
			vk_scissor.extent = vk_swapchainResolution;
			CATCH_SIGNAL(vkCmdSetScissor(vk_hCommandBuffer, 0U, 1U, &vk_scissor));
			CATCH_SIGNAL(vkCmdDraw(vk_hCommandBuffer, sizeof(vertices) / (sizeof(vertices[0]) * RE_VK_VERTEX_TOTAL_SIZE), 1U, 0U, 0U));
			CATCH_SIGNAL(vkCmdEndRenderPass(vk_hCommandBuffer));
			if (!CHECK_VK_RESULT(vkEndCommandBuffer(vk_hCommandBuffer))) {
				RE_FATAL_ERROR(append_to_string("Failed to finish recording the Vulkan graphics command buffer at index ", u32CommandBufferIndex));
				return false;
			}
		}
		return true;
	}

	bool RenderSystem::create_sync_objects() {
		VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {};
		vk_semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		uint32_t u32SemaphoresCreatedSuccessfully = 0U;
		for (uint32_t u32SemaphoreCreateIndex = 0U; u32SemaphoreCreateIndex < RE_VK_SEMAPHORE_COUNT; u32SemaphoreCreateIndex++) {
			if (!CHECK_VK_RESULT(vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hSemaphores[u32SemaphoreCreateIndex]))) {
				RE_FATAL_ERROR(append_to_string("Failed creating Vulkan semaphore at index ", u32SemaphoreCreateIndex));
				break;
			}
			u32SemaphoresCreatedSuccessfully++;
		}
		if (u32SemaphoresCreatedSuccessfully != RE_VK_SEMAPHORE_COUNT) {
			for (uint32_t u32SemaphoreDeleteIndex = u32SemaphoresCreatedSuccessfully + 1U; u32SemaphoreDeleteIndex > 0U; u32SemaphoreDeleteIndex--)
				CATCH_SIGNAL(vkDestroySemaphore(vk_hDevice, vk_hSemaphores[u32SemaphoreDeleteIndex - 1U], nullptr));
			return false;
		}

		VkFenceCreateInfo vk_fenceCreateInfo = {};
		vk_fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		vk_fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		uint32_t u32FencesCreatedSuccessfully = 0U;
		for (uint32_t u32FenceCreateIndex = 0U; u32FenceCreateIndex < RE_VK_FENCE_COUNT; u32FenceCreateIndex++) {
			if (!CHECK_VK_RESULT(vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, &vk_hFences[u32FenceCreateIndex]))) {
				RE_FATAL_ERROR(append_to_string("Failed creating Vulkan fence at index ", u32FenceCreateIndex));
				break;
			}
			u32FencesCreatedSuccessfully++;
		}
		if (u32FencesCreatedSuccessfully != RE_VK_FENCE_COUNT) {
			for (uint32_t u32SemaphoreDeleteIndex = 0U; u32SemaphoreDeleteIndex < RE_VK_SEMAPHORES_PER_FRAME; u32SemaphoreDeleteIndex++) {
				CATCH_SIGNAL(vkDestroySemaphore(vk_hDevice, vk_hSemaphores[u32SemaphoreDeleteIndex * RE_VK_SEMAPHORES_PER_FRAME + RE_VK_SEMAPHORE_IMAGE_AVAILABLE_INDEX], nullptr));
				CATCH_SIGNAL(vkDestroySemaphore(vk_hDevice, vk_hSemaphores[u32SemaphoreDeleteIndex * RE_VK_SEMAPHORES_PER_FRAME + RE_VK_SEMAPHORE_RENDERING_FINISHED_INDEX], nullptr));
			}
			for (uint32_t u32FenceDeleteIndex = u32FencesCreatedSuccessfully + 1U; u32FenceDeleteIndex > 0U; u32FenceDeleteIndex--)
				CATCH_SIGNAL(vkDestroyFence(vk_hDevice, vk_hFences[u32FenceDeleteIndex - 1U], nullptr));
			return false;
		}
		return true;
	}
	
	void RenderSystem::destroy_sync_objects() {
		for (uint32_t u32SemaphoreDeleteIndex = 0U; u32SemaphoreDeleteIndex < RE_VK_SEMAPHORE_COUNT; u32SemaphoreDeleteIndex++)
			CATCH_SIGNAL(vkDestroySemaphore(vk_hDevice, vk_hSemaphores[u32SemaphoreDeleteIndex], nullptr));
		for (uint32_t u32FenceDeleteIndex = 0U; u32FenceDeleteIndex < RE_VK_FENCE_COUNT; u32FenceDeleteIndex++)
			CATCH_SIGNAL(vkDestroyFence(vk_hDevice, vk_hFences[u32FenceDeleteIndex], nullptr));
	}

	void RenderSystem::draw_frame() {
		CATCH_SIGNAL(vkWaitForFences(vk_hDevice, 1U, &vk_hFences[bUseOtherFrame], VK_TRUE, std::numeric_limits<uint64_t>::max()));
		CATCH_SIGNAL(vkResetFences(vk_hDevice, 1U, &vk_hFences[bUseOtherFrame]));
		vertices[14] = (InputMgr::pInstance->get_cursor_x() / static_cast<float>(Window::pInstance->get_size()[0])) * 2.0f - 1.0f;
		vertices[15] = (InputMgr::pInstance->get_cursor_y() / static_cast<float>(Window::pInstance->get_size()[1])) * 2.0f - 1.0f;
		upload_to_vertex_buffer(vertices, RE_VK_VERTEX_TOTAL_SIZE * 3U);
		uint32_t u32NextSwapchainImageIndex;
		VkResult vk_nextImageObtainedResult;
		CATCH_SIGNAL(vk_nextImageObtainedResult = vkAcquireNextImageKHR(vk_hDevice, vk_hSwapchain, std::numeric_limits<uint64_t>::max(), vk_hSemaphores[bUseOtherFrame * RE_VK_SEMAPHORES_PER_FRAME + RE_VK_SEMAPHORE_IMAGE_AVAILABLE_INDEX], VK_NULL_HANDLE, &u32NextSwapchainImageIndex));
		switch (vk_nextImageObtainedResult) {
			case VK_SUCCESS:
				break;
			case VK_SUBOPTIMAL_KHR:
			case VK_ERROR_OUT_OF_DATE_KHR:
				CATCH_SIGNAL(recreate_swapchain());
				break;
			default:
				CHECK_VK_RESULT(vk_nextImageObtainedResult);
				return;
		}
		VkPipelineStageFlags vk_pipelineStageFlags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		VkSubmitInfo vk_queueGraphicsSubmitInfo = {};
		vk_queueGraphicsSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		vk_queueGraphicsSubmitInfo.waitSemaphoreCount = 1U;
		vk_queueGraphicsSubmitInfo.pWaitSemaphores = &vk_hSemaphores[bUseOtherFrame * RE_VK_SEMAPHORES_PER_FRAME + RE_VK_SEMAPHORE_IMAGE_AVAILABLE_INDEX];
		vk_queueGraphicsSubmitInfo.pWaitDstStageMask = vk_pipelineStageFlags;
		vk_queueGraphicsSubmitInfo.commandBufferCount = 1U;
		CATCH_SIGNAL(vk_queueGraphicsSubmitInfo.pCommandBuffers = &vk_phCommandBuffersGraphics[u32NextSwapchainImageIndex]);
		vk_queueGraphicsSubmitInfo.signalSemaphoreCount = 1U;
		vk_queueGraphicsSubmitInfo.pSignalSemaphores = &vk_hSemaphores[bUseOtherFrame * RE_VK_SEMAPHORES_PER_FRAME + RE_VK_SEMAPHORE_RENDERING_FINISHED_INDEX];
		if (!CHECK_VK_RESULT(vkQueueSubmit(vk_hQueues[RE_VK_QUEUE_GRAPHICS_INDEX], 1U, &vk_queueGraphicsSubmitInfo, vk_hFences[bUseOtherFrame]))) {
			RE_FATAL_ERROR("Failed submitting data to the graphics queue in Vulkan");
			return;
		}
		VkPresentInfoKHR vk_queuePresentSubmitInfo = {};
		vk_queuePresentSubmitInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		vk_queuePresentSubmitInfo.waitSemaphoreCount = 1U;
		vk_queuePresentSubmitInfo.pWaitSemaphores = &vk_hSemaphores[bUseOtherFrame * RE_VK_SEMAPHORES_PER_FRAME + RE_VK_SEMAPHORE_RENDERING_FINISHED_INDEX];
		vk_queuePresentSubmitInfo.swapchainCount = 1U;
		vk_queuePresentSubmitInfo.pSwapchains = &vk_hSwapchain;
		vk_queuePresentSubmitInfo.pImageIndices = &u32NextSwapchainImageIndex;
		CATCH_SIGNAL(vkQueuePresentKHR(vk_hQueues[RE_VK_QUEUE_PRESENT_INDEX], &vk_queuePresentSubmitInfo));

		bUseOtherFrame = !bUseOtherFrame;
	}

	void RenderSystem::upload_to_vertex_buffer(const REvertex *const pNewVertexBufferData, const uint32_t u32VertexCount) {
		void* pData;
		CATCH_SIGNAL(vkMapMemory(vk_hDevice, vk_hVertexStagingBufferMemory, 0UL, u32VertexCount * RE_VK_VERTEX_TOTAL_SIZE_BYTES, 0, &pData));
		std::memcpy(pData, pNewVertexBufferData, u32VertexCount * RE_VK_VERTEX_TOTAL_SIZE_BYTES);
		CATCH_SIGNAL(vkUnmapMemory(vk_hDevice, vk_hVertexStagingBufferMemory));
	}

	void RenderSystem::window_resize_event() {
		CATCH_SIGNAL(recreate_swapchain());
	}

	void RenderSystem::wait_for_idle_device() {
		CATCH_SIGNAL(vkDeviceWaitIdle(vk_hDevice));
	}

	bool RenderSystem::is_valid() {
		return bValid;
	}

}