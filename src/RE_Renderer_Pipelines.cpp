#include "RE_Renderer_Pipelines_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"

namespace RE {

	VkPipelineLayout vk_hGraphicsPipelineLayout;
	VkPipelineLayout vk_hComputePipelineLayoutProcessing;
	
	bool create_graphics_pipelines() {
		const VkDescriptorSetLayout vk_ahDescSetLayouts[] = {
			vk_hRenderContentDescSetLayout,
			vk_hTextureDescSetLayout
		};
		const VkPipelineLayoutCreateInfo vk_graphicsPipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = sizeof(vk_ahDescSetLayouts) / sizeof(vk_ahDescSetLayouts[0]),
			.pSetLayouts = vk_ahDescSetLayouts,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};
		if (vkCreatePipelineLayout(vk_hDevice, &vk_graphicsPipelineLayoutCreateInfo, nullptr, &vk_hGraphicsPipelineLayout) == VK_SUCCESS) {
			VkShaderModule vk_hVertexShader;
			if (create_vulkan_shader_from_file("shaders/gameobject_vertex.glsl.spv", 0, &vk_hVertexShader)) {
				VkShaderModule vk_hFragmentShader;
				if (create_vulkan_shader_from_file("shaders/gameobject_fragment.glsl.spv", 0, &vk_hFragmentShader)) {
					if (create_graphics_pipeline_2D(vk_hVertexShader, vk_hFragmentShader)) {
						if (create_graphics_pipeline_2D_opaque_only(vk_hVertexShader, vk_hFragmentShader)) {
							if (create_graphics_pipeline_3D(vk_hVertexShader, vk_hFragmentShader)) {
								if (create_graphics_pipeline_3D_opaque_only(vk_hVertexShader, vk_hFragmentShader)) {
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
					vkDestroyShaderModule(vk_hDevice, vk_hFragmentShader, nullptr);
				}
				vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr);
			}
			vkDestroyPipelineLayout(vk_hDevice, vk_hGraphicsPipelineLayout, nullptr);
		}
		return false;
	}

	void destroy_graphics_pipelines() {
		destroy_graphics_pipeline_2D();
		destroy_graphics_pipeline_2D_opaque_only();
		destroy_graphics_pipeline_3D();
		destroy_graphics_pipeline_3D_opaque_only();
		vkDestroyPipelineLayout(vk_hDevice, vk_hGraphicsPipelineLayout, nullptr);
	}

	bool recreate_graphics_pipelines() {
		VkShaderModule vk_hVertexShader;
		if (create_vulkan_shader_from_file("shaders/gameobject_vertex.glsl.spv", 0, &vk_hVertexShader)) {
			VkShaderModule vk_hFragmentShader;
			if (create_vulkan_shader_from_file("shaders/gameobject_fragment.glsl.spv", 0, &vk_hFragmentShader)) {
				const bool bSuccess = create_graphics_pipeline_2D(vk_hVertexShader, vk_hFragmentShader) 
						&& create_graphics_pipeline_2D_opaque_only(vk_hVertexShader, vk_hFragmentShader) 
						&& create_graphics_pipeline_3D(vk_hVertexShader, vk_hFragmentShader) 
						&& create_graphics_pipeline_3D_opaque_only(vk_hVertexShader, vk_hFragmentShader);
				vkDestroyShaderModule(vk_hDevice, vk_hFragmentShader, nullptr);
				vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr);
				return bSuccess;
			}
			vkDestroyShaderModule(vk_hDevice, vk_hVertexShader, nullptr);
		}
		return false;
	}

	bool create_compute_pipelines() {
		const VkDescriptorSetLayout vk_ahDescSetLayouts[] = {
			vk_hRawGameObjectBufferDescSetLayout,
			vk_hRenderContentDescSetLayout
		};
		const VkPipelineLayoutCreateInfo vk_computePipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = sizeof(vk_ahDescSetLayouts) / sizeof(vk_ahDescSetLayouts[0]),
			.pSetLayouts = vk_ahDescSetLayouts,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};
		if (vkCreatePipelineLayout(vk_hDevice, &vk_computePipelineLayoutCreateInfo, nullptr, &vk_hComputePipelineLayoutProcessing) == VK_SUCCESS) {
			if (create_compute_pipeline_preprocessing()) {
				if (create_compute_pipeline_depth_sorting())
					return true;
				destroy_compute_pipeline_preprocessing();
			}
			vkDestroyPipelineLayout(vk_hDevice, vk_hComputePipelineLayoutProcessing, nullptr);
		}
		return false;
	}

	void destroy_compute_pipelines() {
		destroy_compute_pipeline_preprocessing();
		destroy_compute_pipeline_depth_sorting();
		vkDestroyPipelineLayout(vk_hDevice, vk_hComputePipelineLayoutProcessing, nullptr);
	}

}
