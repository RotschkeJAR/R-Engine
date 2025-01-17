#ifndef __RE_VK_X11_H__
#define __RE_VK_X11_H__

namespace RE {
	
#ifdef RE_OS_LINUX
	class Vulkan_X11 {
		private:
			XDisplay* xDisplay;
			XWindow xWindow;
			void* const vulkanLib;

		protected:
			void* loadFuncInternal();

		public:
			Vulkan_X11() = delete;
			Vulkan_X11(XDisplay* xDisplay, XWindow xWindow);
			~Vulkan_X11();
	};
#endif /* RE_OS_LINUX */

}

#endif /* __RE_VK_X11_H__ */
