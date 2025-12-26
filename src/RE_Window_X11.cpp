#include "RE_Window.hpp"
#include "RE_Window_X11.hpp"
#include "RE_Main.hpp"

#include <cstring>

#ifdef RE_OS_LINUX

namespace RE {

#define RE_EXPECTED_XRANDR_MAJOR_VERSION 1
#define RE_EXPECTED_XRANDR_MINOR_VERSION 5

/**
 * Documentation on X11 atoms:
 * https://specifications.freedesktop.org/wm/latest-single/
 * https://specifications.freedesktop.org/wm/latest/ar01s05.html
 * https://x.org/releases/X11R7.6/doc/xorg-docs/specs/ICCCM/icccm.html
 */
#define RE_X11_ATOM_COUNT 5
#define RE_X11_ATOM_INDEX_CLOSE 0
#define RE_X11_ATOM_INDEX_UTF8 1
#define RE_X11_ATOM_INDEX_NAME 2
#define RE_X11_ATOM_INDEX_STATE 3
#define RE_X11_ATOM_INDEX_FULLSCREEN 4

	enum X11MonitorQuerySupport {
		RE_X11_MONITOR_QUERY_SUPPORT_NONE = 0,
		RE_X11_MONITOR_QUERY_SUPPORT_XRANDR = 1,
		RE_X11_MONITOR_QUERY_SUPPORT_XINERAMA = 2
	};

	Display *x11_pDisplay;
	static XSizeHints *x11_pSizes;
	static Colormap x11_colormap;
	Window x11_hWindow;
	static Atom x11_ahAtoms[RE_X11_ATOM_COUNT];
	static XIM x11_hInputMethod;
	static XIC x11_hInputContext;
	static X11MonitorQuerySupport eMonitorQuerySupport;

	bool x11_create_window() {
		PRINT_DEBUG("Connecting to X11 server");
		x11_pDisplay = XOpenDisplay(nullptr);
		if (x11_pDisplay) {
			PRINT_DEBUG("Allocating X11 window size limits");
			x11_pSizes = XAllocSizeHints();
			if (x11_pSizes) {
				PRINT_DEBUG("Calculating maximum allowed window size by using extent of primary monitor");
				const int32_t i32DefaultScreen = XDefaultScreen(x11_pDisplay);
				const Window x11_hRootWindowOfDefaultScreen = XRootWindow(x11_pDisplay, i32DefaultScreen);

				PRINT_DEBUG("Querying presence of X11-extension Xrandr");
				Vector2u monitorSize;
				int32_t i32EventBase, i32ErrorBase;
				if (XRRQueryExtension(x11_pDisplay, &i32EventBase, &i32ErrorBase) == True) {
					PRINT_DEBUG("Checking version of Xrandr");
					int32_t i32MajorVersion, i32MinorVersion;
					XRRQueryVersion(x11_pDisplay, &i32MajorVersion, &i32MinorVersion);
					if (i32MajorVersion > RE_EXPECTED_XRANDR_MAJOR_VERSION || (i32MajorVersion == RE_EXPECTED_XRANDR_MAJOR_VERSION && i32MinorVersion >= RE_EXPECTED_XRANDR_MINOR_VERSION)) {
						PRINT_DEBUG("Retrieving information about all present monitors with Xrandr");
						int32_t i32MonitorCount;
						XRRMonitorInfo *const x11_paMonitors = XRRGetMonitors(x11_pDisplay, x11_hRootWindowOfDefaultScreen, True, &i32MonitorCount);
						PRINT_DEBUG("Retrieved ", i32MonitorCount, " monitor infos");
						largestMonitorSize.fill(0);
						for (int32_t i32MonitorIndex = 0; i32MonitorIndex < i32MonitorCount; i32MonitorIndex++) {
							PRINT_DEBUG("Traversing monitor info at index ", i32MonitorIndex);
							largestMonitorSize[0] = std::max(largestMonitorSize[0], static_cast<uint32_t>(x11_paMonitors[i32MonitorIndex].width));
							largestMonitorSize[1] = std::max(largestMonitorSize[1], static_cast<uint32_t>(x11_paMonitors[i32MonitorIndex].height));
							if (x11_paMonitors[i32MonitorIndex].primary == True) {
								PRINT_DEBUG("Found primary monitor");
								monitorSize[0] = static_cast<uint32_t>(x11_paMonitors[i32MonitorIndex].width);
								monitorSize[1] = static_cast<uint32_t>(x11_paMonitors[i32MonitorIndex].height);
							}
						}
						XRRFreeMonitors(x11_paMonitors);
						eMonitorQuerySupport = RE_X11_MONITOR_QUERY_SUPPORT_XRANDR;
						goto RE_X11_CONTINUE_CREATING_WINDOW;
					}
					PRINT_DEBUG("Expected Xrandr version was ", RE_EXPECTED_XRANDR_MAJOR_VERSION, ".", RE_EXPECTED_XRANDR_MINOR_VERSION, ", but is ", i32MajorVersion, ".", i32MinorVersion);
				}
				PRINT_DEBUG("Querying presence of X11-extension Xinerama");
				if (XineramaQueryExtension(x11_pDisplay, &i32EventBase, &i32ErrorBase) == True) {
					PRINT_DEBUG("Retrieving information about all present monitors with Xinerama");
					int32_t i32MonitorCount;
					XineramaScreenInfo *const x11_paScreenInfos = XineramaQueryScreens(x11_pDisplay, &i32MonitorCount);
					if (x11_paScreenInfos) {
						PRINT_DEBUG("Retrieved ", i32MonitorCount, " monitor infos");
						for (int32_t i32MonitorIndex = 0; i32MonitorIndex < i32MonitorCount; i32MonitorIndex++) {
							PRINT_DEBUG("Traversing monitor info at index ", i32MonitorIndex);
							largestMonitorSize[0] = std::max(largestMonitorSize[0], static_cast<uint32_t>(x11_paScreenInfos[i32MonitorIndex].width));
							largestMonitorSize[1] = std::max(largestMonitorSize[1], static_cast<uint32_t>(x11_paScreenInfos[i32MonitorIndex].height));
							if (x11_paScreenInfos[i32MonitorIndex].screen_number == 0) {
								PRINT_DEBUG("Found primary monitor");
								monitorSize[0] = static_cast<uint32_t>(x11_paScreenInfos[i32MonitorIndex].width);
								monitorSize[1] = static_cast<uint32_t>(x11_paScreenInfos[i32MonitorIndex].height);
							}
						}
						XFree(x11_paScreenInfos);
						eMonitorQuerySupport = RE_X11_MONITOR_QUERY_SUPPORT_XINERAMA;
						goto RE_X11_CONTINUE_CREATING_WINDOW;
					}
				}
				{ // Last and most unreliable attempt to get extents of monitors
					PRINT_DEBUG("Falling back to default X11 implementation");
					monitorSize[0] = static_cast<uint32_t>(XDisplayWidth(x11_pDisplay, i32DefaultScreen));
					monitorSize[1] = static_cast<uint32_t>(XDisplayHeight(x11_pDisplay, i32DefaultScreen));
					const int32_t i32ScreenCount = XScreenCount(x11_pDisplay);
					for (int32_t i32ScreenIndex = 0; i32ScreenIndex < i32ScreenCount; i32ScreenIndex++) {
						largestMonitorSize[0] = std::max(largestMonitorSize[0], static_cast<uint32_t>(XDisplayWidth(x11_pDisplay, i32ScreenIndex)));
						largestMonitorSize[1] = std::max(largestMonitorSize[1], static_cast<uint32_t>(XDisplayHeight(x11_pDisplay, i32ScreenIndex)));
					}
					eMonitorQuerySupport = RE_X11_MONITOR_QUERY_SUPPORT_NONE;
				}

			RE_X11_CONTINUE_CREATING_WINDOW:
				PRINT_DEBUG("Querying available visuals on X11");
				int32_t i32VisualsCount;
				XVisualInfo x11_visualTemplate = {
					.screen = i32DefaultScreen,
					.c_class = TrueColor
				};
				XVisualInfo *const x11_availableVisualInfos = XGetVisualInfo(x11_pDisplay, VisualScreenMask | VisualClassMask, &x11_visualTemplate, &i32VisualsCount);
				if (i32VisualsCount) {
					PRINT_DEBUG("Selecting first available visuals provided by X11");
					const XVisualInfo x11_visualInfo = x11_availableVisualInfos[0];
					XFree(x11_availableVisualInfos);
					PRINT_DEBUG("Creating X11 colormap");
					x11_colormap = XCreateColormap(x11_pDisplay, x11_hRootWindowOfDefaultScreen, x11_visualInfo.visual, AllocNone);
					if (x11_colormap) {
						PRINT_DEBUG("Creating X11 window");
						XSetWindowAttributes winAttrib = {
							.border_pixel = 0,
							.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
							.colormap = x11_colormap
						};
						windowSize[0] = monitorSize[0] / 5 * 3;
						windowSize[1] = monitorSize[1] / 5 * 3;
						x11_hWindow = XCreateWindow(x11_pDisplay, x11_hRootWindowOfDefaultScreen, (monitorSize[0] - windowSize[0]) / 2, (monitorSize[1] - windowSize[1]) / 2, windowSize[0], windowSize[1], 0, x11_visualInfo.depth, InputOutput, x11_visualInfo.visual, CWColormap | CWEventMask, &winAttrib);
						if (x11_hWindow) {
							PRINT_DEBUG("Applying size limits to X11 window ", x11_hWindow);
							x11_pSizes->flags = PMinSize | PMaxSize;
							x11_pSizes->min_width = MIN_WINDOW_WIDTH;
							x11_pSizes->min_height = MIN_WINDOW_HEIGHT;
							x11_pSizes->max_width = largestMonitorSize[0] + MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR;
							x11_pSizes->max_height = largestMonitorSize[1] + MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR;
							XSetWMNormalHints(x11_pDisplay, x11_hWindow, x11_pSizes);
							PRINT_DEBUG("Setting locale in X11");
							XSetLocaleModifiers("");
							PRINT_DEBUG("Retrieving ", RE_X11_ATOM_COUNT, " atoms of the X11 window");
							const char *apacAtomNames[RE_X11_ATOM_COUNT];
							apacAtomNames[RE_X11_ATOM_INDEX_CLOSE] = "WM_DELETE_WINDOW";
							apacAtomNames[RE_X11_ATOM_INDEX_UTF8] = "UTF8_STRING";
							apacAtomNames[RE_X11_ATOM_INDEX_NAME] = "_NET_WM_NAME";
							apacAtomNames[RE_X11_ATOM_INDEX_STATE] = "_NET_WM_STATE";
							apacAtomNames[RE_X11_ATOM_INDEX_FULLSCREEN] = "_NET_WM_STATE_FULLSCREEN";
							if (XInternAtoms(x11_pDisplay, reinterpret_cast<char**>(&apacAtomNames), RE_X11_ATOM_COUNT, False, reinterpret_cast<Atom*>(&x11_ahAtoms))) {
								PRINT_DEBUG("Applying close-atom on the window");
								XSetWMProtocols(x11_pDisplay, x11_hWindow, &x11_ahAtoms[RE_X11_ATOM_INDEX_CLOSE], 1);
								PRINT_DEBUG("Opening X11 input method");
								x11_hInputMethod = XOpenIM(x11_pDisplay, nullptr, nullptr, nullptr);
								if (x11_hInputMethod) {
									PRINT_DEBUG("Creating X11 input context");
									x11_hInputContext = XCreateIC(x11_hInputMethod, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, x11_hWindow, nullptr);
									if (x11_hInputContext) {
										PRINT_DEBUG("Updating name of displayed X11 window");
										x11_update_window_title();
										return true;
									} else
										RE_ERROR("Failed creating X11 input context");
									PRINT_DEBUG("Closing X11 input method ", x11_hInputMethod, " due to failure initializing the window entirely");
									XCloseIM(x11_hInputMethod);
								} else
									RE_ERROR("Failed creating X11 input method");
							} else
								RE_ERROR("Failed to retrieve atoms of the X11 window");
							PRINT_DEBUG("Destroying X11 window ", x11_hWindow, " due to failure initializing the window entirely");
							XDestroyWindow(x11_pDisplay, x11_hWindow);
						} else
							RE_ERROR("Failed to create X11 window");
						PRINT_DEBUG("Freeing X11 colormap ", x11_colormap, " due to failure initializing the window entirely");
						XFreeColormap(x11_pDisplay, x11_colormap);
					} else
						RE_ERROR("Failed to create X11 colormap");
				} else
					RE_ERROR("No visual information available for X11 window creation");
				PRINT_DEBUG("Freeing X11 window size limits ", x11_pSizes, " due to failure initializing the window entirely");
				XFree(x11_pSizes);
			} else
				RE_ERROR("Failed to allocate X11 size hints");
			PRINT_DEBUG("Disconnecting from X11 server ", x11_pDisplay, " due to failure initializing the window entirely");
			XCloseDisplay(x11_pDisplay);
		} else
			RE_ERROR("Unable to connect to X11 server");
		return false;
	}

	void x11_destroy_window() {
		PRINT_DEBUG("Destroying X11 input context ", x11_hInputContext);
		XDestroyIC(x11_hInputContext);
		PRINT_DEBUG("Closing X11 input method ", x11_hInputMethod);
		XCloseIM(x11_hInputMethod);
		PRINT_DEBUG("Destroying X11 window ", x11_hWindow);
		XDestroyWindow(x11_pDisplay, x11_hWindow);
		PRINT_DEBUG("Freeing X11 colormap ", x11_colormap);
		XFreeColormap(x11_pDisplay, x11_colormap);
		PRINT_DEBUG("Freeing X11 window size limits ", x11_pSizes);
		XFree(x11_pSizes);
		PRINT_DEBUG("Disconnecting from X11 server ", x11_pDisplay);
		XCloseDisplay(x11_pDisplay);
	}

	void x11_show_window() {
		if (are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_VISIBLE_BIT)) {
			PRINT_DEBUG("Showing X11 window ", x11_hWindow);
			XMapWindow(x11_pDisplay, x11_hWindow);
			if (is_fullscreen()) {
				PRINT_DEBUG("Switching window immediatly to fullscreen");
				XEvent x11_fullscreenEvent = {};
				x11_fullscreenEvent.type = ClientMessage;
				x11_fullscreenEvent.xclient.window = x11_hWindow;
				x11_fullscreenEvent.xclient.message_type = x11_ahAtoms[RE_X11_ATOM_INDEX_STATE];
				x11_fullscreenEvent.xclient.format = 32;
				x11_fullscreenEvent.xclient.data.l[0] = _NET_WM_STATE_ADD;
				x11_fullscreenEvent.xclient.data.l[1] = x11_ahAtoms[RE_X11_ATOM_INDEX_FULLSCREEN];
				x11_fullscreenEvent.xclient.data.l[2] = 0;
				x11_fullscreenEvent.xclient.data.l[3] = 1;
				x11_fullscreenEvent.xclient.data.l[4] = 0;
				XSendEvent(x11_pDisplay, XDefaultRootWindow(x11_pDisplay), False, SubstructureRedirectMask | SubstructureNotifyMask, &x11_fullscreenEvent);
			}
		} else {
			PRINT_DEBUG("Hiding X11 window ", x11_hWindow);
			XUnmapWindow(x11_pDisplay, x11_hWindow);
		}
		PRINT_DEBUG("Flushing X11 server ", x11_pDisplay, " to take effect on window's show-status");
		XFlush(x11_pDisplay);
	}

	void x11_update_fullscreen() {
		PRINT_DEBUG("Updating state atom of X11 window to new fullscreen mode");
		XEvent x11_fullscreenEvent = {};
		x11_fullscreenEvent.type = ClientMessage;
		x11_fullscreenEvent.xclient.window = x11_hWindow;
		x11_fullscreenEvent.xclient.message_type = x11_ahAtoms[RE_X11_ATOM_INDEX_STATE];
		x11_fullscreenEvent.xclient.format = 32;
		x11_fullscreenEvent.xclient.data.l[0] = is_fullscreen() ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
		x11_fullscreenEvent.xclient.data.l[1] = x11_ahAtoms[RE_X11_ATOM_INDEX_FULLSCREEN];
		x11_fullscreenEvent.xclient.data.l[2] = 0;
		x11_fullscreenEvent.xclient.data.l[3] = 1;
		x11_fullscreenEvent.xclient.data.l[4] = 0;
		XSendEvent(x11_pDisplay, XDefaultRootWindow(x11_pDisplay), False, SubstructureRedirectMask | SubstructureNotifyMask, &x11_fullscreenEvent);
	}

	void x11_update_window_title() {
		PRINT_DEBUG("Updating property of X11 window ", x11_hWindow, " with atom ", x11_ahAtoms[RE_X11_ATOM_INDEX_NAME], " displaying its name with UTF-8-support atom ", x11_ahAtoms[RE_X11_ATOM_INDEX_UTF8], " and flushing server ", x11_pDisplay);
		XChangeProperty(x11_pDisplay, x11_hWindow, x11_ahAtoms[RE_X11_ATOM_INDEX_NAME], x11_ahAtoms[RE_X11_ATOM_INDEX_UTF8], 8, PropModeReplace, reinterpret_cast<const unsigned char*>(pacWindowTitle), std::strlen(pacWindowTitle));
		XFlush(x11_pDisplay);
	}

	void x11_window_proc() {
		PRINT_DEBUG("Calling X11 window procedure");
		int32_t i32PendingEvents = XPending(x11_pDisplay);
		while (i32PendingEvents > 0) {
			i32PendingEvents--;
			PRINT_DEBUG("Obtaining data of next X11 event. Remaining events after it: ", i32PendingEvents);
			XEvent x11_event;
			XNextEvent(x11_pDisplay, &x11_event);
			switch (x11_event.type) {
				case ClientMessage:
					if (static_cast<Atom>(x11_event.xclient.data.l[0]) == x11_ahAtoms[RE_X11_ATOM_INDEX_CLOSE]) {
						PRINT_DEBUG("Close event of X11 window ", x11_hWindow, " has been received by using its atom ", x11_ahAtoms[RE_X11_ATOM_INDEX_CLOSE]);
						set_bits<uint8_t>(u8WindowFlagBits, true, WINDOW_CLOSE_FLAG_BIT);
					}
					break;
				case ConfigureNotify: /* window resized */
					{
						PRINT_DEBUG("X11 window ", x11_hWindow, " has been resized ");
						const XConfigureEvent &x11_rConfigureNotifyEvent = x11_event.xconfigure;
						window_resize_event(static_cast<uint32_t>(x11_rConfigureNotifyEvent.width), static_cast<uint32_t>(x11_rConfigureNotifyEvent.height));
					}
					break;
				case KeyPress:
				case KeyRelease:
					{
						PRINT_DEBUG("X11 window ", x11_hWindow, " received keyboard input");
						XKeyEvent &x11_rKeyEvent = x11_event.xkey;
						const bool bKeyPressed = x11_rKeyEvent.type == KeyPress;
						const KeyCode x11_scancode = x11_rKeyEvent.keycode;
						KeySym x11_keySym = XLookupKeysym(&x11_rKeyEvent, 0);
						PRINT_DEBUG("Key with scancode ", std::hex, x11_scancode, ", which translates to symbol ", x11_keySym, ", has been received and its new pressed-state is ", bKeyPressed, ". Translating to UTF-8 character");
						char a5cString[5];
						const uint8_t u8CharLength = Xutf8LookupString(x11_hInputContext, &x11_rKeyEvent, a5cString, sizeof(a5cString) - 1, &x11_keySym, nullptr);
						if (bKeyPressed && u8CharLength)
							a5cString[u8CharLength] = '\0';
						PRINT_DEBUG("Firing general input event");
						input_event(key_from_virtual_keycode(static_cast<int64_t>(x11_keySym)), static_cast<uint32_t>(x11_scancode), bKeyPressed, false);
					}
					break;
				case ButtonPress:
				case ButtonRelease: 
					{
						PRINT_DEBUG("X11 window ", x11_hWindow, " received mouse button input");
						const XButtonEvent x11_buttonEvent = x11_event.xbutton;
						const bool bButtonPressed = x11_buttonEvent.type == ButtonPress;
						switch (x11_buttonEvent.button) {
							case Button1: /* left click */
								PRINT_DEBUG("LMB's new pressed-state is ", bButtonPressed);
								input_event(RE_INPUT_BUTTON_LEFT, 0, bButtonPressed, false);
								break;
							case Button2: /* middle click */
								PRINT_DEBUG("MMB's new pressed-state is ", bButtonPressed);
								input_event(RE_INPUT_BUTTON_MIDDLE, 0, bButtonPressed, false);
								break;
							case Button3: /* right click */
								PRINT_DEBUG("RMB's new pressed-state is ", bButtonPressed);
								input_event(RE_INPUT_BUTTON_RIGHT, 0, bButtonPressed, false);
								break;
							case Button4: /* up scroll */
								PRINT_DEBUG("Scrolled up");
								input_event(RE_INPUT_SCROLL_UP, 0, true, false);
								break;
							case Button5: /* down scroll */
								PRINT_DEBUG("Scrolled down");
								input_event(RE_INPUT_SCROLL_DOWN, 0, true, false);
								break;
							default:
								PRINT_DEBUG("Unknown mouse button pressed: ", std::hex, x11_buttonEvent.button);
								break;
						}
					}
					break;
				case MotionNotify: /* mouse moved */
					{
						PRINT_DEBUG("X11 window ", x11_hWindow, " has received mouse movement. Firing general input event");
						const XMotionEvent x11_motionEvent = x11_event.xmotion;
						cursor_event(x11_motionEvent.x, x11_motionEvent.y);
					}
					break;
				case MapNotify: /* window showed or unminimized/restored */
					PRINT_DEBUG("X11 window ", x11_hWindow, " has been showed or unminimized/restored");
					set_bits<uint8_t>(u8WindowFlagBits, false, WINDOW_MINIMIZED_BIT);
					break;
				case UnmapNotify: /* window hidden or minimized/iconified */
					PRINT_DEBUG("X11 window ", x11_hWindow, " has been hidden or minimized/iconified");
					set_bits<uint8_t>(u8WindowFlagBits, true, WINDOW_MINIMIZED_BIT);
					break;
				default:
					PRINT_DEBUG("Unknown event type has been sent to the window: ", std::hex, x11_event.type);
					break;
			}
		}
	}

}

#endif /* RE_OS_LINUX */
