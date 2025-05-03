#include "RE_Window.hpp"
#include "RE_Main.hpp"

#include <cstring>

namespace RE {

#ifdef RE_OS_LINUX
	Window_X11::Window_X11() : x11_colormap(0), x11_hWindow(0), x11_hClose(0), x11_hUTF8(0), x11_hWindowName(0), x11_pSizes(XAllocSizeHints()), x11_pDisplay(XOpenDisplay(nullptr)) {
		if (!x11_pDisplay) {
			RE_ERROR("Unable to connect to X11 server");
			return;
		}
		int32_t i32DefaultScreen;
		CATCH_SIGNAL(i32DefaultScreen = XDefaultScreen(x11_pDisplay));
		XWindow x11_rootWindow;
		CATCH_SIGNAL(x11_rootWindow = XDefaultRootWindow(x11_pDisplay));
		XScreen* x11_pDefaultScreen;
		CATCH_SIGNAL(x11_pDefaultScreen = XScreenOfDisplay(x11_pDisplay, i32DefaultScreen));

		XVisualInfo x11_visualInfo;
		int32_t i32VisualsCount = 0;
		XVisualInfo x11_visualTemplate = {};
		x11_visualTemplate.screen = i32DefaultScreen;
		x11_visualTemplate.c_class = TrueColor;
		XVisualInfo* x11_availableVisualInfos;
		CATCH_SIGNAL(x11_availableVisualInfos = XGetVisualInfo(x11_pDisplay, VisualScreenMask | VisualClassMask, &x11_visualTemplate, &i32VisualsCount));
		if (!i32VisualsCount) {
			RE_ERROR("No visual information available for X11 window creation");
			return;
		}
		x11_visualInfo = x11_availableVisualInfos[0];
		CATCH_SIGNAL(XFree(x11_availableVisualInfos));

		XSetWindowAttributes winAttrib = {};
		CATCH_SIGNAL(x11_colormap = XCreateColormap(x11_pDisplay, x11_rootWindow, x11_visualInfo.visual, AllocNone));
		winAttrib.colormap = x11_colormap;
		winAttrib.border_pixel = 0;
		winAttrib.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ResizeRedirectMask | SubstructureNotifyMask;
		CATCH_SIGNAL(x11_hWindow = XCreateWindow(x11_pDisplay, x11_rootWindow, 0, 0, size[0], size[1], 0, x11_visualInfo.depth, InputOutput, x11_visualInfo.visual, CWColormap | CWEventMask, &winAttrib));

		x11_pSizes->flags = PMinSize | PMaxSize;
		x11_pSizes->min_width = 100;
		x11_pSizes->min_height = 100;
		CATCH_SIGNAL(x11_pSizes->max_width = XWidthOfScreen(x11_pDefaultScreen) - 100);
		CATCH_SIGNAL(x11_pSizes->max_height = XHeightOfScreen(x11_pDefaultScreen) - 100);
		CATCH_SIGNAL(XSetWMNormalHints(x11_pDisplay, x11_hWindow, x11_pSizes));

		CATCH_SIGNAL(x11_hClose = XInternAtom(x11_pDisplay, "WM_DELETE_WINDOW", XFalse));
		CATCH_SIGNAL(XSetWMProtocols(x11_pDisplay, x11_hWindow, &x11_hClose, 1));
		CATCH_SIGNAL(XSetLocaleModifiers(""));
		CATCH_SIGNAL(x11_hUTF8 = XInternAtom(x11_pDisplay, "UTF8_STRING", XFalse));
		CATCH_SIGNAL(x11_hWindowName = XInternAtom(x11_pDisplay, "_NET_WM_NAME", XFalse));

		CATCH_SIGNAL(x11_hInputMethod = XOpenIM(x11_pDisplay, nullptr, nullptr, nullptr));
		if (!x11_hInputMethod) {
			RE_ERROR("Failed creating X11 input method");
			return;
		}
		CATCH_SIGNAL(x11_hInputContext = XCreateIC(x11_hInputMethod, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, x11_hWindow, nullptr));
		if (!x11_hInputContext) {
			RE_ERROR("Failed creating X11 input context");
			return;
		}
		CATCH_SIGNAL(internal_update_title());
		bValid = true;
	}

	Window_X11::~Window_X11() {
		CATCH_SIGNAL(XFree(x11_pSizes));
		if (!x11_pDisplay)
			return;
		if (x11_hWindow) {
			CATCH_SIGNAL(XFreeColormap(x11_pDisplay, x11_colormap));
			CATCH_SIGNAL(XDestroyWindow(x11_pDisplay, x11_hWindow));
			if (x11_hInputContext) {
				CATCH_SIGNAL(XDestroyIC(x11_hInputContext));
				if (x11_hInputMethod) {
					CATCH_SIGNAL(XCloseIM(x11_hInputMethod));
				}
			}
		}
		CATCH_SIGNAL(XCloseDisplay(x11_pDisplay));
	}

	void Window_X11::internal_show_window() {
		if (bVisible)
			CATCH_SIGNAL(XMapWindow(x11_pDisplay, x11_hWindow));
		else
			CATCH_SIGNAL(XUnmapWindow(x11_pDisplay, x11_hWindow));
	}

	void Window_X11::internal_update_title() {
		CATCH_SIGNAL(XChangeProperty(x11_pDisplay, x11_hWindow, x11_hWindowName, x11_hUTF8, 8, PropModeReplace, reinterpret_cast<const unsigned char*>(pcTitle), std::strlen(pcTitle)));
	}

	void Window_X11::internal_window_proc() {
		int32_t i32PendingEvents = CATCH_SIGNAL_AND_RETURN(XPending(x11_pDisplay), int32_t);
		while (i32PendingEvents > 0) {
			i32PendingEvents--;
			XEvent x11_event = {};
			CATCH_SIGNAL(XNextEvent(x11_pDisplay, &x11_event));
			switch (x11_event.type) {
				case XClientMessage:
					if (static_cast<XAtom>(x11_event.xclient.data.l[0]) == x11_hClose)
						bCloseFlag = true;
					break;
				case XKeyPress:
				case XKeyRelease: {
					XKeyEvent x11_keyEvent = x11_event.xkey;
					bool bKeyPressed = x11_keyEvent.type == XKeyPress;
					XKeyCode x11_scancode = x11_keyEvent.keycode;
					char cString[5];
					std::fill(std::begin(cString), std::end(cString), '\0');
					XKeySym x11_keySym;
					CATCH_SIGNAL(x11_keySym = XLookupKeysym(&x11_keyEvent, 0));
					uint8_t u8CharLength;
					CATCH_SIGNAL(u8CharLength = Xutf8LookupString(x11_hInputContext, &x11_keyEvent, cString, sizeof(cString) - 1, &x11_keySym, nullptr));
					if (bKeyPressed && u8CharLength)
						cString[u8CharLength] = '\0';
					CATCH_SIGNAL(inputMgr.input_event(key_from_virtual_keycode(static_cast<int64_t>(x11_keySym)), static_cast<uint32_t>(x11_scancode), bKeyPressed, false));
					} break;
				case XButtonPress:
				case XButtonRelease: {
					XButtonEvent x11_buttonEvent = x11_event.xbutton;
					bool bButtonPressed = x11_buttonEvent.type == XButtonPress;
					switch (x11_buttonEvent.button) {
						case Button1: /* left click */
							CATCH_SIGNAL(inputMgr.input_event(RE_INPUT_BUTTON_LEFT, 0U, bButtonPressed, false));
							break;
						case Button2: /* middle click */
							CATCH_SIGNAL(inputMgr.input_event(RE_INPUT_BUTTON_MIDDLE, 0U, bButtonPressed, false));
							break;
						case Button3: /* right click */
							CATCH_SIGNAL(inputMgr.input_event(RE_INPUT_BUTTON_RIGHT, 0U, bButtonPressed, false));
							break;
						case Button4: /* up scroll */
							CATCH_SIGNAL(inputMgr.input_event(RE_INPUT_SCROLL_UP, 0U, true, false));
							break;
						case Button5: /* down scroll */
							CATCH_SIGNAL(inputMgr.input_event(RE_INPUT_SCROLL_DOWN, 0U, true, false));
							break;
						default:
							break;
					}
					} break;
				case XMotionNotify: { /* mouse moved */
					XMotionEvent x11_motionEvent = x11_event.xmotion;
					CATCH_SIGNAL(inputMgr.cursor_event(x11_motionEvent.x, x11_motionEvent.y));
					} break;
				case XMapNotify: /* window showed or unminimized/restored */
					if (bRunning)
						bMinimized = false;
					break;
				case XUnmapNotify: /* window hidden or minimized/iconified */
					if (bRunning)
						bMinimized = true;
					break;
				case XResizeRequest: { /* window resized */
					XResizeRequestEvent x11_resizeEvent = x11_event.xresizerequest;
					CATCH_SIGNAL(update_window_size(static_cast<uint32_t>(x11_resizeEvent.width), static_cast<uint32_t>(x11_resizeEvent.height)));
					} break;
				default:
					break;
			}
		}
	}

	bool Window_X11::create_vulkan_surface(VkSurfaceKHR &vk_rhSurface) const {
		VkXlibSurfaceCreateInfoKHR vk_x11SurfaceCreateInfo = {};
		vk_x11SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
		vk_x11SurfaceCreateInfo.dpy = x11_pDisplay;
		vk_x11SurfaceCreateInfo.window = x11_hWindow;
		return CHECK_VK_RESULT(vkCreateXlibSurfaceKHR(RE_VK_INSTANCE, &vk_x11SurfaceCreateInfo, nullptr, &vk_rhSurface));
	}

	const char* Window_X11::get_vulkan_required_surface_extension_name() const {
		return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
	}
#endif /* RE_OS_LINUX */

}
