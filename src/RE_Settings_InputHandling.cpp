#include "RE_Settings_Internal.hpp"
#include "RE_Window_Internal.hpp"

namespace RE {

	void settings_handle_keyboard_input(uint32_t u32VirtualKeycode) {
	#ifdef RE_OS_WINDOWS
		switch (u32VirtualKeycode) {
			case VK_ESCAPE:
			case VK_BACK:
				close_settings();
				break;
			case VK_RETURN:
			case VK_SPACE:
				break;
			case VK_LEFT:
				break;
			case VK_RIGHT:
				break;
			case VK_UP:
				break;
			case VK_DOWN:
				break;
			default:
				break;
		}
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				switch (u32VirtualKeycode) {
					case :
						close_settings();
						break;
					case :
						break;
					default:
						break;
				}
				break;
			case LINUX_WINDOW_TYPE_WAYLAND:
				switch (u32VirtualKeycode) {
					case :
						close_settings();
						break;
					case :
						break;
					default:
						break;
				}
				break;
		}
	#endif
	}

}
