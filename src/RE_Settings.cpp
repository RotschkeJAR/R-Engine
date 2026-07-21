#include "RE_Settings_Internal.hpp"

namespace RE {
	
	SettingsFlags_t mSettingsFlags = SETTINGS_FLAG_FULLSCREEN_BIT;
	VkSampleCountFlagBits vk_eMsaaMode = VK_SAMPLE_COUNT_1_BIT;
	float fMaxDeltatime = 1.0f / 15,
		fMinDeltatime = 1.0f / 60;

	bool load_settings() {
		return true;
	}

	bool init_settings() {
		return true;
	}

	void destroy_settings() {
		
	}

	void settings_update() {

	}

	void open_settings() {
		mSettingsFlags |= SETTINGS_FLAG_MENU_OPEN_BIT;
	}

	void close_settings() {
		mSettingsFlags &= ~SETTINGS_FLAG_MENU_OPEN_BIT;
		reset_all_input();
	}

}
