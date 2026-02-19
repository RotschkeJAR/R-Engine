#ifndef __RE_RENDERER_PIPELINES_INTERNAL_H__
#define __RE_RENDERER_PIPELINES_INTERNAL_H__

#include "RE_Renderer_Pipelines.hpp"

namespace RE {

	bool create_graphics_pipeline_2D(VkShaderModule vk_hVertexShader, VkShaderModule vk_hFragmentShader);
	void destroy_graphics_pipeline_2D();
	
	bool create_graphics_pipeline_2D_opaque_only(VkShaderModule vk_hVertexShader, VkShaderModule vk_hFragmentShader);
	void destroy_graphics_pipeline_2D_opaque_only();

	bool create_graphics_pipeline_3D(VkShaderModule vk_hVertexShader, VkShaderModule vk_hFragmentShader);
	void destroy_graphics_pipeline_3D();

	bool create_graphics_pipeline_3D_opaque_only(VkShaderModule vk_hVertexShader, VkShaderModule vk_hFragmentShader);
	void destroy_graphics_pipeline_3D_opaque_only();

	bool create_compute_pipeline_preprocessing();
	void destroy_compute_pipeline_preprocessing();

	bool create_compute_pipeline_depth_sorting();
	void destroy_compute_pipeline_depth_sorting();

}

#endif /* __RE_RENDERER_PIPELINES_INTERNAL_H__ */
