#include "RE_Vulkan_Win64.hpp"

namespace RE {

#ifdef RE_OS_WINDOWS
	void* Vulkan_Win64::loadFuncInternal(const char* funcName) {
		return hVulkan ? reinterpret_cast<void*>(GetProcAddress(hVulkan, funcName)) : nullptr;
	}

	bool Vulkan_Win64::createSurface() {
		PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(loadFunc("vkCreateWin32SurfaceKHR"));
		if (!vkCreateWin32SurfaceKHR)
			return false;
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
		surfaceCreateInfo.hwnd = hWindow;
		surfaceCreateInfo.hinstance = hInstance;
		if (vkCreateWin32SurfaceKHR(vkInstance, &surfaceCreateInfo, nullptr, &vkSurface) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating Vulkan surface linked to the Win32 window");
			return false;
		}
		return true;
	}
	
	Vulkan_Win64::Vulkan_Win64(HINSTANCE hInstance, HWND hWindow, Vector<REushort, 2>& winSize) : hInstance(hInstance), hWindow(hWindow), hVulkan(LoadLibraryW(L"vulkan-1.dll")) {
		if (!hVulkan) {
			RE_FATAL_ERROR("Failed loading the Vulkan dll-library");
			return;
		}
		std::vector<const char*> extensions({"VK_KHR_surface", "VK_KHR_win32_surface"});
		if (!initVulkan(extensions, winSize))
			RE_ERROR("Failed initializing Vulkan");
	}

	Vulkan_Win64::~Vulkan_Win64() {
		if (!hVulkan)
			return;
		destroyVulkan();
	}
#endif /* RE_OS_WINDOWS */

}
