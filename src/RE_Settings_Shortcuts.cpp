#include "RE_Settings_Internal.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	void shortcut_settings_f1(bool bFuncKeyPressed) {
		// Toggle audio mute
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
	}

	void shortcut_settings_f2(bool bFuncKeyPressed) {
		// Decrease volume
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
	}

	void shortcut_settings_f3(bool bFuncKeyPressed) {
		// Increase volume
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
	}

	void shortcut_settings_f4(bool bFuncKeyPressed) {
		// Toggle microphone mute
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
	}

	void shortcut_settings_f5(bool bFuncKeyPressed) {
		// Decrease quality of graphics and audio
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
	}

	void shortcut_settings_f6(bool bFuncKeyPressed) {
		// Increase quality of graphics and audio
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
	}

	void shortcut_settings_f7(bool bFuncKeyPressed) {
		// Decrease screen percentage
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
	}

	void shortcut_settings_f8(bool bFuncKeyPressed) {
		// Increase screen percentage
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
	}

	void shortcut_settings_f9(bool bFuncKeyPressed) {
		// Set all settings to normal
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
	}

	void shortcut_settings_f10(bool bFuncKeyPressed) {
		// Open settings GUI menu
		open_settings();
	}

	void shortcut_settings_f11(bool bFuncKeyPressed) {
		// Toggle fullscreen
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
		if ((mSettingsFlags & SETTINGS_FLAG_FULLSCREEN_BIT))
			mSettingsFlags &= ~SETTINGS_FLAG_FULLSCREEN_BIT;
		else
			mSettingsFlags |= SETTINGS_FLAG_FULLSCREEN_BIT;
		update_fullscreen();
	}

	void shortcut_settings_f12(bool bFuncKeyPressed) {
		// Take screenshot
		if (!bFuncKeyPressed || (mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			return;
	}

}
