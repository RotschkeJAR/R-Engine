#include "RE_Renderer_Mesh_Internal.hpp"

namespace RE {

	VulkanMesh square3D;
	
	bool create_renderer_mesh_square_3D() {
		PRINT_DEBUG("Creating mesh buffer for 3D square in Vulkan");
		const bool bIndexUint8Exists = are_vulkan_features_enabled<ENABLED_FEATURE_INDEX_UINT_8_BIT>();
		if (create_vulkan_buffer(
				0,
				sizeof(VertexData) * 4 + (bIndexUint8Exists ? (sizeof(uint8_t) * 8) : (sizeof(uint16_t) * 6)),
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				1,
				nullptr,
				&square3D.vk_hMeshBuffer)) {
			square3D.vk_indicesOffset = sizeof(VertexData) * 4;
			square3D.u32VertexCount = 4;
			square3D.u32IndexCount = 6;
			square3D.vk_eIndexType = bIndexUint8Exists ? VK_INDEX_TYPE_UINT8 : VK_INDEX_TYPE_UINT16;
			square3D.eMeshType = MESH_TYPE_3D;
			return true;
		} else
			RE_FATAL_ERROR("Failed to create a mesh buffer for the 3D square in Vulkan");
		return false;
	}

	void destroy_renderer_mesh_square_3D() {
		PRINT_DEBUG("Destroying mesh buffer used for 3D square in Vulkan");
		vkDestroyBuffer(vk_hDevice, square3D.vk_hMeshBuffer, nullptr);
	}

	void record_cmd_populate_mesh_square_3D(VkCommandBuffer vk_hCommandBuffer) {
		PRINT_DEBUG("Updating the 3D square-mesh buffer with vertices");
		constexpr VertexData aSquare3DVertices[] = {
			{
				.coords = {-0.5f, 0.5f, 0.0f, 1.0f},
				.texCoords = {0.0f, 0.0f}
			}, {
				.coords = {0.5f, 0.5f, 0.0f, 1.0f},
				.texCoords = {1.0f, 0.0f}
			}, {
				.coords = {0.5f, -0.5f, 0.0f, 1.0f},
				.texCoords = {1.0f, 1.0f}
			}, {
				.coords = {-0.5f, -0.5f, 0.0f, 1.0f},
				.texCoords = {0.0f, 1.0f}
			}
		};
		vkCmdUpdateBuffer(vk_hCommandBuffer, square3D.vk_hMeshBuffer, 0, sizeof(aSquare3DVertices), aSquare3DVertices);
		PRINT_DEBUG("Updating the 3D square-mesh buffer with indices");
		if (are_vulkan_features_enabled<ENABLED_FEATURE_INDEX_UINT_8_BIT>()) {
			constexpr uint8_t au8Square3DIndices[] = {0, 1, 2, 2, 3, 0, /* padding */ 0, 0};
			vkCmdUpdateBuffer(vk_hCommandBuffer, square3D.vk_hMeshBuffer, square3D.vk_indicesOffset, sizeof(au8Square3DIndices), au8Square3DIndices);
		} else {
			constexpr uint16_t au16Square3DIndices[] = {0, 1, 2, 2, 3, 0};
			vkCmdUpdateBuffer(vk_hCommandBuffer, square3D.vk_hMeshBuffer, square3D.vk_indicesOffset, sizeof(au16Square3DIndices), au16Square3DIndices);
		}
	}

	Mesh get_mesh_square_3D() {
		return reinterpret_cast<Mesh>(&square3D);
	}

}