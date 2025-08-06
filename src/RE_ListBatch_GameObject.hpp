#ifndef __RE_BATCH_GAME_OBJECT_H__
#define __RE_BATCH_GAME_OBJECT_H__

#include "RE_Renderer.hpp"
#include "RE_RenderBatch_GameObject.hpp"

namespace RE {
	
	class ListBatch_GameObject final {
		private:
			std::array<GameObject*, RE_VK_RENDERABLE_RECTANGLES_COUNT> apGameObjects;
			RenderBatch_GameObject renderBatch;
			uint16_t u16Count;

		public:
			ListBatch_GameObject();
			~ListBatch_GameObject();

			void add(GameObject *pGameObject);
			bool remove(const GameObject *pGameObject);
			bool contains(const GameObject *pGameObject) const;
			GameObject* get(uint16_t u16Index);

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
		friend void RenderBatch_GameObject::render_opaque();
	};

}

#endif /* __RE_BATCH_GAME_OBJECT_H__ */
