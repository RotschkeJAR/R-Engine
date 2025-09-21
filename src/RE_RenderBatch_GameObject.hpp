#ifndef __RE_RENDER_BATCH_GAME_OBJECT_H__
#define __RE_RENDER_BATCH_GAME_OBJECT_H__

#include "RE_Renderer.hpp"

namespace RE {

	class ListBatch_GameObject;
	
	class RenderBatch_GameObject {
		private:
			ListBatch_GameObject &rGameObjectBatch;

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
