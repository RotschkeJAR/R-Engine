#ifndef __RE_RENDER_BATCH_GAME_OBJECT_H__
#define __RE_RENDER_BATCH_GAME_OBJECT_H__

#include "RE_Renderer.hpp"

namespace RE {

	class Batch_GameObject;
	
	class RenderBatch_GameObject {
		private:
			Batch_GameObject &rGameObjectBatch;
			VkBuffer vk_hStagingVertexBuffer, vk_ahVertexBuffers[RE_VK_FRAMES_IN_FLIGHT];
			VkDeviceMemory vk_hStagingVertexBufferMemory, vk_ahVertexBufferMemories[RE_VK_FRAMES_IN_FLIGHT];
			float *pafVertices;
			VkDeviceSize vk_transparentVerticesOffsetBytes;
			uint16_t u16TransparentCount;

		public:
			RenderBatch_GameObject() = delete;
			RenderBatch_GameObject(Batch_GameObject &rGameObjectBatch);
			~RenderBatch_GameObject();
			bool init();
			void destroy();

			void render_opaque();
			void render_transparent();
	};

}

#endif /* __RE_RENDER_BATCH_GAME_OBJECT_H__ */
