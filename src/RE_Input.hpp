#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__

#include "RE_Internal Header.hpp"
#include "RE_Keycode Translator.hpp"

namespace RE {

#define MAXIMUM_PHYSICAL_KEYS 150
#define KEY_BUFFER_SIZE (MAXIMUM_PHYSICAL_KEYS / 8 + (MAXIMUM_PHYSICAL_KEYS % 8 > 0 ? 1 : 0))

	extern InputAction *pUpdateInputObject;
	
	class InputMgr final {
		public:
			static InputMgr *pInstance;

			InputMgr();
			~InputMgr();
			void input_event(const Input eEnteredInput, const uint32_t u32EnteredScancode, const bool bPressed, const bool bFallbackToInput);
			void cursor_event(int32_t i32X, int32_t i32Y);
			void update_input_buffers();
	};

	bool is_key_down(const Input eInput, const uint32_t u32Scancode);
	bool was_key_down(const Input eInput, const uint32_t u32Scancode);

	int32_t get_cursor_position_x();
	int32_t get_cursor_position_y();
	float get_cursor_normal_position_x();
	float get_cursor_normal_position_y();

	Input map_scancode_to_input(const uint32_t u32Scancode);
	uint32_t map_input_to_scancode(const Input eInput);

}

#endif /* __RE_INPUT_H__ */
