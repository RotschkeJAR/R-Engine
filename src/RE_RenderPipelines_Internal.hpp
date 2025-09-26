#ifndef __RE_RENDER_PIPELINES_INTERNAL_H__
#define __RE_RENDER_PIPELINES_INTERNAL_H__

#include "RE_RenderPipelines.hpp"

namespace RE {
	
	// GameObject
	bool create_render_pipeline_game_objects();
	bool recreate_render_pipeline_game_objects();
	void destroy_render_pipeline_game_objects();

}

#endif /* __RE_RENDER_PIPELINES_INTERNAL_H__ */
