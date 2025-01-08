#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__

#include "RE_Ext Header.hpp"

namespace RE {

#define RE_TOTAL_KEYS 0
#define RE_TOTAL_BUTTONS 0

#define RE_LBUTTON 0
#define RE_RBUTTON 1
#define RE_MBUTTON 2
	
	class InputMgr {
		private:
			bool keys[RE_TOTAL_KEYS], lastKeys[RE_TOTAL_KEYS];
			bool buttons[RE_TOTAL_BUTTONS], lastButtons[RE_TOTAL_BUTTONS];

		public:
			InputMgr();
			~InputMgr();
			void keyInput(Key key, REushort scancode, bool pressed);
			void charInput(wchar_t character);
			void buttonInput(REubyte buttoncode, bool pressed);
			void cursorInput(REushort x, REushort y);
			void scrollInput(float y);
	};

#ifdef RE_OS_WINDOWS
# define VK_A 0x41
# define VK_Z 0x5A
# define VK_0 0x30
# define VK_9 0x39
	constexpr REushort winVirtualFromKey(Key key) {
		switch (key) {
			case Key::Space:
				return VK_SPACE;
			case Key::Backspace:
				return VK_BACK;
			case Key::Tab:
				return VK_TAB;
			case Key::Enter:
				return VK_RETURN;
			case Key::Pause:
				return VK_PAUSE;
			case Key::Caps_Lock:
				return VK_CAPITAL;
			case Key::Escape:
				return VK_ESCAPE;
			case Key::Page_Up:
				return VK_PRIOR;
			case Key::Page_Down:
				return VK_NEXT;
			case Key::End:
				return VK_END;
			case Key::Home:
				return VK_HOME;
			case Key::Arrow_Left:
				return VK_LEFT;
			case Key::Arrow_Right:
				return VK_RIGHT;
			case Key::Arrow_Up:
				return VK_UP;
			case Key::Arrow_Down:
				return VK_DOWN;
			case Key::Print_Screen:
				return VK_SNAPSHOT;
			case Key::Insert:
				return VK_INSERT;
			case Key::Delete:
				return VK_DELETE;
			case Key::Left_Menu:
				return VK_LMENU;
			case Key::Right_Menu:
				return VK_RMENU;
			case Key::Numpad_Multiply:
				return VK_MULTIPLY;
			case Key::Numpad_Add:
				return VK_ADD;
			case Key::Numpad_Subtract:
				return VK_SUBTRACT;
			case Key::Numpad_Divide:
				return VK_DIVIDE;
			case Key::Numpad_Lock:
				return VK_NUMLOCK;
			case Key::Scroll_Lock:
				return VK_SCROLL;
			case Key::Right_Shift:
				return VK_RSHIFT;
			case Key::Left_Shift:
				return VK_LSHIFT;
			case Key::Right_Ctrl:
				return VK_RCONTROL;
			case Key::Left_Ctrl:
				return VK_LCONTROL;
			case Key::Right_Alt:
				return VK_RMENU;
			case Key::Left_Alt:
				return VK_LMENU;
			case Key::Semicolon:
				return VK_OEM_1;
			case Key::Slash:
				return VK_OEM_2;
			case Key::Left_Bracket:
				return VK_OEM_4;
			case Key::Right_Bracket:
				return VK_OEM_6;
			case Key::Backslash:
				return VK_OEM_5;
			case Key::Apostrophe:
				return VK_OEM_7;
			case Key::Accent:
				return VK_OEM_3;
			case Key::Comma:
				return VK_OEM_COMMA;
			case Key::Period:
				return VK_OEM_PERIOD;
			case Key::Hashtag:
				return 0;
			case Key::Equals:
				return VK_OEM_PLUS;
			case Key::Minus:
				return VK_OEM_MINUS;
			case Key::Numpad_Enter:
				return VK_RETURN;
			case Key::Numpad_Period:
				return VK_DECIMAL;
			default:
				REushort keyId = static_cast<REushort>(key);
				if (keyId >= static_cast<REushort>(Key::A) && keyId <= static_cast<REushort>(Key::Z))
					return VK_A + (keyId - static_cast<REushort>(Key::A));
				if (keyId >= static_cast<REushort>(Key::Top_0) && keyId <= static_cast<REushort>(Key::Top_9))
					return VK_0 + (keyId - static_cast<REushort>(Key::Top_0));
				if (keyId >= static_cast<REushort>(Key::F1) && keyId <= static_cast<REushort>(Key::F25))
					return VK_F1 + (keyId - static_cast<REushort>(Key::F1));
				if (keyId >= static_cast<REushort>(Key::Numpad_0) && keyId <= static_cast<REushort>(Key::Numpad_9))
					return VK_NUMPAD0 + (keyId - static_cast<REushort>(Key::Numpad_0));
				break;
		}
		return 0;
	}

	constexpr Key winKeyFromVirtual(REushort vkCode) {
		switch (vkCode) {
			case VK_SPACE:
				return Key::Space;
			case VK_RETURN:
				return Key::Enter;
			case VK_BACK:
				return Key::Backspace;
			case VK_TAB:
				return Key::Tab;
			case VK_PAUSE:
				return Key::Pause;
			case VK_HOME:
				return Key::Home;
			case VK_DELETE:
				return Key::Delete;
			case VK_INSERT:
				return Key::Insert;
			case VK_CAPITAL:
				return Key::Caps_Lock;
			case VK_ESCAPE:
				return Key::Escape;
			case VK_PRIOR:
				return Key::Page_Up;
			case VK_NEXT:
				return Key::Page_Down;
			case VK_END:
				return Key::End;
			case VK_LEFT:
				return Key::Arrow_Left;
			case VK_RIGHT:
				return Key::Arrow_Right;
			case VK_UP:
				return Key::Arrow_Up;
			case VK_DOWN:
				return Key::Arrow_Down;
			case VK_SNAPSHOT:
				return Key::Print_Screen;
			case VK_MULTIPLY:
				return Key::Numpad_Multiply;
			case VK_ADD:
				return Key::Numpad_Add;
			case VK_SUBTRACT:
				return Key::Numpad_Subtract;
			case VK_DIVIDE:
				return Key::Numpad_Divide;
			case VK_NUMLOCK:
				return Key::Numpad_Lock;
			case VK_SCROLL:
				return Key::Scroll_Lock;
			case VK_LSHIFT:
				return Key::Left_Shift;
			case VK_RSHIFT:
				return Key::Right_Shift;
			case VK_LCONTROL:
				return Key::Left_Ctrl;
			case VK_RCONTROL:
				return Key::Right_Ctrl;
			case VK_LMENU:
				return Key::Left_Alt;
			case VK_RMENU:
				return Key::Right_Alt;
			case VK_OEM_1:
				return Key::Semicolon;
			case VK_OEM_2:
				return Key::Slash;
			case VK_OEM_3:
				return Key::Accent;
			case VK_OEM_4:
				return Key::Left_Bracket;
			case VK_OEM_5:
				return Key::Backslash;
			case VK_OEM_6:
				return Key::Right_Bracket;
			case VK_OEM_7:
				return Key::Apostrophe;
			case VK_OEM_COMMA:
				return Key::Comma;
			case VK_OEM_PERIOD:
				return Key::Period;
			case 0:
				return Key::Hashtag;
			case VK_OEM_PLUS:
				return Key::Equals;
			case VK_OEM_MINUS:
				return Key::Minus;
			case VK_SEPARATOR:
			case VK_DECIMAL:
				return Key::Numpad_Period;
			default:
				if (vkCode >= VK_A && vkCode <= VK_Z)
					return static_cast<Key>(vkCode - VK_A + static_cast<REushort>(Key::A));
				if (vkCode >= VK_0 && vkCode <= VK_9)
					return static_cast<Key>(vkCode - VK_0 + static_cast<REushort>(Key::Top_0));
				if (vkCode >= VK_F1 && vkCode <= VK_F24)
					return static_cast<Key>(vkCode - VK_F1 + static_cast<REushort>(Key::F1));
				if (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9)
					return static_cast<Key>(vkCode - VK_NUMPAD0 + static_cast<REushort>(Key::Numpad_0));
				break;
		}
		return Key::Space;
	}
#elif defined RE_OS_LINUX
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

	REushort scancodeFromKey(Key key);
	Key keyFromScancode(REushort scancode);

}

#endif
