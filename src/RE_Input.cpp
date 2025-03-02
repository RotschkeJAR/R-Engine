#include "RE_Input.hpp"
#include "RE_Ext Header.hpp"
#include "RE_Window_X11.hpp"

namespace RE {

#define RE_INPUT_BUFFER_SCROLL_UP 0U
#define RE_INPUT_BUFFER_SCROLL_DOWN 1U

	InputMgr* InputMgr::pInstance = nullptr;

	InputMgr::InputMgr() {
		if (pInstance) {
			RE_FATAL_ERROR("An instance of the class \"InputMgr\" has already been constructed");
			return;
		}
		pInstance = this;
	}

	InputMgr::~InputMgr() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
	}

}
