#include "RE_Renderer_Descriptor Sets_Internal.hpp"

namespace RE {

	bool create_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan descriptor sets");
		if (create_camera_descriptor_sets()) {
			if (create_texture_descriptor_sets())
				return true;
			destroy_camera_descriptor_sets();
		}
		return false;
	}

	void destroy_descriptor_sets() {
		PRINT_DEBUG("Destroying Vulkan descriptor sets");
		destroy_texture_descriptor_sets();
		destroy_camera_descriptor_sets();
	}

}
