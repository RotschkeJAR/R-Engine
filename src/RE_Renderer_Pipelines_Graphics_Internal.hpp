#ifndef __RE_RENDERER_PIPELINES_GRAPHICS_INTERNAL_H__
#define __RE_RENDERER_PIPELINES_GRAPHICS_INTERNAL_H__ 1

#include "RE_Renderer_Pipelines_Graphics.hpp"

namespace RE {
	
	void setup_graphics_pipeline_2D(
			VkPipelineInputAssemblyStateCreateInfo &vk_rInputAssembly,
			VkPipelineRasterizationStateCreateInfo &vk_rRasterization,
			VkPipelineColorBlendAttachmentState &vk_rColorBlendAttachment,
			VkPipelineColorBlendStateCreateInfo &vk_rColorBlend);
	
	void setup_graphics_pipeline_2D_opaque_only(
			VkPipelineInputAssemblyStateCreateInfo &vk_rInputAssembly,
			VkPipelineRasterizationStateCreateInfo &vk_rRasterization,
			VkPipelineColorBlendAttachmentState &vk_rColorBlendAttachment,
			VkPipelineColorBlendStateCreateInfo &vk_rColorBlend);

	void setup_graphics_pipeline_3D(
			VkPipelineInputAssemblyStateCreateInfo &vk_rInputAssembly,
			VkPipelineRasterizationStateCreateInfo &vk_rRasterization,
			VkPipelineColorBlendAttachmentState &vk_rColorBlendAttachment,
			VkPipelineColorBlendStateCreateInfo &vk_rColorBlend);

	void setup_graphics_pipeline_3D_opaque_only(
			VkPipelineInputAssemblyStateCreateInfo &vk_rInputAssembly,
			VkPipelineRasterizationStateCreateInfo &vk_rRasterization,
			VkPipelineColorBlendAttachmentState &vk_rColorBlendAttachment,
			VkPipelineColorBlendStateCreateInfo &vk_rColorBlend);
	
}

#endif /* __RE_RENDERER_PIPELINES_GRAPHICS_INTERNAL_H__ */
