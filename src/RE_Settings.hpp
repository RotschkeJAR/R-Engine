#ifndef __RE_SETTINGS_H__
#define __RE_SETTINGS_H__ 1

#include "RE_GPU.hpp"

namespace RE {

	typedef uint SettingsFlags_t;
	enum SettingsFlag : SettingsFlags_t {
		SETTINGS_FLAG_MENU_OPEN_BIT = 0x1,
		SETTINGS_FLAG_FULLSCREEN_BIT = 0x2,
		SETTINGS_FLAG_VSYNC_ENABLED_BIT = 0x4
	};

#define SCREEN_PERCENTAGE_100_PERCENT 10000U
	
	extern SettingsFlags_t mSettingsFlags;
	extern VkSampleCountFlagBits vk_eMsaaMode;
	extern VkFilter vk_eScreenFilter;
	extern unsigned uScreenPercentage;
	extern float fMaxDeltatime,
		fMinDeltatime,
		fSampleShadingRate;

#define IS_MSAA_ENABLED()                     (vk_eMsaaMode != VK_SAMPLE_COUNT_1_BIT)
#define IS_SAMPLE_SHADING_ENABLED()           (fSampleShadingRate > 0.0f)
#define RENDER_IMAGE_SIZE_EQUALS_SWAPCHAIN()  (uScreenPercentage == SCREEN_PERCENTAGE_100_PERCENT)
#define IS_SINGLESAMPLED_IMAGE_REQUIRED()     (IS_MSAA_ENABLED() && !RENDER_IMAGE_SIZE_EQUALS_SWAPCHAIN())

	bool load_settings();
	bool init_settings();
	void destroy_settings();
	void settings_update();

	void settings_handle_keyboard_input(uint32_t u32VirtualKeycode);

	void shortcut_settings_f1(bool bFuncKeyPressed);
	void shortcut_settings_f2(bool bFuncKeyPressed);
	void shortcut_settings_f3(bool bFuncKeyPressed);
	void shortcut_settings_f4(bool bFuncKeyPressed);
	void shortcut_settings_f5(bool bFuncKeyPressed);
	void shortcut_settings_f6(bool bFuncKeyPressed);
	void shortcut_settings_f7(bool bFuncKeyPressed);
	void shortcut_settings_f8(bool bFuncKeyPressed);
	void shortcut_settings_f9(bool bFuncKeyPressed);
	void shortcut_settings_f10(bool bFuncKeyPressed);
	void shortcut_settings_f11(bool bFuncKeyPressed);
	void shortcut_settings_f12(bool bFuncKeyPressed);

}

#endif /* __RE_SETTINGS_H__ */
