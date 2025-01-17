#include "RE_Vulkan_X11.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX
	void* Vulkan_X11::loadFuncInternal(const char* funcName) {
		return vulkanLib ? reinterpret_cast<void*>(dlsym(vulkanLib, funcName)) : nullptr;
	}

	Vulkan_X11::Vulkan_X11(XDisplay* xDisplay, XWindow xWindow) xDisplay(xDisplay), xWindow(xWindow), vulkanLib(dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCALE)) {
		if (!vulkanLib) {
			RE_FATAL_ERROR("Failed loading the Vulkan so-library");
			return;
		}
		loadAllFunc();
	}

	Vulkan_X11::~Vulkan_X11() {
		if (!vulkanLib)
			return;
		destroyVulkanInstance();
	}
#endif /* RE_OS_LINUX */

}
