#include "RE_Renderer_Pipelines_Compute_Internal.hpp"

namespace RE {
	
	bool create_compute_pipelines() {
		PRINT_DEBUG("Creating Vulkan compute pipelines");
		if (create_compute_pipeline_preprocessing()) {
			if (create_compute_pipeline_depth_sorting())
				return true;
			destroy_compute_pipeline_preprocessing();
		}
		return false;
	}

	void destroy_compute_pipelines() {
		PRINT_DEBUG("Destroying Vulkan compute pipelines");
		destroy_compute_pipeline_preprocessing();
		destroy_compute_pipeline_depth_sorting();
	}

}
