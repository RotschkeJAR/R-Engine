#ifndef __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__
#define __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__

#include "RE_Renderer_Descriptor Sets.hpp"

namespace RE {

	bool create_render_content_descriptor_sets();
	void destroy_render_content_descriptor_sets();
	
	bool create_texture_descriptor_sets();
	void destroy_texture_descriptor_sets();

	bool create_raw_game_object_buffer_descriptor_sets();
	void destroy_raw_game_object_buffer_descriptor_sets();

}

#endif /* __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__ */
