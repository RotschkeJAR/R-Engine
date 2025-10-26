#ifndef __RE_BATCH_GAME_OBJECT_H__
#define __RE_BATCH_GAME_OBJECT_H__

#include "RE_Renderer.hpp"
#include "RE_RenderBatch_GameObject.hpp"

namespace RE {

#define MAXIMUM_GAME_OBJECTS_PER_BATCH 1000
	
	class ListBatch_GameObject final {
		private:
			std::array<GameObject*, MAXIMUM_GAME_OBJECTS_PER_BATCH> apGameObjects;
			RenderBatch_GameObject renderBatch;
			uint16_t u16Count;

		public:
			ListBatch_GameObject();
			~ListBatch_GameObject();

			void add(GameObject *pGameObject);
			bool remove(const GameObject *pGameObject);
			bool contains(const GameObject *pGameObject) const;
			GameObject* at(uint16_t u16Index) const;

			uint16_t size() const;
			bool empty() const;
			bool has_space() const;

			void start();
			void update();
			void end();

			GameObject* operator[](uint16_t u16Index) const;

		friend bool init_game_object_render_batches();
		friend void destroy_game_object_render_batches();
		friend void load_game_object_vertices(bool &rbNeedsRender);
		friend void render_opaque_game_objects();
		friend void render_transparent_game_objects();
		friend void RenderBatch_GameObject::render_opaque();
	};

}

#endif /* __RE_BATCH_GAME_OBJECT_H__ */
