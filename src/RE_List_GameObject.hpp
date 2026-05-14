#ifndef __RE_LIST_GAME_OBJECT_H__
#define __RE_LIST_GAME_OBJECT_H__

#include "RE_Internal Header.hpp"

namespace RE {

#define GAME_OBJECT_BATCH_SIZE 1024

	extern std::list<std::array<GameObject*, GAME_OBJECT_BATCH_SIZE>> newGameObjects,
		deletableGameObjects,
		gameObjects;
	extern uint32_t u32MaxGameObjectCount,
		u32CurrentGameObjectCount,
		u32DeletableGameObjectCount,
		u32NewGameObjectCount;

	void delete_and_add_game_objects();
	void add_game_object(GameObject &rGameObject);

	void start_game_objects();
	void update_game_objects();
	void end_game_objects();

}

#endif /* __RE_LIST_GAME_OBJECT_H__ */
