#include "RE_Input_Internal.hpp"

namespace RE {

	cursorbuffer_t cursorBuffer = 0,
		prevCursorBuffer = 0;
	int aiScrolling[SCROLLING_DIMENSIONS] = {},
		aiPrevScrolling[SCROLLING_DIMENSIONS] = {};
	int32_t ai32CursorPosition[CURSOR_DIMENSIONS] = {},
		ai32PrevCursorPosition[CURSOR_DIMENSIONS] = {};

	bool init_input_cursor() {
		return true;
	}

	void destroy_input_cursor() {}

	void update_cursor_buffers() {
		prevCursorBuffer = cursorBuffer;
		std::copy(std::begin(aiScrolling), std::end(aiScrolling), std::begin(aiPrevScrolling));
		std::fill(std::begin(aiScrolling), std::end(aiScrolling), 0);
		std::copy(std::begin(ai32CursorPosition), std::end(ai32CursorPosition), std::begin(ai32PrevCursorPosition));
	}

	void button_event(Input eButtonInput, bool bPressed) {
		set_bits<cursorbuffer_t>(cursorBuffer, bPressed, static_cast<cursorbuffer_t>(eButtonInput - RE_INPUT_BUTTON_FIRST));
	}

	void cursor_event(int32_t i32X, int32_t i32Y) {
		ai32CursorPosition[CURSOR_X] = i32X;
		ai32CursorPosition[CURSOR_Y] = i32Y;
	}

	void scroll_event(int iHorizontal, int iVertical) {
		aiScrolling[SCROLLING_HORIZONTAL] += iHorizontal;
		aiScrolling[SCROLLING_VERTICAL] += iVertical;
	}

#define FETCH_INDICES_PROC(BUTTON_INDEX_NAME, BIT_INDEX_NAME) \
		unsigned BUTTON_INDEX_NAME = eInput - RE_INPUT_BUTTON_FIRST; \
		const unsigned BIT_INDEX_NAME = BUTTON_INDEX_NAME % CURSOR_BUFFER_CHUNK_SIZE

	bool is_button_down(Input eInput) {
		PRINT_DEBUG("Getting present state of button ", std::hex, eInput);
		FETCH_INDICES_PROC(buttonIndex, bitIndex);
		return are_bits_true<cursorbuffer_t>(cursorBuffer, bitIndex);
	}

	bool was_button_down(Input eInput) {
		PRINT_DEBUG("Getting past state of button ", std::hex, eInput);
		FETCH_INDICES_PROC(buttonIndex, bitIndex);
		return are_bits_true<cursorbuffer_t>(prevCursorBuffer, bitIndex);
	}

	bool is_button_pressed(Input eInput) {
		PRINT_DEBUG("Getting pressed state of button ", std::hex, eInput);
		FETCH_INDICES_PROC(buttonIndex, bitIndex);
		const auto xCursorBufferBitmask = gen_bitmask<cursorbuffer_t>(bitIndex);
		return (cursorBuffer & xCursorBufferBitmask) != 0 && (prevCursorBuffer & xCursorBufferBitmask) == 0;
	}

	bool is_button_released(Input eInput) {
		PRINT_DEBUG("Getting released state of button ", std::hex, eInput);
		FETCH_INDICES_PROC(buttonIndex, bitIndex);
		const auto xCursorBufferBitmask = gen_bitmask<cursorbuffer_t>(bitIndex);
		return (cursorBuffer & xCursorBufferBitmask) == 0 && (prevCursorBuffer & xCursorBufferBitmask) != 0;
	}

	bool is_button_held_down(Input eInput) {
		PRINT_DEBUG("Getting held state of button ", std::hex, eInput);
		FETCH_INDICES_PROC(buttonIndex, bitIndex);
		return (cursorBuffer & prevCursorBuffer & gen_bitmask<cursorbuffer_t>(bitIndex)) != 0;
	}

#undef FETCH_INDICES_PROC

	int32_t get_cursor_position_x() {
		return ai32CursorPosition[CURSOR_X];
	}

	int32_t get_cursor_position_y() {
		return ai32CursorPosition[CURSOR_Y];
	}

	int get_scroll_axis_horizontal() {
		return aiScrolling[SCROLLING_HORIZONTAL];
	}

	int get_scroll_axis_vertical() {
		return aiScrolling[SCROLLING_VERTICAL];
	}

	void reset_mouse_input() {
		PRINT_DEBUG("Resetting all mouse input buffers");
		cursorBuffer = 0;
		prevCursorBuffer = 0;
		std::fill(std::begin(aiScrolling), std::end(aiScrolling), 0);
		std::fill(std::begin(aiPrevScrolling), std::end(aiPrevScrolling), 0);
		std::copy(std::begin(ai32CursorPosition), std::end(ai32CursorPosition), std::begin(ai32PrevCursorPosition));
	}

}
