#ifndef __RE_RENDERER_SETTINGS_GUI_INTERNAL_H__
#define __RE_RENDERER_SETTINGS_GUI_INTERNAL_H__

#include "RE_Renderer_SettingsGUI.hpp"

namespace RE {
	
	extern VkRenderPass vk_hSettingsRenderPass;
	extern std::unique_ptr<VkFramebuffer[]> std_settingsFramebuffers;

}

#endif /* __RE_RENDERER_SETTINGS_GUI_INTERNAL_H__ */
