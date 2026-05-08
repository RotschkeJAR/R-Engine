#include "RE_Renderer_Descriptor Sets_Internal.hpp"
#include "RE_Renderer_Descriptor Set Pools.hpp"

namespace RE {

	bool create_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan descriptor sets");
		if (create_descriptor_set_layouts()) {
			if (create_descriptor_set_pools()) {
				if (create_raw_game_object_buffer_descriptor_sets()) {
					if (create_render_content_descriptor_sets()) {
						if (create_texture_descriptor_sets())
							return true;
						destroy_render_content_descriptor_sets();
					}
					destroy_raw_game_object_buffer_descriptor_sets();
				}
				destroy_descriptor_set_pools();
			}
			destroy_descriptor_set_layouts();
		}
		return false;
	}

	void destroy_descriptor_sets() {
		PRINT_DEBUG("Destroying Vulkan descriptor sets");
		destroy_texture_descriptor_sets();
		destroy_render_content_descriptor_sets();
		destroy_raw_game_object_buffer_descriptor_sets();
		destroy_descriptor_set_pools();
		destroy_descriptor_set_layouts();
	}

}
