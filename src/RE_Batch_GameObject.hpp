#ifndef __RE_BATCH_GAME_OBJECT_H__
#define __RE_BATCH_GAME_OBJECT_H__

#include "RE_Renderer.hpp"
#include "RE_RenderBatch_GameObject.hpp"

namespace RE {

	class RenderBatch_GameObject;
	
	class Batch_GameObject {
		private:
			GameObject *apGameObjects[RE_VK_RENDERABLE_RECTANGLES_COUNT];
			RenderBatch_GameObject renderBatch;
			uint16_t u16Count;

		public:
			Batch_GameObject();
			~Batch_GameObject();

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

			void render_opaque();
			void render_transparent();

		friend bool init_game_object_render_batches();
		friend void destroy_game_object_render_batches();
	};

	void add_to_game_object_batch(GameObject *const pGameObject);
	void remove_from_game_object_batch(const GameObject *const pGameObject);

	void start_game_objects();
	void update_game_objects();
	void end_game_objects();

	bool init_game_object_render_batches();
	void destroy_game_object_render_batches();
	void render_opaque_game_objects();
	void render_transparent_game_objects();

}

#endif /* __RE_BATCH_GAME_OBJECT_H__ */
