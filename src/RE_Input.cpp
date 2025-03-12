#include "RE_Input.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Window_X11.hpp"

namespace RE {

	struct KeyInfo {
		REuint u32Scancode;
		Input eInput;
		std::string pcCharacter;
		REubyte u8State;
	};

	InputMgr* InputMgr::pInstance = nullptr;

	std::vector<REuint> get_all_scancodes() {
		std::vector<REuint> existingScancodes;
#ifdef RE_OS_WINDOWS
		for (REuint u32VirtualKeycodeDummy = 0U; u32VirtualKeycodeDummy < 256U; u32VirtualKeycodeDummy++) {
			switch (u32VirtualKeycodeDummy) {
				case VK_SHIFT:
				case VK_CONTROL:
				case VK_MENU:
					continue;
				default:
					break;
			}
			UINT win_scancode = MapVirtualKeyW(u32VirtualKeycodeDummy, MAPVK_VK_TO_VSC_EX);
			if (!win_scancode || std::find(existingScancodes.begin(), existingScancodes.end(), static_cast<REuint>(win_scancode)) != existingScancodes.end())
				continue;
			bool bExtendedScancode = HIBYTE(LOWORD(win_scancode)) != 0;
			RElong i64ModifiedScancode = (win_scancode << 16L) | (static_cast<RElong>(bExtendedScancode) << 24L);
			wchar_t wcKeyNameDummy[2];
			if (GetKeyState(u32VirtualKeycodeDummy) != 0 || GetKeyNameTextW(i64ModifiedScancode, wcKeyNameDummy, 2) > 0)
				existingScancodes.push_back(static_cast<REuint>(i64ModifiedScancode >> 16L));
		}
		return existingScancodes;
#elif defined RE_OS_LINUX
		XDisplay* const x11_pDisplay = static_cast<Window_X11*>(Window::pInstance)->x11_pDisplay;
		REint i32MinScancode = 0, i32MaxScancode = 0;
		XDisplayKeycodes(x11_pDisplay, &i32MinScancode, &i32MaxScancode);
		return static_cast<REuint>(i32MaxScancode - i32MinScancode + 1);
#endif
	}

	InputMgr::InputMgr() : u32NumberOfKeys(get_all_scancodes().size()), keyTable(new KeyInfo[u32NumberOfKeys]) {
		if (pInstance) {
			RE_FATAL_ERROR("An instance of the class \"InputMgr\" has already been constructed");
			return;
		}
		pInstance = this;
		PRINT_LN(u32NumberOfKeys);
		std::vector<REuint> existingScancodes = get_all_scancodes();
		REuint u32Index = 0U;
		for (REuint u32Scancode : existingScancodes) {
			keyTable[u32Index].u32Scancode = u32Scancode;
#ifdef RE_OS_WINDOWS
			UINT win_virtualKeycode = MapVirtualKeyW(u32Scancode, MAPVK_VSC_TO_VK_EX);
			keyTable[u32Index].eInput = windows_key_from_virtual_keycode(win_virtualKeycode);
			const wchar_t wCharacter[2] = {(wchar_t) MapVirtualKeyW(win_virtualKeycode, MAPVK_VK_TO_CHAR), L'\0'};
			keyTable[u32Index].pcCharacter = convert_wide_chars_to_utf8(wCharacter);
#elif defined RE_OS_LINUX
#endif
			keyTable[u32Index].u8State = 0U;
		}
	}

	InputMgr::~InputMgr() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
	}

	void InputMgr::input_event(Input eInput, REuint u32Scancode, bool bPressed) {
		if (eInput <= RE_INPUT_UNKNOWN || eInput >= RE_INPUT_MAX_ENUM)
			return;
		if (u32Scancode)
			PRINT_LN(append_to_string("Pressed keycode: ", u32Scancode));
	}

	void InputMgr::cursor_event(REint i32X, REint i32Y) {
		cursorPosition[0] = i32X;
		cursorPosition[1] = i32Y;
	}

	void InputMgr::update_input_buffers() {
		prevCursorPosition.copy_from(cursorPosition);
	}

}
