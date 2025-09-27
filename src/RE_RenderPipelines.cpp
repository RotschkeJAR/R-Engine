#include "RE_RenderPipelines_Internal.hpp"

namespace RE {

	bool create_render_pipelines() {
		return create_render_pipeline_game_objects();
	}

	bool recreate_render_pipelines() {
		return recreate_render_pipeline_game_objects();
	}

	void destroy_render_pipelines() {
		destroy_render_pipeline_game_objects();
	}

}
