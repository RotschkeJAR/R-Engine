#include "RE_Renderer_SettingsGUI_Internal.hpp"

namespace RE {
	
	bool init_settings_gui() {
		if (create_settings_gui_renderpass()) {
			return true;
		}
		return false;
	}

	void destroy_settings_gui() {
		destroy_settings_gui_renderpass();
	}

	void render_settings_gui() {
		
	}

}
