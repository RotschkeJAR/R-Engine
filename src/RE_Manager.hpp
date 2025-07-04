#ifndef __RE_MANAGER_H__
#define __RE_MANAGER_H__

#include "RE_Internal Header.hpp"

namespace RE {

	extern Scene *pCurrentScene, *pNextScene;
	extern std::vector<GameObject*> gameObjects, deletableGameObjects, newGameObjects;
	extern bool bDeletingMarkedGameObjects;

	bool is_object_active(const GameObject* pGameObject);
	void game_logic_update();
	void last_game_logic_update();
	bool is_game_valid();

	void mark_deletable(GameObject* pGameObject);

	void set_next_scene(Scene* pNextScene);
	bool is_next_scene_set();
	Scene* get_current_scene();
	uint32_t get_current_scene_id();
	bool is_scene_current(const uint32_t u32SceneId);
	Scene* get_next_scene();
	uint32_t get_next_scene_id();
	bool is_scene_next(const uint32_t u32SceneId);

}

#endif /* __RE_MANAGER_H__ */
