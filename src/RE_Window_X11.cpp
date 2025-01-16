#include "RE_Window_X11.hpp"

#include <cstring>

namespace RE {

#ifdef RE_OS_LINUX
	Window_X11::Window_X11() : xWindow(0), xaClose(0), xaUTF8(0), xaWinName(0), xDisplay(XOpenDisplay(nullptr)) {
		if (!xDisplay) {
			RE_FATAL_ERROR("Unable to connect to X11 server");
			return;
		}
		REint defaultScreen = DefaultScreen(xDisplay);
		XWindow root = RootWindow(xDisplay, defaultScreen);

		REint visualsCount = 0;
		XVisualInfo visualTemplate = {};
		visualTemplate.screen = defaultScreen;
		visualTemplate.c_class = TrueColor;
		XVisualInfo* availableVisualInfos = XGetVisualInfo(xDisplay, VisualScreenMask | VisualClassMask, &visualTemplate, &visualsCount);
		if (!visualsCount) {
			RE_FATAL_ERROR("No visual information available for X11 window creation");
			return;
		}
		XVisualInfo visualInfo = availableVisualInfos[0];
		XFree(availableVisualInfos);

		XSetWindowAttributes winAttrib;
		winAttrib.colormap = XCreateColormap(xDisplay, root, visualInfo.visual, AllocNone);
		winAttrib.border_pixel = 0;
		winAttrib.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ResizeRedirectMask;
		xWindow = XCreateWindow(xDisplay, root, 0, 0, 800, 600, 0, visualInfo.depth, InputOutput, visualInfo.visual, CWColormap | CWEventMask, &winAttrib);

		xaClose = XInternAtom(xDisplay, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(xDisplay, xWindow, &xaClose, 1);
		XSetLocaleModifiers("");
		xaUTF8 = XInternAtom(xDisplay, "UTF8_STRING", False);
		xaWinName = XInternAtom(xDisplay, "_NET_WM_NAME", False);

		xInputMethod = XOpenIM(xDisplay, nullptr, nullptr, nullptr);
		if (!xInputMethod) {
			RE_FATAL_ERROR("Failed creating X11 input method");
			return;
		}
		xInputContext = XCreateIC(xInputMethod, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, xWindow, nullptr);
		if (!xInputContext) {
			RE_FATAL_ERROR("Failed creating X11 input context");
			return;
		}
		updateTitleInternal();
		valid = true;
	}

	Window_X11::~Window_X11() {
		if (!xDisplay)
			return;
		if (xWindow) {
			XDestroyWindow(xDisplay, xWindow);
			if (xInputContext) {
				XDestroyIC(xInputContext);
				if (xInputMethod)
					XCloseIM(xInputMethod);
			}
		}
		XCloseDisplay(xDisplay);
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
				case XClientMessage:
					if (static_cast<XAtom>(event.xclient.data.l[0]) == xaClose)
						closeFlag = true;
					break;
				case XKeyPress:
				case XKeyRelease: {
					XKeyEvent keyEvent = event.xkey;
					bool keyPressed = !static_cast<bool>(keyEvent.type - 2);
					XKeyCode scancode = keyEvent.keycode;
					char string[5];
					std::fill(std::begin(string), std::end(string), '\0');
					XKeySym keySym = XLookupKeysym(&keyEvent, 0);
					REubyte len = Xutf8LookupString(xInputContext, &keyEvent, string, sizeof(string) - 1, &keySym, nullptr);
					if (keyPressed && len) {
						string[len] = '\0';
						inputMgr.charInput(string);
					}
					inputMgr.keyInput(keySym, static_cast<REushort>(scancode), keyPressed);
					} break;
				case XButtonPress:
				case XButtonRelease: {
					XButtonEvent buttonEvent = event.xbutton;
					bool buttonPressed = !static_cast<bool>(buttonEvent.type - 4);
					switch (buttonEvent.button) {
						case Button1: /* left click */
							inputMgr.buttonInput(RE_LBUTTON, buttonPressed);
							break;
						case Button2: /* middle click */
							inputMgr.buttonInput(RE_MBUTTON, buttonPressed);
							break;
						case Button3: /* right click */
							inputMgr.buttonInput(RE_RBUTTON, buttonPressed);
							break;
						case Button4: /* up scroll */
							inputMgr.scrollInput(0.5f);
							break;
						case Button5: /* down scroll */
							inputMgr.scrollInput(0.5f);
							break;
						default:
							break;
					}
					} break;
				case XMotionNotify: { /* mouse moved */
					XMotionEvent motionEvent = event.xmotion;
					inputMgr.cursorInput(motionEvent.x, motionEvent.y);
					} break;
				default:
					break;
			}
		}
	}
#endif /* RE_OS_LINUX */

}
