#include "RE_Input_Internal.hpp"

namespace RE {

	uint8_t u8MouseBuffer = 0,
		u8PrevMouseBuffer = 0;
	Vector2i cursorPosition,
		prevCursorPosition;

	int32_t get_cursor_position_x() {
		return cursorPosition[0];
	}

	int32_t get_cursor_position_y() {
		return cursorPosition[1];
	}

	float get_cursor_normal_position_x() {
		return cursorPosition[0] / static_cast<float>(windowSize[0]);
	}

	float get_cursor_normal_position_y() {
		return cursorPosition[1] / static_cast<float>(windowSize[1]);
	}

	void reset_mouse_input() {
		PRINT_DEBUG("Resetting all mouse input buffers");
		u8MouseBuffer = 0;
		u8PrevMouseBuffer = 0;
		cursorPosition = prevCursorPosition;
	}

}
