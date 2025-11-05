#ifndef __RE_MESH_H__
#define __RE_MESH_H__

#include "RE_Internal Header.hpp"

namespace RE {
	
	struct VulkanMesh final {
		VkBuffer vk_hMeshBuffer;
		VkDeviceMemory vk_hMeshBufferMemory;
		VkDeviceSize vk_indicesByteOffset;
	};

}

#endif /* __RE_MESH_H__ */
