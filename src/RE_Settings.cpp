#include "RE_Settings_Internal.hpp"
#include "RE_Renderer.hpp"

namespace RE {
	
	SettingsFlags_t mSettingsFlags = SETTINGS_FLAG_FULLSCREEN_BIT;
	VkSampleCountFlagBits vk_eMsaaMode = VK_SAMPLE_COUNT_1_BIT;
	VkFilter vk_eScreenFilter = VK_FILTER_NEAREST;
	unsigned uScreenPercentage = SCREEN_PERCENTAGE_100_PERCENT;
	float fMaxDeltatime = 1.0f / 15,
		fMinDeltatime = 1.0f / 60,
		fSampleShadingRate = 1.0f;

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
		if (init_settings_gui())
			mSettingsFlags |= SETTINGS_FLAG_MENU_OPEN_BIT;
		else
			RE_ERROR("Failed to open settings GUI");
	}

	void close_settings() {
		destroy_settings_gui();
		mSettingsFlags &= ~SETTINGS_FLAG_MENU_OPEN_BIT;
		reset_all_input();
	}

}
