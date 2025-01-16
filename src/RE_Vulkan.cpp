#include "RE_Vulkan.hpp"

namespace RE {

	Vulkan* vkClassInst = nullptr;
#define INST_CHECK 	if (vkClassInst) { \
						RE_FATAL_ERROR("Another instance of the Vulkan-class has been initialized"); \
						return; \
					} \
					vkClassInst = this
	
#ifdef RE_OS_WINDOWS
	Vulkan::Vulkan(HINSTANCE hInstance, HWND hWindow) : valid(false), hInstance(hInstance), hWindow(hWindow), hVulkan(LoadLibraryW(L"vulkan-1.dll")) {
		INST_CHECK;
		if (!hVulkan) {
			RE_FATAL_ERROR("Failed loading the Vulkan dll-library");
			return;
		}
		valid = true;
	}

	Vulkan::~Vulkan() {
		vkClassInst = nullptr;
		if (hVulkan)
			FreeLibrary(hVulkan);
	}

	void* Vulkan::loadFunc(const char* funcName) {
		return GetProcAddress(hVulkan, funcName);
	}
#elif defined RE_OS_LINUX
	Vulkan::Vulkan(XDisplay* xDisplay, XWindow xWindow) : valid(false), xDisplay(xDisplay), xWindow(xWindow), vulkanLib(dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL)) {
		INST_CHECK;
		if (!vulkanLib) {
			RE_FATAL_ERROR("Failed loading the Vulkan so-library");
			return;
		}
		valid = true;
	}

	Vulkan::~Vulkan() {
		vkClassInst = nullptr;
		if (vulkanLib)
			dlclose(vulkanLib);
	}

	void* Vulkan::loadFunc(const char* funcName) {
		return dlsym(hVulkan, funcName);
	}
#else
	Vulkan::Vulkan() : valid(false) {
		RE_WARNING("The vulkan loader terminated immediatly, because the OS is unknown");
		INST_CHECK;
	}

	Vulkan::~Vulkan() {
		vkClassInst = nullptr;
	}

	void* Vulkan::loadFunc(const char* funcName) {
		return nullptr;
	}
#endif
	bool Vulkan::isValid() {
		return valid;
	}

}
