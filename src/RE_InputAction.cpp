#include "RE_Internal Header.hpp"
#include "RE_Input.hpp"

namespace RE {
	
	InputAction::InputAction() : u32KeyScancode(0U), eInput(RE_INPUT_UNKNOWN) {}
	InputAction::InputAction(Input eInput) : u32KeyScancode(0U), eInput(eInput) {}
	InputAction::InputAction(REuint u32KeyScancode) : u32KeyScancode(u32KeyScancode), eInput(RE_INPUT_UNKNOWN) {}
	InputAction::~InputAction() {}

	void InputAction::changeInput(Input eNewInput, REuint u32NewKeyScancode) {
		eInput = eNewInput;
		u32KeyScancode = u32NewKeyScancode;
		if (u32KeyScancode) {
			RE_NOTE("You cannot use scancodes for checking input at the moment");
			u32KeyScancode = 0U;
		}
	}

	bool InputAction::is_pressed() {
		if (!InputMgr::pInstance) {
			RE_NOTE("You can't check whether anything has been pressed, when the engine is not running");
			return false;
		} else if (!has_valid_input_values())
			return false;
		return false;
	}

	bool InputAction::is_down() {
		if (!InputMgr::pInstance) {
			RE_NOTE("You can't check whether anything is hold down, when the engine is not running");
			return false;
		} else if (!has_valid_input_values())
			return false;
		return false;
	}

	bool InputAction::is_released() {
		if (!InputMgr::pInstance) {
			RE_NOTE("You can't check whether anything has been released, when the engine is not running");
			return false;
		} else if (!has_valid_input_values())
			return false;
		return false;
	}

	bool InputAction::has_valid_input_values() {
		return u32KeyScancode || (eInput > RE_INPUT_UNKNOWN && eInput < RE_INPUT_MAX_ENUM);
	}

}
