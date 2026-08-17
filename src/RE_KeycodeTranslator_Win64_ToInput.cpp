#include "RE_KeycodeTranslator_Internal.hpp"

namespace RE {

#ifdef RE_OS_WINDOWS

	Input key_from_virtual_win64_keycode(const WORD win_virtualKeyCode) {
		switch (win_virtualKeyCode) {
			case VK_SPACE:
				return RE_INPUT_KEY_SPACE;
			case VK_RETURN:
				return RE_INPUT_KEY_ENTER;
			case VK_BACK:
				return RE_INPUT_KEY_BACKSPACE;
			case VK_TAB:
				return RE_INPUT_KEY_TAB;
			case VK_CLEAR:
				return RE_INPUT_KEY_NUMPAD_5;
			case VK_PAUSE:
				return RE_INPUT_KEY_PAUSE;
			case VK_HOME:
				return RE_INPUT_KEY_HOME;
			case VK_DELETE:
				return RE_INPUT_KEY_DELETE;
			case VK_INSERT:
				return RE_INPUT_KEY_INSERT;
			case VK_ESCAPE:
				return RE_INPUT_KEY_ESCAPE;
			case VK_PRIOR:
				return RE_INPUT_KEY_PAGE_UP;
			case VK_NEXT:
				return RE_INPUT_KEY_PAGE_DOWN;
			case VK_END:
				return RE_INPUT_KEY_END;
			case VK_LEFT:
				return RE_INPUT_KEY_ARROW_LEFT;
			case VK_RIGHT:
				return RE_INPUT_KEY_ARROW_RIGHT;
			case VK_UP:
				return RE_INPUT_KEY_ARROW_UP;
			case VK_DOWN:
				return RE_INPUT_KEY_ARROW_DOWN;
			case VK_SNAPSHOT:
				return RE_INPUT_KEY_PRINT_SCREEN;
			case VK_MULTIPLY:
				return RE_INPUT_KEY_NUMPAD_MULTIPLY;
			case VK_ADD:
				return RE_INPUT_KEY_NUMPAD_ADD;
			case VK_SUBTRACT:
				return RE_INPUT_KEY_NUMPAD_SUBTRACT;
			case VK_DIVIDE:
				return RE_INPUT_KEY_NUMPAD_DIVIDE;
			case VK_SCROLL:
				return RE_INPUT_KEY_SCROLL_LOCK;
			case VK_LSHIFT:
				return RE_INPUT_KEY_SHIFT_LEFT;
			case VK_RSHIFT:
				return RE_INPUT_KEY_SHIFT_RIGHT;
			case VK_LCONTROL:
				return RE_INPUT_KEY_CTRL_LEFT;
			case VK_RCONTROL:
				return RE_INPUT_KEY_CTRL_RIGHT;
			case VK_LMENU:
				return RE_INPUT_KEY_ALT_LEFT;
			case VK_RMENU:
				return RE_INPUT_KEY_ALT_RIGHT;
			case VK_OEM_1:
				return RE_INPUT_KEY_SEMICOLON;
			case VK_OEM_2:
				return RE_INPUT_KEY_SLASH;
			case VK_OEM_3:
				return RE_INPUT_KEY_ACCENT;
			case VK_OEM_4:
				return RE_INPUT_KEY_BRACKET_LEFT;
			case VK_OEM_5:
				return RE_INPUT_KEY_BACKSLASH;
			case VK_OEM_6:
				return RE_INPUT_KEY_BRACKET_RIGHT;
			case VK_OEM_7:
				return RE_INPUT_KEY_APOSTROPHE;
			case VK_OEM_COMMA:
				return RE_INPUT_KEY_COMMA;
			case VK_OEM_PERIOD:
				return RE_INPUT_KEY_PERIOD;
			case VK_OEM_PLUS:
				return RE_INPUT_KEY_EQUALS;
			case VK_OEM_MINUS:
				return RE_INPUT_KEY_MINUS;
			case VK_SEPARATOR:
			case VK_DECIMAL:
				return RE_INPUT_KEY_NUMPAD_PERIOD;
			case VK_APPS:
				return RE_INPUT_KEY_MENU;
			default:
				break;
		}
		if (win_virtualKeyCode >= VK_A && win_virtualKeyCode <= VK_Z)
			return static_cast<Input>(win_virtualKeyCode - VK_A + static_cast<WORD>(RE_INPUT_KEY_A));
		if (win_virtualKeyCode >= VK_0 && win_virtualKeyCode <= VK_9)
			return static_cast<Input>(win_virtualKeyCode - VK_0 + static_cast<WORD>(RE_INPUT_KEY_0));
		if (win_virtualKeyCode >= VK_NUMPAD0 && win_virtualKeyCode <= VK_NUMPAD9)
			return static_cast<Input>(win_virtualKeyCode - VK_NUMPAD0 + static_cast<WORD>(RE_INPUT_KEY_NUMPAD_0));
		return RE_INPUT_UNKNOWN;
	}

#endif

}
