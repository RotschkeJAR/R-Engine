#include "RE_Renderer_Descriptor Sets_Internal.hpp"
#include "RE_Renderer_Descriptor Set Pools.hpp"

namespace RE {

	bool create_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan descriptor sets");
		if (create_descriptor_set_layouts()) {
			if (create_descriptor_set_pools()) {
				if (create_game_objects_descriptor_sets()) {
					if (create_sortable_depth_desc_sets()) {
						if (create_camera_descriptor_sets()) {
							if (create_sprite_descriptor_sets()) {
								if (create_cursor_descriptor_sets())
									return true;
								destroy_sprite_descriptor_sets();
							}
							destroy_camera_descriptor_sets();
						}
						destroy_sortable_depth_desc_sets();
					}
					destroy_game_objects_descriptor_sets();
				}
				destroy_descriptor_set_pools();
			}
			destroy_descriptor_set_layouts();
		}
		return false;
	}

	void destroy_descriptor_sets() {
		PRINT_DEBUG("Destroying Vulkan descriptor sets");
		destroy_cursor_descriptor_sets();
		destroy_sprite_descriptor_sets();
		destroy_camera_descriptor_sets();
		destroy_sortable_depth_desc_sets();
		destroy_game_objects_descriptor_sets();
		destroy_descriptor_set_pools();
		destroy_descriptor_set_layouts();
	}

}
