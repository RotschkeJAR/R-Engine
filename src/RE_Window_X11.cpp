#include "RE_Window_X11.hpp"

#include <cstring>

namespace RE {

#ifdef RE_OS_LINUX
	Window_X11::Window_X11() : x11_hWindow(0), x11_hClose(0), x11_hUTF8(0), x11_hWindowName(0), x11_pSizes(XAllocSizeHints()), x11_pDisplay(XOpenDisplay(nullptr)) {
		if (!x11_pDisplay) {
			RE_FATAL_ERROR("Unable to connect to X11 server");
			return;
		}
		REint i32DefaultScreen = DefaultScreen(x11_pDisplay);
		XWindow root = RootWindow(x11_pDisplay, i32DefaultScreen);

		XVisualInfo x11_visualInfo;
		REint i32VisualsCount = 0;
		XVisualInfo x11_visualTemplate = {};
		x11_visualTemplate.screen = i32DefaultScreen;
		x11_visualTemplate.c_class = TrueColor;
		XVisualInfo* x11_availableVisualInfos = XGetVisualInfo(x11_pDisplay, VisualScreenMask | VisualClassMask, &x11_visualTemplate, &i32VisualsCount);
		if (!i32VisualsCount) {
			RE_FATAL_ERROR("No visual information available for X11 window creation");
			return;
		} else {
			x11_visualInfo = x11_availableVisualInfos[0];
		}
		XFree(x11_availableVisualInfos);

		XSetWindowAttributes winAttrib = {};
		CATCH_SIGNAL(winAttrib.colormap = XCreateColormap(x11_pDisplay, root, x11_visualInfo.visual, AllocNone));
		CATCH_SIGNAL(winAttrib.border_pixel = 0);
		CATCH_SIGNAL(winAttrib.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ResizeRedirectMask);
		CATCH_SIGNAL(x11_hWindow = XCreateWindow(x11_pDisplay, root, 0, 0, size[0], size[1], 0, x11_visualInfo.depth, InputOutput, x11_visualInfo.visual, CWColormap | CWEventMask, &winAttrib));

		x11_pSizes->flags = PMinSize | PMaxSize;
		x11_pSizes->min_width = size[0];
		x11_pSizes->min_height = size[1];
		x11_pSizes->max_width = size[0];
		x11_pSizes->max_height = size[1];
		XSetWMNormalHints(x11_pDisplay, x11_hWindow, x11_pSizes);

		x11_hClose = XInternAtom(x11_pDisplay, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(x11_pDisplay, x11_hWindow, &x11_hClose, 1);
		XSetLocaleModifiers("");
		x11_hUTF8 = XInternAtom(x11_pDisplay, "UTF8_STRING", False);
		x11_hWindowName = XInternAtom(x11_pDisplay, "_NET_WM_NAME", False);

		x11_hInputMethod = XOpenIM(x11_pDisplay, nullptr, nullptr, nullptr);
		if (!x11_hInputMethod) {
			RE_FATAL_ERROR("Failed creating X11 input method");
			return;
		}
		x11_hInputContext = XCreateIC(x11_hInputMethod, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, x11_hWindow, nullptr);
		if (!x11_hInputContext) {
			RE_FATAL_ERROR("Failed creating X11 input context");
			return;
		}
		updateTitleInternal();
		bValid = true;
	}

	Window_X11::~Window_X11() {
		XFree(x11_pSizes);
		if (!x11_pDisplay)
			return;
		if (x11_hWindow) {
			XDestroyWindow(x11_pDisplay, x11_hWindow);
			if (x11_hInputContext) {
				XDestroyIC(x11_hInputContext);
				if (x11_hInputMethod) {
					XCloseIM(x11_hInputMethod);
				}
			}
		}
		XCloseDisplay(x11_pDisplay);
	}

	void Window_X11::showInternal() {
		if (bWindowVisible)
			XMapWindow(x11_pDisplay, x11_hWindow);
		else
			XUnmapWindow(x11_pDisplay, x11_hWindow);
	}

	void Window_X11::updateTitleInternal() {
		XChangeProperty(x11_pDisplay, x11_hWindow, x11_hWindowName, x11_hUTF8, 8, PropModeReplace, reinterpret_cast<const unsigned char*>(pcTitle), strlen(pcTitle));
	}

	void Window_X11::processLoop() {
		while (XPending(x11_pDisplay)) {
			XEvent x11_event = {};
			NextEvent(x11_pDisplay, &x11_event);
			switch (x11_event.type) {
				case XClientMessage:
					if (static_cast<XAtom>(x11_event.xclient.data.l[0]) == x11_hClose)
						bCloseFlag = true;
					break;
				case XKeyPress:
				case XKeyRelease: {
					XKeyEvent x11_keyEvent = x11_event.xkey;
					bool bKeyPressed = !static_cast<bool>(x11_keyEvent.type - 2);
					XKeyCode x11_scancode = x11_keyEvent.keycode;
					char cString[5];
					std::fill(std::begin(cString), std::end(cString), '\0');
					XKeySym x11_keySym = XLookupKeysym(&x11_keyEvent, 0);
					REubyte u8CharLength = Xutf8LookupString(x11_hInputContext, &x11_keyEvent, cString, sizeof(cString) - 1, &x11_keySym, nullptr);
					if (bKeyPressed && u8CharLength) {
						cString[u8CharLength] = '\0';
						CATCH_SIGNAL(inputMgr.charInput(cString));
					}
					CATCH_SIGNAL(inputMgr.keyInput(x11_keySym, static_cast<REuint>(x11_scancode), bKeyPressed));
					} break;
				case XButtonPress:
				case XButtonRelease: {
					XButtonEvent x11_buttonEvent = x11_event.xbutton;
					bool bButtonPressed = !static_cast<bool>(x11_buttonEvent.type - 4);
					switch (x11_buttonEvent.button) {
						case Button1: /* left click */
							inputMgr.buttonInput(RE_LBUTTON, bButtonPressed);
							break;
						case Button2: /* middle click */
							inputMgr.buttonInput(RE_MBUTTON, bButtonPressed);
							break;
						case Button3: /* right click */
							inputMgr.buttonInput(RE_RBUTTON, bButtonPressed);
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
					XMotionEvent x11_motionEvent = x11_event.xmotion;
					inputMgr.cursorInput(x11_motionEvent.x, x11_motionEvent.y);
					} break;
				case XResizeRequest: { /* window resized */
					XResizeRequestEvent x11_resizeEvent = x11_event.xresizerequest;
					CATCH_SIGNAL(updateWindowSize(static_cast<REushort>(x11_resizeEvent.width), static_cast<REushort>(x11_resizeEvent.height)));
					} break;
				default:
					break;
			}
		}
	}
#endif /* RE_OS_LINUX */

}
