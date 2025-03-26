#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__

#include "RE_Internal Header.hpp"
#include "RE_Keycode Translator.hpp"

namespace RE {

#define MAXIMUM_PHYSICAL_KEYS 150
#define KEY_BUFFER_SIZE (MAXIMUM_PHYSICAL_KEYS / 8 + (MAXIMUM_PHYSICAL_KEYS % 8 > 0 ? 1 : 0))
	
	class InputMgr {
		private:
			REuint u32Scancodes[MAXIMUM_PHYSICAL_KEYS];
			Input eInputs[MAXIMUM_PHYSICAL_KEYS];
			REubyte u8KeyBuffer[KEY_BUFFER_SIZE], u8PrevKeyBuffer[KEY_BUFFER_SIZE]; // Keyboard
			REubyte u8NumberOfKeys;
			REubyte u8SpecialInputBuffer, u8PrevSpecialInputBuffer; // Scrolling and mouse buttons
			Vector2i cursorPosition, prevCursorPosition;

			REshort get_index_for_scancode(REuint u32Scancode);
			REshort get_index_for_input(Input eSearchedInput);
			bool process_request(Input &eInput, REuint &u32Scancode, bool bRequestForPast);

		public:
			static InputMgr *pInstance;
			InputAction *pUpdateInputObject;

			InputMgr();
			~InputMgr();
			void input_event(const Input eInput, const REuint u32Scancode, const bool bPressed, const bool bFallbackToInput);
			void cursor_event(REint i32X, REint i32Y);
			void update_input_buffers();
			Input map_scancode_to_input(REuint u32Scancode);
			REuint map_input_to_scancode(Input eInput);
			bool is_down(Input &eInput, REuint &u32Scancode);
			bool was_down(Input &eInput, REuint &u32Scancode);
	};

	Input map_scancode_to_input(REuint u32Scancode);
	REuint map_input_to_scancode(Input eInput);

}

#endif /* __RE_INPUT_H__ */
