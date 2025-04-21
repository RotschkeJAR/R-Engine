#include "RE_Internal Header.hpp"
#include "RE_Input.hpp"

namespace RE {
	
	InputAction::InputAction() : u32KeyScancode(0U), eInput(RE_INPUT_UNKNOWN) {}
	InputAction::InputAction(Input eInput) : u32KeyScancode(map_input_to_scancode(eInput)), eInput(eInput) {}
	InputAction::InputAction(REuint u32KeyScancode) : u32KeyScancode(u32KeyScancode), eInput(map_scancode_to_input(u32KeyScancode)) {}
	InputAction::~InputAction() {
		if (is_updating())
			pUpdateInputObject = nullptr;
	}

	bool InputAction::is_scroll_wheel() {
		return eInput == RE_INPUT_SCROLL_UP || eInput == RE_INPUT_SCROLL_DOWN;
	}
	
	bool InputAction::is_button() {
		return eInput >= RE_INPUT_BUTTON_LEFT && eInput <= RE_INPUT_BUTTON_MIDDLE;
	}
	
	bool InputAction::is_key() {
		return u32KeyScancode || (eInput >= RE_INPUT_KEY_SPACE && eInput < RE_INPUT_MAX_ENUM);
	}

	void InputAction::update_input() {
		if (can_update())
			pUpdateInputObject = this;
	}

	bool InputAction::is_updating() {
		return pUpdateInputObject == this;
	}

	void InputAction::cancel_update() {
		if (is_updating())
			pUpdateInputObject = nullptr;
	}

	bool InputAction::can_update() {
		return !pUpdateInputObject;
	}

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
			RE_NOTE("You can't check whether anything is held down, when the engine is not running");
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
