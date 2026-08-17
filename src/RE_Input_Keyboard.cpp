#include "RE_Input_Internal.hpp"

namespace RE {

	enum AskingState {
		ASKING_STATE_BOTH,
		ASKING_STATE_PRESENT,
		ASKING_STATE_PAST,
		ASKING_STATE_PRESSED,
		ASKING_STATE_RELEASED
	};

	scancode_t auScancodes[MAXIMUM_PHYSICAL_KEYS] = {};
	unsigned auInputToKeyBufferIndexTable[SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE] = {};
	keyboardbuffer_t aKeyBuffer[KEY_BUFFER_SIZE] = {},
		aPrevKeyBuffer[KEY_BUFFER_SIZE] = {};
	unsigned uNumberOfKeys = 0;

	static_assert(sizeof(auScancodes) / sizeof(auScancodes[0]) <= UINT_MAX);
	static_assert(sizeof(auInputToKeyBufferIndexTable) / sizeof(auInputToKeyBufferIndexTable[0]) <= UINT_MAX);
	static_assert(sizeof(aKeyBuffer) / sizeof(aKeyBuffer[0]) <= UINT_MAX);

	bool init_input_keyboard() {
		return true;
	}

	void destroy_input_keyboard() {}

	void update_keyboard_buffers() {
		std::copy(std::begin(aKeyBuffer), std::end(aKeyBuffer), std::begin(aPrevKeyBuffer));
	}

	unsigned get_key_buffer_index_for_scancode(scancode_t searchedScancode) {
		PRINT_DEBUG("Looking for key buffer index for scancode ", std::hex, searchedScancode);
		unsigned uMinIndex = 0,
			uMaxIndex = uNumberOfKeys > 0 ? (uNumberOfKeys - 1) : 0;
		while (uMinIndex <= uMaxIndex) {
			const unsigned uCurrentIndex = uMinIndex + (uMaxIndex - uMinIndex) / 2;
			const scancode_t currentScancode = auScancodes[uCurrentIndex];
			PRINT_DEBUG("Currently checking index ", uCurrentIndex, ", which has scancode ", std::hex, currentScancode);
			if (currentScancode == searchedScancode) {
				const unsigned uIndexFound = uCurrentIndex + KEY_BUFFER_OFFSET;
				PRINT_DEBUG("Key buffer index for scancode ", std::hex, searchedScancode, " is ", std::dec, uIndexFound);
				return uIndexFound;
			} else if (searchedScancode < currentScancode) {
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

	unsigned get_key_buffer_index_for_input(Input eSearchedInput) {
		return auInputToKeyBufferIndexTable[KEYBOARD_INPUT_TO_UINT(eSearchedInput)];
	}

	void keyboard_event(Input eEnteredInput, scancode_t enteredScancode, bool bPressed) {
		PRINT_DEBUG("Input has been received with input ", std::hex, eEnteredInput, ", scancode ", enteredScancode, ", pressed-state ", bPressed);
		unsigned uKeyBufferIndex = FAILURE_KEY_BUFFER_INDEX;
		if (enteredScancode)
			uKeyBufferIndex = get_key_buffer_index_for_scancode(enteredScancode);
		if (uKeyBufferIndex == FAILURE_KEY_BUFFER_INDEX)
			uKeyBufferIndex = get_key_buffer_index_for_input(eEnteredInput);
		if (uKeyBufferIndex != FAILURE_KEY_BUFFER_INDEX)
			set_bits<keyboardbuffer_t>(aKeyBuffer[uKeyBufferIndex / KEY_BUFFER_CHUNK_SIZE], bPressed, uKeyBufferIndex % KEY_BUFFER_CHUNK_SIZE);
		else if (enteredScancode) { // Keyboard input unknown
			if (uNumberOfKeys >= MAXIMUM_PHYSICAL_KEYS) {
				RE_WARNING("New scancode ", std::hex, enteredScancode, " cannot be added, because the list is full");
				return;
			}
			PRINT_DEBUG("New unknown key pressed. Trying to find suitable place for scancode ", std::hex, enteredScancode);
			unsigned uInsertionIndex = uNumberOfKeys;
			{ // Looking for suitable place to insert new scancode to keep ascending order
				unsigned uMinIndex = 0,
					uMaxIndex = uNumberOfKeys > 0 ? (uNumberOfKeys - 1) : 0;
				while (uMinIndex <= uMaxIndex) {
					const unsigned uCurrentIndex = uMinIndex + (uMaxIndex - uMinIndex) / 2;
					const uint32_t currentScancode = auScancodes[uCurrentIndex];
					PRINT_DEBUG("Traversing index ", uCurrentIndex, ", which contains scancode ", std::hex, currentScancode);
					if (uCurrentIndex == 0) {
						if (enteredScancode < currentScancode) {
							uInsertionIndex = 0;
							return;
						} else {
							uMinIndex = 1;
							PRINT_DEBUG("Minimum index updated to ", uMinIndex);
						}
					} else if (enteredScancode > currentScancode) {
						uMinIndex = uCurrentIndex + 1;
						PRINT_DEBUG("Minimum index updated to ", uMinIndex);
					} else {
						const uint32_t u32LowerScancode = auScancodes[uCurrentIndex - 1];
						if (enteredScancode < u32LowerScancode) {
							uMaxIndex = uCurrentIndex - 1;
							PRINT_DEBUG("Maximum index updated to ", uMaxIndex);
						} else {
							uInsertionIndex = uCurrentIndex;
							return;
						}
					}
				}
				PRINT_DEBUG("Picked place for insertion at index ", uInsertionIndex);
			}
			for (unsigned uScancodeIndex = uNumberOfKeys; uScancodeIndex > uInsertionIndex; uScancodeIndex--) {
				const unsigned uLowerIndex = uScancodeIndex - 1;
				PRINT_DEBUG("Moving data at index ", uLowerIndex, " to ", uScancodeIndex, " in input buffers");
				auScancodes[uScancodeIndex] = auScancodes[uLowerIndex];
				const unsigned uKeyBufferIndex = uScancodeIndex + KEY_BUFFER_OFFSET,
					uKeyBufferLowerIndex = uLowerIndex + KEY_BUFFER_OFFSET;
				set_bits<keyboardbuffer_t>(
						aKeyBuffer[uKeyBufferIndex / KEY_BUFFER_CHUNK_SIZE],
						are_bits_true<keyboardbuffer_t>(aKeyBuffer[uKeyBufferLowerIndex / KEY_BUFFER_CHUNK_SIZE], uKeyBufferLowerIndex % KEY_BUFFER_CHUNK_SIZE),
						uKeyBufferIndex % KEY_BUFFER_CHUNK_SIZE);
				set_bits<keyboardbuffer_t>(
						aPrevKeyBuffer[uKeyBufferIndex / KEY_BUFFER_CHUNK_SIZE],
						are_bits_true<keyboardbuffer_t>(aPrevKeyBuffer[uKeyBufferLowerIndex / KEY_BUFFER_CHUNK_SIZE], uKeyBufferLowerIndex % KEY_BUFFER_CHUNK_SIZE),
						uKeyBufferIndex % KEY_BUFFER_CHUNK_SIZE);
			}
			for (unsigned uInputIndex = 0; uInputIndex < SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE; uInputIndex++)
				if (auInputToKeyBufferIndexTable[uInputIndex] >= uInsertionIndex + KEY_BUFFER_OFFSET) {
					PRINT_DEBUG("Incrementing index for input to key buffer-translation in table at index ", uInputIndex);
					auInputToKeyBufferIndexTable[uInputIndex]++;
				}
			PRINT_DEBUG("Inserting new scancode at index ", uInsertionIndex);
			auScancodes[uInsertionIndex] = enteredScancode;
			const unsigned uKeyBufferInsertionIndex = uInsertionIndex + KEY_BUFFER_OFFSET;
			if (is_key_input(eEnteredInput)) {
				const unsigned uInput = KEYBOARD_INPUT_TO_UINT(eEnteredInput);
				PRINT_DEBUG("Setting translation at index ", uInput, " to ", uKeyBufferInsertionIndex, " in input to key buffer-index table");
				auInputToKeyBufferIndexTable[uInput] = uKeyBufferInsertionIndex;
			}
			set_bits<keyboardbuffer_t>(aKeyBuffer[uKeyBufferInsertionIndex / KEY_BUFFER_CHUNK_SIZE], bPressed, uKeyBufferInsertionIndex % KEY_BUFFER_CHUNK_SIZE);
			set_bits<keyboardbuffer_t>(aPrevKeyBuffer[uKeyBufferInsertionIndex / KEY_BUFFER_CHUNK_SIZE], !bPressed, uKeyBufferInsertionIndex % KEY_BUFFER_CHUNK_SIZE);
			uNumberOfKeys++;
			RE_NOTE("Remaining slots for more undetected physical keys: ", MAXIMUM_PHYSICAL_KEYS - uNumberOfKeys);
		}
	}

#define FETCH_INDICES_PROC(KEY_INDEX_NAME, ARRAY_INDEX_NAME, BIT_INDEX_NAME) \
		unsigned KEY_INDEX_NAME = FAILURE_KEY_BUFFER_INDEX; \
		if (scancode != INVALID_SCANCODE) \
			KEY_INDEX_NAME = get_key_buffer_index_for_scancode(scancode); \
		if (KEY_INDEX_NAME == FAILURE_KEY_BUFFER_INDEX) \
			KEY_INDEX_NAME = get_key_buffer_index_for_input(eInput); \
		const unsigned ARRAY_INDEX_NAME = KEY_INDEX_NAME / KEY_BUFFER_CHUNK_SIZE, \
			BIT_INDEX_NAME = KEY_INDEX_NAME % KEY_BUFFER_CHUNK_SIZE

	bool is_key_down(Input eInput, scancode_t scancode) {
		PRINT_DEBUG("Getting present state of key ", std::hex, eInput, " and scancode ", scancode);
		FETCH_INDICES_PROC(keyIndex, arrayIndex, bitIndex);
		return are_bits_true<keyboardbuffer_t>(aKeyBuffer[arrayIndex], bitIndex);
	}
	
	bool was_key_down(Input eInput, scancode_t scancode) {
		PRINT_DEBUG("Getting past state of key ", std::hex, eInput, " and scancode ", scancode);
		FETCH_INDICES_PROC(keyIndex, arrayIndex, bitIndex);
		return are_bits_true<keyboardbuffer_t>(aPrevKeyBuffer[arrayIndex], bitIndex);
	}

	bool is_key_pressed(Input eInput, scancode_t scancode) {
		PRINT_DEBUG("Getting pressed state of key ", std::hex, eInput, " and scancode ", scancode);
		FETCH_INDICES_PROC(keyIndex, arrayIndex, bitIndex);
		const auto xKeyBufferBitmask = gen_bitmask<keyboardbuffer_t>(bitIndex);
		return (aKeyBuffer[arrayIndex] & xKeyBufferBitmask) != 0 && (aPrevKeyBuffer[arrayIndex] & xKeyBufferBitmask) == 0;
	}

	bool is_key_released(Input eInput, scancode_t scancode) {
		PRINT_DEBUG("Getting released state of key ", std::hex, eInput, " and scancode ", scancode);
		FETCH_INDICES_PROC(keyIndex, arrayIndex, bitIndex);
		const auto xKeyBufferBitmask = gen_bitmask<keyboardbuffer_t>(bitIndex);
		return (aKeyBuffer[arrayIndex] & xKeyBufferBitmask) == 0 && (aPrevKeyBuffer[arrayIndex] & xKeyBufferBitmask) != 0;
	}

	bool is_key_held_down(Input eInput, scancode_t scancode) {
		PRINT_DEBUG("Getting held state of key ", std::hex, eInput, " and scancode ", scancode);
		FETCH_INDICES_PROC(keyIndex, arrayIndex, bitIndex);
		return (aKeyBuffer[arrayIndex] & aPrevKeyBuffer[arrayIndex] & gen_bitmask<keyboardbuffer_t>(bitIndex)) != 0;
	}

#undef FETCH_INDICES_PROC

	Input map_scancode_to_input(scancode_t scancode) {
		if (scancode) {
			PRINT_DEBUG("Mapping key scancode ", std::hex, scancode, " to input");
			const unsigned uKeyBufferIndexForScancode = get_key_buffer_index_for_scancode(scancode);
			if (uKeyBufferIndexForScancode == FAILURE_KEY_BUFFER_INDEX) {
				PRINT_DEBUG("Key scancode is unknown. Mapping to input failed");
				return RE_INPUT_UNKNOWN;
			}
			for (unsigned uInputIndex = 0; uInputIndex < SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE; uInputIndex++) {
				PRINT_DEBUG("Checking alias input index ", uInputIndex);
				if (auInputToKeyBufferIndexTable[uInputIndex] == uKeyBufferIndexForScancode) {
					return static_cast<Input>(uInputIndex + RE_INPUT_KEY_FIRST);
				}
			}
			PRINT_DEBUG("No input found corresponding to scancode");
			return RE_INPUT_UNKNOWN;
		}
		RE_ERROR("Invalid scancode: ", std::hex, scancode);
		return RE_INPUT_UNKNOWN;
	}

	scancode_t map_input_to_scancode(Input eInput) {
		if (eInput >= RE_INPUT_KEY_FIRST && eInput <= RE_INPUT_KEY_LAST) {
			PRINT_DEBUG("Mapping input ", std::hex, eInput, " to scancode");
			const unsigned uIndex = auInputToKeyBufferIndexTable[KEYBOARD_INPUT_TO_UINT(eInput)];
			if (uIndex >= KEY_BUFFER_OFFSET) {
				return auScancodes[uIndex - KEY_BUFFER_OFFSET];
			}
			PRINT_DEBUG("No scancode registered to input yet");
			return INVALID_SCANCODE;
		}
		RE_NOTE("Invalid key input. It either is mouse input or the value is invalid in general: ", std::hex, eInput);
		return INVALID_SCANCODE;
	}

	void reset_keyboard_input() {
		PRINT_DEBUG("Resetting all key input buffers");
		std::fill(std::begin(aKeyBuffer), std::end(aKeyBuffer), 0);
		std::fill(std::begin(aPrevKeyBuffer), std::end(aPrevKeyBuffer), 0);
	}

}
