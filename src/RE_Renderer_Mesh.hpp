#ifndef __RE_RENDERER_MESH_H__
#define __RE_RENDERER_MESH_H__ 1

#include "RE_Renderer_Internal.hpp"

namespace RE {

	enum MeshType : uint8_t {
		MESH_TYPE_2D = 1,
		MESH_TYPE_3D = 2
	};

	struct VulkanMesh final {
		VulkanMemory memory;
		VkBuffer vk_hMeshBuffer;
		VkDeviceSize vk_indicesOffset;
		uint32_t u32VertexCount,
			u32IndexCount;
		VkIndexType vk_eIndexType;
		MeshType eMeshType;
	};

	extern VulkanMesh square2D,
		square3D;
	
	bool init_renderer_meshes();
	void destroy_renderer_meshes();

}

#endif /* __RE_RENDERER_MESH_H__ */
