#include "RE_Window.hpp"

namespace RE {
	
	Window* Window::winRef = nullptr;

	Window::Window() : title(u8"Untitled game window"), closeFlag(false), valid(false) {
		if (winRef) {
			RE_ERROR("A window already exists. New window has been discarded");
			return;
		}
		winRef = this;
	}

	Window::~Window() {
		if (winRef == this)
			winRef = nullptr;
	}

	void Window::show(bool showWindow) {
		if (windowVisible == showWindow || !valid)
			return;
		windowVisible = showWindow;
		showInternal();
	}

	void Window::setTitle(const char* newTitle) {
		if (title == newTitle || !valid)
			return;
		title = newTitle;
		updateTitleInternal();
	}

	void Window::update() {
		inputMgr.updateInput();
		processLoop();
	}

	bool Window::shouldClose() {
		return closeFlag;
	}

	bool Window::isValid() {
		return valid;
	}

}
