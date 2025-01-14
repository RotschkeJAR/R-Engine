#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__

#include "RE_Ext Header.hpp"

namespace RE {

#define RE_TOTAL_KEYS static_cast<REushort>(Keyboard::Numpad_Period)

#define RE_LBUTTON static_cast<REuint>(MouseButton::Left)
#define RE_RBUTTON static_cast<REuint>(MouseButton::Right)
#define RE_MBUTTON static_cast<REuint>(MouseButton::Middle)
	
	class InputMgr {
		private:
#define _KEY_ARRAY_LENGTH RE_TOTAL_KEYS / 8 + ((RE_TOTAL_KEYS % 8 != 0) ? 1 : 0)
			REubyte keys[_KEY_ARRAY_LENGTH], lastKeys[_KEY_ARRAY_LENGTH];
#undef _KEY_ARRAY_LENGTH
			REubyte buttons, lastButtons;
			REbyte scroll;
			Vector<REint, 2> cursorPos, lastCursorPos;
			Vector<REushort, 2> winSize;
#ifdef RE_OS_LINUX
			XDisplay* xDisplay;
#endif /* RE_OS_LINUX */

		public:
			InputMgr();
			~InputMgr();
			void keyInput(Keyboard key, REint scancode, bool pressed);
			void charInput(const char* character);
			void buttonInput(REubyte buttoncode, bool pressed);
			void cursorInput(REint x, REint y);
			void scrollInput(REbyte y);
			void updateInput();
			void updateWinSize(Vector<REushort, 2> updatedSize);
#ifdef RE_OS_LINUX
			void setXDisplay(xDisplay* newDisplay);
#endif /* RE_OS_LINUX */

			bool isKeyDown(Keyboard key) const;
			bool wasKeyDown(Keyboard key) const;
			REubyte getScroll() const;
			bool isButtonDown(MouseButton button) const;
			bool wasButtonDown(MouseButton button) const;
			REint getCursorX() const;
			REint getCursorY() const;
			REint getCursorLastX() const;
			REint getCursorLastY() const;

			friend Vector<float, 2> normalCursorPos();
			friend Vector<float, 2> normalCursorDeltaPos();
	};

#ifdef RE_OS_WINDOWS
# define VK_A 0x41
# define VK_Z 0x5A
# define VK_0 0x30
# define VK_9 0x39
	constexpr REint winVirtualFromKey(Keyboard key) {
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
				return 0;
		}
	}

	/* doesn't return Keyboard::Hashtag */
	constexpr Keyboard winKeyFromVirtual(REint vkCode) {
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
					return static_cast<Keyboard>(vkCode - VK_A + static_cast<REint>(Keyboard::A));
				if (vkCode >= VK_0 && vkCode <= VK_9)
					return static_cast<Keyboard>(vkCode - VK_0 + static_cast<REint>(Keyboard::Top_0));
				if (vkCode >= VK_F1 && vkCode <= VK_F24)
					return static_cast<Keyboard>(vkCode - VK_F1 + static_cast<REint>(Keyboard::F1));
				if (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9)
					return static_cast<Keyboard>(vkCode - VK_NUMPAD0 + static_cast<REint>(Keyboard::Numpad_0));
				RE_WARNING("An unknown Windows virtual key code has been passed");
				return Keyboard::Space;
		}
	}
#elif defined RE_OS_LINUX
	constexpr REint x11VirtualFromKey(Keyboard key) {
		switch (key) {
			case Keyboard::Space:
				return XK_space;
			case Keyboard::Slash:
				return XK_slash;
			case Keyboard::Backslash:
				return XK_backslash;
			case Keyboard::Comma:
				return XK_comma;
			case Keyboard::Period:
				return XK_period;
			case Keyboard::Semicolon:
				return XK_semicolon;
			case Keyboard::Apostrophe:
				return XK_apostrophe;
			case Keyboard::Accent:
				return XK_grave;
			case Keyboard::Hashtag:
				return XK_numbersign;
			case Keyboard::Left_Bracket:
				return XK_bracketleft;
			case Keyboard::Right_Bracket:
				return XK_bracketright;
			case Keyboard::Equals:
				return XK_equal;
			case Keyboard::Minus:
				return XK_minus;
			case Keyboard::Right_Ctrl:
				return XK_Control_R;
			case Keyboard::Left_Ctrl:
				return XK_Control_L;
			case Keyboard::Right_Alt:
				return XK_Alt_R;
			case Keyboard::Left_Alt:
				return XK_Alt_L;
			case Keyboard::Right_Shift:
				return XK_Shift_R;
			case Keyboard::Left_Shift:
				return XK_Shift_L;
			case Keyboard::Tab:
				return XK_Tab;
			case Keyboard::Enter:
				return XK_Return;
			case Keyboard::Escape:
				return XK_Escape;
			case Keyboard::Backspace:
				return XK_BackSpace;
			case Keyboard::Arrow_Left:
				return XK_Left;
			case Keyboard::Arrow_Up:
				return XK_Up;
			case Keyboard::Arrow_Down:
				return XK_Down;
			case Keyboard::Arrow_Right:
				return XK_Right;
			case Keyboard::Delete:
				return XK_Delete;
			case Keyboard::Insert:
				return XK_Insert;
			case Keyboard::Home:
				return XK_Home;
			case Keyboard::End:
				return XK_End;
			case Keyboard::Print_Screen:
				return XK_Print;
			case Keyboard::Scroll_Lock:
				return XK_Scroll_Lock;
			case Keyboard::Pause:
				return XK_Pause;
			case Keyboard::Page_Up:
				return XK_Page_Up;
			case Keyboard::Page_Down:
				return XK_Page_Down;
			case Keyboard::Caps_Lock:
				return XK_Caps_Lock;
			case Keyboard::Numpad_Lock:
				return XK_Num_Lock;
			case Keyboard::Numpad_Add:
				return XK_KP_Add;
			case Keyboard::Numpad_Subtract:
				return XK_KP_Subtract;
			case Keyboard::Numpad_Multiply:
				return XK_KP_Multiply;
			case Keyboard::Numpad_Divide:
				return XK_KP_Divide;
			case Keyboard::Numpad_Enter:
				return XK_KP_Enter;
			case Keyboard::Numpad_Period:
				return XK_KP_Decimal;
			default:
				REushort keyId = static_cast<REushort>(key);
				if (keyId >= static_cast<REushort>(Keyboard::A) && keyId <= static_cast<REushort>(Keyboard::Z))
					return XK_a + (keyId - static_cast<REushort>(Keyboard::A));
				if (keyId >= static_cast<REushort>(Keyboard::Top_0) && keyId <= static_cast<REushort>(Keyboard::Top_9))
					return XK_0 + (keyId - static_cast<REushort>(Keyboard::Top_0));
				if (keyId >= static_cast<REushort>(Keyboard::F1) && keyId <= static_cast<REushort>(Keyboard::F25))
					return XK_F1 + (keyId - static_cast<REushort>(Keyboard::F1));
				if (keyId >= static_cast<REushort>(Keyboard::Numpad_0) && keyId <= static_cast<REushort>(Keyboard::Numpad_9))
					return XK_KP_0 + (keyId - static_cast<REushort>(Keyboard::Numpad_0));
				return 0;
		}
	}

	constexpr Keyboard x11KeyFromVirtual(REint vkCode) {
		switch (vkCode) {
			case XK_space:
				return Keyboard::Space;
			case XK_BackSpace:
				return Keyboard::Backspace;
			case XK_Tab:
				return Keyboard::Tab;
			case XK_Return:
				return Keyboard::Enter;
			case XK_Pause:
				return Keyboard::Pause;
			case XK_Scroll_Lock:
				return Keyboard::Scroll_Lock;
			case XK_Escape:
				return Keyboard::Escape;
			case XK_Delete:
				return Keyboard::Delete;
			case XK_Home:
				return Keyboard::Home;
			case XK_Left:
				return Keyboard::Arrow_Left;
			case XK_Up:
				return Keyboard::Arrow_Up;
			case XK_Right:
				return Keyboard::Arrow_Right;
			case XK_Down:
				return Keyboard::Arrow_Down;
			case XK_Page_Up:
				return Keyboard::Page_Up;
			case XK_Page_Down:
				return Keyboard::Page_Down;
			case XK_Insert:
				return Keyboard::Insert;
			case XK_End:
				return Keyboard::End;
			case XK_Print:
				return Keyboard::Print_Screen;
			case XK_Num_Lock:
				return Keyboard::Numpad_Lock;
			case XK_Shift_L:
				return Keyboard::Left_Shift;
			case XK_Shift_R:
				return Keyboard::Right_Shift;
			case XK_Control_L:
				return Keyboard::Left_Ctrl;
			case XK_Control_R:
				return Keyboard::Right_Ctrl;
			case XK_Caps_Lock:
				return Keyboard::Caps_Lock;
			case XK_Alt_L:
				return Keyboard::Left_Alt;
			case XK_Alt_R:
				return Keyboard::Right_Alt;
			case XK_slash:
				return Keyboard::Slash;
			case XK_backslash:
				return Keyboard::Backslash;
			case XK_comma:
				return Keyboard::Comma;
			case XK_period:
				return Keyboard::Period;
			case XK_semicolon:
				return Keyboard::Semicolon;
			case XK_apostrophe:
				return Keyboard::Apostrophe;
			case XK_grave:
				return Keyboard::Accent;
			case XK_numbersign:
				return Keyboard::Hashtag;
			case XK_bracketleft:
				return Keyboard::Left_Bracket;
			case XK_bracketright:
				return Keyboard::Right_Bracket;
			case XK_equal:
				return Keyboard::Equals;
			case XK_minus:
				return Keyboard::Minus;
			case XK_KP_Add:
				return Keyboard::Numpad_Add;
			case XK_KP_Subtract:
				return Keyboard::Numpad_Subtract;
			case XK_KP_Multiply:
				return Keyboard::Numpad_Multiply;
			case XK_KP_Divide:
				return Keyboard::Numpad_Divide;
			case XK_KP_Enter:
				return Keyboard::Numpad_Enter;
			case XK_KP_Separator:
			case XK_KP_Decimal:
				return Keyboard::Numpad_Period;
			default:
				if (vkCode >= XK_a && vkCode <= XK_z)
					return static_cast<Keyboard>(vkCode - XK_a + static_cast<REint>(Keyboard::A));
				if (vkCode >= XK_A && vkCode <= XK_Z)
					return static_cast<Keyboard>(vkCode - XK_A + static_cast<REint>(Keyboard::A));
				if (vkCode >= XK_0 && vkCode <= XK_9)
					return static_cast<Keyboard>(vkCode - XK_0 + static_cast<REint>(Keyboard::Top_0));
				if (vkCode >= XK_F1 && vkCode <= XK_F25)
					return static_cast<Keyboard>(vkCode - XK_F1 + static_cast<REint>(Keyboard::F1));
				if (vkCode >= XK_KP_0 && vkCode <= XK_KP_9)
					return static_cast<Keyboard>(vkCode - XK_KP_9 + static_cast<REint>(Keyboard::Numpad_0));
				RE_WARNING("An unknown X11 virtual key code has been passed");
				return Keyboard::Space;
		}
	}
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

	REint scancodeFromKey(Keyboard key);
	Keyboard keyFromScancode(REint scancode);

	bool isKeyDown(Keyboard key);
	bool isKeyPressed(Keyboard key);
	bool isKeyReleased(Keyboard key);

	bool isButtonDown(MouseButton button);
	bool isButtonPressed(MouseButton button);
	bool isButtonReleased(MouseButton button);

	bool isScrolling();
	bool isScrollingUpward();
	bool isScrollingDownward();
	REbyte scrollDirection();

	Vector<REint, 2> cursorPos();
	REint cursorPosX();
	REint cursorPosY();
	Vector<REint, 2> cursorDeltaPos();
	REint cursorDeltaPosX();
	REint cursorDeltaPosY();
	Vector<float, 2> normalCursorPos();
	float normalCursorPosX();
	float normalCursorPosY();
	Vector<float, 2> normalCursorDeltaPos();
	float normalCursorDeltaPosX();
	float normalCursorDeltaPosY();

}

#endif
