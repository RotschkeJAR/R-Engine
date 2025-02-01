#include "RE_Vulkan.hpp"

namespace RE {

#define VK_INIT 0
#define VK_LAST VK_INIT

	Vulkan::Vulkan() : validation(0), core(nullptr) {
		std::vector<std::string> vulkanInstanceExtensions;
#ifdef RE_OS_WINDOWS
		vulkanInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined RE_OS_LINUX
		vulkanInstanceExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif
		core = new VulkanCore(vulkanInstanceExtensions, false);
		if (!core->isValid()) {
			delete core;
			core = nullptr;
			return;
		}
		setBit<REubyte>(validation, VK_INIT, true);
	}

	Vulkan::~Vulkan() {
		if (isBitTrue<REubyte>(validation, VK_INIT))
			delete core;
	}

	bool Vulkan::isValid() {
		return areBitsTrueRange<REubyte>(validation, VK_INIT, VK_LAST);
	}

}
