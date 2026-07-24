#ifndef __RE_RENDERER_SETTINGS_GUI_RENDER_PASS_H__
#define __RE_RENDERER_SETTINGS_GUI_RENDER_PASS_H__

#include "RE_Renderer_SettingsGUI_Internal.hpp"

namespace RE {
	
	extern VkRenderPass vk_hSettingsRenderPass;

	bool create_settings_gui_renderpass();
	void destroy_settings_gui_renderpass();

}

#endif /* __RE_RENDERER_SETTINGS_GUI_RENDER_PASS_H__ */
