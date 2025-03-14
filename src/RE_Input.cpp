#include "RE_Input.hpp"
#include "RE_Internal Header.hpp"

namespace RE {

	struct KeyInfo {
		REuint u32Scancode;
		Input eInput;
	};

	InputMgr *InputMgr::pInstance = nullptr;

	InputMgr::InputMgr() : u8SpecialInputBuffer(0U), u8PrevSpecialInputBuffer(0U) {
		if (pInstance) {
			RE_FATAL_ERROR("An instance of the class \"InputMgr\" has already been constructed");
			return;
		}
		pInstance = this;
		std::vector<KeyInfo> existingKeys;
#ifdef RE_OS_WINDOWS
		const HKL win_keyboardLayout = GetKeyboardLayout(0);
		for (UINT u32VirtualKeycodeDummy = 0U; u32VirtualKeycodeDummy < 256U; u32VirtualKeycodeDummy++) {
			switch (u32VirtualKeycodeDummy) {
				case VK_LBUTTON: // LMB
				case VK_RBUTTON: // RMB
				case VK_MBUTTON: // middle click
				case VK_XBUTTON1: // another mouse button
				case VK_XBUTTON2: // another mouse button
				case VK_SHIFT: // (left) Shift-key
				case VK_CONTROL: // (left) CTRL-key
				case VK_MENU: // (left) Alt-key
				case VK_LWIN: // left Windows-key
				case VK_RWIN: // right Windows-key
					continue; // Skip redundant or unused keycodes
				default:
					break;
			}
			bool bRetry = false;
			do {
				UINT win_scancode = MapVirtualKeyExW(u32VirtualKeycodeDummy, MAPVK_VK_TO_VSC_EX, win_keyboardLayout);
				if (!win_scancode)
					continue; // No scancode assigned to keycode
				switch (u32VirtualKeycodeDummy) {
					case VK_RETURN:
						bRetry = !bRetry; // Retry required, because virtual keycode can represent multiple physical keys
						// Don't break here
					case VK_DELETE: // Delete-key
					case VK_INSERT: // Insert-key
					case VK_HOME: // Home-key
					case VK_END: // End-key
					case VK_PRIOR: // Page Up-key
					case VK_NEXT: // Page Down-key
					case VK_UP: // Arrow Up-key
					case VK_DOWN: // Arrow Down-key
					case VK_LEFT: // Arrow Left-key
					case VK_RIGHT: // Arrow Right-key
					case VK_SNAPSHOT: // Print screen-key
					case VK_RCONTROL: // right CTRL-key
					case VK_RMENU: // right Alt-key
					case VK_DIVIDE: // Numpad Devide-key
						if (!bRetry)
							win_scancode |= 0xE000U; // Force extended scancode
						break;
					default:
						break;
				}
				if (std::find_if(existingKeys.begin(), existingKeys.end(), [win_scancode](const KeyInfo info){return info.u32Scancode == win_scancode;}) != existingKeys.end())
					continue; // Exists already
				bool bExtendedScancode = HIBYTE(LOWORD(win_scancode)) != 0;
				RElong i64ModifiedScancode = static_cast<RElong>((win_scancode & 0xFFU) << 16U) | (static_cast<RElong>(bExtendedScancode) << 24L);
				wchar_t wcKeyNameDummy[2];
				if (GetKeyState(u32VirtualKeycodeDummy) != 0 || GetKeyNameTextW(i64ModifiedScancode, wcKeyNameDummy, 2) > 0) {
					KeyInfo newInfo = {win_scancode, windows_key_from_virtual_keycode(u32VirtualKeycodeDummy)};
					switch (u32VirtualKeycodeDummy) {
						case VK_RETURN: // Prevents algorithm from not detecting the numpad's enter-key
							if (bRetry)
								newInfo.eInput = RE_INPUT_KEY_ENTER;
							else
								newInfo.eInput = RE_INPUT_KEY_NUMPAD_ENTER;
							break;
						default:
							break;
					}
					existingKeys.push_back(newInfo);
				}
			} while (bRetry);
		}
		std::sort<std::vector<KeyInfo>::iterator>(existingKeys.begin(), existingKeys.end(), [](const KeyInfo info1, const KeyInfo info2){return info1.u32Scancode < info2.u32Scancode;});
#elif defined RE_OS_LINUX
		XDisplay *const x11_pDummyDisplay = XOpenDisplay(nullptr);
		REint i32MinScancode = 0, i32MaxScancode = 0;
		XDisplayKeycodes(x11_pDummyDisplay, &i32MinScancode, &i32MaxScancode);
		REint i32KeysymsPerScancode = 0;
		KeySym *const x11_pKeyMap = XGetKeyboardMapping(x11_pDummyDisplay, i32MinScancode, i32MaxScancode - i32MinScancode + 1, &i32KeysymsPerScancode);
		if (x11_pKeyMap) {
			for (REint i32ScancodeElement = i32MinScancode; i32ScancodeElement <= i32MaxScancode; i32ScancodeElement++)
				for (REint i32KeySymbol = 0; i32KeySymbol < i32KeysymsPerScancode; i32KeySymbol++) {
					KeySym x11_keySymbol = x11_pKeyMap[(i32ScancodeElement - i32MinScancode) * i32KeysymsPerScancode + i32KeySymbol];
					if (x11_keySymbol != XNoSymbol) {
						existingKeys.push_back({static_cast<REuint>(i32ScancodeElement), x11_key_from_virtual_keycode(x11_keySymbol)});
						break;
					}
				}
			XFree(x11_pKeyMap);
		} else {
			RE_FATAL_ERROR("Failed getting key map from X11");
			XCloseDisplay(x11_pDummyDisplay);
			return;
		}
		XCloseDisplay(x11_pDummyDisplay);
#endif
		if (existingKeys.empty()) {
			RE_FATAL_ERROR("No physical keys have been found");
			return;
		}
		REuint u32Index = 0U;
		std::vector<KeyInfo> usedInputs;
		for (KeyInfo info : existingKeys) {
			if (u32Index >= MAXIMUM_PHYSICAL_KEYS) {
				RE_ERROR("There are more physical keys present than the amount, that can be stored. The rest will be discarded and cannot be used");
				break;
			}
			u32Scancodes[u32Index] = info.u32Scancode;
			eInputs[u32Index] = info.eInput;
			u32Index++;
			if (info.eInput == RE_INPUT_UNKNOWN)
				continue;
			std::vector<KeyInfo>::iterator it = std::find_if(usedInputs.begin(), usedInputs.end(), [info](const KeyInfo infoParam){return info.eInput == infoParam.eInput;});
			if (it != usedInputs.end())
				RE_WARNING(append_to_string("Input ", hexadecimal_to_string<REshort>(info.eInput, true), " appears first at scancode ", hexadecimal_to_string(it->u32Scancode, true), " and has been found again at scancode ", hexadecimal_to_string(info.u32Scancode, true)));
			else
				usedInputs.push_back(info);
		}
		u32NumberOfKeys = existingKeys.size();
		std::fill(std::begin(u8KeyBuffer), std::end(u8KeyBuffer), 0U);
		std::fill(std::begin(u8PrevKeyBuffer), std::end(u8PrevKeyBuffer), 0U);
	}

	InputMgr::~InputMgr() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
	}

	REint InputMgr::get_index_for_scancode(REuint u32SearchedScancode) {
		REuint u32CurrentIndex = u32NumberOfKeys / 2U, u32MinIndex = 0U, u32MaxIndex = u32NumberOfKeys - 1U;
		REuint u32CurrentScancode = 0U;
		while (true) {
			CATCH_SIGNAL_DETAILED(u32CurrentScancode = u32Scancodes[u32CurrentIndex], append_to_string("Index: ", u32CurrentIndex).c_str());
			if (u32CurrentScancode == u32SearchedScancode)
				return u32CurrentIndex;
			else if (u32CurrentScancode > u32SearchedScancode && u32MinIndex < u32CurrentIndex)
				u32MaxIndex = u32CurrentIndex - 1U;
			else if (u32CurrentScancode < u32SearchedScancode && u32MaxIndex > u32CurrentIndex)
				u32MinIndex = u32CurrentIndex + 1U;
			else
				return -1;
			u32CurrentIndex = u32MinIndex + (u32MaxIndex - u32MinIndex) / 2U;
		}
	}

	REint InputMgr::get_index_for_input(Input eSearchedInput) {
		for (REuint i = 0U; i < u32NumberOfKeys; i++)
			if (eInputs[i] == eSearchedInput)
				return static_cast<REint>(i);
		return -1;
	}

	bool InputMgr::process_request(Input eInput, REuint u32Scancode, bool bRequestForPast) {
		if (eInput >= RE_INPUT_UNKNOWN && eInput < RE_INPUT_MAX_ENUM) {
			switch (eInput) {
				case RE_INPUT_SCROLL_UP:
				case RE_INPUT_SCROLL_DOWN:
				case RE_INPUT_BUTTON_LEFT:
				case RE_INPUT_BUTTON_RIGHT:
				case RE_INPUT_BUTTON_MIDDLE:
					return is_bit_true<REubyte>(u8SpecialInputBuffer, static_cast<REubyte>(eInput - RE_INPUT_SCROLL_UP));
				default:
					REint i32KeyIndex = -1;
					if (u32Scancode)
						i32KeyIndex = get_index_for_scancode(u32Scancode);
					if (i32KeyIndex < 0 && eInput != RE_INPUT_UNKNOWN)
						i32KeyIndex = get_index_for_input(eInput);
					if (bRequestForPast)
						return i32KeyIndex >= 0 ? is_bit_true<REubyte>(u8PrevKeyBuffer[i32KeyIndex / 8], i32KeyIndex % 8) : false;
					else
						return i32KeyIndex >= 0 ? is_bit_true<REubyte>(u8KeyBuffer[i32KeyIndex / 8], i32KeyIndex % 8) : false;
			}
		}
		return false;
	}

	void InputMgr::input_event(Input eInput, REuint u32Scancode, bool bPressed, bool bFallbackToInput) {
		switch (eInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE:
				CATCH_SIGNAL(set_bit<REubyte>(u8SpecialInputBuffer, static_cast<REubyte>(eInput - RE_INPUT_SCROLL_UP), bPressed));
				break;
			default: // Keyboard input
				if (eInput >= RE_INPUT_UNKNOWN && eInput < RE_INPUT_MAX_ENUM) {
					REint i32KeyIndex = -1;
					if (u32Scancode)
						CATCH_SIGNAL(i32KeyIndex = get_index_for_scancode(u32Scancode));
					if (i32KeyIndex < 0 && bFallbackToInput && eInput != RE_INPUT_UNKNOWN)
						CATCH_SIGNAL(i32KeyIndex = get_index_for_input(eInput));
					if (i32KeyIndex < 0) // Input entirely unknown
						RE_NOTE(append_to_string("The scancode is missing in the scancode list: ", hexadecimal_to_string(u32Scancode, true), " (Assumed input: ", hexadecimal_to_string<REshort>(eInput, true), ")"));
					else
						CATCH_SIGNAL(set_bit<REubyte>(u8KeyBuffer[i32KeyIndex / 8], i32KeyIndex % 8, bPressed));
				}
				break;
		}
	}

	void InputMgr::cursor_event(REint i32X, REint i32Y) {
		cursorPosition[0] = i32X;
		cursorPosition[1] = i32Y;
	}

	void InputMgr::update_input_buffers() {
		CATCH_SIGNAL(prevCursorPosition.copy_from(cursorPosition));
		CATCH_SIGNAL(std::copy(std::begin(u8KeyBuffer), std::end(u8KeyBuffer), std::begin(u8PrevKeyBuffer)));
		u8PrevSpecialInputBuffer = u8SpecialInputBuffer;
		CATCH_SIGNAL(set_bits<REubyte>(u8SpecialInputBuffer, RE_INPUT_SCROLL_UP, RE_INPUT_SCROLL_DOWN, false));
	}

	Input InputMgr::map_scancode_to_input(REuint u32Scancode) {
		if (u32Scancode) {
			REint i32Index = get_index_for_scancode(u32Scancode);
			if (i32Index >= 0 && i32Index < static_cast<REint>(u32NumberOfKeys))
				return eInputs[i32Index];
		}
		return RE_INPUT_UNKNOWN;
	}

	REuint InputMgr::map_input_to_scancode(Input eInput) {
		if (eInput >= RE_INPUT_UNKNOWN && eInput < RE_INPUT_MAX_ENUM) {
			REint i32Index = get_index_for_input(eInput);
			if (i32Index >= 0 && i32Index < static_cast<REint>(u32NumberOfKeys))
				return u32Scancodes[i32Index];
		}
		return 0U;
	}

	bool InputMgr::is_down(Input eInput, REuint u32Scancode) {
		return process_request(eInput, u32Scancode, false);
	}
	
	bool InputMgr::was_down(Input eInput, REuint u32Scancode) {
		return process_request(eInput, u32Scancode, true);
	}

	Input map_scancode_to_input(REuint u32Scancode) {
		return InputMgr::pInstance ? InputMgr::pInstance->map_scancode_to_input(u32Scancode) : RE_INPUT_UNKNOWN;
	}

	REuint map_input_to_scancode(Input eInput) {
		return InputMgr::pInstance ? InputMgr::pInstance->map_input_to_scancode(eInput) : 0U;
	}

}
