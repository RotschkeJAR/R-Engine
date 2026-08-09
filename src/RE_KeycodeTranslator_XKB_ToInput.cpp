#include "RE_KeycodeTranslator_Internal.hpp"

namespace RE {

#ifdef RE_OS_LINUX

	Input key_from_virtual_xkb_keysym(const xkb_keysym_t xkb_keySym) {
		switch (xkb_keySym) {
			case XKB_KEY_BackSpace:
				return RE_INPUT_KEY_BACKSPACE;
			case XKB_KEY_Tab:
				return RE_INPUT_KEY_TAB;
			case XKB_KEY_Return:
				return RE_INPUT_KEY_ENTER;
			case XKB_KEY_Pause:
				return RE_INPUT_KEY_PAUSE;
			case XKB_KEY_Scroll_Lock:
				return RE_INPUT_KEY_SCROLL_LOCK;
			case XKB_KEY_Escape:
				return RE_INPUT_KEY_ESCAPE;
			case XKB_KEY_Delete:
				return RE_INPUT_KEY_DELETE;
			case XKB_KEY_Home:
				return RE_INPUT_KEY_HOME;
			case XKB_KEY_Left:
				return RE_INPUT_KEY_ARROW_LEFT;
			case XKB_KEY_Up:
				return RE_INPUT_KEY_ARROW_UP;
			case XKB_KEY_Right:
				return RE_INPUT_KEY_ARROW_RIGHT;
			case XKB_KEY_Down:
				return RE_INPUT_KEY_ARROW_DOWN;
			case XKB_KEY_Page_Up:
				return RE_INPUT_KEY_PAGE_UP;
			case XKB_KEY_Page_Down:
				return RE_INPUT_KEY_PAGE_DOWN;
			case XKB_KEY_End:
				return RE_INPUT_KEY_END;
			case XKB_KEY_Begin:
				return RE_INPUT_KEY_BEGIN;
			case XKB_KEY_Print:
				return RE_INPUT_KEY_PRINT_SCREEN;
			case XKB_KEY_Menu:
				return RE_INPUT_KEY_MENU;
			case XKB_KEY_KP_Enter:
				return RE_INPUT_KEY_NUMPAD_ENTER;
			case XKB_KEY_KP_4:
			case XKB_KEY_KP_Left:
				return RE_INPUT_KEY_NUMPAD_4;
			case XKB_KEY_KP_8:
			case XKB_KEY_KP_Up:
				return RE_INPUT_KEY_NUMPAD_8;
			case XKB_KEY_KP_6:
			case XKB_KEY_KP_Right:
				return RE_INPUT_KEY_NUMPAD_6;
			case XKB_KEY_KP_2:
			case XKB_KEY_KP_Down:
				return RE_INPUT_KEY_NUMPAD_2;
			case XKB_KEY_KP_9:
			case XKB_KEY_KP_Page_Up:
				return RE_INPUT_KEY_NUMPAD_9;
			case XKB_KEY_KP_3:
			case XKB_KEY_KP_Page_Down:
				return RE_INPUT_KEY_NUMPAD_3;
			case XKB_KEY_KP_1:
			case XKB_KEY_KP_End:
				return RE_INPUT_KEY_NUMPAD_1;
			case XKB_KEY_KP_7:
			case XKB_KEY_KP_Begin:
				return RE_INPUT_KEY_NUMPAD_7;
			case XKB_KEY_KP_5:
				return RE_INPUT_KEY_NUMPAD_5;
			case XKB_KEY_KP_0:
			case XKB_KEY_KP_Insert:
				return RE_INPUT_KEY_NUMPAD_0;
			case XKB_KEY_KP_Decimal:
			case XKB_KEY_KP_Delete:
				return RE_INPUT_KEY_NUMPAD_PERIOD;
			case XKB_KEY_KP_Multiply:
				return RE_INPUT_KEY_NUMPAD_MULTIPLY;
			case XKB_KEY_KP_Add:
				return RE_INPUT_KEY_NUMPAD_ADD;
			case XKB_KEY_KP_Subtract:
				return RE_INPUT_KEY_NUMPAD_SUBTRACT;
			case XKB_KEY_KP_Divide:
				return RE_INPUT_KEY_NUMPAD_DIVIDE;
			case XKB_KEY_Shift_L:
				return RE_INPUT_KEY_SHIFT_LEFT;
			case XKB_KEY_Shift_R:
				return RE_INPUT_KEY_SHIFT_RIGHT;
			case XKB_KEY_Control_L:
				return RE_INPUT_KEY_CTRL_LEFT;
			case XKB_KEY_Control_R:
				return RE_INPUT_KEY_CTRL_RIGHT;
			case XKB_KEY_Alt_L:
				return RE_INPUT_KEY_ALT_LEFT;
			case XKB_KEY_Alt_R:
				return RE_INPUT_KEY_ALT_RIGHT;
			case XKB_KEY_space:
				return RE_INPUT_KEY_SPACE;
			case XKB_KEY_minus:
				return RE_INPUT_KEY_MINUS;
			case XKB_KEY_apostrophe:
				return RE_INPUT_KEY_APOSTROPHE;
			case XKB_KEY_semicolon:
				return RE_INPUT_KEY_SEMICOLON;
			case XKB_KEY_comma:
				return RE_INPUT_KEY_COMMA;
			case XKB_KEY_period:
				return RE_INPUT_KEY_PERIOD;
			case XKB_KEY_slash:
				return RE_INPUT_KEY_SLASH;
			case XKB_KEY_bar:
				return RE_INPUT_KEY_BAR;
			case XKB_KEY_greater:
				return RE_INPUT_KEY_GREATER;
			case XKB_KEY_less:
				return RE_INPUT_KEY_LESS;
			case XKB_KEY_plus:
				return RE_INPUT_KEY_PLUS;
			case XKB_KEY_equal:
				return RE_INPUT_KEY_EQUALS;
			case XKB_KEY_bracketleft:
				return RE_INPUT_KEY_BRACKET_LEFT;
			case XKB_KEY_backslash:
				return RE_INPUT_KEY_BACKSLASH;
			case XKB_KEY_bracketright:
				return RE_INPUT_KEY_BRACKET_RIGHT;
			case XKB_KEY_grave:
				return RE_INPUT_KEY_ACCENT;
			case XKB_KEY_parenleft:
				return RE_INPUT_KEY_PARENTHESIS_LEFT;
			case XKB_KEY_parenright:
				return RE_INPUT_KEY_PARENTHESIS_RIGHT;
			case XKB_KEY_ssharp:
				return RE_INPUT_KEY_SHARP_S;
			default:
				break;
		}
		if (xkb_keySym >= XKB_KEY_a && xkb_keySym <= XKB_KEY_z)
			return static_cast<Input>(xkb_keySym - XKB_KEY_a + static_cast<xkb_keysym_t>(RE_INPUT_KEY_A));
		if (xkb_keySym >= XKB_KEY_A && xkb_keySym <= XKB_KEY_Z)
			return static_cast<Input>(xkb_keySym - XKB_KEY_A + static_cast<xkb_keysym_t>(RE_INPUT_KEY_A));
		if (xkb_keySym >= XKB_KEY_0 && xkb_keySym <= XKB_KEY_9)
			return static_cast<Input>(xkb_keySym - XKB_KEY_0 + static_cast<xkb_keysym_t>(RE_INPUT_KEY_0));
		return RE_INPUT_UNKNOWN;
	}

#endif

}
