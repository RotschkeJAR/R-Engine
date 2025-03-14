#ifndef __RE_MANAGER_H__
#define __RE_MANAGER_H__

#include "RE_Internal Header.hpp"

namespace RE {

	class Manager {
		private:
			bool should_update_object(GameObject* pGameObject);
			void start_proc();
			void update_proc();
			void end_proc();
			void delete_proc();
			void add_proc();

		public:
			static Scene *pCurrentScene, *pNextScene;
			static std::vector<GameObject*> gameObjects, deletableGameObjects, newGameObjects;
			static Manager *pInstance;

			Manager();
			~Manager();
			void game_logic_update();
			void last_game_logic_update();
			bool is_game_valid();
	};

	void mark_delete(GameObject* pGameObject);

	void set_next_scene(Scene* pNextScene);
	bool is_next_scene_set();
	Scene* get_current_scene();
	REuint get_current_scene_id();
	bool is_scene_current(REuint u32SceneId);
	Scene* get_next_scene();
	REuint get_next_scene_id();
	bool is_scene_next(REuint u32SceneId);

}

#endif /* __RE_MANAGER_H__ */
