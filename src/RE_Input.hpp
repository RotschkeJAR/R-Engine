#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__

#include "RE_Internal Header.hpp"
#include "RE_Keycode Translator.hpp"

namespace RE {

#define MAXIMUM_PHYSICAL_KEYS 150
#define KEY_BUFFER_SIZE (MAXIMUM_PHYSICAL_KEYS / 8 + (MAXIMUM_PHYSICAL_KEYS % 8 > 0 ? 1 : 0))

	extern InputAction *pUpdateInputObject;
	
	class InputMgr final {
		private:
			REshort get_index_for_scancode(REuint u32Scancode);
			REshort get_index_for_input(Input eSearchedInput);
			bool process_request(Input eInput, REuint u32Scancode, bool bRequestForPast);

		public:
			static InputMgr *pInstance;

			InputMgr();
			~InputMgr();
			void input_event(const Input eEnteredInput, const REuint u32EnteredScancode, const bool bPressed, const bool bFallbackToInput);
			void cursor_event(REint i32X, REint i32Y);
			void update_input_buffers();
			Input map_scancode_to_input(REuint u32Scancode);
			REuint map_input_to_scancode(Input eInput);
			bool is_down(Input eInput, REuint u32Scancode);
			bool was_down(Input eInput, REuint u32Scancode);
			REint get_cursor_x();
			REint get_cursor_y();
			Vector2i get_cursor();
	};

	int32_t get_cursor_position_x();
	int32_t get_cursor_position_y();
	float get_cursor_normal_position_x();
	float get_cursor_normal_position_y();

	Input map_scancode_to_input(REuint u32Scancode);
	REuint map_input_to_scancode(Input eInput);

}

#endif /* __RE_INPUT_H__ */
