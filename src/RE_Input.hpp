#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__

#include "RE_Internal Header.hpp"

namespace RE {

#define MAXIMUM_PHYSICAL_KEYS 150
#define KEY_BUFFER_SIZE (MAXIMUM_PHYSICAL_KEYS / 8 + (MAXIMUM_PHYSICAL_KEYS % 8 > 0 ? 1 : 0))
	
	class InputMgr {
		private:
			REuint u32Scancodes[MAXIMUM_PHYSICAL_KEYS];
			Input eInputs[MAXIMUM_PHYSICAL_KEYS];
			REubyte u8KeyBuffer[KEY_BUFFER_SIZE], u8PrevKeyBuffer[KEY_BUFFER_SIZE]; // Keyboard
			REuint u32NumberOfKeys;
			REubyte u8SpecialInputBuffer, u8PrevSpecialInputBuffer; // Scrolling and mouse buttons
			Vector2i cursorPosition, prevCursorPosition;

			REint get_index_for_scancode(REuint u32Scancode);
			REint get_index_for_input(Input eSearchedInput);
			bool process_request(Input eInput, REuint u32Scancode, bool bRequestForPast);

		public:
			static InputMgr* pInstance;

			InputMgr();
			~InputMgr();
			void input_event(Input eInput, REuint u32Scancode, bool bPressed, bool bFallbackToInput);
			void cursor_event(REint i32X, REint i32Y);
			void update_input_buffers();
			Input map_scancode_to_input(REuint u32Scancode);
			REuint map_input_to_scancode(Input eInput);
			bool is_down(Input eInput, REuint u32Scancode);
			bool was_down(Input eInput, REuint u32Scancode);
			REuint get_number_of_keys(); // Call once to set u32NumberOfKeys to correct value
	};

	Input map_scancode_to_input(REuint u32Scancode);
	REuint map_input_to_scancode(Input eInput);

#ifdef RE_OS_WINDOWS
# define VK_A 0x41
# define VK_Z 0x5A
# define VK_0 0x30
# define VK_9 0x39
	constexpr RElong windows_virtual_keycode_from_key(Input eKey) {
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
			case RE_INPUT_KEY_CAPS_LOCK:
				return VK_CAPITAL;
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
			default: {
				RElong u64KeyId = static_cast<RElong>(eKey);
				if (u64KeyId >= static_cast<RElong>(RE_INPUT_KEY_A) && u64KeyId <= static_cast<RElong>(RE_INPUT_KEY_Z))
					return VK_A + (u64KeyId - static_cast<RElong>(RE_INPUT_KEY_A));
				if (u64KeyId >= static_cast<RElong>(RE_INPUT_KEY_TOP_0) && u64KeyId <= static_cast<RElong>(RE_INPUT_KEY_TOP_9))
					return VK_0 + (u64KeyId - static_cast<RElong>(RE_INPUT_KEY_TOP_0));
				if (u64KeyId >= static_cast<RElong>(RE_INPUT_KEY_F1) && u64KeyId <= static_cast<RElong>(RE_INPUT_KEY_F25))
					return VK_F1 + (u64KeyId - static_cast<RElong>(RE_INPUT_KEY_F1));
				if (u64KeyId >= static_cast<RElong>(RE_INPUT_KEY_NUMPAD_0) && u64KeyId <= static_cast<RElong>(RE_INPUT_KEY_NUMPAD_9))
					return VK_NUMPAD0 + (u64KeyId - static_cast<RElong>(RE_INPUT_KEY_NUMPAD_0));
				} return 0L;
		}
	}

	constexpr Input windows_key_from_virtual_keycode(RElong i64VirtualKeyCode) {
		switch (i64VirtualKeyCode) {
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
			case VK_CAPITAL:
				return RE_INPUT_KEY_CAPS_LOCK;
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
			case VK_NUMLOCK:
				return RE_INPUT_KEY_NUMPAD_LOCK;
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
			case VK_OEM_102:
				return RE_INPUT_KEY_WORLD_1;
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
			case VK_LWIN:
			case VK_RWIN:
				return RE_INPUT_UNKNOWN;
			default:
				if (i64VirtualKeyCode >= VK_A && i64VirtualKeyCode <= VK_Z)
					return static_cast<Input>(i64VirtualKeyCode - VK_A + static_cast<RElong>(RE_INPUT_KEY_A));
				else if (i64VirtualKeyCode >= VK_0 && i64VirtualKeyCode <= VK_9)
					return static_cast<Input>(i64VirtualKeyCode - VK_0 + static_cast<RElong>(RE_INPUT_KEY_TOP_0));
				else if (i64VirtualKeyCode >= VK_F1 && i64VirtualKeyCode <= VK_F24)
					return static_cast<Input>(i64VirtualKeyCode - VK_F1 + static_cast<RElong>(RE_INPUT_KEY_F1));
				else if (i64VirtualKeyCode >= VK_NUMPAD0 && i64VirtualKeyCode <= VK_NUMPAD9)
					return static_cast<Input>(i64VirtualKeyCode - VK_NUMPAD0 + static_cast<RElong>(RE_INPUT_KEY_NUMPAD_0));
				else
					return RE_INPUT_UNKNOWN;
		}
	}
#elif defined RE_OS_LINUX
	constexpr RElong x11_virtual_keycode_from_key(Input eKey) {
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
			case RE_INPUT_KEY_CAPS_LOCK:
				return XK_Caps_Lock;
			case RE_INPUT_KEY_NUMPAD_LOCK:
				return XK_Num_Lock;
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
			case RE_INPUT_KEY_WORLD_1:
				return XK_less;
			default: {
				RElong u64KeyId = static_cast<RElong>(eKey);
				if (u64KeyId >= static_cast<RElong>(RE_INPUT_KEY_A) && u64KeyId <= static_cast<RElong>(RE_INPUT_KEY_Z))
					return XK_a + (u64KeyId - static_cast<RElong>(RE_INPUT_KEY_A));
				if (u64KeyId >= static_cast<RElong>(RE_INPUT_KEY_TOP_0) && u64KeyId <= static_cast<RElong>(RE_INPUT_KEY_TOP_9))
					return XK_0 + (u64KeyId - static_cast<RElong>(RE_INPUT_KEY_TOP_0));
				if (u64KeyId >= static_cast<RElong>(RE_INPUT_KEY_F1) && u64KeyId <= static_cast<RElong>(RE_INPUT_KEY_F25))
					return XK_F1 + (u64KeyId - static_cast<RElong>(RE_INPUT_KEY_F1));
				if (u64KeyId >= static_cast<RElong>(RE_INPUT_KEY_NUMPAD_0) && u64KeyId <= static_cast<RElong>(RE_INPUT_KEY_NUMPAD_9))
					return XK_KP_0 + (u64KeyId - static_cast<RElong>(RE_INPUT_KEY_NUMPAD_0));
				} return 0L;
		}
	}

	constexpr Input x11_key_from_virtual_keycode(RElong i64VirtualKeyCode) {
		switch (i64VirtualKeyCode) {
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
				return RE_INPUT_KEY_NUMPAD_0;
			case XK_KP_End:
				return RE_INPUT_KEY_NUMPAD_1;
			case XK_KP_Page_Down:
				return RE_INPUT_KEY_NUMPAD_3;
			case XK_KP_Down:
				return RE_INPUT_KEY_NUMPAD_2;
			case XK_KP_Left:
				return RE_INPUT_KEY_NUMPAD_4;
			case XK_KP_Begin:
				return RE_INPUT_KEY_NUMPAD_5;
			case XK_KP_Right:
				return RE_INPUT_KEY_NUMPAD_6;
			case XK_KP_Home:
				return RE_INPUT_KEY_NUMPAD_7;
			case XK_KP_Up:
				return RE_INPUT_KEY_NUMPAD_8;
			case XK_KP_Page_Up:
				return RE_INPUT_KEY_NUMPAD_9;
			case XK_numbersign:
				return RE_INPUT_KEY_TOP_3;
			case XK_dead_circumflex:
				return RE_INPUT_KEY_TOP_6;
			case XK_Menu:
				return RE_INPUT_KEY_MENU;
			default:
				if (i64VirtualKeyCode >= XK_a && i64VirtualKeyCode <= XK_z)
					return static_cast<Input>(i64VirtualKeyCode - XK_a + static_cast<RElong>(RE_INPUT_KEY_A));
				else if (i64VirtualKeyCode >= XK_A && i64VirtualKeyCode <= XK_Z)
					return static_cast<Input>(i64VirtualKeyCode - XK_A + static_cast<RElong>(RE_INPUT_KEY_A));
				else if (i64VirtualKeyCode >= XK_0 && i64VirtualKeyCode <= XK_9)
					return static_cast<Input>(i64VirtualKeyCode - XK_0 + static_cast<RElong>(RE_INPUT_KEY_TOP_0));
				else if (i64VirtualKeyCode >= XK_F1 && i64VirtualKeyCode <= XK_F25)
					return static_cast<Input>(i64VirtualKeyCode - XK_F1 + static_cast<RElong>(RE_INPUT_KEY_F1));
				else if (i64VirtualKeyCode >= XK_KP_0 && i64VirtualKeyCode <= XK_KP_9)
					return static_cast<Input>(i64VirtualKeyCode - XK_KP_0 + static_cast<RElong>(RE_INPUT_KEY_NUMPAD_0));
				else
					return RE_INPUT_UNKNOWN;
		}
	}
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

}

#endif /* __RE_INPUT_H__ */
