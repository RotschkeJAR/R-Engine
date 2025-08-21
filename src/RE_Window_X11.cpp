#include "RE_Window.hpp"
#include "RE_Window_X11.hpp"
#include "RE_Main.hpp"

#ifdef RE_OS_LINUX

#include <cstring>

namespace RE {

	XDisplay *x11_pDisplay = nullptr;
	XSizeHints *x11_pSizes = nullptr;
	XColormap x11_colormap = 0;
	XWindow x11_hWindow = 0;
	XAtom x11_hClose = 0;
	XAtom x11_hUTF8 = 0, x11_hWindowName = 0;
	XInputMethod x11_hInputMethod = 0;
	XInputContext x11_hInputContext = 0;

	bool x11_create_window() {
		PUSH_TO_CALLSTACKTRACE(x11_pDisplay = XOpenDisplay(nullptr));
		if (x11_pDisplay) {
			PUSH_TO_CALLSTACKTRACE(x11_pSizes = XAllocSizeHints());
			if (x11_pSizes) {
				const int32_t i32DefaultScreen = PUSH_TO_CALLSTACKTRACE_AND_RETURN(XDefaultScreen(x11_pDisplay), int32_t);
				const XWindow x11_hRootWindowOfDefaultScreen = PUSH_TO_CALLSTACKTRACE_AND_RETURN(XRootWindow(x11_pDisplay, i32DefaultScreen), XWindow);
				const Vector2i monitorSize = {
					std::clamp(XDisplayWidth(x11_pDisplay, i32DefaultScreen), MIN_MONITOR_WIDTH_FOR_CALCULATION, MAX_MONITOR_WIDTH_FOR_CALCULATION),
					std::clamp(XDisplayHeight(x11_pDisplay, i32DefaultScreen), MIN_MONITOR_HEIGHT_FOR_CALCULATION, MAX_MONITOR_HEIGHT_FOR_CALCULATION)
				};

				int32_t i32VisualsCount;
				XVisualInfo x11_visualTemplate = {
					.screen = i32DefaultScreen,
					.c_class = TrueColor
				};
				XVisualInfo *const x11_availableVisualInfos = PUSH_TO_CALLSTACKTRACE_AND_RETURN(XGetVisualInfo(x11_pDisplay, VisualScreenMask | VisualClassMask, &x11_visualTemplate, &i32VisualsCount), XVisualInfo*);
				if (i32VisualsCount) {
					PRINT_LN(i32VisualsCount);
					const XVisualInfo x11_visualInfo = x11_availableVisualInfos[0];
					PUSH_TO_CALLSTACKTRACE(XFree(x11_availableVisualInfos));
					XSetWindowAttributes winAttrib = {};
					PUSH_TO_CALLSTACKTRACE(x11_colormap = XCreateColormap(x11_pDisplay, x11_hRootWindowOfDefaultScreen, x11_visualInfo.visual, AllocNone));
					if (x11_colormap) {
						winAttrib.colormap = x11_colormap;
						winAttrib.border_pixel = 0;
						winAttrib.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
						windowSize[0] = monitorSize[0] / 4 * 3;
						windowSize[1] = monitorSize[1] / 4 * 3;
						PUSH_TO_CALLSTACKTRACE(x11_hWindow = XCreateWindow(x11_pDisplay, x11_hRootWindowOfDefaultScreen, (monitorSize[0] - windowSize[0]) / 2, (monitorSize[1] - windowSize[1]) / 2, windowSize[0], windowSize[1], 0, x11_visualInfo.depth, InputOutput, x11_visualInfo.visual, CWColormap | CWEventMask, &winAttrib));
						if (x11_hWindow) {
							x11_pSizes->flags = PMinSize | PMaxSize;
							x11_pSizes->min_width = MIN_WINDOW_WIDTH;
							x11_pSizes->min_height = MIN_WINDOW_HEIGHT;
							PUSH_TO_CALLSTACKTRACE(x11_pSizes->max_width = monitorSize[0] + MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR);
							PUSH_TO_CALLSTACKTRACE(x11_pSizes->max_height = monitorSize[1] + MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR);
							PUSH_TO_CALLSTACKTRACE(XSetWMNormalHints(x11_pDisplay, x11_hWindow, x11_pSizes));
							PUSH_TO_CALLSTACKTRACE(x11_hClose = XInternAtom(x11_pDisplay, "WM_DELETE_WINDOW", XFalse));
							PUSH_TO_CALLSTACKTRACE(XSetWMProtocols(x11_pDisplay, x11_hWindow, &x11_hClose, 1));
							PUSH_TO_CALLSTACKTRACE(XSetLocaleModifiers(""));
							PUSH_TO_CALLSTACKTRACE(x11_hUTF8 = XInternAtom(x11_pDisplay, "UTF8_STRING", XFalse));
							PUSH_TO_CALLSTACKTRACE(x11_hWindowName = XInternAtom(x11_pDisplay, "_NET_WM_NAME", XFalse));
							PUSH_TO_CALLSTACKTRACE(x11_hInputMethod = XOpenIM(x11_pDisplay, nullptr, nullptr, nullptr));
							if (x11_hInputMethod) {
								PUSH_TO_CALLSTACKTRACE(x11_hInputContext = XCreateIC(x11_hInputMethod, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, x11_hWindow, nullptr));
								if (x11_hInputContext) {
									PUSH_TO_CALLSTACKTRACE(x11_update_window_title());
									return true;
								} else
									RE_ERROR("Failed creating X11 input context");
								PUSH_TO_CALLSTACKTRACE(XCloseIM(x11_hInputMethod));
								x11_hInputMethod = 0;
							} else
								RE_ERROR("Failed creating X11 input method");
							PUSH_TO_CALLSTACKTRACE(XDestroyWindow(x11_pDisplay, x11_hWindow));
							x11_hWindow = 0;
							x11_hClose = 0;
							x11_hUTF8 = 0;
							x11_hWindowName = 0;
						} else
							RE_ERROR("Failed to create X11 window");
						PUSH_TO_CALLSTACKTRACE(XFreeColormap(x11_pDisplay, x11_colormap));
						x11_colormap = 0;
					} else
						RE_ERROR("Failed to create X11 colormap");
				} else
					RE_ERROR("No visual information available for X11 window creation");
				PUSH_TO_CALLSTACKTRACE(XFree(x11_availableVisualInfos));
				PUSH_TO_CALLSTACKTRACE(XFree(x11_pSizes));
				x11_pSizes = nullptr;
			} else
				RE_ERROR("Failed to allocate X11 size hints");
			PUSH_TO_CALLSTACKTRACE(XCloseDisplay(x11_pDisplay));
			x11_pDisplay = nullptr;
		} else
			RE_ERROR("Unable to connect to X11 server");
		return false;
	}

	void x11_destroy_window() {
		PUSH_TO_CALLSTACKTRACE(XDestroyIC(x11_hInputContext));
		PUSH_TO_CALLSTACKTRACE(XCloseIM(x11_hInputMethod));
		PUSH_TO_CALLSTACKTRACE(XDestroyWindow(x11_pDisplay, x11_hWindow));
		PUSH_TO_CALLSTACKTRACE(XFreeColormap(x11_pDisplay, x11_colormap));
		PUSH_TO_CALLSTACKTRACE(XFree(x11_pSizes));
		PUSH_TO_CALLSTACKTRACE(XCloseDisplay(x11_pDisplay));
		x11_hInputContext = 0;
		x11_hInputMethod = 0;
		x11_hWindow = 0;
		x11_hClose = 0;
		x11_hUTF8 = 0;
		x11_hWindowName = 0;
		x11_colormap = 0;
		x11_pSizes = nullptr;
		x11_pDisplay = nullptr;
	}

	void x11_show_window() {
		if (are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_VISIBLE_BIT))
			PUSH_TO_CALLSTACKTRACE(XMapWindow(x11_pDisplay, x11_hWindow));
		else
			PUSH_TO_CALLSTACKTRACE(XUnmapWindow(x11_pDisplay, x11_hWindow));
		PUSH_TO_CALLSTACKTRACE(XFlush(x11_pDisplay));
	}

	void x11_update_window_title() {
		PUSH_TO_CALLSTACKTRACE(XChangeProperty(x11_pDisplay, x11_hWindow, x11_hWindowName, x11_hUTF8, 8, PropModeReplace, reinterpret_cast<const unsigned char*>(pacWindowTitle), std::strlen(pacWindowTitle)));
		PUSH_TO_CALLSTACKTRACE(XFlush(x11_pDisplay));
	}

	void x11_window_proc() {
		int32_t i32PendingEvents = PUSH_TO_CALLSTACKTRACE_AND_RETURN(XPending(x11_pDisplay), int32_t);
		while (i32PendingEvents > 0) {
			i32PendingEvents--;
			XEvent x11_event;
			PUSH_TO_CALLSTACKTRACE(XNextEvent(x11_pDisplay, &x11_event));
			switch (x11_event.type) {
				case XClientMessage:
					if (static_cast<XAtom>(x11_event.xclient.data.l[0]) == x11_hClose)
						set_bits<uint8_t>(u8WindowFlagBits, true, WINDOW_CLOSE_FLAG_BIT);
					break;
				case XConfigureNotify: /* window resized */
					{
						const XConfigureEvent x11_configureNotifyEvent = x11_event.xconfigure;
						PUSH_TO_CALLSTACKTRACE(window_resize_event(static_cast<uint32_t>(x11_configureNotifyEvent.width), static_cast<uint32_t>(x11_configureNotifyEvent.height)));
					}
					break;
				case XKeyPress:
				case XKeyRelease:
					{
						XKeyEvent x11_keyEvent = x11_event.xkey;
						const bool bKeyPressed = x11_keyEvent.type == XKeyPress;
						const XKeyCode x11_scancode = x11_keyEvent.keycode;
						XKeySym x11_keySym = PUSH_TO_CALLSTACKTRACE_AND_RETURN(XLookupKeysym(&x11_keyEvent, 0), XKeySym);
						char a5cString[5];
						const uint8_t u8CharLength = PUSH_TO_CALLSTACKTRACE_AND_RETURN(Xutf8LookupString(x11_hInputContext, &x11_keyEvent, a5cString, sizeof(a5cString) - 1, &x11_keySym, nullptr), uint8_t);
						if (bKeyPressed && u8CharLength)
							a5cString[u8CharLength] = '\0';
						PUSH_TO_CALLSTACKTRACE(input_event(key_from_virtual_keycode(static_cast<int64_t>(x11_keySym)), static_cast<uint32_t>(x11_scancode), bKeyPressed, false));
					}
					break;
				case XButtonPress:
				case XButtonRelease: 
					{
						const XButtonEvent x11_buttonEvent = x11_event.xbutton;
						const bool bButtonPressed = x11_buttonEvent.type == XButtonPress;
						switch (x11_buttonEvent.button) {
							case Button1: /* left click */
								PUSH_TO_CALLSTACKTRACE(input_event(RE_INPUT_BUTTON_LEFT, 0, bButtonPressed, false));
								break;
							case Button2: /* middle click */
								PUSH_TO_CALLSTACKTRACE(input_event(RE_INPUT_BUTTON_MIDDLE, 0, bButtonPressed, false));
								break;
							case Button3: /* right click */
								PUSH_TO_CALLSTACKTRACE(input_event(RE_INPUT_BUTTON_RIGHT, 0, bButtonPressed, false));
								break;
							case Button4: /* up scroll */
								PUSH_TO_CALLSTACKTRACE(input_event(RE_INPUT_SCROLL_UP, 0, true, false));
								break;
							case Button5: /* down scroll */
								PUSH_TO_CALLSTACKTRACE(input_event(RE_INPUT_SCROLL_DOWN, 0, true, false));
								break;
							default:
								break;
						}
					}
					break;
				case XMotionNotify: /* mouse moved */
					{
						const XMotionEvent x11_motionEvent = x11_event.xmotion;
						PUSH_TO_CALLSTACKTRACE(cursor_event(x11_motionEvent.x, x11_motionEvent.y));
					}
					break;
				case XMapNotify: /* window showed or unminimized/restored */
					set_bits<uint8_t>(u8WindowFlagBits, false, WINDOW_MINIMIZED_BIT);
					break;
				case XUnmapNotify: /* window hidden or minimized/iconified */
					set_bits<uint8_t>(u8WindowFlagBits, true, WINDOW_MINIMIZED_BIT);
					break;
				default:
					break;
			}
		}
	}

}

#endif /* RE_OS_LINUX */
