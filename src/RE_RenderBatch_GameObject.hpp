#ifndef __RE_RENDER_BATCH_GAME_OBJECT_H__
#define __RE_RENDER_BATCH_GAME_OBJECT_H__

#include "RE_Renderer.hpp"

namespace RE {

	struct GameObjectUniformData {
		float a16fModelMatrix[16];
		uint32_t u32TextureId;
	};

	class ListBatch_GameObject;
	
	class RenderBatch_GameObject {
		private:
			ListBatch_GameObject &rGameObjectBatch;
			VkBuffer vk_hStagingUniformBuffer;
			VkDeviceMemory vk_hStagingUniformBufferMemory;
			GameObjectUniformData *pStagingUniformBufferData;

		public:
			RenderBatch_GameObject() = delete;
			RenderBatch_GameObject(ListBatch_GameObject &rGameObjectBatch);
			~RenderBatch_GameObject();
			bool init();
			void destroy();

			void fetch_render_data();
	};

}

#endif /* __RE_RENDER_BATCH_GAME_OBJECT_H__ */
