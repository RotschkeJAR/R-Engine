#ifndef __RE_MANAGER_H__
#define __RE_MANAGER_H__

#include "RE_Internal Header.hpp"

namespace RE {

	enum CallingPhase {
		CALLING_PHASE_NONE = 0,
		CALLING_PHASE_SCENE_START,
		CALLING_PHASE_SCENE_UPDATE,
		CALLING_PHASE_SCENE_END,
		CALLING_PHASE_GAME_OBJECT_START,
		CALLING_PHASE_GAME_OBJECT_UPDATE,
		CALLING_PHASE_GAME_OBJECT_END,
		CALLING_PHASE_CAMERA_START,
		CALLING_PHASE_CAMERA_UPDATE,
		CALLING_PHASE_CAMERA_END
	};

	extern Scene *pCurrentScene,
		*pNextScene;
	extern CallingPhase eCallingPhase;

	bool is_object_active(const GameObject* pGameObject);
	void game_logic_update();
	void last_game_logic_update();
	bool are_scenes_present();

}

#endif /* __RE_MANAGER_H__ */
