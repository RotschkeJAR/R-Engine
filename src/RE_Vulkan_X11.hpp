#ifndef __RE_VK_X11_H__
#define __RE_VK_X11_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX
	class Vulkan_X11 : public Vulkan {
		private:
			XDisplay* const xDisplay;
			XWindow xWindow;
			void* const vulkanLib;

		protected:
			void* loadFuncInternal(const char* funcName);
			bool createSurface();

		public:
			Vulkan_X11() = delete;
			Vulkan_X11(XDisplay* xDisplay, XWindow xWindow, Vector<REushort, 2>& winSize);
			~Vulkan_X11();
	};
#endif /* RE_OS_LINUX */

}

#endif /* __RE_VK_X11_H__ */
