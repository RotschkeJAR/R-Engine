#include "RE_Input.hpp"
#include "RE_Window.hpp"

namespace RE {

#define MAXIMUM_PHYSICAL_KEYS 150
#define SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE (RE_INPUT_MAX_ENUM - RE_INPUT_KEY_SPACE)
#define KEY_BUFFER_OFFSET 1
#define KEY_BUFFER_SIZE ((MAXIMUM_PHYSICAL_KEYS + KEY_BUFFER_OFFSET) / 8 + ((MAXIMUM_PHYSICAL_KEYS + KEY_BUFFER_OFFSET) % 8 > 0 ? 1 : 0))

#define FAILURE_KEY_BUFFER_INDEX 0

	enum AskingState {
		ASKING_STATE_BOTH,
		ASKING_STATE_PRESENT,
		ASKING_STATE_PAST,
		ASKING_STATE_PRESSED,
		ASKING_STATE_RELEASED
	};

	std::array<uint32_t, MAXIMUM_PHYSICAL_KEYS> au32Scancodes = {};
	std::array<uint8_t, SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE> au8InputToKeyBufferIndexTable = {};
	std::array<uint8_t, KEY_BUFFER_SIZE> au8KeyBuffer = {}, au8PrevKeyBuffer = {}; // Keyboard
	uint8_t u8NumberOfKeys = 0;
	uint8_t u8MouseBuffer = 0, u8PrevMouseBuffer = 0; // Scrolling and mouse buttons
	Vector2i cursorPosition, prevCursorPosition;
	InputAction *pUpdateInputObject = nullptr;

	[[nodiscard]]
	static uint8_t mouse_input_to_uint8(const Input eInput) {
		return static_cast<uint8_t>(eInput - RE_INPUT_SCROLL_UP);
	}

	[[nodiscard]]
	static uint8_t key_input_to_uint8(const Input eInput) {
		return static_cast<uint8_t>(eInput - RE_INPUT_KEY_SPACE);
	}

	[[nodiscard]]
	static uint8_t get_key_buffer_index_for_scancode(const uint32_t u32SearchedScancode) {
		uint8_t u8MinIndex = 0, u8MaxIndex = u8NumberOfKeys > 0 ? (u8NumberOfKeys - 1) : 0;
		while (u8MinIndex <= u8MaxIndex) {
			const uint8_t u8CurrentIndex = u8MinIndex + (u8MaxIndex - u8MinIndex) / 2;
			const uint32_t u32CurrentScancode = au32Scancodes[u8CurrentIndex];
			if (u32CurrentScancode == u32SearchedScancode)
				return u8CurrentIndex + KEY_BUFFER_OFFSET;
			else if (u32SearchedScancode < u32CurrentScancode) {
				if (u8CurrentIndex == u8MinIndex)
					break;
				u8MaxIndex = u8CurrentIndex - 1;
			} else
				u8MinIndex = u8CurrentIndex + 1;
		}
		return FAILURE_KEY_BUFFER_INDEX;
	}

	[[nodiscard]]
	static uint8_t get_key_buffer_index_for_input(const Input eSearchedInput) {
		return is_key_input(eSearchedInput) ? au8InputToKeyBufferIndexTable[key_input_to_uint8(eSearchedInput)] : FAILURE_KEY_BUFFER_INDEX;
	}

	template<AskingState eStateToCheck>
	[[nodiscard]]
	static bool get_state_of_user_input(const Input eInput, const uint32_t u32Scancode) {
		switch (eInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE:
				if constexpr (eStateToCheck == ASKING_STATE_PRESENT)
					return are_bits_true<uint8_t>(u8MouseBuffer, mouse_input_to_uint8(eInput));
				else if constexpr (eStateToCheck == ASKING_STATE_PAST)
					return are_bits_true<uint8_t>(u8PrevMouseBuffer, mouse_input_to_uint8(eInput));
				else {
					const uint8_t u8MouseBufferBitmask = gen_bitmask<uint8_t>(mouse_input_to_uint8(eInput));
					if constexpr (eStateToCheck == ASKING_STATE_BOTH)
						return (u8MouseBuffer & u8PrevMouseBuffer & u8MouseBufferBitmask) != 0;
					else if constexpr (eStateToCheck == ASKING_STATE_PRESSED)
						return (u8MouseBuffer & u8MouseBufferBitmask) != 0 && (u8PrevMouseBuffer & u8MouseBufferBitmask) == 0;
					else if constexpr (eStateToCheck == ASKING_STATE_RELEASED)
						return (u8MouseBuffer & u8MouseBufferBitmask) == 0 && (u8PrevMouseBuffer & u8MouseBufferBitmask) != 0;
				}
			default:
				uint8_t u8KeyIndex = FAILURE_KEY_BUFFER_INDEX;
				if (u32Scancode)
					u8KeyIndex = get_key_buffer_index_for_scancode(u32Scancode);
				if (u8KeyIndex == FAILURE_KEY_BUFFER_INDEX)
					u8KeyIndex = get_key_buffer_index_for_input(eInput);
				const uint8_t u8KeyBufferArrayIndex = u8KeyIndex / 8;
				if constexpr (eStateToCheck == ASKING_STATE_PRESENT)
					return are_bits_true<uint8_t>(au8KeyBuffer[u8KeyIndex / 8], u8KeyIndex % 8);
				else if constexpr (eStateToCheck == ASKING_STATE_PAST)
					return are_bits_true<uint8_t>(au8PrevKeyBuffer[u8KeyIndex / 8], u8KeyIndex % 8);
				else {
					const uint8_t u8KeyBufferBitmask = gen_bitmask<uint8_t>(u8KeyIndex % 8);
					if constexpr (eStateToCheck == ASKING_STATE_BOTH)
						return (au8KeyBuffer[u8KeyBufferArrayIndex] & au8PrevKeyBuffer[u8KeyBufferArrayIndex] & u8KeyBufferBitmask) != 0;
					else if constexpr (eStateToCheck == ASKING_STATE_PRESSED)
						return (au8KeyBuffer[u8KeyBufferArrayIndex] & u8KeyBufferBitmask) != 0 && (au8PrevKeyBuffer[u8KeyBufferArrayIndex] & u8KeyBufferBitmask) == 0;
					else if constexpr (eStateToCheck == ASKING_STATE_RELEASED)
						return (au8KeyBuffer[u8KeyBufferArrayIndex] & u8KeyBufferBitmask) == 0 && (au8PrevKeyBuffer[u8KeyBufferArrayIndex] & u8KeyBufferBitmask) != 0;
				}
		}
		return false;
	}

	void input_event(const Input eEnteredInput, const uint32_t u32EnteredScancode, const bool bPressed, const bool bFallbackToInput) {
		switch (eEnteredInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE:
				set_bits<uint8_t>(u8MouseBuffer, bPressed, static_cast<uint8_t>(eEnteredInput - RE_INPUT_SCROLL_UP));
				if (pUpdateInputObject && bPressed) {
					pUpdateInputObject->change_to_input(eEnteredInput);
					pUpdateInputObject = nullptr;
				}
				break;
			default: // Keyboard input
				uint8_t u8KeyBufferIndex = FAILURE_KEY_BUFFER_INDEX;
				if (u32EnteredScancode)
					u8KeyBufferIndex = get_key_buffer_index_for_scancode(u32EnteredScancode);
				if (u8KeyBufferIndex == FAILURE_KEY_BUFFER_INDEX && bFallbackToInput)
					u8KeyBufferIndex = get_key_buffer_index_for_input(eEnteredInput);
				if (u8KeyBufferIndex != FAILURE_KEY_BUFFER_INDEX)
					set_bits<uint8_t>(au8KeyBuffer[u8KeyBufferIndex / 8], bPressed, u8KeyBufferIndex % 8);
				else if (u32EnteredScancode) {
					// Keyboard input unknown
					if (u8NumberOfKeys >= MAXIMUM_PHYSICAL_KEYS) {
						RE_WARNING("New scancode ", std::hex, u32EnteredScancode, std::dec, " cannot be added, because the list is full");
						break;
					}
					uint8_t u8InsertionIndex = u8NumberOfKeys;
					{ // Looking for suitable place to insert new scancode to keep ascending order
						uint8_t u8MinIndex = 0, u8MaxIndex = u8NumberOfKeys > 0 ? (u8NumberOfKeys - 1) : 0;
						while (u8MinIndex <= u8MaxIndex) {
							const uint8_t u8CurrentIndex = u8MinIndex + (u8MaxIndex - u8MinIndex) / 2;
							const uint32_t u32CurrentScancode = au32Scancodes[u8CurrentIndex];
							if (u8CurrentIndex == 0) {
								if (u32EnteredScancode < u32CurrentScancode) {
									u8InsertionIndex = 0;
									break;
								} else
									u8MinIndex = 1;
							} else if (u32EnteredScancode > u32CurrentScancode)
								u8MinIndex = u8CurrentIndex + 1;
							else {
								const uint32_t u32LowerScancode = au32Scancodes[u8CurrentIndex - 1];
								if (u32EnteredScancode < u32LowerScancode)
									u8MaxIndex = u8CurrentIndex - 1;
								else {
									u8InsertionIndex = u8CurrentIndex;
									break;
								}
							}
						}
					}
					for (uint8_t u8ScancodeIndex = u8NumberOfKeys; u8ScancodeIndex > u8InsertionIndex; u8ScancodeIndex--) {
						const uint8_t u8LowerIndex = u8ScancodeIndex - 1;
						au32Scancodes[u8ScancodeIndex] = au32Scancodes[u8LowerIndex];
						const uint8_t u8KeyBufferIndex = u8ScancodeIndex + KEY_BUFFER_OFFSET;
						const uint8_t u8KeyBufferLowerIndex = u8LowerIndex + KEY_BUFFER_OFFSET;
						set_bits<uint8_t>(au8KeyBuffer[u8KeyBufferIndex / 8], are_bits_true<uint8_t>(au8KeyBuffer[u8KeyBufferLowerIndex / 8], u8KeyBufferLowerIndex % 8), u8KeyBufferIndex % 8);
						set_bits<uint8_t>(au8PrevKeyBuffer[u8KeyBufferIndex / 8], are_bits_true<uint8_t>(au8PrevKeyBuffer[u8KeyBufferLowerIndex / 8], u8KeyBufferLowerIndex % 8), u8KeyBufferIndex % 8);
					}
					for (uint8_t u8InputIndex = 0; u8InputIndex < SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE; u8InputIndex++)
						if (au8InputToKeyBufferIndexTable[u8InputIndex] >= u8InsertionIndex + KEY_BUFFER_OFFSET)
							au8InputToKeyBufferIndexTable[u8InputIndex]++;
					au32Scancodes[u8InsertionIndex] = u32EnteredScancode;
					if (is_key_input(eEnteredInput))
						au8InputToKeyBufferIndexTable[key_input_to_uint8(eEnteredInput)] = u8InsertionIndex + KEY_BUFFER_OFFSET;
					const uint8_t u8KeyBufferInsertionIndex = u8InsertionIndex + KEY_BUFFER_OFFSET;
					set_bits<uint8_t>(au8KeyBuffer[u8KeyBufferInsertionIndex / 8], bPressed, u8KeyBufferInsertionIndex % 8);
					set_bits<uint8_t>(au8PrevKeyBuffer[u8KeyBufferInsertionIndex / 8], !bPressed, u8KeyBufferInsertionIndex % 8);
					u8NumberOfKeys++;
					RE_NOTE("Remaining slots for more undetected physical keys: ", MAXIMUM_PHYSICAL_KEYS - u8NumberOfKeys);
				}
				if (pUpdateInputObject && bPressed) {
					if (u32EnteredScancode)
						pUpdateInputObject->change_to_scancode(u32EnteredScancode);
					else
						pUpdateInputObject->change_to_input(eEnteredInput);
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
		prevCursorPosition.copy_from(cursorPosition);
		std::copy(std::begin(au8KeyBuffer), std::end(au8KeyBuffer), std::begin(au8PrevKeyBuffer));
		u8PrevMouseBuffer = u8MouseBuffer;
		set_bits_in_range<uint8_t>(u8MouseBuffer, false, RE_INPUT_SCROLL_UP, RE_INPUT_SCROLL_DOWN + 1);
	}

	[[nodiscard]]
	bool is_down(const Input eInput, const uint32_t u32Scancode) {
		return get_state_of_user_input<ASKING_STATE_PRESENT>(eInput, u32Scancode);
	}
	
	[[nodiscard]]
	bool was_down(const Input eInput, const uint32_t u32Scancode) {
		return get_state_of_user_input<ASKING_STATE_PAST>(eInput, u32Scancode);
	}

	[[nodiscard]]
	bool is_pressed(const Input eInput, const uint32_t u32Scancode) {
		return get_state_of_user_input<ASKING_STATE_PRESSED>(eInput, u32Scancode);
	}

	[[nodiscard]]
	bool is_released(const Input eInput, const uint32_t u32Scancode) {
		return get_state_of_user_input<ASKING_STATE_RELEASED>(eInput, u32Scancode);
	}

	[[nodiscard]]
	bool is_held_down(const Input eInput, const uint32_t u32Scancode) {
		return get_state_of_user_input<ASKING_STATE_BOTH>(eInput, u32Scancode);
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
			uint8_t u8KeyBufferIndexForScancode = get_key_buffer_index_for_scancode(u32Scancode);
			if (u8KeyBufferIndexForScancode == FAILURE_KEY_BUFFER_INDEX)
				return RE_INPUT_UNKNOWN;
			for (uint8_t u8InputIndex = 0; u8InputIndex < RE_INPUT_MAX_ENUM; u8InputIndex++)
				if (au8InputToKeyBufferIndexTable[u8InputIndex] == u8KeyBufferIndexForScancode)
					return static_cast<Input>(u8InputIndex);
		}
		return RE_INPUT_UNKNOWN;
	}

	[[nodiscard]]
	uint32_t map_input_to_scancode(const Input eInput) {
		if (eInput >= RE_INPUT_KEY_SPACE && eInput < RE_INPUT_MAX_ENUM) {
			const uint8_t u8Index = au8InputToKeyBufferIndexTable[key_input_to_uint8(eInput)];
			return u8Index > 0 ? au32Scancodes[u8Index - KEY_BUFFER_OFFSET] : 0;
		}
		return 0;
	}

	void reset_input_at(const Input eInput, const uint32_t u32Scancode) {
		switch (eInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE: {
				const uint8_t u8Index = mouse_input_to_uint8(eInput);
				set_bits<uint8_t>(u8MouseBuffer, false, u8Index);
				set_bits<uint8_t>(u8PrevMouseBuffer, false, u8Index);
				} break;
			default:
				uint8_t u8KeyBufferIndex = FAILURE_KEY_BUFFER_INDEX;
				if (u32Scancode)
					u8KeyBufferIndex = get_key_buffer_index_for_scancode(u32Scancode);
				if (u8KeyBufferIndex == FAILURE_KEY_BUFFER_INDEX)
					u8KeyBufferIndex = get_key_buffer_index_for_input(eInput);
				if (u8KeyBufferIndex == FAILURE_KEY_BUFFER_INDEX)
					break;
				set_bits<uint8_t>(au8KeyBuffer[u8KeyBufferIndex / 8], false, u8KeyBufferIndex % 8);
				set_bits<uint8_t>(au8PrevKeyBuffer[u8KeyBufferIndex / 8], false, u8KeyBufferIndex % 8);
				break;
		}
	}

	void reset_mouse_input() {
		u8MouseBuffer = 0;
		u8PrevMouseBuffer = 0;
		cursorPosition = prevCursorPosition;
	}

	void reset_keyboard_input() {
		std::fill(std::begin(au8KeyBuffer), std::end(au8KeyBuffer), 0);
		std::fill(std::begin(au8PrevKeyBuffer), std::end(au8PrevKeyBuffer), 0);
	}

	void reset_all_input() {
		reset_keyboard_input();
		reset_mouse_input();
	}

}
