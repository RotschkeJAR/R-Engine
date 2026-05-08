#ifndef __RE_RENDERER_PIPELINES_COMPUTE_INTERNAL_H__
#define __RE_RENDERER_PIPELINES_COMPUTE_INTERNAL_H__

#include "RE_Renderer_Pipelines_Compute.hpp"

namespace RE {
	
	bool create_compute_pipeline_preprocessing();
	void destroy_compute_pipeline_preprocessing();

	bool create_compute_pipeline_depth_sorting();
	void destroy_compute_pipeline_depth_sorting();

}

#endif /* __RE_RENDERER_PIPELINES_COMPUTE_INTERNAL_H__ */
