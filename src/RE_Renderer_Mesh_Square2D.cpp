#include "RE_Renderer_Mesh_Internal.hpp"

namespace RE {

	VulkanMesh square2D;
	
	bool create_renderer_mesh_square_2D() {
		PRINT_DEBUG("Creating mesh buffer for 2D square in Vulkan");
		if (create_vulkan_buffer(
				0,
				sizeof(VertexData) * 4,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				1,
				nullptr,
				&square2D.vk_hMeshBuffer)) {
			square2D.u32VertexCount = 4;
			square2D.u32IndexCount = 0;
			square2D.vk_eIndexType = VK_INDEX_TYPE_NONE_KHR;
			square2D.eMeshType = MESH_TYPE_2D;
			return true;
		} else
			RE_FATAL_ERROR("Failed to create a mesh buffer for the 2D square in Vulkan");
		return false;
	}

	void destroy_renderer_mesh_square_2D() {
		PRINT_DEBUG("Destroying mesh buffer used for 2D square in Vulkan");
		vkDestroyBuffer(vk_hDevice, square2D.vk_hMeshBuffer, nullptr);
	}

	void record_cmd_populate_mesh_square_2D(VkCommandBuffer vk_hCommandBuffer) {
		PRINT_DEBUG("Updating the 2D square-mesh buffer with vertices");
		constexpr VertexData aSquare2DVertices[] = {
			{
				.coords = {-0.5f, 0.5f, 0.0f, 1.0f},
				.texCoords = {0.0f, 0.0f}
			}, {
				.coords = {0.5f, 0.5f, 0.0f, 1.0f},
				.texCoords = {1.0f, 0.0f}
			}, {
				.coords = {-0.5f, -0.5f, 0.0f, 1.0f},
				.texCoords = {0.0f, 1.0f}
			}, {
				.coords = {0.5f, -0.5f, 0.0f, 1.0f},
				.texCoords = {1.0f, 1.0f}
			}
		};
		vkCmdUpdateBuffer(vk_hCommandBuffer, square2D.vk_hMeshBuffer, 0, sizeof(aSquare2DVertices), aSquare2DVertices);
	}

	Mesh get_mesh_square_2D() {
		return reinterpret_cast<Mesh>(&square2D);
	}

}
