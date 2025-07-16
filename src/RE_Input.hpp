#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__

#include "RE_Internal Header.hpp"
#include "RE_Keycode Translator.hpp"

namespace RE {

	extern InputAction *pUpdateInputObject;
	
	void input_event(Input eEnteredInput, uint32_t u32EnteredScancode, bool bPressed, bool bFallbackToInput);
	void cursor_event(int32_t i32X, int32_t i32Y);
	void update_input_buffers();
	[[nodiscard]]
	bool is_key_down(Input eInput, uint32_t u32Scancode);
	[[nodiscard]]
	bool was_key_down(Input eInput, uint32_t u32Scancode);
	void reset_input_at(Input eInput, uint32_t u32Scancode);

}

#endif /* __RE_INPUT_H__ */
