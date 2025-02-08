#include "RE_Window.hpp"

namespace RE {
	
	Window* Window::instance = nullptr;

	Window::Window() : title(u8"Untitled game window"), closeFlag(false), valid(false) {
		if (instance) {
			RE_FATAL_ERROR("A window already exists. New window has been discarded");
			return;
		}
		instance = this;
	}

	Window::~Window() {
		if (instance == this)
			instance = nullptr;
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

	Vector<REushort, 2> Window::getSize() {
		Vector<REushort, 2> copySize;
		for (REuint i = 0; i < copySize.getDimensions(); i++)
			copySize[i] = size[i];
		return copySize;
	}

}
