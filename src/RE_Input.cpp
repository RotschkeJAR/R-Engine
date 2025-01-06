#include "RE_Input.hpp"

namespace RE {

	InputMgr* inputMgr = nullptr;

	InputMgr::InputMgr() {
		if (!inputMgr)
			inputMgr = this;
	}

	InputMgr::~InputMgr() {
		if (inputMgr == this)
			inputMgr = nullptr;
	}

	void InputMgr::keyInput(REushort scancode, bool pressed) {
		
	}

	void InputMgr::charInput(wchar_t character) {
		wprintf(L"Entered character: %c\n", character);
	}

	void InputMgr::buttonInput(REubyte buttoncode, bool pressed) {
		
	}

	void InputMgr::cursorInput(REushort x, REushort y) {
		
	}

	void InputMgr::scrollInput(float y) {
		
	}

}
