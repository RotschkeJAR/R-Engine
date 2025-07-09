#ifndef __RE_MANAGER_H__
#define __RE_MANAGER_H__

#include "RE_Internal Header.hpp"

namespace RE {

	extern Scene *pCurrentScene, *pNextScene;
	extern std::vector<GameObject*> newGameObjects, deletableGameObjects;

	bool is_object_active(const GameObject* pGameObject);
	void game_logic_update();
	void last_game_logic_update();
	bool is_game_valid();

	void mark_game_object_deletable(GameObject *const pGameObject);

}

#endif /* __RE_MANAGER_H__ */
