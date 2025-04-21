#include "RE_Input.hpp"
#include "RE_Window.hpp"

namespace RE {

	struct KeyInfo {
		REuint u32Scancode;
		Input eInput;
	};

	REuint u32Scancodes[MAXIMUM_PHYSICAL_KEYS] = {};
	Input eInputs[MAXIMUM_PHYSICAL_KEYS] = {};
	REubyte u8KeyBuffer[KEY_BUFFER_SIZE] = {}, u8PrevKeyBuffer[KEY_BUFFER_SIZE] = {}; // Keyboard
	REubyte u8NumberOfKeys = 0U;
	REubyte u8SpecialInputBuffer = 0U, u8PrevSpecialInputBuffer = 0U; // Scrolling and mouse buttons
	Vector2i cursorPosition, prevCursorPosition;
	InputAction *pUpdateInputObject = nullptr;
	InputMgr *InputMgr::pInstance = nullptr;

	InputMgr::InputMgr() {
		if (pInstance) {
			RE_FATAL_ERROR("An instance of the class \"InputMgr\" has already been constructed");
			return;
		}
		pInstance = this;
		/* Deprecated: Legacy code for filling the scancode table
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
						bRetry = !bRetry; // Retry required, because virtual keycode can represent enter and numpad enter-key
						// Don't break here!
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
					KeyInfo newInfo = {win_scancode, key_from_virtual_keycode(u32VirtualKeycodeDummy)};
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
		XKeySym *const x11_pKeyMap = XGetKeyboardMapping(x11_pDummyDisplay, i32MinScancode, i32MaxScancode - i32MinScancode + 1, &i32KeysymsPerScancode);
		if (x11_pKeyMap) {
			for (REint i32ScancodeElement = i32MinScancode; i32ScancodeElement <= i32MaxScancode; i32ScancodeElement++) {
				if (x11_pKeyMap[(i32ScancodeElement - i32MinScancode) * i32KeysymsPerScancode] == XNoSymbol)
					continue;
				PRINT(append_to_string("Scancode: ", hexadecimal_to_string(i32ScancodeElement, true)));
				Input eInputAlias = RE_INPUT_UNKNOWN;
				for (REint i32KeySymbol = 0; i32KeySymbol < i32KeysymsPerScancode; i32KeySymbol++) {
					eInputAlias = key_from_virtual_keycode(x11_pKeyMap[(i32ScancodeElement - i32MinScancode) * i32KeysymsPerScancode + i32KeySymbol]);
					if (eInputAlias != RE_INPUT_UNKNOWN)
						break;
				}
				if (eInputAlias == RE_INPUT_UNKNOWN) {
					for (REint i = 0; i < i32KeysymsPerScancode; i++)
						print(append_to_string(", ", hexadecimal_to_string(x11_pKeyMap[(i32ScancodeElement - i32MinScancode) * i32KeysymsPerScancode + i])));
					println();
					continue;
				}
				existingKeys.push_back({static_cast<REuint>(i32ScancodeElement), eInputAlias});
				println();
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
		u8NumberOfKeys = static_cast<REubyte>(std::clamp<REuint>(existingKeys.size(), 0U, MAXIMUM_PHYSICAL_KEYS));
		PRINT_LN(static_cast<REuint>(u8NumberOfKeys));
		*/
		std::fill(std::begin(u32Scancodes), std::end(u32Scancodes), 0U);
		std::fill(std::begin(eInputs), std::end(eInputs), RE_INPUT_UNKNOWN);
		std::fill(std::begin(u8KeyBuffer), std::end(u8KeyBuffer), 0U);
		std::fill(std::begin(u8PrevKeyBuffer), std::end(u8PrevKeyBuffer), 0U);
	}

	InputMgr::~InputMgr() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
	}

	REshort InputMgr::get_index_for_scancode(REuint u32SearchedScancode) {
		REuint u32CurrentIndex = u8NumberOfKeys / 2U, u32MinIndex = 0U, u32MaxIndex = u8NumberOfKeys > 0U ? (u8NumberOfKeys - 1U) : 0U;
		REuint u32CurrentScancode = 0U;
		while (true) {
			CATCH_SIGNAL_DETAILED(u32CurrentScancode = u32Scancodes[u32CurrentIndex], append_to_string("Index: ", u32CurrentIndex).c_str());
			if (u32CurrentScancode == u32SearchedScancode)
				return static_cast<REshort>(u32CurrentIndex);
			else if (u32CurrentScancode > u32SearchedScancode && u32MinIndex < u32CurrentIndex)
				u32MaxIndex = u32CurrentIndex - 1U;
			else if (u32CurrentScancode < u32SearchedScancode && u32MaxIndex > u32CurrentIndex)
				u32MinIndex = u32CurrentIndex + 1U;
			else
				return -1;
			u32CurrentIndex = u32MinIndex + (u32MaxIndex - u32MinIndex) / 2U;
		}
	}

	REshort InputMgr::get_index_for_input(Input eSearchedInput) {
		for (REuint i = 0U; i < u8NumberOfKeys; i++)
			if (eInputs[i] == eSearchedInput)
				return static_cast<REshort>(i);
		return -1;
	}

	bool InputMgr::process_request(Input eInput, REuint u32Scancode, bool bRequestForPast) {
		if (eInput >= RE_INPUT_UNKNOWN && eInput < RE_INPUT_MAX_ENUM) {
			DEFINE_SIGNAL_GUARD(sigGuardProcessingRequest);
			switch (eInput) {
				case RE_INPUT_SCROLL_UP:
				case RE_INPUT_SCROLL_DOWN:
				case RE_INPUT_BUTTON_LEFT:
				case RE_INPUT_BUTTON_RIGHT:
				case RE_INPUT_BUTTON_MIDDLE:
					if (bRequestForPast)
						return is_bit_true<REubyte>(u8PrevSpecialInputBuffer, static_cast<REubyte>(eInput - RE_INPUT_SCROLL_UP));
					else
						return is_bit_true<REubyte>(u8SpecialInputBuffer, static_cast<REubyte>(eInput - RE_INPUT_SCROLL_UP));
				default:
					REshort i16KeyIndex = -1;
					if (u32Scancode)
						i16KeyIndex = get_index_for_scancode(u32Scancode);
					if (i16KeyIndex < 0 && eInput != RE_INPUT_UNKNOWN) {
						i16KeyIndex = get_index_for_input(eInput);
						if (i16KeyIndex >= 0)
							u32Scancode = u32Scancodes[i16KeyIndex];
					}
					if (i16KeyIndex < 0)
						return false;
					if (bRequestForPast)
						return i16KeyIndex >= 0 ? is_bit_true<REubyte>(u8PrevKeyBuffer[i16KeyIndex / 8], i16KeyIndex % 8) : false;
					else
						return i16KeyIndex >= 0 ? is_bit_true<REubyte>(u8KeyBuffer[i16KeyIndex / 8], i16KeyIndex % 8) : false;
			}
		}
		return false;
	}

	void InputMgr::input_event(const Input eEnteredInput, const REuint u32EnteredScancode, const bool bPressed, const bool bFallbackToInput) {
		switch (eEnteredInput) {
			case RE_INPUT_SCROLL_UP:
			case RE_INPUT_SCROLL_DOWN:
			case RE_INPUT_BUTTON_LEFT:
			case RE_INPUT_BUTTON_RIGHT:
			case RE_INPUT_BUTTON_MIDDLE:
				CATCH_SIGNAL(set_bit<REubyte>(u8SpecialInputBuffer, static_cast<REubyte>(eEnteredInput - RE_INPUT_SCROLL_UP), bPressed));
				if (pUpdateInputObject && bPressed) {
					pUpdateInputObject->change_input(eEnteredInput);
					pUpdateInputObject = nullptr;
				}
				break;
			default: // Keyboard input
				if ((!u32EnteredScancode && !bFallbackToInput) || eEnteredInput < RE_INPUT_UNKNOWN || eEnteredInput >= RE_INPUT_MAX_ENUM)
					break;
				REshort i16KeyIndex = -1;
				if (u32EnteredScancode)
					CATCH_SIGNAL(i16KeyIndex = get_index_for_scancode(u32EnteredScancode));
				if (i16KeyIndex < 0 && bFallbackToInput && eEnteredInput != RE_INPUT_UNKNOWN)
					CATCH_SIGNAL(i16KeyIndex = get_index_for_input(eEnteredInput));
				if (i16KeyIndex >= 0)
					CATCH_SIGNAL(set_bit<REubyte>(u8KeyBuffer[i16KeyIndex / 8], i16KeyIndex % 8, bPressed));
				else { // Keyboard input unknown
					if (u8NumberOfKeys >= MAXIMUM_PHYSICAL_KEYS) {
						RE_WARNING(append_to_string("New scancode ", hexadecimal_to_string(u32EnteredScancode, true), " cannot be added, because the list is full"));
						break;
					}
					REubyte u8InsertionIndex = u8NumberOfKeys;
					{ // Looking for suitable place to insert new scancode to keep ascending order
						REubyte u8CurrentIndex = u8NumberOfKeys / 2U, u8MinIndex = 0U, u8MaxIndex = u8NumberOfKeys > 0U ? (u8NumberOfKeys - 1U) : 0U;
						while (true) {
							const REuint u32CurrentScancode = u32Scancodes[u8CurrentIndex];
							if (u32EnteredScancode < u32CurrentScancode) {
								if (u8CurrentIndex == 0U) // Success: Place for new scancode found
									break;
								else {
									const REuint u32LowerScancode = u32Scancodes[u8CurrentIndex - 1U];
									if (u32EnteredScancode > u32LowerScancode) // Success: Place for new scancode found
										break;
									u8MaxIndex = u8CurrentIndex > 1U ? (u8CurrentIndex - 2U) : 0U;
								}
							} else {
								if (u8CurrentIndex == u8MaxIndex && u8CurrentIndex < MAXIMUM_PHYSICAL_KEYS) // Success: Place for new scancode found
									break;
								u8MinIndex = std::clamp(u8CurrentIndex + 1U, 0U, MAXIMUM_PHYSICAL_KEYS - 1U);
							}
							u8CurrentIndex = std::clamp(u8MinIndex + (u8MaxIndex - u8MinIndex) / 2U, 0U, MAXIMUM_PHYSICAL_KEYS - 1U);
						}
						u8InsertionIndex = u8CurrentIndex;
					}
					for (REubyte i = u8NumberOfKeys; i > u8InsertionIndex; i--) {
						const REubyte u8LowerIndex = i - 1U;
						u32Scancodes[i] = u32Scancodes[u8LowerIndex];
						eInputs[i] = eInputs[u8LowerIndex];
						set_bit<REubyte>(u8KeyBuffer[u8LowerIndex / 8U], u8LowerIndex % 8U, is_bit_true<REubyte>(u8KeyBuffer[i / 8U], i % 8U));
						set_bit<REubyte>(u8PrevKeyBuffer[u8LowerIndex / 8U], u8LowerIndex % 8U, is_bit_true<REubyte>(u8PrevKeyBuffer[i / 8U], i % 8U));
					}
					u32Scancodes[u8InsertionIndex] = u32EnteredScancode;
					eInputs[u8InsertionIndex] = eEnteredInput;
					set_bit<REubyte>(u8KeyBuffer[u8InsertionIndex / 8U], u8InsertionIndex % 8U, bPressed);
					set_bit<REubyte>(u8PrevKeyBuffer[u8InsertionIndex / 8U], u8InsertionIndex % 8U, !bPressed);
					u8NumberOfKeys++;
					RE_NOTE(append_to_string("Remaining slots for more undetected physical keys: ", MAXIMUM_PHYSICAL_KEYS - u8NumberOfKeys));
				}
				if (pUpdateInputObject && bPressed) {
					if (u32EnteredScancode)
						pUpdateInputObject->change_scancode(u32EnteredScancode);
					else
						pUpdateInputObject->change_input(eEnteredInput);
					pUpdateInputObject = nullptr;
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
			REshort i16Index = get_index_for_scancode(u32Scancode);
			if (i16Index >= 0 && i16Index < static_cast<REshort>(u8NumberOfKeys))
				return eInputs[i16Index];
		}
		return RE_INPUT_UNKNOWN;
	}

	REuint InputMgr::map_input_to_scancode(Input eInput) {
		if (eInput >= RE_INPUT_KEY_SPACE && eInput < RE_INPUT_MAX_ENUM) {
			REshort i16Index = get_index_for_input(eInput);
			if (i16Index >= 0 && i16Index < static_cast<REshort>(u8NumberOfKeys))
				return u32Scancodes[i16Index];
		}
		return 0U;
	}

	bool InputMgr::is_down(Input eInput, REuint u32Scancode) {
		return process_request(eInput, u32Scancode, false);
	}
	
	bool InputMgr::was_down(Input eInput, REuint u32Scancode) {
		return process_request(eInput, u32Scancode, true);
	}

	REint InputMgr::get_cursor_x() {
		return cursorPosition[0];
	}
	
	REint InputMgr::get_cursor_y() {
		return cursorPosition[1];
	}
	
	Vector2i InputMgr::get_cursor() {
		return Vector2i(cursorPosition);
	}

	int32_t get_cursor_position_x() {
		return cursorPosition[0];
	}

	int32_t get_cursor_position_y() {
		return cursorPosition[1];
	}

	float get_cursor_normal_position_x() {
		return cursorPosition[0] / static_cast<float>(Window::pInstance->get_size()[0]);
	}

	float get_cursor_normal_position_y() {
		return cursorPosition[1] / static_cast<float>(Window::pInstance->get_size()[1]);
	}

	Input map_scancode_to_input(REuint u32Scancode) {
		return InputMgr::pInstance ? InputMgr::pInstance->map_scancode_to_input(u32Scancode) : RE_INPUT_UNKNOWN;
	}

	REuint map_input_to_scancode(Input eInput) {
		return InputMgr::pInstance ? InputMgr::pInstance->map_input_to_scancode(eInput) : 0U;
	}

}
