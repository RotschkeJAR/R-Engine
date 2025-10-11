#include "RE_Input.hpp"
#include "RE_Window.hpp"

namespace RE {

#define MAXIMUM_PHYSICAL_KEYS 150
#define FIRST_KEY_IN_INPUT RE_INPUT_KEY_SPACE
#define SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE (RE_INPUT_MAX_ENUM - FIRST_KEY_IN_INPUT)
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
		return static_cast<uint8_t>(eInput - FIRST_KEY_IN_INPUT);
	}

	[[nodiscard]]
	static uint8_t get_key_buffer_index_for_scancode(const uint32_t u32SearchedScancode) {
		PRINT_DEBUG("Looking for key buffer index for scancode ", std::hex, u32SearchedScancode);
		uint8_t u8MinIndex = 0, u8MaxIndex = u8NumberOfKeys > 0 ? (u8NumberOfKeys - 1) : 0;
		while (u8MinIndex <= u8MaxIndex) {
			const uint8_t u8CurrentIndex = u8MinIndex + (u8MaxIndex - u8MinIndex) / 2;
			const uint32_t u32CurrentScancode = au32Scancodes[u8CurrentIndex];
			PRINT_DEBUG("Currently checking index ", u8CurrentIndex, ", which has scancode ", std::hex, u32CurrentScancode);
			if (u32CurrentScancode == u32SearchedScancode) {
				const uint8_t u8IndexFound = u8CurrentIndex + KEY_BUFFER_OFFSET;
				PRINT_DEBUG("Key buffer index for scancode ", std::hex, u32SearchedScancode, " is ", std::dec, u8IndexFound);
				return u8IndexFound;
			} else if (u32SearchedScancode < u32CurrentScancode) {
				if (u8CurrentIndex == u8MinIndex)
					break;
				u8MaxIndex = u8CurrentIndex - 1;
				PRINT_DEBUG("Maximum index updated to ", u8MaxIndex);
			} else {
				u8MinIndex = u8CurrentIndex + 1;
				PRINT_DEBUG("Minimum index updated to ", u8MinIndex);
			}
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
				PRINT_DEBUG("Querying input state of mouse input ", std::hex, eInput);
				if constexpr (eStateToCheck == ASKING_STATE_PRESENT)
					return are_bits_true<uint8_t>(u8MouseBuffer, mouse_input_to_uint8(eInput));
				else if constexpr (eStateToCheck == ASKING_STATE_PAST)
					return are_bits_true<uint8_t>(u8PrevMouseBuffer, mouse_input_to_uint8(eInput));
				else {
					const uint8_t u8MouseBufferBitmask = gen_bitmask<uint8_t>(mouse_input_to_uint8(eInput));
					PRINT_DEBUG("Generated mouse buffer bitmask ", std::hex, u8MouseBufferBitmask, " to fetch data from multiple buffers");
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
				const uint8_t u8KeyBufferArrayIndex = u8KeyIndex / 8, u8KeyBufferBitIndex = u8KeyIndex % 8;
				PRINT_DEBUG("Querying input state at array index ", u8KeyBufferArrayIndex, " and bit index ", u8KeyBufferBitIndex);
				if constexpr (eStateToCheck == ASKING_STATE_PRESENT)
					return are_bits_true<uint8_t>(au8KeyBuffer[u8KeyBufferArrayIndex], u8KeyBufferBitIndex);
				else if constexpr (eStateToCheck == ASKING_STATE_PAST)
					return are_bits_true<uint8_t>(au8PrevKeyBuffer[u8KeyBufferArrayIndex], u8KeyBufferBitIndex);
				else {
					const uint8_t u8KeyBufferBitmask = gen_bitmask<uint8_t>(u8KeyBufferBitIndex);
					PRINT_DEBUG("Generated key buffer bitmask ", std::hex, u8KeyBufferBitmask, " to fetch data from multiple buffers");
					if constexpr (eStateToCheck == ASKING_STATE_BOTH)
						return (au8KeyBuffer[u8KeyBufferArrayIndex] & au8PrevKeyBuffer[u8KeyBufferArrayIndex] & u8KeyBufferBitmask) != 0;
					else if constexpr (eStateToCheck == ASKING_STATE_PRESSED)
						return (au8KeyBuffer[u8KeyBufferArrayIndex] & u8KeyBufferBitmask) != 0 && (au8PrevKeyBuffer[u8KeyBufferArrayIndex] & u8KeyBufferBitmask) == 0;
					else if constexpr (eStateToCheck == ASKING_STATE_RELEASED)
						return (au8KeyBuffer[u8KeyBufferArrayIndex] & u8KeyBufferBitmask) == 0 && (au8PrevKeyBuffer[u8KeyBufferArrayIndex] & u8KeyBufferBitmask) != 0;
				}
		}
		PRINT_DEBUG("Input ", std::hex, eInput, " and scancode ", u32Scancode, " didn't match with any data to get input state");
		return false;
	}

	void input_event(const Input eEnteredInput, const uint32_t u32EnteredScancode, const bool bPressed, const bool bFallbackToInput) {
		PRINT_DEBUG("Input has been received with input ", std::hex, eEnteredInput, ", scancode ", u32EnteredScancode, ", pressed-state ", bPressed, " and fallback-to-input-state ", bFallbackToInput);
		switch (eEnteredInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE:
				set_bits<uint8_t>(u8MouseBuffer, bPressed, static_cast<uint8_t>(eEnteredInput - RE_INPUT_SCROLL_UP));
				if (pUpdateInputObject && bPressed) {
					PRINT_DEBUG("Changing input in object ", pUpdateInputObject, " to mouse input ", std::hex, eEnteredInput);
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
				else if (u32EnteredScancode) { // Keyboard input unknown
					if (u8NumberOfKeys >= MAXIMUM_PHYSICAL_KEYS) {
						RE_WARNING("New scancode ", std::hex, u32EnteredScancode, " cannot be added, because the list is full");
						break;
					}
					PRINT_DEBUG("New unknown key pressed. Trying to find suitable place for scancode ", std::hex, u32EnteredScancode);
					uint8_t u8InsertionIndex = u8NumberOfKeys;
					{ // Looking for suitable place to insert new scancode to keep ascending order
						uint8_t u8MinIndex = 0, u8MaxIndex = u8NumberOfKeys > 0 ? (u8NumberOfKeys - 1) : 0;
						while (u8MinIndex <= u8MaxIndex) {
							const uint8_t u8CurrentIndex = u8MinIndex + (u8MaxIndex - u8MinIndex) / 2;
							const uint32_t u32CurrentScancode = au32Scancodes[u8CurrentIndex];
							PRINT_DEBUG("Traversing index ", u8CurrentIndex, ", which contains scancode ", std::hex, u32CurrentScancode);
							if (u8CurrentIndex == 0) {
								if (u32EnteredScancode < u32CurrentScancode) {
									u8InsertionIndex = 0;
									break;
								} else {
									u8MinIndex = 1;
									PRINT_DEBUG("Minimum index updated to ", u8MinIndex);
								}
							} else if (u32EnteredScancode > u32CurrentScancode) {
								u8MinIndex = u8CurrentIndex + 1;
								PRINT_DEBUG("Minimum index updated to ", u8MinIndex);
							} else {
								const uint32_t u32LowerScancode = au32Scancodes[u8CurrentIndex - 1];
								if (u32EnteredScancode < u32LowerScancode) {
									u8MaxIndex = u8CurrentIndex - 1;
									PRINT_DEBUG("Maximum index updated to ", u8MaxIndex);
								} else {
									u8InsertionIndex = u8CurrentIndex;
									break;
								}
							}
						}
						PRINT_DEBUG("Picked place for insertion at index ", u8InsertionIndex);
					}
					for (uint8_t u8ScancodeIndex = u8NumberOfKeys; u8ScancodeIndex > u8InsertionIndex; u8ScancodeIndex--) {
						const uint8_t u8LowerIndex = u8ScancodeIndex - 1;
						PRINT_DEBUG("Moving data at index ", u8LowerIndex, " to ", u8ScancodeIndex, " in input buffers");
						au32Scancodes[u8ScancodeIndex] = au32Scancodes[u8LowerIndex];
						const uint8_t u8KeyBufferIndex = u8ScancodeIndex + KEY_BUFFER_OFFSET;
						const uint8_t u8KeyBufferLowerIndex = u8LowerIndex + KEY_BUFFER_OFFSET;
						set_bits<uint8_t>(au8KeyBuffer[u8KeyBufferIndex / 8], are_bits_true<uint8_t>(au8KeyBuffer[u8KeyBufferLowerIndex / 8], u8KeyBufferLowerIndex % 8), u8KeyBufferIndex % 8);
						set_bits<uint8_t>(au8PrevKeyBuffer[u8KeyBufferIndex / 8], are_bits_true<uint8_t>(au8PrevKeyBuffer[u8KeyBufferLowerIndex / 8], u8KeyBufferLowerIndex % 8), u8KeyBufferIndex % 8);
					}
					for (uint8_t u8InputIndex = 0; u8InputIndex < SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE; u8InputIndex++)
						if (au8InputToKeyBufferIndexTable[u8InputIndex] >= u8InsertionIndex + KEY_BUFFER_OFFSET) {
							PRINT_DEBUG("Incrementing index for input to key buffer-translation in table at index ", u8InputIndex);
							au8InputToKeyBufferIndexTable[u8InputIndex]++;
						}
					PRINT_DEBUG("Inserting new scancode at index ", u8InsertionIndex);
					au32Scancodes[u8InsertionIndex] = u32EnteredScancode;
					const uint8_t u8KeyBufferInsertionIndex = u8InsertionIndex + KEY_BUFFER_OFFSET;
					if (is_key_input(eEnteredInput)) {
						const uint8_t u8Input = key_input_to_uint8(eEnteredInput);
						PRINT_DEBUG("Setting translation at index ", u8Input, " to ", u8KeyBufferInsertionIndex, " in input to key buffer-index table");
						au8InputToKeyBufferIndexTable[u8Input] = u8KeyBufferInsertionIndex;
					}
					set_bits<uint8_t>(au8KeyBuffer[u8KeyBufferInsertionIndex / 8], bPressed, u8KeyBufferInsertionIndex % 8);
					set_bits<uint8_t>(au8PrevKeyBuffer[u8KeyBufferInsertionIndex / 8], !bPressed, u8KeyBufferInsertionIndex % 8);
					u8NumberOfKeys++;
					RE_NOTE("Remaining slots for more undetected physical keys: ", MAXIMUM_PHYSICAL_KEYS - u8NumberOfKeys);
				}
				if (pUpdateInputObject && bPressed) {
					if (u32EnteredScancode) {
						PRINT_DEBUG("Changing input in object ", pUpdateInputObject, " to key scancode 0x", std::hex, u32EnteredScancode);
						pUpdateInputObject->change_to_scancode(u32EnteredScancode);
					} else {
						PRINT_DEBUG("Changing input in object ", pUpdateInputObject, " to key input 0x", std::hex, u32EnteredScancode);
						pUpdateInputObject->change_to_input(eEnteredInput);
					}
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
		PRINT_DEBUG("Copying values from present to past input buffers");
		prevCursorPosition.copy_from(cursorPosition);
		std::copy(std::begin(au8KeyBuffer), std::end(au8KeyBuffer), std::begin(au8PrevKeyBuffer));
		u8PrevMouseBuffer = u8MouseBuffer;
		set_bits_in_range<uint8_t>(u8MouseBuffer, false, RE_INPUT_SCROLL_UP, RE_INPUT_SCROLL_DOWN + 1);
	}

	[[nodiscard]]
	bool is_down(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Getting present state of input ", std::hex, eInput, " and scancode ", u32Scancode);
		return get_state_of_user_input<ASKING_STATE_PRESENT>(eInput, u32Scancode);
	}
	
	[[nodiscard]]
	bool was_down(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Getting past state of input ", std::hex, eInput, " and scancode ", u32Scancode);
		return get_state_of_user_input<ASKING_STATE_PAST>(eInput, u32Scancode);
	}

	[[nodiscard]]
	bool is_pressed(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Getting pressed state of input ", std::hex, eInput, " and scancode ", u32Scancode);
		return get_state_of_user_input<ASKING_STATE_PRESSED>(eInput, u32Scancode);
	}

	[[nodiscard]]
	bool is_released(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Getting released state of input ", std::hex, eInput, " and scancode ", u32Scancode);
		return get_state_of_user_input<ASKING_STATE_RELEASED>(eInput, u32Scancode);
	}

	[[nodiscard]]
	bool is_held_down(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Getting held state of input ", std::hex, eInput, " and scancode ", u32Scancode);
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
			PRINT_DEBUG("Mapping key scancode ", std::hex, u32Scancode, " to input");
			const uint8_t u8KeyBufferIndexForScancode = get_key_buffer_index_for_scancode(u32Scancode);
			if (u8KeyBufferIndexForScancode == FAILURE_KEY_BUFFER_INDEX) {
				PRINT_DEBUG("Key scancode is unknown. Mapping to input failed");
				return RE_INPUT_UNKNOWN;
			}
			for (uint8_t u8InputIndex = 0; u8InputIndex < SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE; u8InputIndex++) {
				PRINT_DEBUG("Checking alias input index ", u8InputIndex);
				if (au8InputToKeyBufferIndexTable[u8InputIndex] == u8KeyBufferIndexForScancode) {
					const uint8_t u8InputEnum = u8InputIndex + FIRST_KEY_IN_INPUT;
					PRINT_DEBUG("Scancode mapped to input ", std::hex, u8InputEnum);
					return static_cast<Input>(u8InputEnum);
				}
			}
			PRINT_DEBUG("No input found corresponding to scancode");
			return RE_INPUT_UNKNOWN;
		}
		RE_NOTE("Invalid scancode: ", std::hex, u32Scancode);
		return RE_INPUT_UNKNOWN;
	}

	[[nodiscard]]
	uint32_t map_input_to_scancode(const Input eInput) {
		if (eInput >= FIRST_KEY_IN_INPUT && eInput < RE_INPUT_MAX_ENUM) {
			PRINT_DEBUG("Mapping input ", std::hex, eInput, " to scancode");
			const uint8_t u8Index = au8InputToKeyBufferIndexTable[key_input_to_uint8(eInput)];
			if (u8Index >= KEY_BUFFER_OFFSET) {
				const uint8_t u8IndexToScancode = u8Index - KEY_BUFFER_OFFSET;
				PRINT_DEBUG("Returning scancode from index ", u8IndexToScancode);
				return au32Scancodes[u8IndexToScancode];
			}
			PRINT_DEBUG("No scancode registered to input yet");
			return 0;
		}
		RE_NOTE("Invalid key input. It either is mouse input or the value is invalid in general: ", std::hex, eInput);
		return 0;
	}

	void reset_input_at(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Resetting state at input ", std::hex, eInput, " and/or scancode ", u32Scancode);
		switch (eInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE: {
				const uint8_t u8Index = mouse_input_to_uint8(eInput);
				PRINT_DEBUG("Resetting state of mouse input at array index ", u8Index);
				set_bits<uint8_t>(u8MouseBuffer, false, u8Index);
				set_bits<uint8_t>(u8PrevMouseBuffer, false, u8Index);
				} break;
			default:
				uint8_t u8KeyBufferIndex = FAILURE_KEY_BUFFER_INDEX;
				if (u32Scancode)
					u8KeyBufferIndex = get_key_buffer_index_for_scancode(u32Scancode);
				if (u8KeyBufferIndex == FAILURE_KEY_BUFFER_INDEX)
					u8KeyBufferIndex = get_key_buffer_index_for_input(eInput);
				if (u8KeyBufferIndex == FAILURE_KEY_BUFFER_INDEX) {
					RE_NOTE("Invalid input ", std::hex, eInput, " and/or scancode ", u32Scancode, ". Can't reset any data");
					break;
				}
				const uint8_t u8KeyBufferArrayIndex = u8KeyBufferIndex / 8, u8KeyBufferBitIndex = u8KeyBufferIndex % 8;
				PRINT_DEBUG("Resetting state of key input at array index ", u8KeyBufferArrayIndex, " and bit index ", u8KeyBufferBitIndex);
				set_bits<uint8_t>(au8KeyBuffer[u8KeyBufferArrayIndex], false, u8KeyBufferBitIndex);
				set_bits<uint8_t>(au8PrevKeyBuffer[u8KeyBufferArrayIndex], false, u8KeyBufferBitIndex);
				break;
		}
	}

	void reset_mouse_input() {
		PRINT_DEBUG("Resetting all mouse input buffers");
		u8MouseBuffer = 0;
		u8PrevMouseBuffer = 0;
		cursorPosition = prevCursorPosition;
	}

	void reset_keyboard_input() {
		PRINT_DEBUG("Resetting all key input buffers");
		std::fill(std::begin(au8KeyBuffer), std::end(au8KeyBuffer), 0);
		std::fill(std::begin(au8PrevKeyBuffer), std::end(au8PrevKeyBuffer), 0);
	}

	void reset_all_input() {
		PRINT_DEBUG("Resetting all input buffers");
		reset_keyboard_input();
		reset_mouse_input();
	}

}
