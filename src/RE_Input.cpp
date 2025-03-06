#include "RE_Input.hpp"
#include "RE_Internal Header.hpp"

namespace RE {

	InputMgr* InputMgr::pInstance = nullptr;

	InputMgr::InputMgr() {
		if (pInstance) {
			RE_FATAL_ERROR("An instance of the class \"InputMgr\" has already been constructed");
			return;
		}
		pInstance = this;
		std::fill(std::begin(u8InputBuffer), std::end(u8InputBuffer), 0U);
		std::fill(std::begin(u8PrevInputBuffer), std::end(u8PrevInputBuffer), 0U);
	}

	InputMgr::~InputMgr() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
	}

	void InputMgr::modify_input_buffer(Input eInput, bool bNewState) {
		REint i32Index = static_cast<REint>(eInput);
		set_bit<REubyte>(u8InputBuffer[i32Index], i32Index % 8, bNewState);
	}

	void InputMgr::input_event(Input eInput, REuint u32Scancode, bool bPressed) {
		if (eInput <= RE_INPUT_UNKNOWN || eInput >= RE_INPUT_MAX_ENUM)
			return;
		modify_input_buffer(eInput, bPressed);
	}

	void InputMgr::cursor_event(REint i32X, REint i32Y) {
		cursorPosition[0] = i32X;
		cursorPosition[1] = i32Y;
	}

	void InputMgr::update_input_buffers() {
		std::copy(std::begin(u8InputBuffer), std::end(u8InputBuffer), std::begin(u8PrevInputBuffer));
		prevCursorPosition.copy_from(cursorPosition);
		modify_input_buffer(RE_INPUT_SCROLL_UP, false);
		modify_input_buffer(RE_INPUT_SCROLL_DOWN, false);
	}

}
