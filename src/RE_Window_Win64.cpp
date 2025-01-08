#include "RE_Window_Win64.hpp"

namespace RE {

#ifdef RE_OS_WINDOWS
# define WINDOW_CLASS_NAME L"RE_WindowClass"
	Window_Win64* win64 = nullptr;

	LRESULT CALLBACK windowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (!win64)
			RE_FATAL_ERROR("Window process function has been called when no window is active");
		else {
			switch (uMsg) {
				case WM_SIZE: /* resized */
					win64->size[0] = LOWORD(lParam);
					win64->size[1] = HIWORD(lParam);
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
					win64->inputMgr.keyInput(winKeyFromVirtual(vkCode), scanCode, !static_cast<bool>(keyReleased));
					} return 0;
				case WM_CHAR:
					win64->inputMgr.charInput(static_cast<wchar_t>(wParam));
					return 0;
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
					signed int xPos = GET_X_LPARAM(lParam);
					signed int yPos = GET_Y_LPARAM(lParam);
					} return 0;
				case WM_SETCURSOR:
					if (LOWORD(lParam) == HTCLIENT) {
						SetCursor(win64->hCursor);
						return TRUE;
					}
					break;
				case WM_MOUSEWHEEL: /* mouse wheel used/scrolled */
					return 0;
				default:
					break;
			}
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	Window_Win64::Window_Win64() : hInstance(GetModuleHandle(nullptr)), hWindow(nullptr), msg({ }), hCursor(LoadCursor(nullptr, IDC_ARROW)), hDevice(nullptr), hRenderContext(nullptr) {
		win64 = this;
		if (!hInstance) {
			RE_ERROR("Failed getting the HINSTANCE for window creation");
			return;
		}
		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.lpfnWndProc = windowProcess;
		wc.hInstance = hInstance;
		wc.lpszClassName = WINDOW_CLASS_NAME;
		if (!RegisterClassExW(&wc)) {
			RE_ERROR("Failed registering class for window creation");
			return;
		}
		hWindow = CreateWindowExW(0,
			WINDOW_CLASS_NAME,
			title,
			WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
			10, 10, 200, 200,
			nullptr,
			nullptr,
			hInstance,
			nullptr);
		if (!hWindow) {
			RE_ERROR("Failed creating window");
			return;
		}
		hDevice = GetDC(hWindow);
		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cRedBits = 8;
		pfd.cRedShift = 0;
		pfd.cGreenBits = 8;
		pfd.cGreenShift = 0;
		pfd.cBlueBits = 8;
		pfd.cBlueShift = 0;
		pfd.cAlphaBits = 8;
		pfd.cAlphaShift = 0;
		pfd.cAccumBits = 0;
		pfd.cAccumRedBits = 0;
		pfd.cAccumGreenBits = 0;
		pfd.cAccumBlueBits = 0;
		pfd.cAccumAlphaBits = 0;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 0;
		pfd.cAuxBuffers = 0;
		pfd.iLayerType = PFD_MAIN_PLANE;
		pfd.bReserved = 0;
		pfd.dwLayerMask = 0;
		pfd.dwVisibleMask = 0;
		pfd.dwDamageMask = 0;
		signed int pixelFormat = ChoosePixelFormat(hDevice, &pfd);
		if (!pixelFormat) {
			RE_ERROR("Failed creating a pixel format for OpenGL");
			return;
		}
		if (!SetPixelFormat(hDevice, pixelFormat, &pfd)) {
			RE_ERROR("Failed setting the pixel format to current");
			return;
		}
		hRenderContext = wglCreateContext(hDevice);
		if (!hRenderContext) {
			RE_ERROR("Failed creating render context");
			return;
		}
		wglMakeCurrent(hDevice, hRenderContext);
		valid = true;
	}
	
	Window_Win64::~Window_Win64() {
		if (win64 != this)
			return;
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(hRenderContext);
		ReleaseDC(hWindow, hDevice);
		DestroyWindow(hWindow);
		UnregisterClassW(WINDOW_CLASS_NAME, hInstance);
		win64 = nullptr;
	}

	void Window_Win64::showInternal(bool showWindow) {
		ShowWindow(hWindow, showWindow ? TRUE : FALSE);
	}

	void Window_Win64::processLoop() {
		while (PeekMessageW(&msg, hWindow, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessageW(&msg); /* calls window procedure */
		}
		SwapBuffers(hDevice);
	}

	void Window_Win64::fullscreen() {
		//HMONITOR hMonitor = MonitorFromWindow(hWindow, MONITOR_DEFAULTTOPRIMARY);
	}
	
	void Window_Win64::windowedFullscreen() {
		//HMONITOR hMonitor = MonitorFromWindow(hWindow, MONITOR_DEFAULTTOPRIMARY);
	}
	
	void Window_Win64::window() {

	}
#endif /* RE_OS_WINDOWS */

}
