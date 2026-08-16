#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__ 1

#include "RE_Internal.hpp"
#include "RE_Window.hpp"

namespace RE {

	typedef uint32_t scancode_t;

#define INVALID_SCANCODE   0

	bool init_input();
	void destroy_input();
	void keyboard_event(Input eEnteredInput, scancode_t enteredScancode, bool bPressed);
	void button_event(Input eButtonInput, bool bPressed);
	void cursor_event(int32_t i32X, int32_t i32Y);
	void scroll_event(int iHorizontal, int iVertical);
	void update_input_buffers();

	bool is_key_down(Input eInput, scancode_t scancode = INVALID_SCANCODE);
	bool was_key_down(Input eInput, scancode_t scancode = INVALID_SCANCODE);
	bool is_key_pressed(Input eInput, scancode_t scancode = INVALID_SCANCODE);
	bool is_key_released(Input eInput, scancode_t scancode = INVALID_SCANCODE);
	bool is_key_held_down(Input eInput, scancode_t scancode = INVALID_SCANCODE);
	bool is_button_down(Input eInput);
	bool was_button_down(Input eInput);
	bool is_button_pressed(Input eInput);
	bool is_button_released(Input eInput);
	bool is_button_held_down(Input eInput);
	int32_t get_cursor_position_x();
	int32_t get_cursor_position_y();
#define get_cursor_normal_position_x() (get_cursor_position_x() / static_cast<float>(windowSize[0]))
#define get_cursor_normal_position_y() (get_cursor_position_y() / static_cast<float>(windowSize[1]))
	int get_scroll_axis_horizontal();
	int get_scroll_axis_vertical();

}

#endif /* __RE_INPUT_H__ */
