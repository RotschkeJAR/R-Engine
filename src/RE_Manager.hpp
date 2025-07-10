#ifndef __RE_MANAGER_H__
#define __RE_MANAGER_H__

#include "RE_Internal Header.hpp"

namespace RE {

	extern Scene *pCurrentScene, *pNextScene;

	bool is_object_active(const GameObject* pGameObject);
	void game_logic_update();
	void last_game_logic_update();
	bool are_scenes_present();

}

#endif /* __RE_MANAGER_H__ */
