#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__

#include "RE_Ext Header.hpp"

namespace RE {

#define RE_INPUT_BUFFER_LMB 2U
#define RE_INPUT_BUFFER_RMB 3U
#define RE_INPUT_BUFFER_MMB 4U
	
	class InputMgr {
		private:

		public:
			static InputMgr* pInstance;

			InputMgr();
			~InputMgr();
	};

#ifdef RE_OS_WINDOWS
# define VK_A 0x41
# define VK_Z 0x5A
# define VK_0 0x30
# define VK_9 0x39
	constexpr RElong winVirtualFromKey(Input eKey) {
		switch (eKey) {
			case Input::Space:
				return VK_SPACE;
			case Input::Backspace:
				return VK_BACK;
			case Input::Tab:
				return VK_TAB;
			case Input::Enter:
				return VK_RETURN;
			case Input::Pause:
				return VK_PAUSE;
			case Input::Caps_Lock:
				return VK_CAPITAL;
			case Input::Escape:
				return VK_ESCAPE;
			case Input::Page_Up:
				return VK_PRIOR;
			case Input::Page_Down:
				return VK_NEXT;
			case Input::End:
				return VK_END;
			case Input::Home:
				return VK_HOME;
			case Input::Arrow_Left:
				return VK_LEFT;
			case Input::Arrow_Right:
				return VK_RIGHT;
			case Input::Arrow_Up:
				return VK_UP;
			case Input::Arrow_Down:
				return VK_DOWN;
			case Input::Print_Screen:
				return VK_SNAPSHOT;
			case Input::Insert:
				return VK_INSERT;
			case Input::Delete:
				return VK_DELETE;
			case Input::Numpad_Multiply:
				return VK_MULTIPLY;
			case Input::Numpad_Add:
				return VK_ADD;
			case Input::Numpad_Subtract:
				return VK_SUBTRACT;
			case Input::Numpad_Divide:
				return VK_DIVIDE;
			case Input::Numpad_Lock:
				return VK_NUMLOCK;
			case Input::Scroll_Lock:
				return VK_SCROLL;
			case Input::Right_Shift:
				return VK_RSHIFT;
			case Input::Left_Shift:
				return VK_LSHIFT;
			case Input::Right_Ctrl:
				return VK_RCONTROL;
			case Input::Left_Ctrl:
				return VK_LCONTROL;
			case Input::Right_Alt:
				return VK_RMENU;
			case Input::Left_Alt:
				return VK_LMENU;
			case Input::Semicolon:
				return VK_OEM_1;
			case Input::Slash:
				return VK_OEM_2;
			case Input::Left_Bracket:
				return VK_OEM_4;
			case Input::Right_Bracket:
				return VK_OEM_6;
			case Input::Backslash:
				return VK_OEM_5;
			case Input::Apostrophe:
				return VK_OEM_7;
			case Input::Accent:
				return VK_OEM_3;
			case Input::Comma:
				return VK_OEM_COMMA;
			case Input::Period:
				return VK_OEM_PERIOD;
			case Input::Equals:
				return VK_OEM_PLUS;
			case Input::Minus:
				return VK_OEM_MINUS;
			case Input::Numpad_Enter:
				return VK_RETURN;
			case Input::Numpad_Period:
				return VK_DECIMAL;
			case Input::Menu:
				return VK_APPS;
			case Input::World_1:
				return VK_OEM_102;
			default:
				RElong u64KeyId = static_cast<RElong>(eKey);
				if (u64KeyId >= static_cast<RElong>(Input::A) && u64KeyId <= static_cast<RElong>(Input::Z))
					return VK_A + (u64KeyId - static_cast<RElong>(Input::A));
				if (u64KeyId >= static_cast<RElong>(Input::Top_0) && u64KeyId <= static_cast<RElong>(Input::Top_9))
					return VK_0 + (u64KeyId - static_cast<RElong>(Input::Top_0));
				if (u64KeyId >= static_cast<RElong>(Input::F1) && u64KeyId <= static_cast<RElong>(Input::F25))
					return VK_F1 + (u64KeyId - static_cast<RElong>(Input::F1));
				if (u64KeyId >= static_cast<RElong>(Input::Numpad_0) && u64KeyId <= static_cast<RElong>(Input::Numpad_9))
					return VK_NUMPAD0 + (u64KeyId - static_cast<RElong>(Input::Numpad_0));
				return 0L;
		}
	}

	constexpr Input winKeyFromVirtual(RElong u64VirtualKeyCode) {
		switch (u64VirtualKeyCode) {
			case VK_SPACE:
				return Input::Space;
			case VK_RETURN:
				return Input::Enter;
			case VK_BACK:
				return Input::Backspace;
			case VK_TAB:
				return Input::Tab;
			case VK_PAUSE:
				return Input::Pause;
			case VK_HOME:
				return Input::Home;
			case VK_DELETE:
				return Input::Delete;
			case VK_INSERT:
				return Input::Insert;
			case VK_CAPITAL:
				return Input::Caps_Lock;
			case VK_ESCAPE:
				return Input::Escape;
			case VK_PRIOR:
				return Input::Page_Up;
			case VK_NEXT:
				return Input::Page_Down;
			case VK_END:
				return Input::End;
			case VK_LEFT:
				return Input::Arrow_Left;
			case VK_RIGHT:
				return Input::Arrow_Right;
			case VK_UP:
				return Input::Arrow_Up;
			case VK_DOWN:
				return Input::Arrow_Down;
			case VK_SNAPSHOT:
				return Input::Print_Screen;
			case VK_MULTIPLY:
				return Input::Numpad_Multiply;
			case VK_ADD:
				return Input::Numpad_Add;
			case VK_SUBTRACT:
				return Input::Numpad_Subtract;
			case VK_DIVIDE:
				return Input::Numpad_Divide;
			case VK_NUMLOCK:
				return Input::Numpad_Lock;
			case VK_SCROLL:
				return Input::Scroll_Lock;
			case VK_LSHIFT:
				return Input::Left_Shift;
			case VK_RSHIFT:
				return Input::Right_Shift;
			case VK_LCONTROL:
				return Input::Left_Ctrl;
			case VK_RCONTROL:
				return Input::Right_Ctrl;
			case VK_LMENU:
				return Input::Left_Alt;
			case VK_RMENU:
				return Input::Right_Alt;
			case VK_OEM_1:
				return Input::Semicolon;
			case VK_OEM_2:
				return Input::Slash;
			case VK_OEM_3:
				return Input::Accent;
			case VK_OEM_4:
				return Input::Left_Bracket;
			case VK_OEM_5:
				return Input::Backslash;
			case VK_OEM_6:
				return Input::Right_Bracket;
			case VK_OEM_7:
				return Input::Apostrophe;
			case VK_OEM_102:
				return Input::World_1;
			case VK_OEM_COMMA:
				return Input::Comma;
			case VK_OEM_PERIOD:
				return Input::Period;
			case VK_OEM_PLUS:
				return Input::Equals;
			case VK_OEM_MINUS:
				return Input::Minus;
			case VK_SEPARATOR:
			case VK_DECIMAL:
				return Input::Numpad_Period;
			case VK_APPS:
				return Input::Menu;
			default:
				if (u64VirtualKeyCode >= VK_A && u64VirtualKeyCode <= VK_Z)
					return static_cast<Input>(u64VirtualKeyCode - VK_A + static_cast<RElong>(Input::A));
				if (u64VirtualKeyCode >= VK_0 && u64VirtualKeyCode <= VK_9)
					return static_cast<Input>(u64VirtualKeyCode - VK_0 + static_cast<RElong>(Input::Top_0));
				if (u64VirtualKeyCode >= VK_F1 && u64VirtualKeyCode <= VK_F24)
					return static_cast<Input>(u64VirtualKeyCode - VK_F1 + static_cast<RElong>(Input::F1));
				if (u64VirtualKeyCode >= VK_NUMPAD0 && u64VirtualKeyCode <= VK_NUMPAD9)
					return static_cast<Input>(u64VirtualKeyCode - VK_NUMPAD0 + static_cast<RElong>(Input::Numpad_0));
				return Input::Unknown;
		}
	}
#elif defined RE_OS_LINUX
	constexpr RElong x11VirtualFromKey(Input eKey) {
		switch (eKey) {
			case Input::Space:
				return XK_space;
			case Input::Slash:
				return XK_slash;
			case Input::Backslash:
				return XK_backslash;
			case Input::Comma:
				return XK_comma;
			case Input::Period:
				return XK_period;
			case Input::Semicolon:
				return XK_semicolon;
			case Input::Apostrophe:
				return XK_apostrophe;
			case Input::Accent:
				return XK_grave;
			case Input::Left_Bracket:
				return XK_bracketleft;
			case Input::Right_Bracket:
				return XK_bracketright;
			case Input::Equals:
				return XK_equal;
			case Input::Minus:
				return XK_minus;
			case Input::Right_Ctrl:
				return XK_Control_R;
			case Input::Left_Ctrl:
				return XK_Control_L;
			case Input::Right_Alt:
				return XK_Alt_R;
			case Input::Left_Alt:
				return XK_Alt_L;
			case Input::Right_Shift:
				return XK_Shift_R;
			case Input::Left_Shift:
				return XK_Shift_L;
			case Input::Tab:
				return XK_Tab;
			case Input::Enter:
				return XK_Return;
			case Input::Escape:
				return XK_Escape;
			case Input::Backspace:
				return XK_BackSpace;
			case Input::Arrow_Left:
				return XK_Left;
			case Input::Arrow_Up:
				return XK_Up;
			case Input::Arrow_Down:
				return XK_Down;
			case Input::Arrow_Right:
				return XK_Right;
			case Input::Delete:
				return XK_Delete;
			case Input::Insert:
				return XK_Insert;
			case Input::Home:
				return XK_Home;
			case Input::End:
				return XK_End;
			case Input::Print_Screen:
				return XK_Print;
			case Input::Scroll_Lock:
				return XK_Scroll_Lock;
			case Input::Pause:
				return XK_Pause;
			case Input::Page_Up:
				return XK_Page_Up;
			case Input::Page_Down:
				return XK_Page_Down;
			case Input::Caps_Lock:
				return XK_Caps_Lock;
			case Input::Numpad_Lock:
				return XK_Num_Lock;
			case Input::Numpad_Add:
				return XK_KP_Add;
			case Input::Numpad_Subtract:
				return XK_KP_Subtract;
			case Input::Numpad_Multiply:
				return XK_KP_Multiply;
			case Input::Numpad_Divide:
				return XK_KP_Divide;
			case Input::Numpad_Enter:
				return XK_KP_Enter;
			case Input::Numpad_Period:
				return XK_KP_Decimal;
			case Input::Menu:
				return XK_Menu;
			case Input::World_1:
				return XK_less;
			default:
				RElong u64KeyId = static_cast<RElong>(eKey);
				if (u64KeyId >= static_cast<RElong>(Input::A) && u64KeyId <= static_cast<RElong>(Input::Z))
					return XK_a + (u64KeyId - static_cast<RElong>(Input::A));
				if (u64KeyId >= static_cast<RElong>(Input::Top_0) && u64KeyId <= static_cast<RElong>(Input::Top_9))
					return XK_0 + (u64KeyId - static_cast<RElong>(Input::Top_0));
				if (u64KeyId >= static_cast<RElong>(Input::F1) && u64KeyId <= static_cast<RElong>(Input::F25))
					return XK_F1 + (u64KeyId - static_cast<RElong>(Input::F1));
				if (u64KeyId >= static_cast<RElong>(Input::Numpad_0) && u64KeyId <= static_cast<RElong>(Input::Numpad_9))
					return XK_KP_0 + (u64KeyId - static_cast<RElong>(Input::Numpad_0));
				return 0L;
		}
	}

	constexpr Input x11KeyFromVirtual(RElong u64VirtualKeyCode) {
		switch (u64VirtualKeyCode) {
			case XK_space:
				return Input::Space;
			case XK_BackSpace:
				return Input::Backspace;
			case XK_Tab:
				return Input::Tab;
			case XK_Return:
				return Input::Enter;
			case XK_Pause:
				return Input::Pause;
			case XK_Scroll_Lock:
				return Input::Scroll_Lock;
			case XK_Escape:
				return Input::Escape;
			case XK_Delete:
				return Input::Delete;
			case XK_Home:
				return Input::Home;
			case XK_Left:
				return Input::Arrow_Left;
			case XK_Up:
				return Input::Arrow_Up;
			case XK_Right:
				return Input::Arrow_Right;
			case XK_Down:
				return Input::Arrow_Down;
			case XK_Page_Up:
				return Input::Page_Up;
			case XK_Page_Down:
				return Input::Page_Down;
			case XK_Insert:
				return Input::Insert;
			case XK_End:
				return Input::End;
			case XK_Print:
				return Input::Print_Screen;
			case XK_Num_Lock:
				return Input::Numpad_Lock;
			case XK_Shift_L:
				return Input::Left_Shift;
			case XK_Shift_R:
				return Input::Right_Shift;
			case XK_Control_L:
				return Input::Left_Ctrl;
			case XK_Control_R:
				return Input::Right_Ctrl;
			case XK_Caps_Lock:
				return Input::Caps_Lock;
			case XK_Alt_L:
				return Input::Left_Alt;
			case XK_ISO_Level3_Shift:
			case XK_Alt_R:
				return Input::Right_Alt;
			case XK_slash:
				return Input::Slash;
			case XK_backslash:
				return Input::Backslash;
			case XK_less:
			case XK_greater:
				return Input::World_1;
			case XK_comma:
				return Input::Comma;
			case XK_period:
				return Input::Period;
			case XK_semicolon:
				return Input::Semicolon;
			case XK_dead_acute:
			case XK_apostrophe:
				return Input::Apostrophe;
			case XK_grave:
				return Input::Accent;
			case XK_bracketleft:
				return Input::Left_Bracket;
			case XK_bracketright:
				return Input::Right_Bracket;
			case XK_plus:
			case XK_equal:
				return Input::Equals;
			case XK_minus:
				return Input::Minus;
			case XK_KP_Add:
				return Input::Numpad_Add;
			case XK_KP_Subtract:
				return Input::Numpad_Subtract;
			case XK_KP_Multiply:
				return Input::Numpad_Multiply;
			case XK_KP_Divide:
				return Input::Numpad_Divide;
			case XK_KP_Enter:
				return Input::Numpad_Enter;
			case XK_KP_Delete:
			case XK_KP_Separator:
			case XK_KP_Decimal:
				return Input::Numpad_Period;
			case XK_KP_Insert:
				return Input::Numpad_0;
			case XK_KP_End:
				return Input::Numpad_1;
			case XK_KP_Page_Down:
				return Input::Numpad_3;
			case XK_KP_Down:
				return Input::Numpad_2;
			case XK_KP_Left:
				return Input::Numpad_4;
			case XK_KP_Begin:
				return Input::Numpad_5;
			case XK_KP_Right:
				return Input::Numpad_6;
			case XK_KP_Home:
				return Input::Numpad_7;
			case XK_KP_Up:
				return Input::Numpad_8;
			case XK_KP_Page_Up:
				return Input::Numpad_9;
			case XK_numbersign:
				return Input::Top_3;
			case XK_dead_circumflex:
				return Input::Top_6;
			case XK_Menu:
				return Input::Menu;
			case XK_adiaeresis:
				return Input::A;
			case XK_odiaeresis:
				return Input::O;
			case XK_udiaeresis:
				return Input::U;
			default:
				if (u64VirtualKeyCode >= XK_a && u64VirtualKeyCode <= XK_z)
					return static_cast<Input>(u64VirtualKeyCode - XK_a + static_cast<RElong>(Input::A));
				if (u64VirtualKeyCode >= XK_A && u64VirtualKeyCode <= XK_Z)
					return static_cast<Input>(u64VirtualKeyCode - XK_A + static_cast<RElong>(Input::A));
				if (u64VirtualKeyCode >= XK_0 && u64VirtualKeyCode <= XK_9)
					return static_cast<Input>(u64VirtualKeyCode - XK_0 + static_cast<RElong>(Input::Top_0));
				if (u64VirtualKeyCode >= XK_F1 && u64VirtualKeyCode <= XK_F25)
					return static_cast<Input>(u64VirtualKeyCode - XK_F1 + static_cast<RElong>(Input::F1));
				if (u64VirtualKeyCode >= XK_KP_0 && u64VirtualKeyCode <= XK_KP_9)
					return static_cast<Input>(u64VirtualKeyCode - XK_KP_0 + static_cast<RElong>(Input::Numpad_0));
				return Input::Unknown;
		}
	}
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

}

#endif /* __RE_INPUT_H__ */
