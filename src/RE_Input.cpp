#include "RE_Input_Internal.hpp"
#include "RE_Window.hpp"

namespace RE {

	enum AskingState {
		ASKING_STATE_BOTH,
		ASKING_STATE_PRESENT,
		ASKING_STATE_PAST,
		ASKING_STATE_PRESSED,
		ASKING_STATE_RELEASED
	};

	InputAction *pUpdateInputObject = nullptr;

	static unsigned get_key_buffer_index_for_scancode(const uint32_t u32SearchedScancode) {
		PRINT_DEBUG("Looking for key buffer index for scancode ", std::hex, u32SearchedScancode);
		unsigned uMinIndex = 0,
			uMaxIndex = uNumberOfKeys > 0 ? (uNumberOfKeys - 1) : 0;
		while (uMinIndex <= uMaxIndex) {
			const unsigned uCurrentIndex = uMinIndex + (uMaxIndex - uMinIndex) / 2;
			const uint32_t u32CurrentScancode = au32Scancodes[uCurrentIndex];
			PRINT_DEBUG("Currently checking index ", uCurrentIndex, ", which has scancode ", std::hex, u32CurrentScancode);
			if (u32CurrentScancode == u32SearchedScancode) {
				const unsigned uIndexFound = uCurrentIndex + KEY_BUFFER_OFFSET;
				PRINT_DEBUG("Key buffer index for scancode ", std::hex, u32SearchedScancode, " is ", std::dec, uIndexFound);
				return uIndexFound;
			} else if (u32SearchedScancode < u32CurrentScancode) {
				if (uCurrentIndex == uMinIndex)
					break;
				uMaxIndex = uCurrentIndex - 1;
				PRINT_DEBUG("Maximum index updated to ", uMaxIndex);
			} else {
				uMinIndex = uCurrentIndex + 1;
				PRINT_DEBUG("Minimum index updated to ", uMinIndex);
			}
		}
		return FAILURE_KEY_BUFFER_INDEX;
	}

	static unsigned get_key_buffer_index_for_input(const Input eSearchedInput) {
		return is_key_input(eSearchedInput) ? auInputToKeyBufferIndexTable[KEYBOARD_INPUT_TO_UINT(eSearchedInput)] : FAILURE_KEY_BUFFER_INDEX;
	}

	void input_event(const Input eEnteredInput, const uint32_t u32EnteredScancode, const bool bPressed, const bool bFallbackToInput) {
		PRINT_DEBUG("Input has been received with input ", std::hex, eEnteredInput, ", scancode ", u32EnteredScancode, ", pressed-state ", bPressed, " and fallback-to-input-state ", bFallbackToInput);
		switch (eEnteredInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE:
				set_bits<decltype(u8MouseBuffer[0])>(u8MouseBuffer, bPressed, static_cast<decltype(u8MouseBuffer[0])>(eEnteredInput - RE_INPUT_SCROLL_UP));
				if (pUpdateInputObject && bPressed) {
					PRINT_DEBUG("Changing input in object ", pUpdateInputObject, " to mouse input ", std::hex, eEnteredInput);
					pUpdateInputObject->change_to_input(eEnteredInput);
					pUpdateInputObject = nullptr;
				}
				break;
			default: // Keyboard input
				uint8_t uKeyBufferIndex = FAILURE_KEY_BUFFER_INDEX;
				if (u32EnteredScancode)
					uKeyBufferIndex = get_key_buffer_index_for_scancode(u32EnteredScancode);
				if (uKeyBufferIndex == FAILURE_KEY_BUFFER_INDEX && bFallbackToInput)
					uKeyBufferIndex = get_key_buffer_index_for_input(eEnteredInput);
				if (uKeyBufferIndex != FAILURE_KEY_BUFFER_INDEX)
					set_bits<decltype(au8KeyBuffer[0])>(au8KeyBuffer[uKeyBufferIndex / 8], bPressed, uKeyBufferIndex % 8);
				else if (u32EnteredScancode) { // Keyboard input unknown
					if (uNumberOfKeys >= MAXIMUM_PHYSICAL_KEYS) {
						RE_WARNING("New scancode ", std::hex, u32EnteredScancode, " cannot be added, because the list is full");
						break;
					}
					PRINT_DEBUG("New unknown key pressed. Trying to find suitable place for scancode ", std::hex, u32EnteredScancode);
					unsigned uInsertionIndex = uNumberOfKeys;
					{ // Looking for suitable place to insert new scancode to keep ascending order
						unsigned uMinIndex = 0,
							uMaxIndex = uNumberOfKeys > 0 ? (uNumberOfKeys - 1) : 0;
						while (uMinIndex <= uMaxIndex) {
							const unsigned uCurrentIndex = uMinIndex + (uMaxIndex - uMinIndex) / 2;
							const uint32_t u32CurrentScancode = au32Scancodes[uCurrentIndex];
							PRINT_DEBUG("Traversing index ", uCurrentIndex, ", which contains scancode ", std::hex, u32CurrentScancode);
							if (uCurrentIndex == 0) {
								if (u32EnteredScancode < u32CurrentScancode) {
									uInsertionIndex = 0;
									break;
								} else {
									uMinIndex = 1;
									PRINT_DEBUG("Minimum index updated to ", uMinIndex);
								}
							} else if (u32EnteredScancode > u32CurrentScancode) {
								uMinIndex = uCurrentIndex + 1;
								PRINT_DEBUG("Minimum index updated to ", uMinIndex);
							} else {
								const uint32_t u32LowerScancode = au32Scancodes[uCurrentIndex - 1];
								if (u32EnteredScancode < u32LowerScancode) {
									uMaxIndex = uCurrentIndex - 1;
									PRINT_DEBUG("Maximum index updated to ", uMaxIndex);
								} else {
									uInsertionIndex = uCurrentIndex;
									break;
								}
							}
						}
						PRINT_DEBUG("Picked place for insertion at index ", uInsertionIndex);
					}
					for (unsigned uScancodeIndex = uNumberOfKeys; uScancodeIndex > uInsertionIndex; uScancodeIndex--) {
						const unsigned uLowerIndex = uScancodeIndex - 1;
						PRINT_DEBUG("Moving data at index ", uLowerIndex, " to ", uScancodeIndex, " in input buffers");
						au32Scancodes[uScancodeIndex] = au32Scancodes[uLowerIndex];
						const unsigned uKeyBufferIndex = uScancodeIndex + KEY_BUFFER_OFFSET,
							uKeyBufferLowerIndex = uLowerIndex + KEY_BUFFER_OFFSET;
						set_bits<decltype(au8KeyBuffer[0])>(au8KeyBuffer[uKeyBufferIndex / 8], are_bits_true<decltype(au8KeyBuffer[0])>(au8KeyBuffer[uKeyBufferLowerIndex / 8], uKeyBufferLowerIndex % 8), uKeyBufferIndex % 8);
						set_bits<decltype(au8PrevKeyBuffer[0])>(au8PrevKeyBuffer[uKeyBufferIndex / 8], are_bits_true<decltype(au8PrevKeyBuffer[0])>(au8PrevKeyBuffer[uKeyBufferLowerIndex / 8], uKeyBufferLowerIndex % 8), uKeyBufferIndex % 8);
					}
					for (unsigned uInputIndex = 0; uInputIndex < SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE; uInputIndex++)
						if (auInputToKeyBufferIndexTable[uInputIndex] >= uInsertionIndex + KEY_BUFFER_OFFSET) {
							PRINT_DEBUG("Incrementing index for input to key buffer-translation in table at index ", uInputIndex);
							auInputToKeyBufferIndexTable[uInputIndex]++;
						}
					PRINT_DEBUG("Inserting new scancode at index ", uInsertionIndex);
					au32Scancodes[uInsertionIndex] = u32EnteredScancode;
					const unsigned uKeyBufferInsertionIndex = uInsertionIndex + KEY_BUFFER_OFFSET;
					if (is_key_input(eEnteredInput)) {
						const unsigned uInput = KEYBOARD_INPUT_TO_UINT(eEnteredInput);
						PRINT_DEBUG("Setting translation at index ", uInput, " to ", uKeyBufferInsertionIndex, " in input to key buffer-index table");
						auInputToKeyBufferIndexTable[uInput] = uKeyBufferInsertionIndex;
					}
					set_bits<decltype(au8KeyBuffer[0])>(au8KeyBuffer[uKeyBufferInsertionIndex / 8], bPressed, uKeyBufferInsertionIndex % 8);
					set_bits<decltype(au8PrevKeyBuffer[0])>(au8PrevKeyBuffer[uKeyBufferInsertionIndex / 8], !bPressed, uKeyBufferInsertionIndex % 8);
					uNumberOfKeys++;
					RE_NOTE("Remaining slots for more undetected physical keys: ", MAXIMUM_PHYSICAL_KEYS - uNumberOfKeys);
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

	Input map_scancode_to_input(const uint32_t u32Scancode) {
		if (u32Scancode) {
			PRINT_DEBUG("Mapping key scancode ", std::hex, u32Scancode, " to input");
			const unsigned uKeyBufferIndexForScancode = get_key_buffer_index_for_scancode(u32Scancode);
			if (uKeyBufferIndexForScancode == FAILURE_KEY_BUFFER_INDEX) {
				PRINT_DEBUG("Key scancode is unknown. Mapping to input failed");
				return RE_INPUT_UNKNOWN;
			}
			for (unsigned uInputIndex = 0; uInputIndex < SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE; uInputIndex++) {
				PRINT_DEBUG("Checking alias input index ", uInputIndex);
				if (auInputToKeyBufferIndexTable[uInputIndex] == uKeyBufferIndexForScancode) {
					const uint8_t u8InputEnum = uInputIndex + FIRST_KEY_IN_INPUT;
					PRINT_DEBUG("Scancode mapped to input ", std::hex, u8InputEnum);
					return static_cast<Input>(u8InputEnum);
				}
			}
			PRINT_DEBUG("No input found corresponding to scancode");
			return RE_INPUT_UNKNOWN;
		}
		RE_ERROR("Invalid scancode: ", std::hex, u32Scancode);
		return RE_INPUT_UNKNOWN;
	}

	uint32_t map_input_to_scancode(const Input eInput) {
		if (eInput >= FIRST_KEY_IN_INPUT && eInput < RE_INPUT_MAX_ENUM) {
			PRINT_DEBUG("Mapping input ", std::hex, eInput, " to scancode");
			const unsigned uIndex = auInputToKeyBufferIndexTable[KEYBOARD_INPUT_TO_UINT(eInput)];
			if (uIndex >= KEY_BUFFER_OFFSET) {
				const unsigned uIndexToScancode = uIndex - KEY_BUFFER_OFFSET;
				PRINT_DEBUG("Returning scancode from index ", uIndexToScancode);
				return au32Scancodes[uIndexToScancode];
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
			case RE_INPUT_BUTTON_MIDDLE:
				{
					const unsigned uIndex = CURSOR_INPUT_TO_UINT(eInput);
					PRINT_DEBUG("Resetting state of mouse input at array index ", uIndex);
					set_bits<decltype(u8MouseBuffer[0])>(u8MouseBuffer, false, uIndex);
					set_bits<decltype(u8PrevMouseBuffer[0])>(u8PrevMouseBuffer, false, uIndex);
				}
				break;
			default:
				unsigned uKeyBufferIndex = FAILURE_KEY_BUFFER_INDEX;
				if (u32Scancode)
					uKeyBufferIndex = get_key_buffer_index_for_scancode(u32Scancode);
				if (uKeyBufferIndex == FAILURE_KEY_BUFFER_INDEX)
					uKeyBufferIndex = get_key_buffer_index_for_input(eInput);
				if (uKeyBufferIndex == FAILURE_KEY_BUFFER_INDEX) {
					RE_NOTE("Invalid input ", std::hex, eInput, " and/or scancode ", u32Scancode, ". Can't reset any data");
					break;
				}
				const unsigned uKeyBufferArrayIndex = uKeyBufferIndex / 8,
					uKeyBufferBitIndex = uKeyBufferIndex % 8;
				PRINT_DEBUG("Resetting state of key input at array index ", uKeyBufferArrayIndex, " and bit index ", uKeyBufferBitIndex);
				set_bits<decltype(au8KeyBuffer[0])>(au8KeyBuffer[uKeyBufferArrayIndex], false, uKeyBufferBitIndex);
				set_bits<decltype(au8PrevKeyBuffer[0])>(au8PrevKeyBuffer[uKeyBufferArrayIndex], false, uKeyBufferBitIndex);
				break;
		}
	}

	void reset_all_input() {
		PRINT_DEBUG("Resetting all input buffers");
		reset_keyboard_input();
		reset_mouse_input();
	}

}
