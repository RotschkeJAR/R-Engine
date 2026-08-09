#include "RE_KeycodeTranslator_Internal.hpp"

namespace RE {

#ifdef RE_OS_LINUX

	KeySym virtual_x11_keycode_from_key(const Input eKey) {
		switch (eKey) {
			case RE_INPUT_KEY_SPACE:
				return XK_space;
			case RE_INPUT_KEY_SLASH:
				return XK_slash;
			case RE_INPUT_KEY_BACKSLASH:
				return XK_backslash;
			case RE_INPUT_KEY_COMMA:
				return XK_comma;
			case RE_INPUT_KEY_PERIOD:
				return XK_period;
			case RE_INPUT_KEY_SEMICOLON:
				return XK_semicolon;
			case RE_INPUT_KEY_APOSTROPHE:
				return XK_apostrophe;
			case RE_INPUT_KEY_ACCENT:
				return XK_grave;
			case RE_INPUT_KEY_BRACKET_LEFT:
				return XK_bracketleft;
			case RE_INPUT_KEY_BRACKET_RIGHT:
				return XK_bracketright;
			case RE_INPUT_KEY_EQUALS:
				return XK_equal;
			case RE_INPUT_KEY_MINUS:
				return XK_minus;
			case RE_INPUT_KEY_PLUS:
				return XK_plus;
			case RE_INPUT_KEY_CTRL_RIGHT:
				return XK_Control_R;
			case RE_INPUT_KEY_CTRL_LEFT:
				return XK_Control_L;
			case RE_INPUT_KEY_ALT_RIGHT:
				return XK_Alt_R;
			case RE_INPUT_KEY_ALT_LEFT:
				return XK_Alt_L;
			case RE_INPUT_KEY_SHIFT_RIGHT:
				return XK_Shift_R;
			case RE_INPUT_KEY_SHIFT_LEFT:
				return XK_Shift_L;
			case RE_INPUT_KEY_TAB:
				return XK_Tab;
			case RE_INPUT_KEY_ENTER:
				return XK_Return;
			case RE_INPUT_KEY_ESCAPE:
				return XK_Escape;
			case RE_INPUT_KEY_BACKSPACE:
				return XK_BackSpace;
			case RE_INPUT_KEY_ARROW_LEFT:
				return XK_Left;
			case RE_INPUT_KEY_ARROW_UP:
				return XK_Up;
			case RE_INPUT_KEY_ARROW_DOWN:
				return XK_Down;
			case RE_INPUT_KEY_ARROW_RIGHT:
				return XK_Right;
			case RE_INPUT_KEY_DELETE:
				return XK_Delete;
			case RE_INPUT_KEY_INSERT:
				return XK_Insert;
			case RE_INPUT_KEY_HOME:
				return XK_Home;
			case RE_INPUT_KEY_END:
				return XK_End;
			case RE_INPUT_KEY_BEGIN:
				return XK_Begin;
			case RE_INPUT_KEY_PRINT_SCREEN:
				return XK_Print;
			case RE_INPUT_KEY_SCROLL_LOCK:
				return XK_Scroll_Lock;
			case RE_INPUT_KEY_PAUSE:
				return XK_Pause;
			case RE_INPUT_KEY_PAGE_UP:
				return XK_Page_Up;
			case RE_INPUT_KEY_PAGE_DOWN:
				return XK_Page_Down;
			case RE_INPUT_KEY_NUMPAD_ADD:
				return XK_KP_Add;
			case RE_INPUT_KEY_NUMPAD_SUBTRACT:
				return XK_KP_Subtract;
			case RE_INPUT_KEY_NUMPAD_MULTIPLY:
				return XK_KP_Multiply;
			case RE_INPUT_KEY_NUMPAD_DIVIDE:
				return XK_KP_Divide;
			case RE_INPUT_KEY_NUMPAD_ENTER:
				return XK_KP_Enter;
			case RE_INPUT_KEY_NUMPAD_PERIOD:
				return XK_KP_Decimal;
			case RE_INPUT_KEY_MENU:
				return XK_Menu;
			case RE_INPUT_KEY_LESS:
				return XK_less;
			case RE_INPUT_KEY_GREATER:
				return XK_greater;
			case RE_INPUT_KEY_BAR:
				return XK_bar;
			case RE_INPUT_KEY_PARENTHESIS_RIGHT:
				return XK_parenright;
			case RE_INPUT_KEY_PARENTHESIS_LEFT:
				return XK_parenleft;
			case RE_INPUT_KEY_SHARP_S:
				return XK_ssharp;
			default:
				break;
		}
		const KeySym x11_keyId = static_cast<KeySym>(eKey);
		if (x11_keyId >= static_cast<KeySym>(RE_INPUT_KEY_A) && x11_keyId <= static_cast<KeySym>(RE_INPUT_KEY_Z))
			return XK_a + (x11_keyId - static_cast<KeySym>(RE_INPUT_KEY_A));
		if (x11_keyId >= static_cast<KeySym>(RE_INPUT_KEY_0) && x11_keyId <= static_cast<KeySym>(RE_INPUT_KEY_9))
			return XK_0 + (x11_keyId - static_cast<KeySym>(RE_INPUT_KEY_0));
		if (x11_keyId >= static_cast<KeySym>(RE_INPUT_KEY_NUMPAD_0) && x11_keyId <= static_cast<KeySym>(RE_INPUT_KEY_NUMPAD_9))
			return XK_KP_0 + (x11_keyId - static_cast<KeySym>(RE_INPUT_KEY_NUMPAD_0));
		return XK_VoidSymbol;
	}

#endif

}
