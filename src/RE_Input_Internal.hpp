#ifndef __RE_INPUT_INTERNAL_H__
#define __RE_INPUT_INTERNAL_H__ 1

#include "RE_Input.hpp"

namespace RE {

	typedef unsigned keyboardbuffer_t;
#define KEY_BUFFER_CHUNK_SIZE                      UINT_WIDTH
#define MAXIMUM_PHYSICAL_KEYS                      150
#define SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE    (RE_INPUT_KEY_LAST - RE_INPUT_KEY_FIRST + 1)
#define KEY_BUFFER_OFFSET                          1
#define KEY_BUFFER_SIZE                            ((MAXIMUM_PHYSICAL_KEYS + KEY_BUFFER_OFFSET) / KEY_BUFFER_CHUNK_SIZE + ((MAXIMUM_PHYSICAL_KEYS + KEY_BUFFER_OFFSET) % KEY_BUFFER_CHUNK_SIZE > 0 ? 1 : 0))
#define FAILURE_KEY_BUFFER_INDEX                   0

	typedef unsigned cursorbuffer_t;
#define CURSOR_BUFFER_CHUNK_SIZE       UINT_WIDTH

#define CURSOR_DIMENSIONS  2
#define CURSOR_X           0
#define CURSOR_Y           1

#define SCROLLING_DIMENSIONS  2
#define SCROLLING_HORIZONTAL  0
#define SCROLLING_VERTICAL    1

	// Keyboard
	extern scancode_t aScancodes[MAXIMUM_PHYSICAL_KEYS];
	extern unsigned auInputToKeyBufferIndexTable[SIZE_OF_INPUT_TO_KEY_BUFFER_INDEX_TABLE];
	extern keyboardbuffer_t aKeyBuffer[KEY_BUFFER_SIZE],
		aPrevKeyBuffer[KEY_BUFFER_SIZE];
	extern unsigned uNumberOfKeys;
	bool init_input_keyboard();
	void destroy_input_keyboard();
	void update_keyboard_buffers();
	unsigned get_key_buffer_index_for_scancode(scancode_t searchedScancode);
	unsigned get_key_buffer_index_for_input(Input eSearchedInput);

	// Cursor
	extern cursorbuffer_t cursorBuffer,
		prevCursorBuffer;
	extern int aiScrolling[SCROLLING_DIMENSIONS],
		aiPrevScrolling[SCROLLING_DIMENSIONS];
	extern int aiCursorPosition[CURSOR_DIMENSIONS],
		aiPrevCursorPosition[CURSOR_DIMENSIONS];
	bool init_input_cursor();
	void destroy_input_cursor();
	void update_cursor_buffers();

#define SCROLL_INPUT_TO_UINT(INPUT)    static_cast<unsigned>(INPUT - RE_INPUT_SCROLL_FIRST)
#define BUTTON_INPUT_TO_UINT(INPUT)    static_cast<cursorbuffer_t>(INPUT - RE_INPUT_BUTTON_FIRST)
#define KEYBOARD_INPUT_TO_UINT(INPUT)  static_cast<keyboardbuffer_t>(INPUT - RE_INPUT_KEY_FIRST)

}

#endif /* __RE_INPUT_INTERNAL_H__ */
