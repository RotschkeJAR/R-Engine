#ifndef __RE_VK_WIN64_H__
#define __RE_VK_WIN64_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
#ifdef RE_OS_WINDOWS
	class Vulkan_Win64 : public Vulkan {
		private:
			HINSTANCE hInstance;
			HWND hWindow;
			HMODULE hVulkan;

		protected:
			void* loadFuncInternal(const char* funcName);
			bool createSurface();

		public:
			Vulkan_Win64() = delete;
			Vulkan_Win64(HINSTANCE hInstance, HWND hWindow);
			~Vulkan_Win64();
	};
#endif

}

#endif /* __RE_VK_WIN64_H__ */
