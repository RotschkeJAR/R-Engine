#include "RE_Internal Header.hpp"
#include "RE_Input.hpp"

namespace RE {
	
	InputAction::InputAction() : u32KeyScancode(0), eInput(RE_INPUT_UNKNOWN) {}
	InputAction::InputAction(const Input eInput) : u32KeyScancode(0), eInput(eInput) {}
	InputAction::InputAction(const uint32_t u32KeyScancode) : u32KeyScancode(u32KeyScancode), eInput(RE_INPUT_UNKNOWN) {}
	InputAction::~InputAction() {
		if (is_updating())
			pUpdateInputObject = nullptr;
	}

	[[nodiscard]]
	bool InputAction::is_scroll_wheel() const {
		return is_scroll_input(eInput);
	}

	[[nodiscard]]
	bool InputAction::is_button() const {
		return is_button_input(eInput);
	}

	[[nodiscard]]
	bool InputAction::is_mouse() const {
		return is_mouse_input(eInput);
	}

	[[nodiscard]]
	bool InputAction::is_key() const {
		return is_key_input(eInput) || (u32KeyScancode && map_scancode_to_input(u32KeyScancode) != RE_INPUT_UNKNOWN);
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

	void InputAction::change_to_input(const Input eNewInput) {
		eInput = eNewInput;
		u32KeyScancode = 0;
	}

	void InputAction::change_to_scancode(const uint32_t u32NewScancode) {
		u32KeyScancode = u32NewScancode;
		eInput = RE_INPUT_UNKNOWN;
	}

	[[nodiscard]]
	bool InputAction::is_pressed() const {
		return RE::is_pressed(eInput, u32KeyScancode);
	}

	[[nodiscard]]
	bool InputAction::is_down() const {
		return RE::is_down(eInput, u32KeyScancode);
	}

	[[nodiscard]]
	bool InputAction::was_down() const {
		return RE::was_down(eInput, u32KeyScancode);
	}

	[[nodiscard]]
	bool InputAction::is_released() const {
		return RE::is_released(eInput, u32KeyScancode);
	}

	[[nodiscard]]
	bool InputAction::is_held_down() const {
		return RE::is_held_down(eInput, u32KeyScancode);
	}

	void InputAction::reset_input_state() const {
		reset_input_at(eInput, u32KeyScancode);
	}

	[[nodiscard]]
	bool InputAction::has_valid_input_values() const {
		return u32KeyScancode || (eInput > RE_INPUT_UNKNOWN && eInput < RE_INPUT_MAX_ENUM);
	}

}
