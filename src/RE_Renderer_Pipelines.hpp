#ifndef __RE_RENDERER_PIPELINES_H__
#define __RE_RENDERER_PIPELINES_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

#ifdef RE_OS_LINUX
# define WINDOW_FRAME_RENDER_MODE_SHADOWS 0
# define WINDOW_FRAME_RENDER_MODE_BAR 1
# define WINDOW_FRAME_RENDER_MODE_BUTTONS 2
# define WINDOW_FRAME_RENDER_MODE_TITLE 3
#endif

	extern VkPipelineLayout vk_hGraphicsPipelineLayout,
		vk_hSortDepthPipelineLayout,
		vk_hProcessingPipelineLayout,
		vk_hTextPipelineLayout;
#ifdef RE_OS_LINUX
	extern VkPipelineLayout vk_hWindowFramePipelineLayout;
#endif /* RE_OS_LINUX */

	extern VkPipeline vk_hGraphicsPipeline2D,
		vk_hGraphicsPipeline2DOpaqueOnly,
		vk_hGraphicsPipeline3D,
		vk_hGraphicsPipeline3DOpaqueOnly,
		vk_hGraphicsPipelineText,
		vk_hComputePipelinePreprocessing,
		vk_hComputePipelineDepthSorting;
#ifdef RE_OS_LINUX
	extern VkPipeline vk_hGraphicsPipelineWindowFrame;
#endif /* RE_OS_LINUX */

	bool create_renderer_pipelines();
	void destroy_renderer_pipelines();

	bool recreate_graphics_pipelines();

}

#endif /* __RE_RENDERER_PIPELINES_H__ */
