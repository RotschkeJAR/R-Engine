#ifndef __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__
#define __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__

#include "RE_Renderer_Descriptor Sets.hpp"

namespace RE {

	bool create_game_objects_descriptor_sets();
	void destroy_game_objects_descriptor_sets();
	
	bool create_sprite_descriptor_sets();
	void destroy_sprite_descriptor_sets();

	bool create_camera_descriptor_sets();
	void destroy_camera_descriptor_sets();

	bool create_sortable_depth_desc_sets();
	void destroy_sortable_depth_desc_sets();

}

#endif /* __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__ */
