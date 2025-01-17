#include "RE_Vulkan_Win64.hpp"

namespace RE {

#ifdef RE_OS_WINDOWS
	void* Vulkan_Win64::loadFuncInternal(const char* funcName) {
		return hVulkan ? reinterpret_cast<void*>(GetProcAddress(hVulkan, funcName)) : nullptr;
	}
	
	Vulkan_Win64::Vulkan_Win64(HINSTANCE hInstance, HWND hWindow) : hInstance(hInstance), hWindow(hWindow), hVulkan(LoadLibraryW(L"vulkan-1.dll")) {
		if (!hVulkan) {
			RE_FATAL_ERROR("Failed loading the Vulkan dll-library");
			return;
		}
		loadAllFunc();
	}

	Vulkan_Win64::~Vulkan_Win64() {
		if (!hVulkan)
			return;
		destroyVulkanInstance();
	}
#endif /* RE_OS_WINDOWS */

}
