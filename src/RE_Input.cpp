#include "RE_Input.hpp"
#include "RE_Ext Header.hpp"

namespace RE {

	InputMgr* inputMgr = nullptr;

	InputMgr::InputMgr() : buttons(0), lastButtons(0) {
		if (!inputMgr)
			inputMgr = this;
	}

	InputMgr::~InputMgr() {
		if (inputMgr == this)
			inputMgr = nullptr;
	}

	void InputMgr::keyInput(Key key, REushort scancode, bool pressed) {
		if (pressed)
			println(key);
	}

	void InputMgr::charInput(wchar_t character) {
		wprintf(L"Entered character: %c\n", character);
	}

	void InputMgr::buttonInput(REubyte buttoncode, bool pressed) {
		REubyte buttonMask = static_cast<REubyte>(genBitmask(buttoncode));
		if (!pressed) {
			buttonMask = ~buttonMask;
			buttons = buttons & buttonMask;
		} else
			buttons = buttons | buttonMask;
	}

	void InputMgr::cursorInput(REushort x, REushort y) {
		
	}

	void InputMgr::scrollInput(float y) {
		
	}

	void InputMgr::updateInput() {
		lastButtons = buttons;
	}

	REushort scancodeFromKey(Key key) {
#ifdef RE_OS_WINDOWS
		return MapVirtualKeyW(winVirtualFromKey(key), MAPVK_VK_TO_VSC_EX);
#elif defined RE_OS_LINUX
		return 0;
#else
		return 0;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	Key keyFromScancode(REushort scancode) {
#ifdef RE_OS_WINDOWS
		return winKeyFromVirtual(MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK_EX));
#elif defined RE_OS_LINUX
		return Key::Space;
#else
		return Key::Space;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

}
