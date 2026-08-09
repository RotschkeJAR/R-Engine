#include "RE_KeycodeTranslator_Internal.hpp"

namespace RE {

#ifdef RE_OS_LINUX

	xkb_keysym_t virtual_xkb_keysym_from_key(const Input eInput) {
		switch (eInput) {
			case RE_INPUT_KEY_SPACE:
				return XKB_KEY_space;
			case RE_INPUT_KEY_SLASH:
				return XKB_KEY_slash;
			case RE_INPUT_KEY_BACKSLASH:
				return XKB_KEY_backslash;
			case RE_INPUT_KEY_COMMA:
				return XKB_KEY_comma;
			case RE_INPUT_KEY_PERIOD:
				return XKB_KEY_period;
			case RE_INPUT_KEY_SEMICOLON:
				return XKB_KEY_semicolon;
			case RE_INPUT_KEY_APOSTROPHE:
				return XKB_KEY_apostrophe;
			case RE_INPUT_KEY_ACCENT:
				return XKB_KEY_grave;
			case RE_INPUT_KEY_BRACKET_LEFT:
				return XKB_KEY_bracketleft;
			case RE_INPUT_KEY_BRACKET_RIGHT:
				return XKB_KEY_bracketright;
			case RE_INPUT_KEY_EQUALS:
				return XKB_KEY_equal;
			case RE_INPUT_KEY_MINUS:
				return XKB_KEY_minus;
			case RE_INPUT_KEY_PLUS:
				return XKB_KEY_plus;
			case RE_INPUT_KEY_CTRL_RIGHT:
				return XKB_KEY_Control_R;
			case RE_INPUT_KEY_CTRL_LEFT:
				return XKB_KEY_Control_L;
			case RE_INPUT_KEY_ALT_RIGHT:
				return XKB_KEY_Alt_R;
			case RE_INPUT_KEY_ALT_LEFT:
				return XKB_KEY_Alt_L;
			case RE_INPUT_KEY_SHIFT_RIGHT:
				return XKB_KEY_Shift_R;
			case RE_INPUT_KEY_SHIFT_LEFT:
				return XKB_KEY_Shift_L;
			case RE_INPUT_KEY_MENU:
				return XKB_KEY_Menu;
			case RE_INPUT_KEY_TAB:
				return XKB_KEY_Tab;
			case RE_INPUT_KEY_ENTER:
				return XKB_KEY_Return;
			case RE_INPUT_KEY_ESCAPE:
				return XKB_KEY_Escape;
			case RE_INPUT_KEY_BACKSPACE:
				return XKB_KEY_BackSpace;
			case RE_INPUT_KEY_ARROW_LEFT:
				return XKB_KEY_Left;
			case RE_INPUT_KEY_ARROW_RIGHT:
				return XKB_KEY_Right;
			case RE_INPUT_KEY_ARROW_UP:
				return XKB_KEY_Up;
			case RE_INPUT_KEY_ARROW_DOWN:
				return XKB_KEY_Down;
			case RE_INPUT_KEY_DELETE:
				return XKB_KEY_Delete;
			case RE_INPUT_KEY_INSERT:
				return XKB_KEY_Insert;
			case RE_INPUT_KEY_HOME:
				return XKB_KEY_Home;
			case RE_INPUT_KEY_END:
				return XKB_KEY_End;
			case RE_INPUT_KEY_BEGIN:
				return XKB_KEY_Begin;
			case RE_INPUT_KEY_PRINT_SCREEN:
				return XKB_KEY_Print;
			case RE_INPUT_KEY_SCROLL_LOCK:
				return XKB_KEY_Scroll_Lock;
			case RE_INPUT_KEY_PAUSE:
				return XKB_KEY_Pause;
			case RE_INPUT_KEY_PAGE_UP:
				return XKB_KEY_Page_Up;
			case RE_INPUT_KEY_PAGE_DOWN:
				return XKB_KEY_Page_Down;
			case RE_INPUT_KEY_NUMPAD_ADD:
				return XKB_KEY_KP_Add;
			case RE_INPUT_KEY_NUMPAD_SUBTRACT:
				return XKB_KEY_KP_Subtract;
			case RE_INPUT_KEY_NUMPAD_MULTIPLY:
				return XKB_KEY_KP_Multiply;
			case RE_INPUT_KEY_NUMPAD_DIVIDE:
				return XKB_KEY_KP_Divide;
			case RE_INPUT_KEY_NUMPAD_ENTER:
				return XKB_KEY_KP_Enter;
			case RE_INPUT_KEY_NUMPAD_PERIOD:
				return XKB_KEY_KP_Decimal;
			case RE_INPUT_KEY_LESS:
				return XKB_KEY_less;
			case RE_INPUT_KEY_PARENTHESIS_LEFT:
				return XKB_KEY_parenleft;
			case RE_INPUT_KEY_PARENTHESIS_RIGHT:
				return XKB_KEY_parenright;
			case RE_INPUT_KEY_SHARP_S:
				return XKB_KEY_ssharp;
			default:
				break;
		}
		const xkb_keysym_t xkb_keyId = static_cast<xkb_keysym_t>(eInput);
		if (xkb_keyId >= static_cast<xkb_keysym_t>(RE_INPUT_KEY_A) && xkb_keyId <= static_cast<xkb_keysym_t>(RE_INPUT_KEY_Z))
			return XKB_KEY_a + (xkb_keyId - static_cast<xkb_keysym_t>(RE_INPUT_KEY_A));
		if (xkb_keyId >= static_cast<xkb_keysym_t>(RE_INPUT_KEY_0) && xkb_keyId <= static_cast<xkb_keysym_t>(RE_INPUT_KEY_9))
			return XKB_KEY_0 + (xkb_keyId - static_cast<xkb_keysym_t>(RE_INPUT_KEY_0));
		if (xkb_keyId >= static_cast<xkb_keysym_t>(RE_INPUT_KEY_NUMPAD_0) && xkb_keyId <= static_cast<xkb_keysym_t>(RE_INPUT_KEY_NUMPAD_9))
			return XKB_KEY_KP_0 + (xkb_keyId - static_cast<xkb_keysym_t>(RE_INPUT_KEY_NUMPAD_0));
		return XKB_KEY_NoSymbol;
	}

#endif

}
