#ifndef __RE_RENDERER_PIPELINES_LAYOUTS_H__
#define __RE_RENDERER_PIPELINES_LAYOUTS_H__

#include "RE_Renderer_Pipelines_Internal.hpp"

namespace RE {
	
	extern VkPipelineLayout vk_hGraphicsPipelineLayout,
		vk_hComputePipelineLayoutProcessing;

	bool create_pipeline_layouts();
	void destroy_pipeline_layouts();

}

#endif /* __RE_RENDERER_PIPELINES_LAYOUTS_H__ */