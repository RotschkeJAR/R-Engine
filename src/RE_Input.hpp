#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__ 1

#include "RE_Internal.hpp"

namespace RE {

	extern InputAction *pUpdateInputObject;
	
	void input_event(Input eEnteredInput, uint32_t u32EnteredScancode, bool bPressed, bool bFallbackToInput);
	void cursor_event(int32_t i32X, int32_t i32Y);
	void update_input_buffers();

	bool is_down(Input eInput, uint32_t u32Scancode = 0);
	bool was_down(Input eInput, uint32_t u32Scancode = 0);
	bool is_pressed(Input eInput, uint32_t u32Scancode = 0);
	bool is_released(Input eInput, uint32_t u32Scancode = 0);
	bool is_held_down(Input eInput, uint32_t u32Scancode = 0);

}

#endif /* __RE_INPUT_H__ */
