#include "RE_Input_Internal.hpp"

namespace RE {

	bool init_input() {
		if (init_input_keyboard()) {
			if (init_input_cursor()) {
				reset_all_input();
				return true;
			}
			destroy_input_keyboard();
		}
		return false;
	}

	void destroy_input() {
		destroy_input_cursor();
		destroy_input_keyboard();
	}

	void update_input_buffers() {
		update_keyboard_buffers();
		update_cursor_buffers();
	}

	void reset_all_input() {
		PRINT_DEBUG("Resetting all input buffers");
		reset_keyboard_input();
		reset_mouse_input();
	}

}
