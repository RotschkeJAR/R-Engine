#include "RE_Renderer_Pipelines_Internal.hpp"

namespace RE {
	
	bool create_renderer_pipelines() {
		PRINT_DEBUG("Creating Vulkan pipelines");
		if (create_pipeline_layouts()) {
			if (create_graphics_pipelines()) {
				if (create_compute_pipelines())
					return true;
				destroy_graphics_pipelines();
			}
			destroy_pipeline_layouts();
		}
		return false;
	}

	void destroy_renderer_pipelines() {
		PRINT_DEBUG("Destroying Vulkan pipelines");
		destroy_compute_pipelines();
		destroy_graphics_pipelines();
		destroy_pipeline_layouts();
	}

}
