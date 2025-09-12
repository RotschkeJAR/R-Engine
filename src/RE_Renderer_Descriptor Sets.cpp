#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	bool create_descriptor_sets() {
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_camera_descriptor_sets(), bool)) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_texture_descriptor_sets(), bool))
				return true;
			PUSH_TO_CALLSTACKTRACE(destroy_camera_descriptor_sets());
		}
		return false;
	}

	void destroy_descriptor_sets() {
		PUSH_TO_CALLSTACKTRACE(destroy_texture_descriptor_sets());
		PUSH_TO_CALLSTACKTRACE(destroy_camera_descriptor_sets());
	}

}
