#include "RE_Sub Renderer_Internal.hpp"

namespace RE {
	
	bool init_sub_renderers() {
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_game_object_renderer(), bool))
			return true;
		return false;
	}

	void destroy_sub_renderers() {
		PUSH_TO_CALLSTACKTRACE(destroy_game_object_renderer());
	}

}
