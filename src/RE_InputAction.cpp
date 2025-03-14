#include "RE_Internal Header.hpp"
#include "RE_Input.hpp"

namespace RE {
	
	InputAction::InputAction() : u32KeyScancode(0U), eInput(RE_INPUT_UNKNOWN) {}
	InputAction::InputAction(Input eInput) : u32KeyScancode(map_input_to_scancode(eInput)), eInput(eInput) {}
	InputAction::InputAction(REuint u32KeyScancode) : u32KeyScancode(u32KeyScancode), eInput(map_scancode_to_input(u32KeyScancode)) {}
	InputAction::~InputAction() {}

	void InputAction::change_input(Input eNewInput) {
		if (eInput == eNewInput)
			return;
		eInput = eNewInput;
		u32KeyScancode = map_input_to_scancode(eNewInput);
	}

	void InputAction::change_scancode(REuint u32NewScancode) {
		if (u32KeyScancode == u32NewScancode)
			return;
		u32KeyScancode = u32NewScancode;
		eInput = map_scancode_to_input(u32NewScancode);
	}

	bool InputAction::is_pressed() {
		if (!InputMgr::pInstance) {
			RE_NOTE("You can't check whether anything has been pressed, when the engine is not running");
			return false;
		} else if (!has_valid_input_values())
			return false;
		return InputMgr::pInstance->is_down(eInput, u32KeyScancode) && !InputMgr::pInstance->was_down(eInput, u32KeyScancode);
	}

	bool InputAction::is_down() {
		if (!InputMgr::pInstance) {
			RE_NOTE("You can't check whether anything is hold down, when the engine is not running");
			return false;
		} else if (!has_valid_input_values())
			return false;
		return InputMgr::pInstance->is_down(eInput, u32KeyScancode);
	}

	bool InputAction::is_released() {
		if (!InputMgr::pInstance) {
			RE_NOTE("You can't check whether anything has been released, when the engine is not running");
			return false;
		} else if (!has_valid_input_values())
			return false;
		return !InputMgr::pInstance->is_down(eInput, u32KeyScancode) && InputMgr::pInstance->was_down(eInput, u32KeyScancode);
	}

	bool InputAction::has_valid_input_values() {
		return u32KeyScancode || (eInput > RE_INPUT_UNKNOWN && eInput < RE_INPUT_MAX_ENUM);
	}

}
