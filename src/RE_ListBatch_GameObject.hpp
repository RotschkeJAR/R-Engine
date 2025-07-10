#ifndef __RE_BATCH_GAME_OBJECT_H__
#define __RE_BATCH_GAME_OBJECT_H__

#include "RE_Renderer.hpp"
#include "RE_RenderBatch_GameObject.hpp"

namespace RE {
	
	class ListBatch_GameObject final {
		private:
			GameObject *apGameObjects[RE_VK_RENDERABLE_RECTANGLES_COUNT];
			RenderBatch_GameObject renderBatch;
			uint16_t u16Count;

		public:
			ListBatch_GameObject();
			~ListBatch_GameObject();

			void add(GameObject *const pGameObject);
			bool remove(const GameObject *const pGameObject);
			bool contains(const GameObject *const pGameObject) const;
			GameObject* get(const uint16_t u16Index);

			uint16_t size() const;
			bool empty() const;
			bool has_space() const;

			void start();
			void update();
			void end();

		friend bool init_game_object_render_batches();
		friend void destroy_game_object_render_batches();
		friend void render_opaque_game_objects();
		friend void render_transparent_game_objects();
	};

}

#endif /* __RE_BATCH_GAME_OBJECT_H__ */
