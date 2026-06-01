#include "RE_Renderer_Pipelines_Graphics_Internal.hpp"

namespace RE {

#define VERTEX_SHADER_PATH "shaders/vertex_general.glsl.spv"
#define FRAGMENT_SHADER_PATH "shaders/fragment_general.glsl.spv"
	
	bool create_graphics_pipelines() {
		PRINT_DEBUG("Creating temporary Vulkan shader module for vertices");
		VkShaderModule vk_hVertexShader;
		if (create_vulkan_shader_from_file(VERTEX_SHADER_PATH, 0, &vk_hVertexShader)) {
			PRINT_DEBUG("Creating temporary Vulkan shader module for fragments");
			VkShaderModule vk_hFragmentShader;
			if (create_vulkan_shader_from_file(FRAGMENT_SHADER_PATH, 0, &vk_hFragmentShader)) {
				if (create_graphics_pipeline_2D(vk_hVertexShader, vk_hFragmentShader)) {
					if (create_graphics_pipeline_2D_opaque_only(vk_hVertexShader, vk_hFragmentShader)) {
						if (create_graphics_pipeline_3D(vk_hVertexShader, vk_hFragmentShader)) {
							if (create_graphics_pipeline_3D_opaque_only(vk_hVertexShader, vk_hFragmentShader)) {
								PRINT_DEBUG("Destroying temporary Vulkan shader modules");
								vkDestroyShaderModule(vk_hDevice, vk_hFragmentShader, nullptr);
								vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr);
								return true;
							}
							destroy_graphics_pipeline_3D();
						}
						destroy_graphics_pipeline_2D_opaque_only();
					}
					destroy_graphics_pipeline_2D();
				}
				PRINT_DEBUG("Destroying temporary Vulkan shader module for fragments");
				vkDestroyShaderModule(vk_hDevice, vk_hFragmentShader, nullptr);
			} else
				RE_FATAL_ERROR("Failed to create temporary Vulkan shader module for fragments");
			PRINT_DEBUG("Destroying temporary Vulkan shader module for vertices");
			vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create temporary Vulkan shader module for vertices");
		return false;
	}

	void destroy_graphics_pipelines() {
		PRINT_DEBUG("Destroying all Vulkan graphics pipelines");
		destroy_graphics_pipeline_3D_opaque_only();
		destroy_graphics_pipeline_3D();
		destroy_graphics_pipeline_2D_opaque_only();
		destroy_graphics_pipeline_2D();
	}

	bool recreate_graphics_pipelines() {
		PRINT_DEBUG("Creating temporary Vulkan shader module for vertices for recreating all graphics pipelines");
		VkShaderModule vk_hVertexShader;
		if (create_vulkan_shader_from_file(VERTEX_SHADER_PATH, 0, &vk_hVertexShader)) {
			PRINT_DEBUG("Creating temporary Vulkan shader module for fragments for recreating all graphics pipelines");
			VkShaderModule vk_hFragmentShader;
			if (create_vulkan_shader_from_file(FRAGMENT_SHADER_PATH, 0, &vk_hFragmentShader)) {
				PRINT_DEBUG("Recreating all Vulkan graphics pipelines");
				const bool bSuccess = create_graphics_pipeline_2D(vk_hVertexShader, vk_hFragmentShader)
						&& create_graphics_pipeline_2D_opaque_only(vk_hVertexShader, vk_hFragmentShader)
						&& create_graphics_pipeline_3D(vk_hVertexShader, vk_hFragmentShader)
						&& create_graphics_pipeline_3D_opaque_only(vk_hVertexShader, vk_hFragmentShader);
				PRINT_DEBUG("Destroying all temporary Vulkan shader modules for recreating all graphics pipelines");
				vkDestroyShaderModule(vk_hDevice, vk_hFragmentShader, nullptr);
				vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr);
				return bSuccess;
			} else
				RE_FATAL_ERROR("Failed to create temporary Vulkan shader module for fragments for recreating all graphics pipelines");
			PRINT_DEBUG("Destroying temporary Vulkan shader module for vertices for recreating all graphics pipelines due to failure creating all shader modules");
			vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create temporary Vulkan shader module for vertices for recreating all graphics pipelines");
		return false;
	}

}
