#ifndef __RE_VK_H__
#define __RE_VK_H__

#include "RE_Ext Header.hpp"
#ifdef RE_OS_WINDOWS
# define VK_USE_PLATFORM_WIN32_KHR
#elif defined RE_OS_LINUX
# define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <vulkan/vulkan.h>

namespace RE {

	class Vulkan {
		private:
			bool valid;

			void loadFunc(const char* funcName);

#ifdef RE_OS_WINDOWS
			HINSTANCE hInstance;
			HWND hWindow;
			HMODULE hVulkan;

		public:
			Vulkan() = delete;
			Vulkan(HINSTANCE hInstance, HWND hWindow);
#elif defined RE_OS_LINUX
			XDisplay* const xDisplay;
			const XWindow xWindow;
			void* const vulkanLib;

		public:
			Vulkan() = delete;
			Vulkan(XDisplay* xDisplay, XWindow xWindow);
#else
		public:
			Vulkan();
#endif
			~Vulkan();
			bool isValid();
	};

	extern Vulkan* vkClassInst;

}

#endif /* __RE_VK_H__ */
