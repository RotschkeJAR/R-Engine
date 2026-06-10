#include "RE_Renderer_Pipelines_Graphics_Internal.hpp"

namespace RE {

#define VERTEX_SHADER_PATH "shaders/vertex_general.glsl.spv"
#define FRAGMENT_SHADER_PATH "shaders/fragment_general.glsl.spv"
	
	bool create_graphics_pipelines() {
		PRINT_DEBUG("Creating temporary Vulkan shader modules for vertex- and fragment-stage");
		Vulkan_Shader generalVertexShader(VERTEX_SHADER_PATH),
			generalFragmentShader(FRAGMENT_SHADER_PATH);
		if (generalVertexShader.valid() && generalFragmentShader.valid()) {
			if (create_graphics_pipeline_2D(generalVertexShader(), generalFragmentShader())) {
				if (create_graphics_pipeline_2D_opaque_only(generalVertexShader(), generalFragmentShader())) {
					if (create_graphics_pipeline_3D(generalVertexShader(), generalFragmentShader())) {
						if (create_graphics_pipeline_3D_opaque_only(generalVertexShader(), generalFragmentShader())) {
							if (create_graphics_pipeline_window_frame()) {
								if (create_graphics_pipeline_text())
									return true;
								destroy_graphics_pipeline_window_frame();
							}
							destroy_graphics_pipeline_3D_opaque_only();
						}
						destroy_graphics_pipeline_3D();
					}
					destroy_graphics_pipeline_2D_opaque_only();
				}
				destroy_graphics_pipeline_2D();
			}
		} else
			RE_FATAL_ERROR("Failed to create temporary Vulkan shader modules for vertex- and fragment-stage");
		return false;
	}

	void destroy_graphics_pipelines() {
		destroy_graphics_pipeline_text();
		destroy_graphics_pipeline_window_frame();
		destroy_graphics_pipeline_3D_opaque_only();
		destroy_graphics_pipeline_3D();
		destroy_graphics_pipeline_2D_opaque_only();
		destroy_graphics_pipeline_2D();
	}

	bool recreate_graphics_pipelines() {
		PRINT_DEBUG("Creating temporary Vulkan shader module for vertices for recreating graphics pipelines");
		VkShaderModule vk_hVertexShader;
		if (create_vulkan_shader_from_file(VERTEX_SHADER_PATH, 0, &vk_hVertexShader)) {
			PRINT_DEBUG("Creating temporary Vulkan shader module for fragments for recreating graphics pipelines");
			VkShaderModule vk_hFragmentShader;
			if (create_vulkan_shader_from_file(FRAGMENT_SHADER_PATH, 0, &vk_hFragmentShader)) {
				PRINT_DEBUG("Recreating Vulkan graphics pipelines");
				const bool bSuccess = create_graphics_pipeline_2D(vk_hVertexShader, vk_hFragmentShader)
						&& create_graphics_pipeline_2D_opaque_only(vk_hVertexShader, vk_hFragmentShader)
						&& create_graphics_pipeline_3D(vk_hVertexShader, vk_hFragmentShader)
						&& create_graphics_pipeline_3D_opaque_only(vk_hVertexShader, vk_hFragmentShader);
				PRINT_DEBUG("Destroying all temporary Vulkan shader modules for recreating graphics pipelines");
				vkDestroyShaderModule(vk_hDevice, vk_hFragmentShader, nullptr);
				vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr);
				return bSuccess;
			} else
				RE_FATAL_ERROR("Failed to create temporary Vulkan shader module for fragments for recreating graphics pipelines");
			PRINT_DEBUG("Destroying temporary Vulkan shader module for vertices for recreating graphics pipelines due to failure creating all shader modules");
			vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create temporary Vulkan shader module for vertices for recreating graphics pipelines");
		return false;
	}

}
