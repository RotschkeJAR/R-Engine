#include "RE_Window.hpp"
#include "RE_Window_Win64.hpp"
#include "RE_Main.hpp"

#ifdef RE_OS_WINDOWS

namespace RE {

# define RE_WIN64_WINDOW_CLASS_NAME L"RE_WindowClass"
# define RE_WIN64_WINDOW_STYLE_FLAGS (WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU)
# define RE_WIN64_WINDOW_EXTENDED_STYLE_FLAGS 0
# define RE_WIN64_WINDOWED_FULLSCREEN_STYLE_FLAGS WS_POPUP
# define RE_WIN64_WINDOWED_FULLSCREEN_EXTENDED_STYLE_FLAGS WS_EX_APPWINDOW
# define RE_WIN64_WM_MAXIMIZED WM_APP

	HINSTANCE win_hInstance = nullptr;
	HWND win_hWindow = nullptr;
	static HMONITOR win_hMonitor = nullptr;
	static HCURSOR win_hCursor;
	static HKL win_keyboardLayout;

	static LRESULT CALLBACK win64_window_proc_callback(const HWND win_hWndParam, const UINT win_uMsg, const WPARAM win_wParam, const LPARAM win_lParam) {
		PRINT_DEBUG("Message ", std::hex, win_uMsg, std::dec, " received for window ", win_hWndParam, " with wide parameter ", std::hex, win_wParam, std::dec, " and long parameter ", std::hex, win_lParam, std::dec);
		if (win_hWindow != win_hWndParam)
			RE_NOTE("Window process function has been called by another window (actual: ", win_hWindow, "; passed to procedure: ", win_hWndParam, ")");
		else {
			switch (win_uMsg) {
				case WM_SIZE: /* resized */
					{
						PRINT_DEBUG("Obtaining extent of the window content area on Windows");
						RECT win_contentRect;
						GetClientRect(win_hWndParam, &win_contentRect);
						set_bits<uint8_t>(u8WindowFlagBits, win_wParam == SIZE_MINIMIZED, WINDOW_MINIMIZED_BIT);
						set_bits<uint8_t>(u8WindowFlagBits, win_wParam == SIZE_MAXIMIZED, WINDOW_MAXIMIZED_BIT);
						if (are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_MAXIMIZED_BIT)) {
							PRINT_DEBUG("Posting custom window message to notify maximization");
							PostMessageW(win_hWndParam, RE_WIN64_WM_MAXIMIZED, static_cast<WPARAM>(0), static_cast<LPARAM>(0));
						}
						PRINT_DEBUG("Firing general window resize event");
						window_resize_event(
								static_cast<uint32_t>(std::abs(win_contentRect.right - win_contentRect.left)), 
								static_cast<uint32_t>(std::abs(win_contentRect.bottom - win_contentRect.top)));
					}
					return 0;
				case WM_CLOSE: /* close */
					PRINT_DEBUG("Posting window quit-message with exit code 0");
					set_bits<uint8_t>(u8WindowFlagBits, true, WINDOW_CLOSE_FLAG_BIT);
					PostQuitMessage(0);
					return 0;
				case WM_DESTROY: /* window destroyed */
					return 0;
				case WM_GETMINMAXINFO:
					{
						PRINT_DEBUG("Querying information about primary monitor to calculate minimum and maximum allowed extent of the window");
						MINMAXINFO *const win_pWindowSizeLimits = reinterpret_cast<MINMAXINFO*>(win_lParam);
						win_pWindowSizeLimits->ptMinTrackSize.x = MIN_WINDOW_WIDTH;
						win_pWindowSizeLimits->ptMinTrackSize.y = MIN_WINDOW_HEIGHT;
						MONITORINFO win_monitorInfo;
						win_monitorInfo.cbSize = sizeof(MONITORINFO);
						if (GetMonitorInfoW(MonitorFromWindow(win_hWndParam, MONITOR_DEFAULTTONEAREST), &win_monitorInfo) == TRUE) {
							win_pWindowSizeLimits->ptMaxTrackSize.x = std::abs(win_monitorInfo.rcWork.right - win_monitorInfo.rcWork.left);
							win_pWindowSizeLimits->ptMaxTrackSize.y = std::abs(win_monitorInfo.rcWork.bottom - win_monitorInfo.rcWork.top);
						} else
							RE_FATAL_ERROR("Failed getting monitor info, where the window is currently on, on Windows");
					}
					return 0;
				case WM_KEYDOWN: /* key pressed */
				case WM_SYSKEYDOWN:
				case WM_KEYUP: /* key released */
				case WM_SYSKEYUP:
					{
						WORD win_virtualKeyCode = LOWORD(win_wParam);
						const WORD win_keyFlags = HIWORD(win_lParam);
						const BYTE win_scancode = LOBYTE(win_keyFlags);
						WORD win_extScancode = static_cast<WORD>(win_scancode);
						if ((win_keyFlags & KF_EXTENDED) == KF_EXTENDED)
							win_extScancode = MAKEWORD(win_scancode, 0xE0);
						if ((win_keyFlags & KF_ALTDOWN) == KF_ALTDOWN && win_virtualKeyCode == VK_F4)
							PostMessageW(win_hWndParam, WM_CLOSE, 0, 0);
						bool bFallbackToInput = false;
						switch (win_virtualKeyCode) {
							case VK_SHIFT: // Have to be differentiated between left and right
							case VK_CONTROL:
							case VK_MENU:
								PRINT_DEBUG("Mapping scancode to extended virtual key information");
								win_virtualKeyCode = LOWORD(MapVirtualKeyExW(win_extScancode, MAPVK_VSC_TO_VK_EX, win_keyboardLayout));
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
						PRINT_DEBUG("Firing general input event");
						input_event(
								key_from_virtual_keycode(static_cast<int64_t>(win_virtualKeyCode)), 
								static_cast<uint32_t>(win_extScancode), 
								(win_keyFlags & KF_UP) != KF_UP, 
								bFallbackToInput);
					}
					return 0;
				case WM_CHAR:
					{
						//const wchar_t a2wcCharacter[2] = {static_cast<wchar_t>(win_wParam), L'\0'};
					}
					return 0;
				case WM_LBUTTONDOWN: /* left mouse button pressed */
					PRINT_DEBUG("Firing general input event for LMB-pressed");
					input_event(RE_INPUT_BUTTON_LEFT, 0, true, false);
					SetCapture(win_hWndParam);
					return 0;
				case WM_LBUTTONUP: /* left mouse button released */
					PRINT_DEBUG("Firing general input event for LMB-released");
					input_event(RE_INPUT_BUTTON_LEFT, 0, false, false);
					ReleaseCapture();
					return 0;
				case WM_RBUTTONDOWN: /* right mouse button pressed */
					PRINT_DEBUG("Firing general input event for RMB-pressed");
					input_event(RE_INPUT_BUTTON_RIGHT, 0, true, false);
					SetCapture(win_hWndParam);
					return 0;
				case WM_RBUTTONUP: /* right mouse button released */
					PRINT_DEBUG("Firing general input event for RMB-released");
					input_event(RE_INPUT_BUTTON_RIGHT, 0, false, false);
					ReleaseCapture();
					return 0;
				case WM_MBUTTONDOWN: /* middle mouse button pressed */
					PRINT_DEBUG("Firing general input event for MMB-pressed");
					input_event(RE_INPUT_BUTTON_MIDDLE, 0, true, false);
					SetCapture(win_hWndParam);
					return 0;
				case WM_MBUTTONUP: /* middle mouse button released */
					PRINT_DEBUG("Firing general input event for MMB-released");
					input_event(RE_INPUT_BUTTON_MIDDLE, 0, false, false);
					ReleaseCapture();
					return 0;
				case WM_MOUSEMOVE: /* mouse moved */
					{
						PRINT_DEBUG("Firing general input event for mouse movement");
						const int32_t i32XPos = GET_X_LPARAM(win_lParam);
						const int32_t i32YPos = GET_Y_LPARAM(win_lParam);
						cursor_event(i32XPos, i32YPos);
					}
					return 0;
				case WM_SETCURSOR:
					if (LOWORD(win_lParam) == HTCLIENT) {
						PRINT_DEBUG("Setting Windows cursor to the default one defined by the application");
						SetCursor(win_hCursor);
						return TRUE;
					}
					break;
				case WM_MOUSEWHEEL: /* mouse wheel used/scrolled */
					{
						PRINT_DEBUG("Firing general input event for scrolling");
						const int32_t deltaMouseWheel = GET_WHEEL_DELTA_WPARAM(win_wParam);
						if (deltaMouseWheel > 0)
							input_event(RE_INPUT_SCROLL_UP, 0, true, false);
						else
							input_event(RE_INPUT_SCROLL_DOWN, 0, true, false);
					}
					return 0;
				case RE_WIN64_WM_MAXIMIZED:
					{
						PRINT_DEBUG("Querying information about the monitor the window is currently displayed on");
						MONITORINFO win_monitorInfo;
						win_monitorInfo.cbSize = sizeof(MONITORINFO);
						if (GetMonitorInfoW(MonitorFromWindow(win_hWndParam, MONITOR_DEFAULTTOPRIMARY), &win_monitorInfo) == FALSE) {
							RE_FATAL_ERROR("Failed getting monitor info, where the window is currently on, on Windows");
							return 1;
						}
						PRINT_DEBUG("Triggering Windows to resend window message querying minimum and maximum allowed extent of the window");
						const Vector2i monitorSize(
								std::abs(win_monitorInfo.rcWork.right - win_monitorInfo.rcWork.left), 
								std::abs(win_monitorInfo.rcWork.bottom - win_monitorInfo.rcWork.top));
						SetWindowPos(win_hWndParam, nullptr, 0, 0, monitorSize[0], monitorSize[1], SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
					}
					return 0;
				default:
					break;
			}
		}
		PRINT_DEBUG("Calling default window procedure defined by Windows");
		return DefWindowProcW(win_hWndParam, win_uMsg, win_wParam, win_lParam);
	}
	
	bool win64_create_window() {
		PRINT_DEBUG("Telling Windows R-Engine's DPI awareness");
		if (SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) != TRUE)
			RE_WARNING("Failed to tell Windows R-Engine's DPI awareness. May cause weird scaling on high DPI-monitors");
		PRINT_DEBUG("Loading Windows' default cursor");
		win_hCursor = LoadCursorA(nullptr, IDC_ARROW);
		PRINT_DEBUG("Getting Windows' default keyboard layout");
		win_keyboardLayout = GetKeyboardLayout(0);
		if (!win_hInstance) {
			PRINT_DEBUG("Getting HINSTANCE for this application");
			win_hInstance = GetModuleHandle(nullptr);
		}
		if (!win_hInstance) {
			RE_FATAL_ERROR("Failed getting the HINSTANCE for window creation on Windows");
			return false;
		}
		PRINT_DEBUG("Converting title name to wide character string");
		const std::wstring wideTitleStr = convert_chars_to_wide(pacWindowTitle);
		PRINT_DEBUG("Registering window class in Windows");
		const WNDCLASSEXW win_WinClass = {
			.cbSize = sizeof(WNDCLASSEXW),
			.lpfnWndProc = win64_window_proc_callback,
			.hInstance = win_hInstance,
			.lpszClassName = RE_WIN64_WINDOW_CLASS_NAME
		};
		if (RegisterClassExW(&win_WinClass)) {
			PRINT_DEBUG("Obtaining information about primary monitor");
			const HMONITOR win_hPrimaryMonitor = MonitorFromPoint(POINT{}, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFO win_primaryMonitorInfo;
			win_primaryMonitorInfo.cbSize = sizeof(MONITORINFO);
			const bool bMonitorInfoRetrieved = GetMonitorInfoW(win_hPrimaryMonitor, &win_primaryMonitorInfo) == TRUE;
			if (bMonitorInfoRetrieved) {
				if (is_fullscreen()) {
					PRINT_DEBUG("Creating fullscreen window on Windows on the primary monitor");
					win_hMonitor = win_hPrimaryMonitor;
					windowSize[0] = std::abs(win_primaryMonitorInfo.rcMonitor.right - win_primaryMonitorInfo.rcMonitor.left);
					windowSize[1] = std::abs(win_primaryMonitorInfo.rcMonitor.bottom - win_primaryMonitorInfo.rcMonitor.top);
					win_hWindow = CreateWindowExW(
							RE_WIN64_WINDOWED_FULLSCREEN_EXTENDED_STYLE_FLAGS, 
							RE_WIN64_WINDOW_CLASS_NAME, 
							wideTitleStr.c_str(), 
							RE_WIN64_WINDOWED_FULLSCREEN_STYLE_FLAGS, 
							win_primaryMonitorInfo.rcMonitor.left, 
							win_primaryMonitorInfo.rcMonitor.top, 
							windowSize[0], 
							windowSize[1], 
							nullptr, 
							nullptr, 
							win_hInstance, 
							nullptr);
				} else {
					const Vector<LONG, 2> monitorWorkSize(
						std::abs(win_primaryMonitorInfo.rcWork.right - win_primaryMonitorInfo.rcWork.left), 
						std::abs(win_primaryMonitorInfo.rcWork.bottom - win_primaryMonitorInfo.rcWork.top)
					);
					PRINT_DEBUG("Adjusting window extent to Windows' conventions");
					RECT win_adjustableSize = {
						.left = 0,
						.top = 0,
						.right = monitorWorkSize[0] / 5 * 3,
						.bottom = monitorWorkSize[1] / 5 * 3
					};
					AdjustWindowRect(&win_adjustableSize, RE_WIN64_WINDOW_STYLE_FLAGS, FALSE);
					PRINT_DEBUG("Creating window on Windows centered on the primary monitor");
					windowSize[0] = std::abs(win_adjustableSize.right - win_adjustableSize.left);
					windowSize[1] = std::abs(win_adjustableSize.bottom - win_adjustableSize.top);
					win_hWindow = CreateWindowExW(
							RE_WIN64_WINDOW_EXTENDED_STYLE_FLAGS, 
							RE_WIN64_WINDOW_CLASS_NAME, 
							wideTitleStr.c_str(), 
							RE_WIN64_WINDOW_STYLE_FLAGS, 
							win_primaryMonitorInfo.rcWork.left + (monitorWorkSize[0] - windowSize[0]) / 2, 
							win_primaryMonitorInfo.rcWork.top + (monitorWorkSize[1] - windowSize[1]) / 2, 
							windowSize[0], 
							windowSize[1], 
							nullptr, 
							nullptr, 
							win_hInstance, 
							nullptr);
				}
				if (!win_hWindow) {
					RE_FATAL_ERROR("Failed to create window on Windows");
					goto WIN64_WINDOW_CREATION_FAILURE;
				}
			} else {
				if (is_fullscreen()) {
					RE_WARNING("No info was retrieved about the monitor, therefore the window starts with default size and position");
					set_bits<uint8_t>(u8WindowFlagBits, false, WINDOW_FULLSCREEN_BIT);
				}
				PRINT_DEBUG("Creating window on Windows with default extent and position");
				win_hWindow = CreateWindowExW(
						RE_WIN64_WINDOW_EXTENDED_STYLE_FLAGS, 
						RE_WIN64_WINDOW_CLASS_NAME, 
						wideTitleStr.c_str(), 
						RE_WIN64_WINDOW_STYLE_FLAGS, 
						CW_USEDEFAULT, 
						CW_USEDEFAULT, 
						CW_USEDEFAULT, 
						CW_USEDEFAULT, 
						nullptr, 
						nullptr, 
						win_hInstance, 
						nullptr);
				if (win_hWindow) {
					PRINT_DEBUG("Obtaining window's extent");
					RECT win_windowRect;
					if (GetWindowRect(win_hWindow, &win_windowRect) == TRUE) {
						windowSize[0] = std::abs(win_windowRect.right - win_windowRect.left);
						windowSize[1] = std::abs(win_windowRect.bottom - win_windowRect.top);
					} else {
						RE_FATAL_ERROR("Failed to get the window size, which was set by Windows for not getting the monitor size before");
						goto WIN64_WINDOW_SIZE_NOT_RETRIEVED;
					}
				} else {
					RE_FATAL_ERROR("Failed to create window on Windows");
					goto WIN64_WINDOW_CREATION_FAILURE;
				}
			}
			PRINT_DEBUG("Enumerating through each monitor on Windows");
			EnumDisplayMonitors(
					nullptr, 
					nullptr, 
					[] (const HMONITOR hMonitor, const HDC hdcMonitor, const LPRECT lprcMonitor, const LPARAM dwData) -> BOOL {
						PRINT_DEBUG("Checking size of monitor ", hMonitor, " of device context ", hdcMonitor);
						largestMonitorSize[0] = std::max(largestMonitorSize[0], static_cast<uint32_t>(std::abs(lprcMonitor->right - lprcMonitor->left)));
						largestMonitorSize[1] = std::max(largestMonitorSize[1], static_cast<uint32_t>(std::abs(lprcMonitor->bottom - lprcMonitor->top)));
						return TRUE;
					}, 
					0);
			PRINT_DEBUG("Largest monitor size yields ", largestMonitorSize);
			return true;

		WIN64_WINDOW_SIZE_NOT_RETRIEVED:
			PRINT_DEBUG("Destroying window due to failure telling Windows the application's DPI awareness");
			DestroyWindow(win_hWindow);
			win_hWindow = nullptr;

		WIN64_WINDOW_CREATION_FAILURE:
			PRINT_DEBUG("Unregistering window class from Windows");
			UnregisterClassW(RE_WIN64_WINDOW_CLASS_NAME, win_hInstance);

		} else
			RE_FATAL_ERROR("Failed registering class for window creation on Windows");
		return false;
	}
	
	void win64_destroy_window() {
		PRINT_DEBUG("Destroying window on Windows");
		DestroyWindow(win_hWindow);
		win_hWindow = nullptr;
		PRINT_DEBUG("Unregistering window class from Windows");
		UnregisterClassW(RE_WIN64_WINDOW_CLASS_NAME, win_hInstance);
	}

	void win64_show_window() {
		if (are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_VISIBLE_BIT)) {
			PRINT_DEBUG("Showing window on Windows");
			ShowWindow(win_hWindow, SW_SHOWNORMAL);
		} else {
			PRINT_DEBUG("Hiding window on Windows");
			ShowWindow(win_hWindow, SW_HIDE);
		}
	}

	void win64_update_fullscreen() {
		MONITORINFO win_monitorInfo;
		win_monitorInfo.cbSize = sizeof(MONITORINFO);
		if (is_fullscreen()) {
			PRINT_DEBUG("Retrieving monitor information where window ", win_hWindow, " is displayed on the most");
			win_hMonitor = MonitorFromWindow(win_hWindow, MONITOR_DEFAULTTONEAREST);
			if (GetMonitorInfoW(win_hMonitor, &win_monitorInfo) == FALSE) {
				RE_FATAL_ERROR("Failed to retrieve monitor information to set window to windowed fullscreen");
				return;
			}
			PRINT_DEBUG("Setting new style flags for the window");
			SetWindowLongPtr(win_hWindow, GWL_EXSTYLE, RE_WIN64_WINDOWED_FULLSCREEN_EXTENDED_STYLE_FLAGS);
			SetWindowLongPtr(win_hWindow, GWL_STYLE, RE_WIN64_WINDOWED_FULLSCREEN_STYLE_FLAGS);
			windowSize[0] = static_cast<uint32_t>(std::abs(win_monitorInfo.rcMonitor.right - win_monitorInfo.rcMonitor.left));
			windowSize[1] = static_cast<uint32_t>(std::abs(win_monitorInfo.rcMonitor.bottom - win_monitorInfo.rcMonitor.top));
			PRINT_DEBUG("Repositioning window to (", win_monitorInfo.rcMonitor.left, ", ", win_monitorInfo.rcMonitor.top, ") and resizing to ", windowSize, " to fit on monitor ", win_hMonitor);
			if (SetWindowPos(
					win_hWindow, 
					HWND_TOP, 
					win_monitorInfo.rcMonitor.left, 
					win_monitorInfo.rcMonitor.top, 
					windowSize[0], 
					windowSize[1], 
					SWP_FRAMECHANGED | SWP_SHOWWINDOW) == 0)
				RE_FATAL_ERROR("Failed to set and reposition the window to windowed fullscreen");
		} else {
			PRINT_DEBUG("Retrieving information about monitor ", win_hMonitor);
			if (GetMonitorInfoW(win_hMonitor, &win_monitorInfo) == FALSE) {
				RE_FATAL_ERROR("Failed to retrieve monitor information to reset window from fullscreen");
				return;
			}
			const Vector<LONG, 2> monitorSize(
					std::abs(win_monitorInfo.rcWork.right - win_monitorInfo.rcWork.left), 
					std::abs(win_monitorInfo.rcWork.bottom - win_monitorInfo.rcWork.top));
			PRINT_DEBUG("Setting new style flags for window ", win_hWindow);
			SetWindowLongPtr(win_hWindow, GWL_EXSTYLE, RE_WIN64_WINDOW_EXTENDED_STYLE_FLAGS);
			SetWindowLongPtr(win_hWindow, GWL_STYLE, RE_WIN64_WINDOW_STYLE_FLAGS);
			windowSize[0] = monitorSize[0] / 5 * 3;
			windowSize[1] = monitorSize[1] / 5 * 3;
			PRINT_DEBUG("Repositioning and resizing to ", windowSize);
			if (SetWindowPos(
					win_hWindow, 
					HWND_TOP, 
					win_monitorInfo.rcWork.left + (monitorSize[0] - windowSize[0]) / 2, 
					win_monitorInfo.rcWork.top + (monitorSize[1] - windowSize[1]) / 2, 
					windowSize[0], 
					windowSize[1], 
					SWP_FRAMECHANGED | SWP_SHOWWINDOW) == 0)
				RE_FATAL_ERROR("Failed to reset and reposition the window from fullscreen");
			win_hMonitor = nullptr;
		}
	}

	void win64_update_window_title() {
		PRINT_DEBUG("Updating window title on Windows");
		if (SetWindowTextW(win_hWindow, convert_chars_to_wide(pacWindowTitle).c_str()) == FALSE)
			RE_FATAL_ERROR("Failed updating the title in the window title bar on Windows");
	}

	void win64_window_proc() {
		BOOL win_msgInQueue;
		MSG win_msg;
		do {
			PRINT_DEBUG("Obtaining next window message sent to application");
			win_msgInQueue = PeekMessageW(&win_msg, win_hWindow, 0, 0, PM_REMOVE);
			if (!win_msgInQueue || LOWORD(win_msg.message) == WM_QUIT)
				break;
			PRINT_DEBUG("Translating window message");
			TranslateMessage(&win_msg);
			PRINT_DEBUG("Calling procedure on window");
			DispatchMessageW(&win_msg); /* calls window procedure */
		} while (win_msgInQueue);
	}

	void win64_set_hinstance(const HINSTANCE win_hNewInstance) {
		PRINT_DEBUG("Setting HINSTANCE in engine to ", win_hNewInstance);
		win_hInstance = win_hNewInstance;
	}

}

#endif /* RE_OS_WINDOWS */
