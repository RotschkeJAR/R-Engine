#include "RE_Renderer.hpp"
#include "RE_Manager.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

#define RE_VK_VERTEX_COUNT (RE_VK_RENDERABLE_RECTANGLES_COUNT * 4U)
#define RE_VK_VERTEX_POSITION_SIZE 3U
#define RE_VK_VERTEX_POSITION_SIZE_BYTES (RE_VK_VERTEX_POSITION_SIZE * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_POSITION_OFFSET 0U
#define RE_VK_VERTEX_POSITION_OFFSET_BYTES (RE_VK_VERTEX_POSITION_OFFSET * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_COLOR_SIZE 4U
#define RE_VK_VERTEX_COLOR_SIZE_BYTES (RE_VK_VERTEX_COLOR_SIZE * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_COLOR_OFFSET RE_VK_VERTEX_POSITION_SIZE
#define RE_VK_VERTEX_COLOR_OFFSET_BYTES (RE_VK_VERTEX_COLOR_OFFSET * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_TOTAL_SIZE (RE_VK_VERTEX_POSITION_SIZE + RE_VK_VERTEX_COLOR_SIZE)
#define RE_VK_VERTEX_TOTAL_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_BUFFER_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE_BYTES * 4U * RE_VK_RENDERABLE_RECTANGLES_COUNT)

#define RE_VK_VIEW_MATRIX_SIZE (4U * 4U)
#define RE_VK_VIEW_MATRIX_OFFSET 0U
#define RE_VK_PROJECTION_MATRIX_SIZE (4U * 4U)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE

	VkShaderModule vk_hGameObjectVertexShader = VK_NULL_HANDLE, vk_hGameObjectFragmentShader = VK_NULL_HANDLE;

	bool init_gameobject_renderer() {
		if (CATCH_SIGNAL_AND_RETURN(create_vulkan_shader_from_file("shaders/vertex.spv", &vk_hGameObjectVertexShader), bool)) {
			if (CATCH_SIGNAL_AND_RETURN(create_vulkan_shader_from_file("shaders/fragment.spv", &vk_hGameObjectFragmentShader), bool))
				return true;
			vkDestroyShaderModule(vk_hDevice, vk_hGameObjectVertexShader, nullptr);
		}
		return false;
	}

	void destroy_gameobject_renderer() {
		vkDestroyShaderModule(vk_hDevice, vk_hGameObjectVertexShader, nullptr);
		vkDestroyShaderModule(vk_hDevice, vk_hGameObjectFragmentShader, nullptr);
	}

}
