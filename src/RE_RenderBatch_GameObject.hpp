#ifndef __RE_RENDER_BATCH_GAME_OBJECT_H__
#define __RE_RENDER_BATCH_GAME_OBJECT_H__

#include "RE_Renderer.hpp"
#include "RE_Renderer_GameObject.hpp"

namespace RE {

	class ListBatch_GameObject;
	
	class RenderBatch_GameObject {
		private:
			ListBatch_GameObject &rGameObjectBatch;
			VkBuffer vk_hStagingVertexBuffer;
			std::array<VkBuffer, RE_VK_FRAMES_IN_FLIGHT> vk_ahVertexBuffers;
			VkDeviceMemory vk_hStagingVertexBufferMemory;
			std::array<VkDeviceMemory, RE_VK_FRAMES_IN_FLIGHT> vk_ahVertexBufferMemories;
			GameObject_VertexData *paVertices;
			VkDeviceSize vk_transparentVerticesOffsetBytes;
			uint16_t u16OpaqueCount, u16TransparentCount;

		public:
			RenderBatch_GameObject() = delete;
			RenderBatch_GameObject(ListBatch_GameObject &rGameObjectBatch);
			~RenderBatch_GameObject();
			bool init();
			void destroy();

			void load_vertices(bool &rbNeedsRender);
			void render_opaque();
			void render_transparent();
	};

}

#endif /* __RE_RENDER_BATCH_GAME_OBJECT_H__ */
