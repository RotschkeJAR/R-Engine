#include "RE_Window_Win64.hpp"
#include "RE_Main.hpp"

namespace RE {

#ifdef RE_OS_WINDOWS
# define WINDOW_CLASS_NAME L"RE_WindowClass"
# define WINDOW_STYLE_FLAGS (WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU)
	HINSTANCE Window_Win64::win_hInstance = nullptr;
	Window_Win64* pWin64 = nullptr;

	LRESULT CALLBACK windows_window_proc(HWND win_hWnd, UINT win_uMsg, WPARAM win_wParam, LPARAM win_lParam) {
		if (!pWin64)
			RE_FATAL_ERROR("Window process function has been called when no window is active");
		else if (pWin64->win_hWindow != win_hWnd && bRunning)
			RE_FATAL_ERROR("Window process function has been called by another window");
		else {
			switch (win_uMsg) {
				case WM_SIZE: /* resized */
					CATCH_SIGNAL(pWin64->update_window_size(static_cast<REushort>(LOWORD(win_lParam)), static_cast<REushort>(HIWORD(win_lParam))));
					return 0;
				case WM_CLOSE: /* close */
					pWin64->bCloseFlag = true;
					return 0;
				case WM_DESTROY: /* destroy window */
					PostQuitMessage(0);
					return 0;
				case WM_KEYDOWN: /* key pressed */
				case WM_SYSKEYDOWN:
				case WM_KEYUP: /* key released */
				case WM_SYSKEYUP: {
					WORD win_vkCode = LOWORD(win_wParam);
					WORD win_keyFlags = HIWORD(win_lParam);
					WORD win_scanCode = LOBYTE(win_keyFlags);
					BOOL win_extendedKey = (win_keyFlags & KF_EXTENDED) == KF_EXTENDED;
					if (win_extendedKey)
						win_scanCode = MAKEWORD(win_scanCode, 0xE0);
					BOOL win_keyReleased = (win_keyFlags & KF_UP) == KF_UP;
					switch (win_vkCode) {
						case VK_SHIFT:
						case VK_CONTROL:
						case VK_MENU:
							win_vkCode = LOWORD(MapVirtualKeyW(win_scanCode, MAPVK_VSC_TO_VK_EX));
							break;
						default:
							break;
					}
					CATCH_SIGNAL(pWin64->inputMgr.input_event(windows_key_from_virtual_keycode(static_cast<RElong>(win_vkCode)), static_cast<REuint>(win_scanCode), static_cast<bool>(win_keyReleased)));
					} return 0;
				case WM_CHAR: {
					wchar_t wCharacter[2] = {static_cast<wchar_t>(win_wParam), L'\0'};
					} return 0;
				case WM_LBUTTONDOWN: /* left mouse button pressed */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_LEFT, 0U, true));
					SetCapture(win_hWnd);
					return 0;
				case WM_LBUTTONUP: /* left mouse button released */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_LEFT, 0U, false));
					ReleaseCapture();
					return 0;
				case WM_RBUTTONDOWN: /* right mouse button pressed */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_RIGHT, 0U, true));
					SetCapture(win_hWnd);
					return 0;
				case WM_RBUTTONUP: /* right mouse button released */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_RIGHT, 0U, false));
					ReleaseCapture();
					return 0;
				case WM_MBUTTONDOWN: /* middle mouse button pressed */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_MIDDLE, 0U, true));
					SetCapture(win_hWnd);
					return 0;
				case WM_MBUTTONUP: /* middle mouse button released */
					CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_BUTTON_MIDDLE, 0U, false));
					ReleaseCapture();
					return 0;
				case WM_MOUSEMOVE: { /* mouse moved */
					REint i32XPos = GET_X_LPARAM(win_lParam);
					REint i32YPos = GET_Y_LPARAM(win_lParam);
					CATCH_SIGNAL(pWin64->inputMgr.cursor_event(i32XPos, i32YPos));
					} return 0;
				case WM_SETCURSOR:
					if (LOWORD(win_lParam) == HTCLIENT) {
						SetCursor(pWin64->win_hCursor);
						return TRUE;
					}
					break;
				case WM_MOUSEWHEEL: { /* mouse wheel used/scrolled */
					REint deltaMouseWheel = GET_WHEEL_DELTA_WPARAM(win_wParam);
					if (deltaMouseWheel > 0)
						CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_SCROLL_UP, 0U, true));
					else
						CATCH_SIGNAL(pWin64->inputMgr.input_event(RE_INPUT_SCROLL_DOWN, 0U, true));
					} return 0;
				default:
					break;
			}
		}
		LRESULT win_lResult = 0;
		CATCH_SIGNAL(win_lResult = DefWindowProcW(win_hWnd, win_uMsg, win_wParam, win_lParam));
		return win_lResult;
	}
	
	Window_Win64::Window_Win64() : win_hWindow(nullptr), win_msg{}, win_hCursor(LoadCursor(nullptr, IDC_ARROW)) {
		if (pWin64) {
			RE_FATAL_ERROR("An instance of the class \"Window_Win64\" has already been constructed. Discarding new one");
			return;
		}
		pWin64 = this;
		if (!win_hInstance)
			win_hInstance = GetModuleHandle(nullptr);
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
		if (!RegisterClassExW(&win_WinClass)) {
			RE_FATAL_ERROR("Failed registering class for window creation");
			return;
		};
		Vector2i adjustedSize = get_adjusted_window_size(size);
		win_hWindow = CreateWindowExW(0, WINDOW_CLASS_NAME, wideTitleStr.c_str(), WINDOW_STYLE_FLAGS, CW_USEDEFAULT, CW_USEDEFAULT, adjustedSize[0], adjustedSize[1], nullptr, nullptr, win_hInstance, nullptr);
		if (!win_hWindow) {
			RE_FATAL_ERROR("Failed creating window");
			return;
		}
		bValid = true;
	}
	
	Window_Win64::~Window_Win64() {
		if (pWin64 != this)
			return;
		DestroyWindow(win_hWindow);
		UnregisterClassW(WINDOW_CLASS_NAME, win_hInstance);
		pWin64 = nullptr;
	}

	Vector2i Window_Win64::get_adjusted_window_size(Vector<REushort, 2> size) {
		RECT win_adjustableSize = {0, 0, size[0], size[1]};
		AdjustWindowRect(&win_adjustableSize, WINDOW_STYLE_FLAGS, FALSE);
		return Vector2i(win_adjustableSize.right - win_adjustableSize.left, win_adjustableSize.bottom - win_adjustableSize.top);
	}

	void Window_Win64::internal_show_window() {
		ShowWindow(win_hWindow, bWindowVisible ? TRUE : FALSE);
	}

	void Window_Win64::internal_update_title() {
		if (!SetWindowTextW(win_hWindow, convert_chars_to_wide(pcTitle).c_str()))
			RE_FATAL_ERROR("Failed updating the title in the window title bar");
	}

	void Window_Win64::internal_window_proc() {
		while (PeekMessageW(&win_msg, win_hWindow, 0, 0, PM_REMOVE)) {
			if (win_msg.message == WM_QUIT)
				break;
			TranslateMessage(&win_msg);
			CATCH_SIGNAL(DispatchMessageW(&win_msg)); /* calls window procedure */
		}
	}

	void set_hinstance(HINSTANCE win_hInstance) {
		Window_Win64::win_hInstance = win_hInstance;
	}
#endif /* RE_OS_WINDOWS */

}
