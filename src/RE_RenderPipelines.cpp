#include "RE_RenderPipelines_Internal.hpp"

namespace RE {

	bool create_render_pipelines() {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_render_pipeline_game_objects(), bool);
	}

	bool recreate_render_pipelines() {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(recreate_render_pipeline_game_objects(), bool);
	}

	void destroy_render_pipelines() {
		PUSH_TO_CALLSTACKTRACE(destroy_render_pipeline_game_objects());
	}

}
