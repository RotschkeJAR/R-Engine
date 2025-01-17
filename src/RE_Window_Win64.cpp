#include "RE_Window_Win64.hpp"
#include "RE_Main.hpp"
#include "RE_Vulkan_Win64.hpp"

namespace RE {

#ifdef RE_OS_WINDOWS
# define WINDOW_CLASS_NAME L"RE_WindowClass"
	Window_Win64* win64 = nullptr;

	LRESULT CALLBACK windowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (!win64)
			RE_FATAL_ERROR("Window process function has been called when no window is active");
		else if (win64->hWindow != hWnd && running)
			RE_FATAL_ERROR("Window process function has been called by another window");
		else {
			switch (uMsg) {
				case WM_SIZE: /* resized */
					win64->size[0] = LOWORD(lParam);
					win64->size[1] = HIWORD(lParam);
					win64->inputMgr.updateWinSize(win64->size);
					return 0;
				case WM_CLOSE: /* close */
					win64->closeFlag = true;
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
					win64->inputMgr.keyInput(vkCode, static_cast<REushort>(scanCode), !static_cast<bool>(keyReleased));
					} return 0;
				case WM_CHAR: {
					wchar_t character[2] = {static_cast<wchar_t>(wParam), L'\0'};
					win64->inputMgr.charInput(convertToUTF8(character).c_str());
					} return 0;
				case WM_LBUTTONDOWN: /* left mouse button pressed */
					win64->inputMgr.buttonInput(RE_LBUTTON, true);
					SetCapture(hWnd);
					return 0;
				case WM_LBUTTONUP: /* left mouse button released */
					win64->inputMgr.buttonInput(RE_LBUTTON, false);
					ReleaseCapture();
					return 0;
				case WM_RBUTTONDOWN: /* right mouse button pressed */
					win64->inputMgr.buttonInput(RE_RBUTTON, true);
					SetCapture(hWnd);
					return 0;
				case WM_RBUTTONUP: /* right mouse button released */
					win64->inputMgr.buttonInput(RE_RBUTTON, false);
					ReleaseCapture();
					return 0;
				case WM_MBUTTONDOWN: /* middle mouse button pressed */
					win64->inputMgr.buttonInput(RE_MBUTTON, true);
					SetCapture(hWnd);
					return 0;
				case WM_MBUTTONUP: /* middle mouse button released */
					win64->inputMgr.buttonInput(RE_MBUTTON, false);
					ReleaseCapture();
					return 0;
				case WM_MOUSEMOVE: { /* mouse moved */
					REint xPos = GET_X_LPARAM(lParam);
					REint yPos = GET_Y_LPARAM(lParam);
					win64->inputMgr.cursorInput(xPos, yPos);
					} return 0;
				case WM_SETCURSOR:
					if (LOWORD(lParam) == HTCLIENT) {
						SetCursor(win64->hCursor);
						return TRUE;
					}
					break;
				case WM_MOUSEWHEEL: /* mouse wheel used/scrolled */
					win64->inputMgr.scrollInput(static_cast<REubyte>(GET_WHEEL_DELTA_WPARAM(wParam)));
					return 0;
				default:
					break;
			}
		}
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
	
	Window_Win64::Window_Win64() : hInstance(GetModuleHandle(nullptr)), hWindow(nullptr), msg{}, hCursor(LoadCursor(nullptr, IDC_ARROW)) {
		win64 = this;
		if (!hInstance) {
			RE_FATAL_ERROR("Failed getting the HINSTANCE for window creation");
			return;
		}
		const std::wstring wTitleStr = convertToWide(title);
		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.lpfnWndProc = windowProcess;
		wc.hInstance = hInstance;
		wc.lpszClassName = WINDOW_CLASS_NAME;
		if (!RegisterClassExW(&wc)) {
			RE_FATAL_ERROR("Failed registering class for window creation");
			return;
		}
		hWindow = CreateWindowExW(0,
			WINDOW_CLASS_NAME,
			wTitleStr.c_str(),
			WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
			10, 10, 200, 200,
			nullptr,
			nullptr,
			hInstance,
			nullptr);
		if (!hWindow) {
			RE_FATAL_ERROR("Failed creating window");
			return;
		}
		vkRenderPipeline = new Vulkan_Win64(hInstance, hWindow);
		if (!vkRenderPipeline->isValid())
			return;
		valid = true;
	}
	
	Window_Win64::~Window_Win64() {
		if (win64 != this)
			return;
		DestroyWindow(hWindow);
		UnregisterClassW(WINDOW_CLASS_NAME, hInstance);
		win64 = nullptr;
	}

	void Window_Win64::showInternal() {
		ShowWindow(hWindow, windowVisible ? TRUE : FALSE);
	}

	void Window_Win64::updateTitleInternal() {
		SetWindowTextW(hWindow, convertToWide(title).c_str());
	}

	void Window_Win64::processLoop() {
		while (PeekMessageW(&msg, hWindow, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessageW(&msg); /* calls window procedure */
		}
	}
#endif /* RE_OS_WINDOWS */

}
