#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__

#include "RE_Ext Header.hpp"

namespace RE {

#define RE_TOTAL_KEYS 0

#define RE_LBUTTON static_cast<REuint>(MouseButton::Left)
#define RE_RBUTTON static_cast<REuint>(MouseButton::Right)
#define RE_MBUTTON static_cast<REuint>(MouseButton::Middle)
	
	class InputMgr {
		private:
#define _KEY_ARRAY_LENGTH RE_TOTAL_KEYS / 8 + ((RE_TOTAL_KEYS % 8 != 0) ? 1 : 0)
			REubyte keys[_KEY_ARRAY_LENGTH], lastKeyboards[_KEY_ARRAY_LENGTH];
			REubyte buttons, lastButtons;
#undef _KEY_ARRAY_LENGTH
			float scroll;

		public:
			InputMgr();
			~InputMgr();
			void keyInput(Keyboard key, REushort scancode, bool pressed);
			void charInput(wchar_t character);
			void buttonInput(REubyte buttoncode, bool pressed);
			void cursorInput(REushort x, REushort y);
			void scrollInput(float y);
			void updateInput();
	};

#ifdef RE_OS_WINDOWS
# define VK_A 0x41
# define VK_Z 0x5A
# define VK_0 0x30
# define VK_9 0x39
	constexpr REushort winVirtualFromKey(Keyboard key) {
		switch (key) {
			case Keyboard::Space:
				return VK_SPACE;
			case Keyboard::Backspace:
				return VK_BACK;
			case Keyboard::Tab:
				return VK_TAB;
			case Keyboard::Enter:
				return VK_RETURN;
			case Keyboard::Pause:
				return VK_PAUSE;
			case Keyboard::Caps_Lock:
				return VK_CAPITAL;
			case Keyboard::Escape:
				return VK_ESCAPE;
			case Keyboard::Page_Up:
				return VK_PRIOR;
			case Keyboard::Page_Down:
				return VK_NEXT;
			case Keyboard::End:
				return VK_END;
			case Keyboard::Home:
				return VK_HOME;
			case Keyboard::Arrow_Left:
				return VK_LEFT;
			case Keyboard::Arrow_Right:
				return VK_RIGHT;
			case Keyboard::Arrow_Up:
				return VK_UP;
			case Keyboard::Arrow_Down:
				return VK_DOWN;
			case Keyboard::Print_Screen:
				return VK_SNAPSHOT;
			case Keyboard::Insert:
				return VK_INSERT;
			case Keyboard::Delete:
				return VK_DELETE;
			case Keyboard::Left_Menu:
				return VK_LMENU;
			case Keyboard::Right_Menu:
				return VK_RMENU;
			case Keyboard::Numpad_Multiply:
				return VK_MULTIPLY;
			case Keyboard::Numpad_Add:
				return VK_ADD;
			case Keyboard::Numpad_Subtract:
				return VK_SUBTRACT;
			case Keyboard::Numpad_Divide:
				return VK_DIVIDE;
			case Keyboard::Numpad_Lock:
				return VK_NUMLOCK;
			case Keyboard::Scroll_Lock:
				return VK_SCROLL;
			case Keyboard::Right_Shift:
				return VK_RSHIFT;
			case Keyboard::Left_Shift:
				return VK_LSHIFT;
			case Keyboard::Right_Ctrl:
				return VK_RCONTROL;
			case Keyboard::Left_Ctrl:
				return VK_LCONTROL;
			case Keyboard::Right_Alt:
				return VK_RMENU;
			case Keyboard::Left_Alt:
				return VK_LMENU;
			case Keyboard::Semicolon:
				return VK_OEM_1;
			case Keyboard::Slash:
				return VK_OEM_2;
			case Keyboard::Left_Bracket:
				return VK_OEM_4;
			case Keyboard::Right_Bracket:
				return VK_OEM_6;
			case Keyboard::Backslash:
				return VK_OEM_5;
			case Keyboard::Apostrophe:
				return VK_OEM_7;
			case Keyboard::Accent:
				return VK_OEM_3;
			case Keyboard::Comma:
				return VK_OEM_COMMA;
			case Keyboard::Period:
				return VK_OEM_PERIOD;
			case Keyboard::Hashtag:
				return 0;
			case Keyboard::Equals:
				return VK_OEM_PLUS;
			case Keyboard::Minus:
				return VK_OEM_MINUS;
			case Keyboard::Numpad_Enter:
				return VK_RETURN;
			case Keyboard::Numpad_Period:
				return VK_DECIMAL;
			default:
				REushort keyId = static_cast<REushort>(key);
				if (keyId >= static_cast<REushort>(Keyboard::A) && keyId <= static_cast<REushort>(Keyboard::Z))
					return VK_A + (keyId - static_cast<REushort>(Keyboard::A));
				if (keyId >= static_cast<REushort>(Keyboard::Top_0) && keyId <= static_cast<REushort>(Keyboard::Top_9))
					return VK_0 + (keyId - static_cast<REushort>(Keyboard::Top_0));
				if (keyId >= static_cast<REushort>(Keyboard::F1) && keyId <= static_cast<REushort>(Keyboard::F25))
					return VK_F1 + (keyId - static_cast<REushort>(Keyboard::F1));
				if (keyId >= static_cast<REushort>(Keyboard::Numpad_0) && keyId <= static_cast<REushort>(Keyboard::Numpad_9))
					return VK_NUMPAD0 + (keyId - static_cast<REushort>(Keyboard::Numpad_0));
				break;
		}
		return 0;
	}

	/* doesn't return Keyboard::Hashtag */
	constexpr Keyboard winKeyFromVirtual(REushort vkCode) {
		switch (vkCode) {
			case VK_SPACE:
				return Keyboard::Space;
			case VK_RETURN:
				return Keyboard::Enter;
			case VK_BACK:
				return Keyboard::Backspace;
			case VK_TAB:
				return Keyboard::Tab;
			case VK_PAUSE:
				return Keyboard::Pause;
			case VK_HOME:
				return Keyboard::Home;
			case VK_DELETE:
				return Keyboard::Delete;
			case VK_INSERT:
				return Keyboard::Insert;
			case VK_CAPITAL:
				return Keyboard::Caps_Lock;
			case VK_ESCAPE:
				return Keyboard::Escape;
			case VK_PRIOR:
				return Keyboard::Page_Up;
			case VK_NEXT:
				return Keyboard::Page_Down;
			case VK_END:
				return Keyboard::End;
			case VK_LEFT:
				return Keyboard::Arrow_Left;
			case VK_RIGHT:
				return Keyboard::Arrow_Right;
			case VK_UP:
				return Keyboard::Arrow_Up;
			case VK_DOWN:
				return Keyboard::Arrow_Down;
			case VK_SNAPSHOT:
				return Keyboard::Print_Screen;
			case VK_MULTIPLY:
				return Keyboard::Numpad_Multiply;
			case VK_ADD:
				return Keyboard::Numpad_Add;
			case VK_SUBTRACT:
				return Keyboard::Numpad_Subtract;
			case VK_DIVIDE:
				return Keyboard::Numpad_Divide;
			case VK_NUMLOCK:
				return Keyboard::Numpad_Lock;
			case VK_SCROLL:
				return Keyboard::Scroll_Lock;
			case VK_LSHIFT:
				return Keyboard::Left_Shift;
			case VK_RSHIFT:
				return Keyboard::Right_Shift;
			case VK_LCONTROL:
				return Keyboard::Left_Ctrl;
			case VK_RCONTROL:
				return Keyboard::Right_Ctrl;
			case VK_LMENU:
				return Keyboard::Left_Alt;
			case VK_RMENU:
				return Keyboard::Right_Alt;
			case VK_OEM_1:
				return Keyboard::Semicolon;
			case VK_OEM_2:
				return Keyboard::Slash;
			case VK_OEM_3:
				return Keyboard::Accent;
			case VK_OEM_4:
				return Keyboard::Left_Bracket;
			case VK_OEM_5:
				return Keyboard::Backslash;
			case VK_OEM_6:
				return Keyboard::Right_Bracket;
			case VK_OEM_7:
				return Keyboard::Apostrophe;
			case VK_OEM_COMMA:
				return Keyboard::Comma;
			case VK_OEM_PERIOD:
				return Keyboard::Period;
			case VK_OEM_PLUS:
				return Keyboard::Equals;
			case VK_OEM_MINUS:
				return Keyboard::Minus;
			case VK_SEPARATOR:
			case VK_DECIMAL:
				return Keyboard::Numpad_Period;
			default:
				if (vkCode >= VK_A && vkCode <= VK_Z)
					return static_cast<Keyboard>(vkCode - VK_A + static_cast<REushort>(Keyboard::A));
				if (vkCode >= VK_0 && vkCode <= VK_9)
					return static_cast<Keyboard>(vkCode - VK_0 + static_cast<REushort>(Keyboard::Top_0));
				if (vkCode >= VK_F1 && vkCode <= VK_F24)
					return static_cast<Keyboard>(vkCode - VK_F1 + static_cast<REushort>(Keyboard::F1));
				if (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9)
					return static_cast<Keyboard>(vkCode - VK_NUMPAD0 + static_cast<REushort>(Keyboard::Numpad_0));
				break;
		}
		return Keyboard::Space;
	}
#elif defined RE_OS_LINUX
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

	REushort scancodeFromKey(Keyboard key);
	Keyboard keyFromScancode(REushort scancode);

}

#endif
