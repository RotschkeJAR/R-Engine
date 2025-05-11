#include "RE_Window.hpp"
#include "RE_Main.hpp"

namespace RE {

#ifdef RE_OS_WINDOWS
# define WINDOW_CLASS_NAME L"RE_WindowClass"
# define WINDOW_STYLE_FLAGS (WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU)
# define RE_WM_MAXIMIZED WM_APP
	HINSTANCE Window_Win64::win_hInstance = nullptr;
	Window_Win64* pWin64 = nullptr;

	LRESULT CALLBACK windows_window_proc(HWND win_hWndParam, UINT win_uMsg, WPARAM win_wParam, LPARAM win_lParam) {
		if (!pWin64)
			RE_ERROR("Window process function has been called when no window is active");
		else if (pWin64->win_hWindow != win_hWndParam && bRunning)
			RE_ERROR(append_to_string("Window process function has been called by another window (actual: ", pWin64->win_hWindow, "; passed to procedure: ", win_hWndParam, ")"));
		else {
			switch (win_uMsg) {
				case WM_SIZE: /* resized */
					RECT win_contentRect;
					CATCH_SIGNAL(GetClientRect(win_hWndParam, &win_contentRect));
					pWin64->bMinimized = win_wParam == SIZE_MINIMIZED;
					pWin64->bMaximized = win_wParam == SIZE_MAXIMIZED;
					PRINT_LN(pWin64->bMaximized);
					if (pWin64->bMaximized) {
						PRINT_LN("Maximized");
						PostMessageW(win_hWndParam, RE_WM_MAXIMIZED, (WPARAM) 0, (LPARAM) 0);
					}
					CATCH_SIGNAL(pWin64->update_window_size(static_cast<uint32_t>(win_contentRect.right - win_contentRect.left), static_cast<uint32_t>(win_contentRect.bottom - win_contentRect.top)));
					return 0;
				case WM_CLOSE: /* close */
					pWin64->bCloseFlag = true;
					return 0;
				case WM_DESTROY: /* destroy window */
					CATCH_SIGNAL(PostQuitMessage(0));
					return 0;
				case WM_GETMINMAXINFO: {
					MINMAXINFO* win_windowSizeLimits = (MINMAXINFO*) win_lParam;
					win_windowSizeLimits->ptMinTrackSize.x = MIN_WINDOW_WIDTH;
					win_windowSizeLimits->ptMinTrackSize.y = MIN_WINDOW_HEIGHT;
					MONITORINFO win_monitorInfo = {};
					win_monitorInfo.cbSize = sizeof(MONITORINFO);
					BOOL win_monitorInfoRetrieved;
					CATCH_SIGNAL(win_monitorInfoRetrieved = GetMonitorInfoW(MonitorFromWindow(win_hWndParam, MONITOR_DEFAULTTOPRIMARY), &win_monitorInfo));
					if (win_monitorInfoRetrieved) {
						const Vector2i monitorSize(win_monitorInfo.rcWork.right - win_monitorInfo.rcWork.left, win_monitorInfo.rcWork.bottom - win_monitorInfo.rcWork.top);
						if (!pWin64->bMaximized) {
							win_windowSizeLimits->ptMaxTrackSize.x = std::abs(monitorSize[0] + MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR);
							win_windowSizeLimits->ptMaxTrackSize.y = std::abs(monitorSize[1] + MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR);
						} else {
							win_windowSizeLimits->ptMaxTrackSize.x = monitorSize[0];
							win_windowSizeLimits->ptMaxTrackSize.y = monitorSize[1];
						}
						PRINT_LN(append_to_string("New maximum window size limit: ", win_windowSizeLimits->ptMaxTrackSize.x, ", ", win_windowSizeLimits->ptMaxTrackSize.y, "\nMonitor size: ", monitorSize));
					} else
						RE_FATAL_ERROR("Failed getting monitor info, where the window is currently on");
					} return 0;
				case WM_KEYDOWN: /* key pressed */
				case WM_SYSKEYDOWN:
				case WM_KEYUP: /* key released */
				case WM_SYSKEYUP: {
					WORD win_virtualKeyCode = LOWORD(win_wParam);
					WORD win_keyFlags = HIWORD(win_lParam);
					BYTE win_scancode = LOBYTE(win_keyFlags);
					WORD win_extScancode = static_cast<WORD>(win_scancode);
					if ((win_keyFlags & KF_EXTENDED) == KF_EXTENDED)
						win_extScancode = MAKEWORD(win_scancode, 0xE0);
					bool bFallbackToInput = false;
					switch (win_virtualKeyCode) {
						case VK_SHIFT: // Have to be differentiated between left and right
						case VK_CONTROL:
						case VK_MENU:
							win_virtualKeyCode = LOWORD(MapVirtualKeyExW(win_extScancode, MAPVK_VSC_TO_VK_EX, pWin64->win_keyboardLayout));
							break;
						case VK_LWIN: // Ignore Windows-keys
						case VK_RWIN:
							return 0;
						case VK_NUMLOCK: // May have different scancode than expected
						case VK_SNAPSHOT:
							bFallbackToInput = true;
							break;
						default:
							break;
					}
					BOOL win_keyReleased = (win_keyFlags & KF_UP) == KF_UP;
					CATCH_SIGNAL(pWin64->inputMgr.input_event(key_from_virtual_keycode(static_cast<int64_t>(win_virtualKeyCode)), static_cast<uint32_t>(win_extScancode), !static_cast<bool>(win_keyReleased), bFallbackToInput));
					} return 0;
				case WM_CHAR: {
					//wchar_t wCharacter[2] = {static_cast<wchar_t>(win_wParam), L'\0'};
					} return 0;
				case WM_LBUTTONDOWN: /* left mouse button pressed */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_LEFT, 0U, true, false));
					CATCH_SIGNAL(SetCapture(win_hWndParam));
					return 0;
				case WM_LBUTTONUP: /* left mouse button released */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_LEFT, 0U, false, false));
					CATCH_SIGNAL(ReleaseCapture());
					return 0;
				case WM_RBUTTONDOWN: /* right mouse button pressed */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_RIGHT, 0U, true, false));
					CATCH_SIGNAL(SetCapture(win_hWndParam));
					return 0;
				case WM_RBUTTONUP: /* right mouse button released */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_RIGHT, 0U, false, false));
					CATCH_SIGNAL(ReleaseCapture());
					return 0;
				case WM_MBUTTONDOWN: /* middle mouse button pressed */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_MIDDLE, 0U, true, false));
					CATCH_SIGNAL(SetCapture(win_hWndParam));
					return 0;
				case WM_MBUTTONUP: /* middle mouse button released */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_MIDDLE, 0U, false, false));
					CATCH_SIGNAL(ReleaseCapture());
					return 0;
				case WM_MOUSEMOVE: { /* mouse moved */
					const int32_t i32XPos = GET_X_LPARAM(win_lParam);
					const int32_t i32YPos = GET_Y_LPARAM(win_lParam);
					CATCH_SIGNAL(pWin64->inputMgr.cursor_event(i32XPos, i32YPos));
					} return 0;
				case WM_SETCURSOR:
					if (LOWORD(win_lParam) == HTCLIENT) {
						CATCH_SIGNAL(SetCursor(pWin64->win_hCursor));
						return TRUE;
					}
					break;
				case WM_MOUSEWHEEL: { /* mouse wheel used/scrolled */
					const int32_t deltaMouseWheel = GET_WHEEL_DELTA_WPARAM(win_wParam);
					if (deltaMouseWheel > 0)
						CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_SCROLL_UP, 0U, true, false));
					else
						CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_SCROLL_DOWN, 0U, true, false));
					} return 0;
				case RE_WM_MAXIMIZED: {
					PRINT_LN("Received custom message");
					MONITORINFO win_monitorInfo = {};
					win_monitorInfo.cbSize = sizeof(MONITORINFO);
					BOOL win_monitorInfoRetrieved;
					CATCH_SIGNAL(win_monitorInfoRetrieved = GetMonitorInfoW(MonitorFromWindow(win_hWndParam, MONITOR_DEFAULTTOPRIMARY), &win_monitorInfo));
					if (!win_monitorInfoRetrieved) {
						RE_FATAL_ERROR("Failed getting monitor info, where the window is currently on");
						return 1;
					}
					const Vector2i monitorSize(win_monitorInfo.rcWork.right - win_monitorInfo.rcWork.left, win_monitorInfo.rcWork.bottom - win_monitorInfo.rcWork.top);
					CATCH_SIGNAL(SetWindowPos(win_hWndParam, nullptr, 0, 0, monitorSize[0], monitorSize[1], SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER));
					} return 0;
				default:
					break;
			}
		}
		LRESULT win_lResult = 0;
		CATCH_SIGNAL(win_lResult = DefWindowProcW(win_hWndParam, win_uMsg, win_wParam, win_lParam));
		return win_lResult;
	}
	
	Window_Win64::Window_Win64() : win_hWindow(nullptr), win_hCursor(LoadCursor(nullptr, IDC_ARROW)), win_keyboardLayout(GetKeyboardLayout(0)) {
		if (pWin64) {
			RE_FATAL_ERROR("An instance of the class \"Window_Win64\" has already been constructed. Discarding new one");
			return;
		}
		pWin64 = this;
		if (!win_hInstance)
			CATCH_SIGNAL(win_hInstance = GetModuleHandle(nullptr));
		if (!win_hInstance) {
			RE_FATAL_ERROR("Failed getting the HINSTANCE for window creation");
			return;
		}
		const std::wstring wideTitleStr = convert_chars_to_wide(pcTitle);
		WNDCLASSEXW win_WinClass = {};
		win_WinClass.cbSize = sizeof(WNDCLASSEXW);
		win_WinClass.lpfnWndProc = windows_window_proc;
		win_WinClass.hInstance = win_hInstance;
		win_WinClass.lpszClassName = WINDOW_CLASS_NAME;
		ATOM win_registeredWndClassAtom;
		CATCH_SIGNAL(win_registeredWndClassAtom = RegisterClassExW(&win_WinClass));
		if (!win_registeredWndClassAtom) {
			RE_FATAL_ERROR("Failed registering class for window creation");
			return;
		};
		RECT win_adjustableSize = {0, 0, static_cast<LONG>(windowSize[0]), static_cast<LONG>(windowSize[1])};
		CATCH_SIGNAL(AdjustWindowRect(&win_adjustableSize, WINDOW_STYLE_FLAGS, FALSE));
		CATCH_SIGNAL(win_hWindow = CreateWindowExW(0, WINDOW_CLASS_NAME, wideTitleStr.c_str(), WINDOW_STYLE_FLAGS, CW_USEDEFAULT, CW_USEDEFAULT, win_adjustableSize.right - win_adjustableSize.left, win_adjustableSize.bottom - win_adjustableSize.top, nullptr, nullptr, win_hInstance, nullptr));
		if (!win_hWindow) {
			RE_FATAL_ERROR("Failed creating window");
			return;
		}
		BOOL win_dpiAwarenessChangedResult;
		CATCH_SIGNAL(win_dpiAwarenessChangedResult = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE));
		if (!win_dpiAwarenessChangedResult) {
			RE_FATAL_ERROR("Failed telling Windows R-Engine's DPI awareness");
			return;
		}
		bValid = true;
	}
	
	Window_Win64::~Window_Win64() {
		if (pWin64 != this)
			return;
		CATCH_SIGNAL(DestroyWindow(win_hWindow));
		CATCH_SIGNAL(UnregisterClassW(WINDOW_CLASS_NAME, win_hInstance));
		pWin64 = nullptr;
	}

	void Window_Win64::internal_show_window() {
		CATCH_SIGNAL(ShowWindow(win_hWindow, bVisible ? TRUE : FALSE));
	}

	void Window_Win64::internal_update_title() {
		BOOL win_success;
		CATCH_SIGNAL(win_success = SetWindowTextW(win_hWindow, convert_chars_to_wide(pcTitle).c_str()));
		if (!win_success)
			RE_FATAL_ERROR("Failed updating the title in the window title bar");
	}

	void Window_Win64::internal_window_proc() {
		BOOL win_msgInQueue;
		MSG win_msg;
		do {
			CATCH_SIGNAL(win_msgInQueue = PeekMessageW(&win_msg, win_hWindow, 0, 0, PM_REMOVE));
			if (!win_msgInQueue || LOWORD(win_msg.message) == WM_QUIT)
				break;
			CATCH_SIGNAL(TranslateMessage(&win_msg));
			CATCH_SIGNAL(DispatchMessageW(&win_msg)); /* calls window procedure */
		} while (win_msgInQueue);
	}

	bool Window_Win64::create_vulkan_surface(VkSurfaceKHR &vk_rhSurface) const {
		VkWin32SurfaceCreateInfoKHR vk_win32SurfaceCreateInfo = {};
		vk_win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		vk_win32SurfaceCreateInfo.hwnd = win_hWindow;
		vk_win32SurfaceCreateInfo.hinstance = win_hInstance;
		return CHECK_VK_RESULT(vkCreateWin32SurfaceKHR(RE_VK_INSTANCE, &vk_win32SurfaceCreateInfo, nullptr, &vk_rhSurface));
	}

	const char* Window_Win64::get_vulkan_required_surface_extension_name() const {
		return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
	}

	HWND Window_Win64::get_hwindow() const {
		return win_hWindow;
	}

	void set_hinstance(HINSTANCE win_hInstance) {
		Window_Win64::win_hInstance = win_hInstance;
	}
#endif /* RE_OS_WINDOWS */

}
