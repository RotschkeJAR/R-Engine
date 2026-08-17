#ifndef __RE_RENDERER_MESH_INTERNAL_H__
#define __RE_RENDERER_MESH_INTERNAL_H__ 1

#include "RE_Renderer_Mesh.hpp"

namespace RE {

#define PERMANENT_MESH_COUNT    2
	
	bool create_renderer_mesh_square_2D();
	void destroy_renderer_mesh_square_2D();
	void record_cmd_populate_mesh_square_2D(VkCommandBuffer vk_hCommandBuffer);

	bool create_renderer_mesh_square_3D();
	void destroy_renderer_mesh_square_3D();
	void record_cmd_populate_mesh_square_3D(VkCommandBuffer vk_hCommandBuffer);

	bool alloc_memory_for_permanent_meshes();
	void free_memory_for_permanent_meshes();

}

#endif /* __RE_RENDERER_MESH_INTERNAL_H__ */
