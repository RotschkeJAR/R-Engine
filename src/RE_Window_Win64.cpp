#include "RE_Window_Win64.hpp"
#include "RE_Main.hpp"

namespace RE {

#ifdef RE_OS_WINDOWS
# define WINDOW_CLASS_NAME L"RE_WindowClass"
# define WINDOW_STYLE_FLAGS (WS_BORDER | WS_CAPTION | WS_SYSMENU)
	HINSTANCE Window_Win64::win_hInstance = nullptr;
	Window_Win64* pWin64 = nullptr;

	LRESULT CALLBACK windowProcess(HWND win_hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (!pWin64)
			RE_FATAL_ERROR("Window process function has been called when no window is active");
		else if (pWin64->win_hWindow != win_hWnd && bRunning)
			RE_FATAL_ERROR("Window process function has been called by another window");
		else {
			switch (uMsg) {
				case WM_SIZE: /* resized */
					CATCH_SIGNAL(pWin64->updateWindowSize(static_cast<REushort>(LOWORD(lParam)), static_cast<REushort>(HIWORD(lParam))));
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
					WORD vkCode = LOWORD(wParam);
					WORD keyFlags = HIWORD(lParam);
					WORD scanCode = LOBYTE(keyFlags);
					BOOL extendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
					if (extendedKey)
						scanCode = MAKEWORD(scanCode, 0xE0);
					BOOL keyReleased = (keyFlags & KF_UP) == KF_UP;
					switch (vkCode) {
						case VK_SHIFT:
						case VK_CONTROL:
						case VK_MENU:
							vkCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
							break;
						default:
							break;
					}
					CATCH_SIGNAL(pWin64->inputMgr.keyInput(vkCode, static_cast<REuint>(scanCode), !static_cast<bool>(keyReleased)));
					} return 0;
				case WM_CHAR: {
					wchar_t character[2] = {static_cast<wchar_t>(wParam), L'\0'};
					CATCH_SIGNAL(pWin64->inputMgr.charInput(convertToUTF8(character).c_str()));
					} return 0;
				case WM_LBUTTONDOWN: /* left mouse button pressed */
					CATCH_SIGNAL(pWin64->inputMgr.buttonInput(RE_LBUTTON, true));
					SetCapture(win_hWnd);
					return 0;
				case WM_LBUTTONUP: /* left mouse button released */
					CATCH_SIGNAL(pWin64->inputMgr.buttonInput(RE_LBUTTON, false));
					ReleaseCapture();
					return 0;
				case WM_RBUTTONDOWN: /* right mouse button pressed */
					CATCH_SIGNAL(pWin64->inputMgr.buttonInput(RE_RBUTTON, true));
					SetCapture(win_hWnd);
					return 0;
				case WM_RBUTTONUP: /* right mouse button released */
					CATCH_SIGNAL(pWin64->inputMgr.buttonInput(RE_RBUTTON, false));
					ReleaseCapture();
					return 0;
				case WM_MBUTTONDOWN: /* middle mouse button pressed */
					CATCH_SIGNAL(pWin64->inputMgr.buttonInput(RE_MBUTTON, true));
					SetCapture(win_hWnd);
					return 0;
				case WM_MBUTTONUP: /* middle mouse button released */
					CATCH_SIGNAL(pWin64->inputMgr.buttonInput(RE_MBUTTON, false));
					ReleaseCapture();
					return 0;
				case WM_MOUSEMOVE: { /* mouse moved */
					REint iXPos = GET_X_LPARAM(lParam);
					REint iYPos = GET_Y_LPARAM(lParam);
					pWin64->inputMgr.cursorInput(iXPos, iYPos);
					} return 0;
				case WM_SETCURSOR:
					if (LOWORD(lParam) == HTCLIENT) {
						SetCursor(pWin64->win_hCursor);
						return TRUE;
					}
					break;
				case WM_MOUSEWHEEL: /* mouse wheel used/scrolled */
					pWin64->inputMgr.scrollInput(static_cast<REubyte>(GET_WHEEL_DELTA_WPARAM(wParam)));
					return 0;
				default:
					break;
			}
		}
		LRESULT lResult = static_cast<LRESULT>(0);
		CATCH_SIGNAL(lResult = DefWindowProcW(win_hWnd, uMsg, wParam, lParam));
		return lResult;
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
		const std::wstring wideTitleStr = convertToWide(pcTitle);
		WNDCLASSEXW win_WinClass = {};
		win_WinClass.cbSize = sizeof(WNDCLASSEXW);
		win_WinClass.lpfnWndProc = windowProcess;
		win_WinClass.hInstance = win_hInstance;
		win_WinClass.lpszClassName = WINDOW_CLASS_NAME;
		if (!RegisterClassExW(&win_WinClass)) {
			RE_FATAL_ERROR("Failed registering class for window creation");
			return;
		};
		Vector2i adjustedSize = getAdjustedWindowSize(size);
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

	Vector2i Window_Win64::getAdjustedWindowSize(Vector<REushort, 2> size) {
		RECT win_adjustableSize = {0, 0, size[0], size[1]};
		AdjustWindowRect(&win_adjustableSize, WINDOW_STYLE_FLAGS, FALSE);
		return Vector2i(win_adjustableSize.right - win_adjustableSize.left, win_adjustableSize.bottom - win_adjustableSize.top);
	}

	void Window_Win64::showInternal() {
		ShowWindow(win_hWindow, bWindowVisible ? TRUE : FALSE);
	}

	void Window_Win64::updateTitleInternal() {
		if (!SetWindowTextW(win_hWindow, convertToWide(pcTitle).c_str()))
			RE_FATAL_ERROR("Failed updating the title in the window title bar");
	}

	void Window_Win64::processLoop() {
		while (PeekMessageW(&win_msg, win_hWindow, 0, 0, PM_REMOVE)) {
			if (win_msg.message == WM_QUIT)
				break;
			TranslateMessage(&win_msg);
			CATCH_SIGNAL(DispatchMessageW(&win_msg)); /* calls window procedure */
		}
	}

	void setHInstance(HINSTANCE win_hInstance) {
		Window_Win64::win_hInstance = win_hInstance;
	}
#endif /* RE_OS_WINDOWS */

}
