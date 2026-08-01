#ifndef __RE_MANAGER_H__
#define __RE_MANAGER_H__ 1

#include "RE_Internal.hpp"

namespace RE {

	extern Scene *pCurrentScene,
		*pNextScene;

	bool is_object_active(const GameObject* pGameObject);
	bool init_manager();
	void destroy_manager();
	void game_logic_update();
	void last_game_logic_update();
	bool are_scenes_present();

	void add_camera(Camera *pNewCamera);
	void remove_camera(Camera *pRemovableCamera);

	void add_game_object(GameObject *pNewGameObject);
	void remove_game_object(GameObject *pRemovableGameObject);

}

#endif /* __RE_MANAGER_H__ */
