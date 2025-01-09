#include "RE_Window_X11.hpp"

#include <cstring>

namespace RE {

	Window_X11::Window_X11() : xDisplay(XOpenDisplay(nullptr)), xWindow(0), xaClose(0), xaUTF8(0), xaWinName(0), glxContext(nullptr) {
		if (!xDisplay) {
			RE_ERROR("Unable to connect to X11 server");
			return;
		}
		REint glxAttribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
		REint defaultScreen = DefaultScreen(xDisplay);
		XWindow root = RootWindow(xDisplay, defaultScreen);
		XVisualInfo* visualInfo = glXChooseVisual(xDisplay, defaultScreen, glxAttribs);
		if (!visualInfo) {
			RE_ERROR("No valid visual info struct has been created");
			return;
		}
		XSetWindowAttributes winAttrib;
		winAttrib.colormap = XCreateColormap(xDisplay, root, visualInfo->visual, AllocNone);
		winAttrib.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask;
		xWindow = XCreateWindow(xDisplay, root, 0, 0, 800, 600, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWColormap | CWEventMask, &winAttrib);
		XStoreName(xDisplay, xWindow, "Untitled game window (ASCII)");
		glxContext = glXCreateContext(xDisplay, visualInfo, nullptr, GL_TRUE);
		if (!glxContext) {
			RE_ERROR("No GLX context has been created");
			return;
		}
		xaClose = XInternAtom(xDisplay, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(xDisplay, xWindow, &xaClose, 1);
		xaUTF8 = XInternAtom(xDisplay, "UTF8_STRING", False);
		xaWinName = XInternAtom(xDisplay, "_NET_WM_NAME", False);
		updateTitleInternal();
		glXMakeCurrent(xDisplay, xWindow, glxContext);
		valid = true;
	}

	Window_X11::~Window_X11() {
		if (xDisplay) {
			glXMakeCurrent(xDisplay, None, nullptr);
			if (glxContext)
				glXDestroyContext(xDisplay, glxContext);
			if (xWindow)
				XDestroyWindow(xDisplay, xWindow);
			XCloseDisplay(xDisplay);
		}
	}

	void Window_X11::showInternal() {
		if (windowVisible)
			XMapWindow(xDisplay, xWindow);
		else
			XUnmapWindow(xDisplay, xWindow);
	}

	void Window_X11::updateTitleInternal() {
		XChangeProperty(xDisplay, xWindow, xaWinName, xaUTF8, 8, PropModeReplace, reinterpret_cast<const unsigned char*>(title), strlen(title));
	}

	void Window_X11::processLoop() {
		while (XPending(xDisplay)) {
			XEvent event;
			XNextEvent(xDisplay, &event);
			switch (event.type) {
				case ClientMessage:
					if (static_cast<XAtom>(event.xclient.data.l[0]) == xaClose)
						closeFlag = true;
					break;
				case KeyPress:
					break;
				case KeyRelease:
					break;
				case ButtonPress:
					break;
				case ButtonRelease:
					break;
				default:
					break;
			}
		}
		glXSwapBuffers(xDisplay, xWindow);
	}

	void Window_X11::fullscreen() {

	}

	void Window_X11::windowedFullscreen() {

	}

	void Window_X11::window() {

	}

}
