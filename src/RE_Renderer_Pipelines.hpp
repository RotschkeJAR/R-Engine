#ifndef __RE_RENDERER_PIPELINES_H__
#define __RE_RENDERER_PIPELINES_H__ 1

#define GRAPHICS_PIPELINE_COUNT       4
#define COMPUTE_PIPELINE_COUNT           2

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern VkPipelineLayout vk_hGraphicsPipelineLayout,
		vk_hSortDepthPipelineLayout,
		vk_hProcessingPipelineLayout,
		vk_hTextPipelineLayout;
		
#define GRAPHICS_PIPELINE_2D          0
#define GRAPHICS_PIPELINE_2D_OPAQUE   1
#define GRAPHICS_PIPELINE_3D          2
#define GRAPHICS_PIPELINE_3D_OPAQUE   3

	extern VkPipeline vk_ahGraphicsPipelines[GRAPHICS_PIPELINE_COUNT];

#define vk_hGraphicsPipeline2D           vk_ahGraphicsPipelines[GRAPHICS_PIPELINE_2D]
#define vk_hGraphicsPipeline2DOpaqueOnly vk_ahGraphicsPipelines[GRAPHICS_PIPELINE_2D_OPAQUE]
#define vk_hGraphicsPipeline3D           vk_ahGraphicsPipelines[GRAPHICS_PIPELINE_3D]
#define vk_hGraphicsPipeline3DOpaqueOnly vk_ahGraphicsPipelines[GRAPHICS_PIPELINE_3D_OPAQUE]

#define COMPUTE_PIPELINE_PREPROCESSING   0
#define COMPUTE_PIPELINE_DEPTH_SORTING   1

	extern VkPipeline vk_ahComputePipelines[COMPUTE_PIPELINE_COUNT];

#define vk_hComputePipelinePreprocessing vk_ahComputePipelines[COMPUTE_PIPELINE_PREPROCESSING]
#define vk_hComputePipelineDepthSorting  vk_ahComputePipelines[COMPUTE_PIPELINE_DEPTH_SORTING]

	bool create_renderer_pipelines();
	void destroy_renderer_pipelines();

#define recreate_graphics_pipelines create_renderer_pipelines

}

#endif /* __RE_RENDERER_PIPELINES_H__ */
