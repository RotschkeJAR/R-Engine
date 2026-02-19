#ifndef __RE_RENDERER_PIPELINES_H__
#define __RE_RENDERER_PIPELINES_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern VkPipelineLayout vk_hGraphicsPipelineLayout;
	extern VkPipelineLayout vk_hComputePipelineLayoutProcessing;
	extern VkPipeline vk_hGraphicsPipeline2D;
	extern VkPipeline vk_hGraphicsPipeline2DOpaqueOnly;
	extern VkPipeline vk_hGraphicsPipeline3D;
	extern VkPipeline vk_hGraphicsPipeline3DOpaqueOnly;
	extern VkPipeline vk_hComputePipelinePreprocessing;

	enum RendererPipeline {
		PIPELINE_GRAPHICS_2D,
		PIPELINE_GRAPHICS_2D_OPAQUE_ONLY,
		PIPELINE_GRAPHICS_3D,
		PIPELINE_GRAPHICS_3D_OPAQUE_ONLY
	};

	bool create_graphics_pipelines();
	void destroy_graphics_pipelines();
	bool recreate_graphics_pipelines();

	bool create_compute_pipelines();
	void destroy_compute_pipelines();

}

#endif /* __RE_RENDERER_PIPELINES_H__ */
