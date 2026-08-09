#include "RE_KeycodeTranslator_Internal.hpp"

namespace RE {

#ifdef RE_OS_WINDOWS

	WORD virtual_win64_keycode_from_key(const Input eKey) {
		switch (eKey) {
			case RE_INPUT_KEY_SPACE:
				return VK_SPACE;
			case RE_INPUT_KEY_BACKSPACE:
				return VK_BACK;
			case RE_INPUT_KEY_TAB:
				return VK_TAB;
			case RE_INPUT_KEY_NUMPAD_ENTER:
			case RE_INPUT_KEY_ENTER:
				return VK_RETURN;
			case RE_INPUT_KEY_PAUSE:
				return VK_PAUSE;
			case RE_INPUT_KEY_ESCAPE:
				return VK_ESCAPE;
			case RE_INPUT_KEY_PAGE_UP:
				return VK_PRIOR;
			case RE_INPUT_KEY_PAGE_DOWN:
				return VK_NEXT;
			case RE_INPUT_KEY_END:
				return VK_END;
			case RE_INPUT_KEY_HOME:
				return VK_HOME;
			case RE_INPUT_KEY_ARROW_LEFT:
				return VK_LEFT;
			case RE_INPUT_KEY_ARROW_RIGHT:
				return VK_RIGHT;
			case RE_INPUT_KEY_ARROW_UP:
				return VK_UP;
			case RE_INPUT_KEY_ARROW_DOWN:
				return VK_DOWN;
			case RE_INPUT_KEY_PRINT_SCREEN:
				return VK_SNAPSHOT;
			case RE_INPUT_KEY_INSERT:
				return VK_INSERT;
			case RE_INPUT_KEY_DELETE:
				return VK_DELETE;
			case RE_INPUT_KEY_NUMPAD_MULTIPLY:
				return VK_MULTIPLY;
			case RE_INPUT_KEY_NUMPAD_ADD:
				return VK_ADD;
			case RE_INPUT_KEY_NUMPAD_SUBTRACT:
				return VK_SUBTRACT;
			case RE_INPUT_KEY_NUMPAD_DIVIDE:
				return VK_DIVIDE;
			case RE_INPUT_KEY_NUMPAD_LOCK:
				return VK_NUMLOCK;
			case RE_INPUT_KEY_SCROLL_LOCK:
				return VK_SCROLL;
			case RE_INPUT_KEY_SHIFT_RIGHT:
				return VK_RSHIFT;
			case RE_INPUT_KEY_SHIFT_LEFT:
				return VK_LSHIFT;
			case RE_INPUT_KEY_CTRL_RIGHT:
				return VK_RCONTROL;
			case RE_INPUT_KEY_CTRL_LEFT:
				return VK_LCONTROL;
			case RE_INPUT_KEY_ALT_RIGHT:
				return VK_RMENU;
			case RE_INPUT_KEY_ALT_LEFT:
				return VK_LMENU;
			case RE_INPUT_KEY_SEMICOLON:
				return VK_OEM_1;
			case RE_INPUT_KEY_SLASH:
				return VK_OEM_2;
			case RE_INPUT_KEY_BRACKET_LEFT:
				return VK_OEM_4;
			case RE_INPUT_KEY_BRACKET_RIGHT:
				return VK_OEM_6;
			case RE_INPUT_KEY_BACKSLASH:
				return VK_OEM_5;
			case RE_INPUT_KEY_APOSTROPHE:
				return VK_OEM_7;
			case RE_INPUT_KEY_ACCENT:
				return VK_OEM_3;
			case RE_INPUT_KEY_COMMA:
				return VK_OEM_COMMA;
			case RE_INPUT_KEY_PERIOD:
				return VK_OEM_PERIOD;
			case RE_INPUT_KEY_EQUALS:
				return VK_OEM_PLUS;
			case RE_INPUT_KEY_MINUS:
				return VK_OEM_MINUS;
			case RE_INPUT_KEY_NUMPAD_PERIOD:
				return VK_DECIMAL;
			case RE_INPUT_KEY_MENU:
				return VK_APPS;
			case RE_INPUT_KEY_WORLD_1:
				return VK_OEM_102;
			default:
				break;
		}
		const WORD win64_keyId = static_cast<WORD>(eKey);
		if (win64_keyId >= static_cast<WORD>(RE_INPUT_KEY_A) && win64_keyId <= static_cast<WORD>(RE_INPUT_KEY_Z))
			return VK_A + (win64_keyId - static_cast<WORD>(RE_INPUT_KEY_A));
		if (win64_keyId >= static_cast<WORD>(RE_INPUT_KEY_0) && win64_keyId <= static_cast<WORD>(RE_INPUT_KEY_9))
			return VK_0 + (win64_keyId - static_cast<WORD>(RE_INPUT_KEY_0));
		if (win64_keyId >= static_cast<WORD>(RE_INPUT_KEY_NUMPAD_0) && win64_keyId <= static_cast<WORD>(RE_INPUT_KEY_NUMPAD_9))
			return VK_NUMPAD0 + (win64_keyId - static_cast<WORD>(RE_INPUT_KEY_NUMPAD_0));
		return 0;
	}

#endif

}
