#ifndef __RE_INPUT_INTERNAL_H__
#define __RE_INPUT_INTERNAL_H__ 1

#include "RE_Input.hpp"

namespace RE {

#define MAXIMUM_PHYSICAL_KEYS                      150
#define FIRST_KEY_IN_INPUT                         RE_INPUT_KEY_SPACE
#define SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE    (RE_INPUT_MAX_ENUM - FIRST_KEY_IN_INPUT)
#define KEY_BUFFER_OFFSET                          1
#define KEY_BUFFER_SIZE                            ((MAXIMUM_PHYSICAL_KEYS + KEY_BUFFER_OFFSET) / 8 + ((MAXIMUM_PHYSICAL_KEYS + KEY_BUFFER_OFFSET) % 8 > 0 ? 1 : 0))

#define FAILURE_KEY_BUFFER_INDEX      0

	// Keyboard
	extern uint32_t au32Scancodes[MAXIMUM_PHYSICAL_KEYS];
	extern unsigned auInputToKeyBufferIndexTable[SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE];
	extern uint8_t au8KeyBuffer[KEY_BUFFER_SIZE],
		au8PrevKeyBuffer[KEY_BUFFER_SIZE];
	extern unsigned uNumberOfKeys;

	// Cursor
	extern uint8_t u8MouseBuffer,
		u8PrevMouseBuffer;
	extern Vector2i cursorPosition,
		prevCursorPosition;

#define CURSOR_INPUT_TO_UINT(INPUT)    static_cast<unsigned>(INPUT - RE_INPUT_SCROLL_UP)
#define KEYBOARD_INPUT_TO_UINT(INPUT)  static_cast<unsigned>(INPUT - FIRST_KEY_IN_INPUT)

}

#endif /* __RE_INPUT_INTERNAL_H__ */
