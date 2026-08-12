#include "RE_Input_Internal.hpp"

namespace RE {

	uint32_t au32Scancodes[MAXIMUM_PHYSICAL_KEYS] = {};
	unsigned auInputToKeyBufferIndexTable[SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE] = {};
	uint8_t au8KeyBuffer[KEY_BUFFER_SIZE] = {},
		au8PrevKeyBuffer[KEY_BUFFER_SIZE] = {};
	unsigned uNumberOfKeys = 0;

	template <AskingState eStateToCheck>
	static bool get_state_of_user_input(const Input eInput, const uint32_t u32Scancode) {
		switch (eInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE:
				PRINT_DEBUG("Querying input state of mouse input ", std::hex, eInput);
				if constexpr (eStateToCheck == ASKING_STATE_PRESENT)
					return are_bits_true<decltype(u8MouseBuffer[0])>(u8MouseBuffer, mouse_input_to_uint8(eInput));
				else if constexpr (eStateToCheck == ASKING_STATE_PAST)
					return are_bits_true<decltype(u8PrevMouseBuffer[0])>(u8PrevMouseBuffer, mouse_input_to_uint8(eInput));
				else {
					const auto xMouseBufferBitmask = gen_bitmask<decltype(u8MouseBuffer[0])>(mouse_input_to_uint8(eInput));
					PRINT_DEBUG("Generated mouse buffer bitmask ", std::hex, xMouseBufferBitmask, " to fetch data from multiple buffers");
					if constexpr (eStateToCheck == ASKING_STATE_BOTH)
						return (u8MouseBuffer & u8PrevMouseBuffer & xMouseBufferBitmask) != 0;
					else if constexpr (eStateToCheck == ASKING_STATE_PRESSED)
						return (u8MouseBuffer & xMouseBufferBitmask) != 0 && (u8PrevMouseBuffer & xMouseBufferBitmask) == 0;
					else if constexpr (eStateToCheck == ASKING_STATE_RELEASED)
						return (u8MouseBuffer & xMouseBufferBitmask) == 0 && (u8PrevMouseBuffer & xMouseBufferBitmask) != 0;
				}
			default:
				unsigned uKeyIndex = FAILURE_KEY_BUFFER_INDEX;
				if (u32Scancode)
					uKeyIndex = get_key_buffer_index_for_scancode(u32Scancode);
				if (uKeyIndex == FAILURE_KEY_BUFFER_INDEX)
					uKeyIndex = get_key_buffer_index_for_input(eInput);
				const unsigned uKeyBufferArrayIndex = uKeyIndex / 8,
					uKeyBufferBitIndex = uKeyIndex % 8;
				PRINT_DEBUG("Querying input state at array index ", uKeyBufferArrayIndex, " and bit index ", uKeyBufferBitIndex);
				if constexpr (eStateToCheck == ASKING_STATE_PRESENT)
					return are_bits_true<decltype(au8KeyBuffer[0])>(au8KeyBuffer[uKeyBufferArrayIndex], uKeyBufferBitIndex);
				else if constexpr (eStateToCheck == ASKING_STATE_PAST)
					return are_bits_true<decltype(au8PrevKeyBuffer[0])>(au8PrevKeyBuffer[uKeyBufferArrayIndex], uKeyBufferBitIndex);
				else {
					const auto xKeyBufferBitmask = gen_bitmask<decltype(au8KeyBuffer[0])>(uKeyBufferBitIndex);
					PRINT_DEBUG("Generated key buffer bitmask ", std::hex, xKeyBufferBitmask, " to fetch data from multiple buffers");
					if constexpr (eStateToCheck == ASKING_STATE_BOTH)
						return (au8KeyBuffer[uKeyBufferArrayIndex] & au8PrevKeyBuffer[uKeyBufferArrayIndex] & xKeyBufferBitmask) != 0;
					else if constexpr (eStateToCheck == ASKING_STATE_PRESSED)
						return (au8KeyBuffer[uKeyBufferArrayIndex] & xKeyBufferBitmask) != 0 && (au8PrevKeyBuffer[uKeyBufferArrayIndex] & xKeyBufferBitmask) == 0;
					else if constexpr (eStateToCheck == ASKING_STATE_RELEASED)
						return (au8KeyBuffer[uKeyBufferArrayIndex] & xKeyBufferBitmask) == 0 && (au8PrevKeyBuffer[uKeyBufferArrayIndex] & xKeyBufferBitmask) != 0;
				}
		}
		PRINT_DEBUG("Input ", std::hex, eInput, " and scancode ", u32Scancode, " didn't match with any data to get input state");
		return false;
	}

	bool is_down(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Getting present state of input ", std::hex, eInput, " and scancode ", u32Scancode);
		return get_state_of_user_input<ASKING_STATE_PRESENT>(eInput, u32Scancode);
	}
	
	bool was_down(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Getting past state of input ", std::hex, eInput, " and scancode ", u32Scancode);
		return get_state_of_user_input<ASKING_STATE_PAST>(eInput, u32Scancode);
	}

	bool is_pressed(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Getting pressed state of input ", std::hex, eInput, " and scancode ", u32Scancode);
		return get_state_of_user_input<ASKING_STATE_PRESSED>(eInput, u32Scancode);
	}

	bool is_released(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Getting released state of input ", std::hex, eInput, " and scancode ", u32Scancode);
		return get_state_of_user_input<ASKING_STATE_RELEASED>(eInput, u32Scancode);
	}

	bool is_held_down(const Input eInput, const uint32_t u32Scancode) {
		PRINT_DEBUG("Getting held state of input ", std::hex, eInput, " and scancode ", u32Scancode);
		return get_state_of_user_input<ASKING_STATE_BOTH>(eInput, u32Scancode);
	}

	void reset_keyboard_input() {
		PRINT_DEBUG("Resetting all key input buffers");
		std::fill(std::begin(au8KeyBuffer), std::end(au8KeyBuffer), 0);
		std::fill(std::begin(au8PrevKeyBuffer), std::end(au8PrevKeyBuffer), 0);
	}

}
