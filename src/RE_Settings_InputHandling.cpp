#include "RE_Settings_Internal.hpp"
#include "RE_Window.hpp"

namespace RE {

	static void settings_step_back() {
		close_settings();
	}

	static void settings_step_select() {

	}

	static void settings_step_left() {

	}

	static void settings_step_right() {

	}

	static void settings_step_up() {

	}

	static void settings_step_down() {

	}

	void settings_handle_keyboard_input(uint32_t u32VirtualKeycode) {
	#ifdef RE_OS_WINDOWS
		switch (u32VirtualKeycode) {
			case VK_ESCAPE:
			case VK_BACK:
				settings_step_back();
				break;
			case VK_RETURN:
			case VK_SPACE:
				settings_step_select();
				break;
			case VK_LEFT:
				settings_step_left();
				break;
			case VK_RIGHT:
				settings_step_right();
				break;
			case VK_UP:
				settings_step_up();
				break;
			case VK_DOWN:
				settings_step_down();
				break;
			default:
				break;
		}
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				switch (u32VirtualKeycode) {
					case XK_Escape:
					case XK_BackSpace:
						settings_step_back();
						break;
					case XK_Return:
					case XK_space:
					case XK_KP_Enter:
					case XK_KP_Space:
						settings_step_select();
						break;
					case XK_Left:
					case XK_KP_Left:
						settings_step_left();
						break;
					case XK_Right:
					case XK_KP_Right:
						settings_step_right();
						break;
					case XK_Up:
					case XK_KP_Up:
						settings_step_up();
						break;
					case XK_Down:
					case XK_KP_Down:
						settings_step_down();
						break;
					default:
						break;
				}
				break;
			case LINUX_WINDOW_TYPE_WAYLAND:
				switch (u32VirtualKeycode) {
					case XKB_KEY_BackSpace:
					case XKB_KEY_Escape:
						settings_step_back();
						break;
					case XKB_KEY_Return:
					case XKB_KEY_space:
					case XKB_KEY_KP_Enter:
					case XKB_KEY_KP_Space:
						settings_step_select();
						break;
					case XKB_KEY_Left:
					case XKB_KEY_KP_Left:
						settings_step_left();
						break;
					case XKB_KEY_Right:
					case XKB_KEY_KP_Right:
						settings_step_right();
						break;
					case XKB_KEY_Up:
					case XKB_KEY_KP_Up:
						settings_step_up();
						break;
					case XKB_KEY_Down:
					case XKB_KEY_KP_Down:
						settings_step_down();
						break;
					default:
						break;
				}
				break;
		}
	#endif
	}

}
