#include "RE_Vulkan_X11.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX
	void* Vulkan_X11::loadFuncInternal(const char* funcName) {
		return vulkanLib ? reinterpret_cast<void*>(dlsym(vulkanLib, funcName)) : nullptr;
	}

	bool Vulkan_X11::createSurface() {
		PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(loadFunc("vkCreateXlibSurfaceKHR"));
		if (!vkCreateXlibSurfaceKHR)
			return false;
		VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = { VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR };
		surfaceCreateInfo.dpy = xDisplay;
		surfaceCreateInfo.window = xWindow;
		if (vkCreateXlibSurfaceKHR(vkInstance, &surfaceCreateInfo, nullptr, &vkSurface) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed creating Vulkan surface linked to X11 window");
			return false;
		}
		setBit<REubyte>(validation, _VK_SURF, true);
		return true;
	}

	Vulkan_X11::Vulkan_X11(XDisplay* xDisplay, XWindow xWindow, Vector<REushort, 2>& winSize) : xDisplay(xDisplay), xWindow(xWindow), vulkanLib(dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL)) {
		if (!vulkanLib) {
			RE_FATAL_ERROR("Failed loading the Vulkan so-library");
			return;
		}
		constexpr REuint numExtensions = 2;
		const char** extensions = new const char*[numExtensions] {"VK_KHR_surface", "VK_KHR_xlib_surface"};
		initVulkan(extensions, numExtensions, winSize);
		delete[] extensions;
	}

	Vulkan_X11::~Vulkan_X11() {
		if (!vulkanLib)
			return;
		destroyVulkan();
	}
#endif /* RE_OS_LINUX */

}
