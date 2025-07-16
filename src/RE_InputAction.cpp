#include "RE_Internal Header.hpp"
#include "RE_Input.hpp"

namespace RE {
	
	InputAction::InputAction() : u32KeyScancode(0U), eInput(RE_INPUT_UNKNOWN) {}
	InputAction::InputAction(const Input eInput) : u32KeyScancode(map_input_to_scancode(eInput)), eInput(eInput) {}
	InputAction::InputAction(const uint32_t u32KeyScancode) : u32KeyScancode(u32KeyScancode), eInput(map_scancode_to_input(u32KeyScancode)) {}
	InputAction::~InputAction() {
		if (is_updating())
			pUpdateInputObject = nullptr;
	}

	[[nodiscard]]
	bool InputAction::is_scroll_wheel() const {
		return eInput == RE_INPUT_SCROLL_UP || eInput == RE_INPUT_SCROLL_DOWN;
	}

	[[nodiscard]]
	bool InputAction::is_button() const {
		return eInput >= RE_INPUT_BUTTON_LEFT && eInput <= RE_INPUT_BUTTON_MIDDLE;
	}

	[[nodiscard]]
	bool InputAction::is_key() const {
		return u32KeyScancode || (eInput >= RE_INPUT_KEY_SPACE && eInput < RE_INPUT_MAX_ENUM);
	}

	void InputAction::update_input() {
		if (can_update())
			pUpdateInputObject = this;
	}

	[[nodiscard]]
	bool InputAction::is_updating() const {
		return pUpdateInputObject == this;
	}

	void InputAction::cancel_update() const {
		if (is_updating())
			pUpdateInputObject = nullptr;
	}

	[[nodiscard]]
	bool InputAction::can_update() {
		return !pUpdateInputObject;
	}

	void InputAction::change_input(const Input eNewInput) {
		if (eInput == eNewInput)
			return;
		eInput = eNewInput;
		u32KeyScancode = map_input_to_scancode(eNewInput);
	}

	void InputAction::change_scancode(const uint32_t u32NewScancode) {
		if (u32KeyScancode == u32NewScancode)
			return;
		u32KeyScancode = u32NewScancode;
		eInput = map_scancode_to_input(u32NewScancode);
	}

	[[nodiscard]]
	bool InputAction::is_pressed() const {
		return has_valid_input_values() ? (is_key_down(eInput, u32KeyScancode) && !was_key_down(eInput, u32KeyScancode)) : false;
	}

	[[nodiscard]]
	bool InputAction::is_down() const {
		return has_valid_input_values() ? is_key_down(eInput, u32KeyScancode) : false;
	}

	[[nodiscard]]
	bool InputAction::is_released() const {
		return has_valid_input_values() ? (!is_key_down(eInput, u32KeyScancode) && was_key_down(eInput, u32KeyScancode)) : false;
	}

	void InputAction::reset_input_state() const {
		reset_input_at(eInput, u32KeyScancode);
	}

	[[nodiscard]]
	bool InputAction::has_valid_input_values() const {
		return u32KeyScancode || (eInput > RE_INPUT_UNKNOWN && eInput < RE_INPUT_MAX_ENUM);
	}

}
