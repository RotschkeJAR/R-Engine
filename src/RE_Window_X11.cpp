#include "RE_Window_X11.hpp"

namespace RE {

	Window_X11::Window_X11() : xDisplay(XOpenDisplay(nullptr)) {
		if (!xDisplay) {
			RE_ERROR("Unable to connect to X11 server");
			return;
		}
		valid = true;
		closeFlag = true;
	}

	Window_X11::~Window_X11() {
		if (xDisplay)
			XCloseDisplay(xDisplay);
	}

	void Window_X11::showInternal(bool showWindow) {

	}

	void Window_X11::processLoop() {

	}

	void Window_X11::fullscreen() {

	}

	void Window_X11::windowedFullscreen() {

	}

	void Window_X11::window() {

	}

}
