#include "RE_Input.hpp"
#include "RE_Window.hpp"

namespace RE {

#define MAXIMUM_PHYSICAL_KEYS 150
#define KEY_BUFFER_SIZE (MAXIMUM_PHYSICAL_KEYS / 8 + (MAXIMUM_PHYSICAL_KEYS % 8 > 0 ? 1 : 0))

#define FAILURE_INDEX -1

	uint32_t u32Scancodes[MAXIMUM_PHYSICAL_KEYS] = {};
	Input eInputs[MAXIMUM_PHYSICAL_KEYS] = {};
	uint8_t u8KeyBuffer[KEY_BUFFER_SIZE] = {}, u8PrevKeyBuffer[KEY_BUFFER_SIZE] = {}; // Keyboard
	uint8_t u8NumberOfKeys = 0;
	uint8_t u8MouseInputBuffer = 0, u8PrevMouseInputBuffer = 0; // Scrolling and mouse buttons
	Vector2i cursorPosition, prevCursorPosition;
	InputAction *pUpdateInputObject = nullptr;

	[[nodiscard]]
	static int16_t get_index_for_scancode(const uint32_t u32SearchedScancode) {
		uint32_t u32MinIndex = 0;
		uint32_t u32MaxIndex = u8NumberOfKeys > 0 ? (u8NumberOfKeys - 1) : 0;
		while (u32MinIndex <= u32MaxIndex) {
			const uint32_t u32CurrentIndex = u32MinIndex + (u32MaxIndex - u32MinIndex) / 2;
			const uint32_t u32CurrentScancode = u32Scancodes[u32CurrentIndex];
			if (u32CurrentScancode == u32SearchedScancode)
				return static_cast<int16_t>(u32CurrentIndex);
			else if (u32SearchedScancode < u32CurrentScancode) {
				if (u32CurrentIndex == u32MinIndex)
					return FAILURE_INDEX;
				u32MaxIndex = u32CurrentIndex - 1;
			} else
				u32MinIndex = u32CurrentIndex + 1;
		}
		return FAILURE_INDEX;
	}

	[[nodiscard]]
	static int16_t get_index_for_input(const Input eSearchedInput) {
		for (uint8_t i = 0; i < u8NumberOfKeys; i++)
			if (eInputs[i] == eSearchedInput)
				return static_cast<int16_t>(i);
		return FAILURE_INDEX;
	}

	[[nodiscard]]
	static bool check_key_down_state(const Input eInput, const uint32_t u32Scancode, const bool bRequestForPast) {
		switch (eInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE:
				return is_bit_true<uint8_t>(bRequestForPast ? u8PrevMouseInputBuffer : u8MouseInputBuffer, static_cast<uint8_t>(eInput - RE_INPUT_SCROLL_UP));
			default:
				int16_t i16KeyIndex = FAILURE_INDEX;
				if (u32Scancode)
					i16KeyIndex = get_index_for_scancode(u32Scancode);
				if (i16KeyIndex == FAILURE_INDEX && eInput != RE_INPUT_UNKNOWN)
					i16KeyIndex = get_index_for_input(eInput);
				if (i16KeyIndex == FAILURE_INDEX)
					return false;
				return is_bit_true<uint8_t>(bRequestForPast ? u8PrevKeyBuffer[i16KeyIndex / 8] : u8KeyBuffer[i16KeyIndex / 8], i16KeyIndex % 8);
		}
	}

	void input_event(const Input eEnteredInput, const uint32_t u32EnteredScancode, const bool bPressed, const bool bFallbackToInput) {
		switch (eEnteredInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE:
				CATCH_SIGNAL(set_bit<uint8_t>(u8MouseInputBuffer, static_cast<uint8_t>(eEnteredInput - RE_INPUT_SCROLL_UP), bPressed));
				if (pUpdateInputObject && bPressed) {
					pUpdateInputObject->change_input(eEnteredInput);
					pUpdateInputObject = nullptr;
				}
				break;
			default: // Keyboard input
				if ((!u32EnteredScancode && !bFallbackToInput) || eEnteredInput < RE_INPUT_UNKNOWN || eEnteredInput >= RE_INPUT_MAX_ENUM)
					break;
				int16_t i16KeyIndex = FAILURE_INDEX;
				if (u32EnteredScancode)
					CATCH_SIGNAL(i16KeyIndex = get_index_for_scancode(u32EnteredScancode));
				if (i16KeyIndex == FAILURE_INDEX && bFallbackToInput && eEnteredInput != RE_INPUT_UNKNOWN)
					CATCH_SIGNAL(i16KeyIndex = get_index_for_input(eEnteredInput));
				if (i16KeyIndex != FAILURE_INDEX)
					CATCH_SIGNAL(set_bit<uint8_t>(u8KeyBuffer[i16KeyIndex / 8], i16KeyIndex % 8, bPressed));
				else { // Keyboard input unknown
					if (u8NumberOfKeys >= MAXIMUM_PHYSICAL_KEYS) {
						RE_WARNING(append_to_string("New scancode ", hexadecimal_to_string(u32EnteredScancode, true), " cannot be added, because the list is full"));
						break;
					}
					uint8_t u8InsertionIndex = u8NumberOfKeys;
					{ // Looking for suitable place to insert new scancode to keep ascending order
						uint8_t u8MinIndex = 0U, u8MaxIndex = u8NumberOfKeys > 0U ? (u8NumberOfKeys - 1U) : 0U;
						while (u8MinIndex <= u8MaxIndex) {
							const uint8_t u8CurrentIndex = u8MinIndex + (u8MaxIndex - u8MinIndex) / 2U;
							const uint32_t u32CurrentScancode = u32Scancodes[u8CurrentIndex];
							if (u8CurrentIndex == 0U) {
								if (u32EnteredScancode < u32CurrentScancode) {
									u8InsertionIndex = 0U;
									break;
								} else
									u8MinIndex = 1U;
							} else if (u32EnteredScancode > u32CurrentScancode) {
								u8MinIndex = u8CurrentIndex + 1U;
							} else {
								const uint32_t u32LowerScancode = u32Scancodes[u8CurrentIndex - 1U];
								if (u32EnteredScancode < u32LowerScancode)
									u8MaxIndex = u8CurrentIndex - 1U;
								else {
									u8InsertionIndex = u8CurrentIndex;
									break;
								}
							}
						}
					}
					for (uint8_t i = u8NumberOfKeys; i > u8InsertionIndex; i--) {
						const uint8_t u8LowerIndex = i - 1U;
						u32Scancodes[i] = u32Scancodes[u8LowerIndex];
						eInputs[i] = eInputs[u8LowerIndex];
						set_bit<uint8_t>(u8KeyBuffer[i / 8U], i % 8U, is_bit_true<uint8_t>(u8KeyBuffer[u8LowerIndex / 8U], u8LowerIndex % 8U));
						set_bit<uint8_t>(u8PrevKeyBuffer[i / 8U], i % 8U, is_bit_true<uint8_t>(u8PrevKeyBuffer[u8LowerIndex / 8U], u8LowerIndex % 8U));
					}
					u32Scancodes[u8InsertionIndex] = u32EnteredScancode;
					eInputs[u8InsertionIndex] = eEnteredInput;
					set_bit<uint8_t>(u8KeyBuffer[u8InsertionIndex / 8U], u8InsertionIndex % 8U, bPressed);
					set_bit<uint8_t>(u8PrevKeyBuffer[u8InsertionIndex / 8U], u8InsertionIndex % 8U, !bPressed);
					u8NumberOfKeys++;
					RE_NOTE(append_to_string("Remaining slots for more undetected physical keys: ", MAXIMUM_PHYSICAL_KEYS - u8NumberOfKeys));
				}
				if (pUpdateInputObject && bPressed) {
					if (u32EnteredScancode)
						pUpdateInputObject->change_scancode(u32EnteredScancode);
					else
						pUpdateInputObject->change_input(eEnteredInput);
					pUpdateInputObject = nullptr;
				}
				break;
		}
	}

	void cursor_event(const int32_t i32X, const int32_t i32Y) {
		cursorPosition[0] = i32X;
		cursorPosition[1] = i32Y;
	}

	void update_input_buffers() {
		CATCH_SIGNAL(prevCursorPosition.copy_from(cursorPosition));
		std::copy(std::begin(u8KeyBuffer), std::end(u8KeyBuffer), std::begin(u8PrevKeyBuffer));
		u8PrevMouseInputBuffer = u8MouseInputBuffer;
		set_bits<uint8_t>(u8MouseInputBuffer, RE_INPUT_SCROLL_UP, RE_INPUT_SCROLL_DOWN + 1, false);
	}

	[[nodiscard]]
	bool is_key_down(const Input eInput, const uint32_t u32Scancode) {
		return check_key_down_state(eInput, u32Scancode, false);
	}
	
	[[nodiscard]]
	bool was_key_down(const Input eInput, const uint32_t u32Scancode) {
		return check_key_down_state(eInput, u32Scancode, true);
	}

	[[nodiscard]]
	int32_t get_cursor_position_x() {
		return cursorPosition[0];
	}

	[[nodiscard]]
	int32_t get_cursor_position_y() {
		return cursorPosition[1];
	}

	[[nodiscard]]
	float get_cursor_normal_position_x() {
		return cursorPosition[0] / static_cast<float>(windowSize[0]);
	}

	[[nodiscard]]
	float get_cursor_normal_position_y() {
		return cursorPosition[1] / static_cast<float>(windowSize[1]);
	}

	[[nodiscard]]
	Input map_scancode_to_input(const uint32_t u32Scancode) {
		if (u32Scancode) {
			int16_t i16Index = get_index_for_scancode(u32Scancode);
			if (i16Index != FAILURE_INDEX && i16Index < static_cast<int16_t>(u8NumberOfKeys))
				return eInputs[i16Index];
		}
		return RE_INPUT_UNKNOWN;
	}

	[[nodiscard]]
	uint32_t map_input_to_scancode(const Input eInput) {
		if (eInput >= RE_INPUT_KEY_SPACE && eInput < RE_INPUT_MAX_ENUM) {
			int16_t i16Index = get_index_for_input(eInput);
			if (i16Index != FAILURE_INDEX && i16Index < static_cast<int16_t>(u8NumberOfKeys))
				return u32Scancodes[i16Index];
		}
		return 0U;
	}

	void reset_input_at(const Input eInput, const uint32_t u32Scancode) {
		switch (eInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE: {
				const int16_t i16Index = get_index_for_input(eInput);
				set_bit<uint8_t>(u8MouseInputBuffer, i16Index, false);
				set_bit<uint8_t>(u8PrevMouseInputBuffer, i16Index, false);
				} break;
			default:
				int16_t i16KeyIndex = FAILURE_INDEX;
				if (u32Scancode)
					i16KeyIndex = get_index_for_scancode(u32Scancode);
				if (i16KeyIndex == FAILURE_INDEX && eInput != RE_INPUT_UNKNOWN)
					i16KeyIndex = get_index_for_input(eInput);
				if (i16KeyIndex == FAILURE_INDEX)
					return;
				set_bit<uint8_t>(u8KeyBuffer[i16KeyIndex / 8], i16KeyIndex % 8, false);
				set_bit<uint8_t>(u8PrevKeyBuffer[i16KeyIndex / 8], i16KeyIndex % 8, false);
				break;
		}
	}

	void reset_mouse_input() {
		u8MouseInputBuffer = 0U;
		u8PrevMouseInputBuffer = 0U;
		cursorPosition = prevCursorPosition;
	}

	void reset_keyboard_input() {
		std::fill(std::begin(u8KeyBuffer), std::end(u8KeyBuffer), 0U);
		std::fill(std::begin(u8PrevKeyBuffer), std::end(u8PrevKeyBuffer), 0U);
	}

	void reset_all_input() {
		reset_keyboard_input();
		reset_mouse_input();
	}

}
