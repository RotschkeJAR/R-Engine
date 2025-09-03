#ifndef __RE_SUB_RENDERER_H__
#define __RE_SUB_RENDERER_H__

#include "RE_Renderer.hpp"

namespace RE {

#define RE_VK_GAME_OBJECT_VERTEX_POSITION_SIZE 3
#define RE_VK_GAME_OBJECT_VERTEX_COLOR_SIZE 4
#define RE_VK_GAME_OBJECT_VERTEX_TEXTURE_ID_SIZE 1
#define RE_VK_GAME_OBJECT_VERTEX_TEXTURE_COORDS_SIZE 2

	struct GameObject_VertexData final {
		float afPosition[RE_VK_GAME_OBJECT_VERTEX_POSITION_SIZE];
		float afColor[RE_VK_GAME_OBJECT_VERTEX_COLOR_SIZE];
		int32_t i32TexId;
		float afTexCoords[RE_VK_GAME_OBJECT_VERTEX_TEXTURE_COORDS_SIZE];
	};

#define RE_VK_GAME_OBJECT_VERTEX_COUNT (RE_VK_RENDERABLE_RECTANGLES_COUNT * 4U)
#define RE_VK_GAME_OBJECT_VERTEX_POSITION_SIZE_BYTES sizeof(GameObject_VertexData::afPosition)
#define RE_VK_GAME_OBJECT_VERTEX_POSITION_OFFSET_BYTES offsetof(GameObject_VertexData, afPosition)
#define RE_VK_GAME_OBJECT_VERTEX_COLOR_SIZE_BYTES sizeof(GameObject_VertexData::afColor)
#define RE_VK_GAME_OBJECT_VERTEX_COLOR_OFFSET_BYTES offsetof(GameObject_VertexData, afColor)
#define RE_VK_GAME_OBJECT_VERTEX_TEXTURE_ID_SIZE_BYTES sizeof(GameObject_VertexData::i32TexId)
#define RE_VK_GAME_OBJECT_VERTEX_TEXTURE_ID_OFFSET_BYTES offsetof(GameObject_VertexData, i32TexId)
#define RE_VK_GAME_OBJECT_VERTEX_TEXTURE_COORDS_SIZE_BYTES sizeof(GameObject_VertexData::afTexCoords)
#define RE_VK_GAME_OBJECT_VERTEX_TEXTURE_COORDS_OFFSET_BYTES offsetof(GameObject_VertexData, afTexCoords)
#define RE_VK_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES sizeof(GameObject_VertexData)
#define RE_VK_GAME_OBJECT_VERTEX_BUFFER_SIZE_BYTES (RE_VK_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES * 4U * RE_VK_RENDERABLE_RECTANGLES_COUNT)

	extern VkCommandBuffer vk_ahGameObjectVertexTransferCommandBuffers[RE_VK_FRAMES_IN_FLIGHT], vk_ahGameObjectSecondaryCommandBuffers[RE_VK_FRAMES_IN_FLIGHT];
	
	bool init_game_object_renderer();
	void destroy_game_object_renderer();
	bool load_game_object_vertices_and_transfer(bool &rbNeedsRender);
	bool render_game_objects();

	bool recreate_game_object_render_pipelines();

}

#endif /* __RE_SUB_RENDERER_H__ */
