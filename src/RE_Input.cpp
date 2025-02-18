#include "RE_Input.hpp"
#include "RE_Ext Header.hpp"
#include "RE_Window_X11.hpp"

namespace RE {

	InputMgr* inputMgr = nullptr;

	InputMgr::InputMgr() : u8Buttons(0), u8LastButtons(0), i8scroll(0) {
		if (!inputMgr)
			inputMgr = this;
		std::fill(std::begin(u8Keys), std::end(u8Keys), 0);
		std::fill(std::begin(u8LastKeys), std::end(u8LastKeys), 0);
	}

	InputMgr::~InputMgr() {
		if (inputMgr == this)
			inputMgr = nullptr;
	}

	void InputMgr::keyInput(REulong u64VirtualKeyCode, REushort u16Scancode, bool bPressed) {
		Keyboard eKey = Keyboard::Unknown;
#ifdef RE_OS_WINDOWS
		eKey = winKeyFromVirtual(u64VirtualKeyCode);
#elif defined RE_OS_LINUX
		eKey = x11KeyFromVirtual(u64VirtualKeyCode);
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
		if (eKey == Keyboard::Unknown && bPressed) {
			RE_WARNING(appendStrings("An unknown key (scancode: ", u16Scancode, ", virtual key code: ", u64VirtualKeyCode, ") has been pressed"));
			return;
		}
		REushort u16KeyIndex = static_cast<REushort>(eKey);
		REushort u16KeyBitmask = static_cast<REushort>(1) << (u16KeyIndex - ((u16KeyIndex / 8) * 8));
		u16KeyIndex /= 8;
		if (bPressed)
			u8Keys[u16KeyIndex] |= u16KeyBitmask;
		else
			u8Keys[u16KeyIndex] &= ~u16KeyBitmask;
	}

	void InputMgr::charInput(const char* pcCharacter) {
		println("Entered character: ", pcCharacter);
	}

	void InputMgr::buttonInput(REubyte u8Buttoncode, bool bPressed) {
		REubyte u8ButtonMask = static_cast<REubyte>(genBitmask(u8Buttoncode));
		if (bPressed)
			u8Buttons |= u8ButtonMask;
		else
			u8Buttons &= ~u8ButtonMask;
	}

	void InputMgr::cursorInput(REint i32CursorX, REint i32CursorY) {
		cursorPos[0] = i32CursorX;
		cursorPos[1] = i32CursorY;
	}

	void InputMgr::scrollInput(REbyte i8ScrollY) {
		i8scroll += std::clamp(i8ScrollY, static_cast<REbyte>(-1), static_cast<REbyte>(1));
	}

	void InputMgr::updateInput() {
		u8LastButtons = u8Buttons;
		std::copy(std::begin(u8Keys), std::end(u8Keys), std::begin(u8LastKeys));
		i8scroll = static_cast<REbyte>(0);
		lastCursorPos[0] = cursorPos[0];
		lastCursorPos[1] = cursorPos[1];
	}

	void InputMgr::updateWindowSize(Vector<REushort, 2> updatedSize) {
		for (REubyte i = 0; i < winSize.getDimensions(); i++)
			winSize[i] = updatedSize[i];
	}

	bool InputMgr::isKeyDown(Keyboard eKey) const {
		REushort u16KeyIndex = static_cast<REushort>(eKey);
		return (u8Keys[u16KeyIndex / 8] & genBitmask(u16KeyIndex % 8)) != 0;
	}
	
	bool InputMgr::wasKeyDown(Keyboard eKey) const {
		REushort u16KeyIndex = static_cast<REushort>(eKey);
		return (u8LastKeys[u16KeyIndex / 8] & genBitmask(u16KeyIndex % 8)) != 0;
	}
	
	REbyte InputMgr::getScroll() const {
		return i8scroll;
	}

	bool InputMgr::isButtonDown(MouseButton eButton) const {
		return u8Buttons & genBitmask(static_cast<REubyte>(eButton));
	}

	bool InputMgr::wasButtonDown(MouseButton eButton) const {
		return u8LastButtons & genBitmask(static_cast<REubyte>(eButton));
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

	REushort scancodeFromKey(Keyboard eKey) {
#ifdef RE_OS_WINDOWS
		return MapVirtualKeyW(winVirtualFromKey(eKey), MAPVK_VK_TO_VSC_EX);
#elif defined RE_OS_LINUX
		return XKeysymToKeycode(static_cast<Window_X11*>(Window::instance)->xDisplay, x11VirtualFromKey(eKey));
#else
		RE_WARNING("Scancodes of a key cannot be determined, because the current OS is unknown");
		return 0;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	Keyboard keyFromScancode(REushort u16Scancode) {
#ifdef RE_OS_WINDOWS
		return winKeyFromVirtual(MapVirtualKeyW(u16Scancode, MAPVK_VSC_TO_VK_EX));
#elif defined RE_OS_LINUX
		return x11KeyFromVirtual(XkbKeycodeToKeysym(static_cast<Window_X11*>(Window::instance)->xDisplay, u16Scancode, 0, 0));
#else
		RE_WARNING("Scancodes cannot be used to determine the key, because the current OS is unknown");
		return Keyboard::Unknown;
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */
	}

	bool isKeyDown(Keyboard eKey) {
		return inputMgr->isKeyDown(eKey);
	}

	bool isKeyPressed(Keyboard eKey) {
		return inputMgr->isKeyDown(eKey) && !inputMgr->wasKeyDown(eKey);
	}

	bool isKeyReleased(Keyboard eKey) {
		return !inputMgr->isKeyDown(eKey) && inputMgr->wasKeyDown(eKey);
	}

	bool isButtonDown(MouseButton eButton) {
		return inputMgr->isButtonDown(eButton);
	}

	bool isButtonPressed(MouseButton eButton) {
		return inputMgr->isButtonDown(eButton) && !inputMgr->wasButtonDown(eButton);
	}

	bool isButtonReleased(MouseButton eButton) {
		return !inputMgr->isButtonDown(eButton) && inputMgr->wasButtonDown(eButton);
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
		for (REubyte u8Index = 0; u8Index < normalPos.getDimensions(); u8Index++)
			normalPos[u8Index] = (static_cast<float>(position[u8Index]) / static_cast<float>(inputMgr->winSize[u8Index])) * 2.0f - 1.0f;
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
		for (REubyte u8Index = 0; u8Index < normalDelta.getDimensions(); u8Index++)
			normalDelta[u8Index] = static_cast<float>(deltaPos[u8Index]) / static_cast<float>(inputMgr->winSize[u8Index]);
		return normalDelta;
	}

	float normalCursorDeltaPosX() {
		return normalCursorDeltaPos()[0];
	}

	float normalCursorDeltaPosY() {
		return normalCursorDeltaPos()[1];
	}

}
