#ifndef __RE_RENDERER_PIPELINES_H__
#define __RE_RENDERER_PIPELINES_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern VkPipelineLayout vk_hGraphicsPipelineLayout,
		vk_hSortDepthPipelineLayout,
		vk_hProcessingPipelineLayout;

	extern VkPipeline vk_hGraphicsPipeline2D,
		vk_hGraphicsPipeline2DOpaqueOnly,
		vk_hGraphicsPipeline3D,
		vk_hGraphicsPipeline3DOpaqueOnly,
		vk_hComputePipelinePreprocessing,
		vk_hComputePipelineDepthSorting;

	bool create_renderer_pipelines();
	void destroy_renderer_pipelines();

	bool recreate_graphics_pipelines();

}

#endif /* __RE_RENDERER_PIPELINES_H__ */
