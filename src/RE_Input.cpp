#include "RE_Input.hpp"
#include "RE_Ext Header.hpp"
#include "RE_Window_X11.hpp"

namespace RE {

	InputMgr* inputMgr = nullptr;

	InputMgr::InputMgr() : buttons(0), lastButtons(0), scroll(0) {
		if (!inputMgr)
			inputMgr = this;
		std::fill(std::begin(keys), std::end(keys), 0);
		std::fill(std::begin(lastKeys), std::end(lastKeys), 0);
	}

	InputMgr::~InputMgr() {
		if (inputMgr == this)
			inputMgr = nullptr;
	}

	void InputMgr::keyInput(REulong vkCode, REushort scancode, bool pressed) {
		Keyboard key = Keyboard::Unknown;
#ifdef RE_OS_WINDOWS
		key = winKeyFromVirtual(vkCode);
#elif defined RE_OS_LINUX
		key = x11KeyFromVirtual(vkCode);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		if (key == Keyboard::Unknown && pressed) {
			RE_WARNING(appendStrings("An unknown key (scancode: ", scancode, ", virtual key code: ", vkCode, ") has been pressed"));
			return;
		}
		REushort keyIndex = static_cast<REushort>(key);
		REushort keyBitmask = static_cast<REushort>(1) << (keyIndex - ((keyIndex / 8) * 8));
		keyIndex /= 8;
		if (pressed)
			keys[keyIndex] |= keyBitmask;
		else
			keys[keyIndex] &= ~keyBitmask;
	}

	void InputMgr::charInput(const char* character) {
		println("Entered character: ", character);
	}

	void InputMgr::buttonInput(REubyte buttoncode, bool pressed) {
		REubyte buttonMask = static_cast<REubyte>(genBitmask(buttoncode));
		if (pressed)
			buttons |= buttonMask;
		else
			buttons &= ~buttonMask;
	}

	void InputMgr::cursorInput(REint x, REint y) {
		cursorPos[0] = x;
		cursorPos[1] = y;
	}

	void InputMgr::scrollInput(REbyte y) {
		scroll += std::clamp(y, static_cast<REbyte>(-1), static_cast<REbyte>(1));
	}

	void InputMgr::updateInput() {
		lastButtons = buttons;
		std::copy(std::begin(keys), std::end(keys), std::begin(lastKeys));
		scroll = 0;
		lastCursorPos[0] = cursorPos[0];
		lastCursorPos[1] = cursorPos[1];
	}

	void InputMgr::updateWinSize(Vector<REushort, 2> updatedSize) {
		for (REubyte i = 0; i < winSize.getDimensions(); i++)
			winSize[i] = updatedSize[i];
	}

	bool InputMgr::isKeyDown(Keyboard key) const {
		REushort keyIndex = static_cast<REushort>(key);
		return (keys[keyIndex / 8] & genBitmask(keyIndex % 8)) != 0;
	}
	
	bool InputMgr::wasKeyDown(Keyboard key) const {
		REushort keyIndex = static_cast<REushort>(key);
		return (lastKeys[keyIndex / 8] & genBitmask(keyIndex % 8)) != 0;
	}
	
	REubyte InputMgr::getScroll() const {
		return scroll;
	}

	bool InputMgr::isButtonDown(MouseButton button) const {
		return buttons & genBitmask(static_cast<REubyte>(button));
	}

	bool InputMgr::wasButtonDown(MouseButton button) const {
		return lastButtons & genBitmask(static_cast<REubyte>(button));
	}

	REint InputMgr::getCursorX() const {
		return cursorPos[0];
	}

	REint InputMgr::getCursorY() const {
		return cursorPos[1];
	}

	REint InputMgr::getCursorLastX() const {
		return lastCursorPos[0];
	}
	
	REint InputMgr::getCursorLastY() const {
		return lastCursorPos[1];
	}

	REushort scancodeFromKey(Keyboard key) {
#ifdef RE_OS_WINDOWS
		return MapVirtualKeyW(winVirtualFromKey(key), MAPVK_VK_TO_VSC_EX);
#elif defined RE_OS_LINUX
		return XKeysymToKeycode(static_cast<Window_X11*>(Window::instance)->xDisplay, x11VirtualFromKey(key));
#else
		RE_WARNING("Scancodes of a key cannot be determined, because the current OS is unknown");
		return 0;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	Keyboard keyFromScancode(REushort scancode) {
#ifdef RE_OS_WINDOWS
		return winKeyFromVirtual(MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK_EX));
#elif defined RE_OS_LINUX
		return x11KeyFromVirtual(XkbKeycodeToKeysym(static_cast<Window_X11*>(Window::instance)->xDisplay, scancode, 0, 0));
#else
		RE_WARNING("Scancodes cannot be used to determine the key, because the current OS is unknown");
		return Keyboard::Unknown;
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

	Vector<REint, 2> cursorPos() {
		Vector<REint, 2> position;
		position[0] = inputMgr->getCursorX();
		position[1] = inputMgr->getCursorY();
		return position;
	}

	REint cursorPosX() {
		return cursorPos()[0];
	}

	REint cursorPosY() {
		return cursorPos()[1];
	}

	Vector<REint, 2> cursorDeltaPos() {
		Vector<REint, 2> delta;
		delta[0] = inputMgr->getCursorX() - inputMgr->getCursorLastX();
		delta[1] = inputMgr->getCursorY() - inputMgr->getCursorLastY();
		return delta;
	}

	REint cursorDeltaPosX() {
		return cursorDeltaPos()[0];
	}

	REint cursorDeltaPosY() {
		return cursorDeltaPos()[1];
	}

	Vector<float, 2> normalCursorPos() {
		Vector<REint, 2> position = cursorPos();
		Vector<float, 2> normalPos;
		for (REubyte i = 0; i < normalPos.getDimensions(); i++)
			normalPos[i] = (static_cast<float>(position[i]) / static_cast<float>(inputMgr->winSize[i])) * 2.0f - 1.0f;
		return normalPos;
	}

	float normalCursorPosX() {
		return normalCursorPos()[0];
	}

	float normalCursorPosY() {
		return normalCursorPos()[1];
	}

	Vector<float, 2> normalCursorDeltaPos() {
		Vector<REint, 2> deltaPos = cursorDeltaPos();
		Vector<float, 2> normalDelta;
		for (REubyte i = 0; i < normalDelta.getDimensions(); i++)
			normalDelta[i] = static_cast<float>(deltaPos[i]) / static_cast<float>(inputMgr->winSize[i]);
		return normalDelta;
	}

	float normalCursorDeltaPosX() {
		return normalCursorDeltaPos()[0];
	}

	float normalCursorDeltaPosY() {
		return normalCursorDeltaPos()[1];
	}

}
