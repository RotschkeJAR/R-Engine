#include "RE_KeycodeTranslator_Internal.hpp"

namespace RE {

#ifdef RE_OS_LINUX

	Input key_from_virtual_x11_keycode(const KeySym x11_virtualKeyCode) {
		switch (x11_virtualKeyCode) {
			case XK_space:
				return RE_INPUT_KEY_SPACE;
			case XK_BackSpace:
				return RE_INPUT_KEY_BACKSPACE;
			case XK_Tab:
				return RE_INPUT_KEY_TAB;
			case XK_Return:
				return RE_INPUT_KEY_ENTER;
			case XK_Pause:
				return RE_INPUT_KEY_PAUSE;
			case XK_Scroll_Lock:
				return RE_INPUT_KEY_SCROLL_LOCK;
			case XK_Escape:
				return RE_INPUT_KEY_ESCAPE;
			case XK_Delete:
				return RE_INPUT_KEY_DELETE;
			case XK_Home:
				return RE_INPUT_KEY_HOME;
			case XK_Left:
				return RE_INPUT_KEY_ARROW_LEFT;
			case XK_Up:
				return RE_INPUT_KEY_ARROW_UP;
			case XK_Right:
				return RE_INPUT_KEY_ARROW_RIGHT;
			case XK_Down:
				return RE_INPUT_KEY_ARROW_DOWN;
			case XK_Page_Up:
				return RE_INPUT_KEY_PAGE_UP;
			case XK_Page_Down:
				return RE_INPUT_KEY_PAGE_DOWN;
			case XK_Insert:
				return RE_INPUT_KEY_INSERT;
			case XK_End:
				return RE_INPUT_KEY_END;
			case XK_Print:
				return RE_INPUT_KEY_PRINT_SCREEN;
			case XK_Num_Lock:
				return RE_INPUT_KEY_NUMPAD_LOCK;
			case XK_Shift_L:
				return RE_INPUT_KEY_SHIFT_LEFT;
			case XK_Shift_R:
				return RE_INPUT_KEY_SHIFT_RIGHT;
			case XK_Control_L:
				return RE_INPUT_KEY_CTRL_LEFT;
			case XK_Control_R:
				return RE_INPUT_KEY_CTRL_RIGHT;
			case XK_Caps_Lock:
				return RE_INPUT_KEY_CAPS_LOCK;
			case XK_Alt_L:
				return RE_INPUT_KEY_ALT_LEFT;
			case XK_ISO_Level3_Shift:
			case XK_Alt_R:
				return RE_INPUT_KEY_ALT_RIGHT;
			case XK_slash:
				return RE_INPUT_KEY_SLASH;
			case XK_backslash:
				return RE_INPUT_KEY_BACKSLASH;
			case XK_less:
			case XK_greater:
				return RE_INPUT_KEY_WORLD_1;
			case XK_comma:
				return RE_INPUT_KEY_COMMA;
			case XK_period:
				return RE_INPUT_KEY_PERIOD;
			case XK_semicolon:
			case XK_odiaeresis:
				return RE_INPUT_KEY_SEMICOLON;
			case XK_dead_acute:
			case XK_apostrophe:
			case XK_adiaeresis:
				return RE_INPUT_KEY_APOSTROPHE;
			case XK_grave:
				return RE_INPUT_KEY_ACCENT;
			case XK_bracketleft:
			case XK_udiaeresis:
				return RE_INPUT_KEY_BRACKET_LEFT;
			case XK_bracketright:
				return RE_INPUT_KEY_BRACKET_RIGHT;
			case XK_plus:
			case XK_equal:
				return RE_INPUT_KEY_EQUALS;
			case XK_minus:
				return RE_INPUT_KEY_MINUS;
			case XK_KP_Add:
				return RE_INPUT_KEY_NUMPAD_ADD;
			case XK_KP_Subtract:
				return RE_INPUT_KEY_NUMPAD_SUBTRACT;
			case XK_KP_Multiply:
				return RE_INPUT_KEY_NUMPAD_MULTIPLY;
			case XK_KP_Divide:
				return RE_INPUT_KEY_NUMPAD_DIVIDE;
			case XK_KP_Enter:
				return RE_INPUT_KEY_NUMPAD_ENTER;
			case XK_KP_Delete:
			case XK_KP_Separator:
			case XK_KP_Decimal:
				return RE_INPUT_KEY_NUMPAD_PERIOD;
			case XK_KP_Insert:
			case XK_KP_0:
				return RE_INPUT_KEY_NUMPAD_0;
			case XK_KP_End:
			case XK_KP_1:
				return RE_INPUT_KEY_NUMPAD_1;
			case XK_KP_Page_Down:
			case XK_KP_2:
				return RE_INPUT_KEY_NUMPAD_3;
			case XK_KP_Down:
			case XK_KP_3:
				return RE_INPUT_KEY_NUMPAD_2;
			case XK_KP_Left:
			case XK_KP_4:
				return RE_INPUT_KEY_NUMPAD_4;
			case XK_KP_Begin:
			case XK_KP_5:
				return RE_INPUT_KEY_NUMPAD_5;
			case XK_KP_Right:
			case XK_KP_6:
				return RE_INPUT_KEY_NUMPAD_6;
			case XK_KP_Home:
			case XK_KP_7:
				return RE_INPUT_KEY_NUMPAD_7;
			case XK_KP_Up:
			case XK_KP_8:
				return RE_INPUT_KEY_NUMPAD_8;
			case XK_KP_Page_Up:
			case XK_KP_9:
				return RE_INPUT_KEY_NUMPAD_9;
			case XK_numbersign:
				return RE_INPUT_KEY_TOP_3;
			case XK_dollar:
				return RE_INPUT_KEY_TOP_4;
			case XK_dead_circumflex:
				return RE_INPUT_KEY_TOP_6;
			case XK_parenleft:
				return RE_INPUT_KEY_TOP_9;
			case XK_parenright:
				return RE_INPUT_KEY_TOP_0;
			case XK_Menu:
				return RE_INPUT_KEY_MENU;
			default:
				if (x11_virtualKeyCode >= XK_a && x11_virtualKeyCode <= XK_z)
					return static_cast<Input>(x11_virtualKeyCode - XK_a + static_cast<KeySym>(RE_INPUT_KEY_A));
				if (x11_virtualKeyCode >= XK_A && x11_virtualKeyCode <= XK_Z)
					return static_cast<Input>(x11_virtualKeyCode - XK_A + static_cast<KeySym>(RE_INPUT_KEY_A));
				if (x11_virtualKeyCode >= XK_0 && x11_virtualKeyCode <= XK_9)
					return static_cast<Input>(x11_virtualKeyCode - XK_0 + static_cast<KeySym>(RE_INPUT_KEY_TOP_0));
				return RE_INPUT_UNKNOWN;
		}
	}

#endif

}
