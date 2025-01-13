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

	void InputMgr::keyInput(Keyboard key, REushort scancode, bool pressed) {
		REushort keyIndex = static_cast<REushort>(key);
		REushort keyBitmask = static_cast<REushort>(1) << (keyIndex - (keyIndex / 8));
		keyIndex /= 8;
		if (pressed)
			keys[keyIndex] |= keyBitmask;
		else
			keys[keyIndex] &= ~keyBitmask;
	}

	void InputMgr::charInput(wchar_t character) {
		wprintf(L"Entered character: %c\n", character);
	}

	void InputMgr::buttonInput(REubyte buttoncode, bool pressed) {
		REubyte buttonMask = static_cast<REubyte>(genBitmask(buttoncode));
		if (!pressed) {
			buttonMask = ~buttonMask;
			buttons &= buttonMask;
		} else
			buttons |= buttonMask;
	}

	void InputMgr::cursorInput(REushort x, REushort y) {
		
	}

	void InputMgr::scrollInput(REbyte y) {
		scroll += std::clamp(y, static_cast<REbyte>(0), static_cast<REbyte>(1));
	}

	void InputMgr::updateInput() {
		lastButtons = buttons;
		std::copy(std::begin(keys), std::end(keys), std::begin(lastKeys));
		scroll = 0;
	}

	bool InputMgr::isKeyDown(Keyboard key) {
		REushort keyIndex = static_cast<REushort>(key);
		return (keys[keyIndex / 8] & genBitmask(keyIndex % 8)) != 0;
	}
	
	bool InputMgr::wasKeyDown(Keyboard key) {
		REushort keyIndex = static_cast<REushort>(key);
		return (lastKeys[keyIndex / 8] & genBitmask(keyIndex % 8)) != 0;
	}
	
	REubyte InputMgr::getScroll() {
		return scroll;
	}

	bool InputMgr::isButtonDown(MouseButton button) {
		return buttons & genBitmask(static_cast<REubyte>(button));
	}

	bool InputMgr::wasButtonDown(MouseButton button) {
		return lastButtons & genBitmask(static_cast<REubyte>(button));
	}

	REushort scancodeFromKey(Keyboard key) {
#ifdef RE_OS_WINDOWS
		return MapVirtualKeyW(winVirtualFromKey(key), MAPVK_VK_TO_VSC_EX);
#elif defined RE_OS_LINUX
		return 0;
#else
		return 0;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	Keyboard keyFromScancode(REushort scancode) {
#ifdef RE_OS_WINDOWS
		return winKeyFromVirtual(MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK_EX));
#elif defined RE_OS_LINUX
		return Keyboard::Space;
#else
		return Keyboard::Space;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	bool isKeyDown(Keyboard key) {
		return inputMgr->isKeyDown(key);
	}

	bool isKeyPressed(Keyboard key) {
		return inputMgr->isKeyDown(key) && !inputMgr->wasKeyDown(key);
	}

	bool isKeyReleased(Keyboard key) {
		return !inputMgr->isKeyDown(key) && inputMgr->wasKeyDown(key);
	}

	bool isButtonDown(MouseButton button) {
		return inputMgr->isButtonDown(button);
	}

	bool isButtonPressed(MouseButton button) {
		return inputMgr->isButtonDown(button) && !inputMgr->wasButtonDown(button);
	}

	bool isButtonReleased(MouseButton button) {
		return !inputMgr->isButtonDown(button) && inputMgr->wasButtonDown(button);
	}

	bool isScrolling() {
		return inputMgr->getScroll() != 0;
	}

	bool isScrollingUpward() {
		return inputMgr->getScroll() > 0;
	}

	bool isScrollingDownward() {
		return inputMgr->getScroll() < 0;
	}

	REbyte scrollDirection() {
		return inputMgr->getScroll();
	}

}
