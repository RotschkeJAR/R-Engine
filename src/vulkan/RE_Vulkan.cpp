#include "RE_Vulkan.hpp"

namespace RE {

#define VK_INIT 0
#define VK_LAST VK_INIT

	Vulkan::Vulkan() : validation(0), core(nullptr) {
		constexpr REuint vulkanInstanceExtensionCount = 1;
		const char** vulkanInstanceExtensions = new const char*[vulkanInstanceExtensionCount] {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
		core = new VulkanCore(vulkanInstanceExtensions, vulkanInstanceExtensionCount);
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
