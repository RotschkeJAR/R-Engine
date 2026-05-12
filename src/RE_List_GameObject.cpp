#include "RE_List_GameObject.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	std::list<GameObject*[GAME_OBJECT_BATCH_SIZE]> newGameObjects,
		deletableGameObjects,
		gameObjects;
	uint32_t u32MaxGameObjectCount = GAME_OBJECT_BATCH_SIZE,
		u32CurrentGameObjectCount = 0,
		u32DeletableGameObjectCount = 0,
		u32NewGameObjectCount = 0;
	bool bDeletingAddingGameObjects = false;

	void delete_and_add_game_objects() {
		PRINT_DEBUG("Running the game object addition procedure");
		bDeletingAddingGameObjects = true;
		while (u32DeletableGameObjectCount) {
			GameObject *const pDeletableGameObject = deletableGameObjects.back()[(u32DeletableGameObjectCount - 1) % GAME_OBJECT_BATCH_SIZE];
			if (u32NewGameObjectCount) {
				GameObject *const pReplacementGameObject = newGameObjects.back()[(u32NewGameObjectCount - 1) % GAME_OBJECT_BATCH_SIZE];
				auto newGameObjectIter = gameObjects.begin();
				std::advance(newGameObjectIter, pDeletableGameObject->u32ListIndex / GAME_OBJECT_BATCH_SIZE);
				(*newGameObjectIter)[pDeletableGameObject->u32ListIndex % GAME_OBJECT_BATCH_SIZE] = pReplacementGameObject;
				pReplacementGameObject->u32ListIndex = pDeletableGameObject->u32ListIndex;
				pReplacementGameObject->bNew = false;
				u32NewGameObjectCount--;
				if ((u32NewGameObjectCount % GAME_OBJECT_BATCH_SIZE) == 0)
					newGameObjects.pop_back();
			}
			PRINT_DEBUG("Deleting game object ", pDeletableGameObject);
			delete pDeletableGameObject;
			u32DeletableGameObjectCount--;
			if ((u32DeletableGameObjectCount % GAME_OBJECT_BATCH_SIZE) == 0)
				deletableGameObjects.pop_back();
		}
		while (u32NewGameObjectCount) {
			GameObject *const pObject = newGameObjects.back()[(u32NewGameObjectCount - 1) % GAME_OBJECT_BATCH_SIZE];
			PRINT_DEBUG("Adding new enqueued game object ", pObject);
			add_game_object(*pObject);
			u32NewGameObjectCount--;
			if ((u32NewGameObjectCount % GAME_OBJECT_BATCH_SIZE) == 0)
				newGameObjects.pop_back();
		}
		bDeletingAddingGameObjects = false;
	}

	void add_game_object(GameObject &rGameObject) {
		if ((u32CurrentGameObjectCount % GAME_OBJECT_BATCH_SIZE) == 0) {
			gameObjects.emplace_back();
			gameObjects.back()[0] = std::addressof(rGameObject);
		} else
			gameObjects.back()[u32CurrentGameObjectCount % GAME_OBJECT_BATCH_SIZE] = std::addressof(rGameObject);
		rGameObject.u32ListIndex = u32CurrentGameObjectCount;
		rGameObject.bNew = false;
		u32CurrentGameObjectCount++;
	}

	void start_game_objects() {
		eCallingPhase = CALLING_PHASE_GAME_OBJECT_START;
		auto gameObjectIter = gameObjects.begin();
		uint32_t u32CurrentIndex = 0;
		for (uint32_t u32CurrentGameObjectIndex = 0; u32CurrentGameObjectIndex < u32CurrentGameObjectCount; u32CurrentGameObjectIndex++) {
			(*gameObjectIter)[u32CurrentIndex]->start(pCurrentScene);
			u32CurrentIndex++;
			if (u32CurrentIndex == GAME_OBJECT_BATCH_SIZE) {
				u32CurrentIndex = 0;
				gameObjectIter++;
			}
		}
	}

	void update_game_objects() {
		eCallingPhase = CALLING_PHASE_GAME_OBJECT_UPDATE;
		auto gameObjectIter = gameObjects.begin();
		uint32_t u32CurrentIndex = 0;
		for (uint32_t u32CurrentGameObjectIndex = 0; u32CurrentGameObjectIndex < u32CurrentGameObjectCount; u32CurrentGameObjectIndex++) {
			(*gameObjectIter)[u32CurrentIndex]->update(pCurrentScene);
			u32CurrentIndex++;
			if (u32CurrentIndex == GAME_OBJECT_BATCH_SIZE) {
				u32CurrentIndex = 0;
				gameObjectIter++;
			}
		}
	}

	void end_game_objects() {
		eCallingPhase = CALLING_PHASE_GAME_OBJECT_END;
		auto gameObjectIter = gameObjects.begin();
		uint32_t u32CurrentIndex = 0;
		for (uint32_t u32CurrentGameObjectIndex = 0; u32CurrentGameObjectIndex < u32CurrentGameObjectCount; u32CurrentGameObjectIndex++) {
			(*gameObjectIter)[u32CurrentIndex]->end(pCurrentScene);
			u32CurrentIndex++;
			if (u32CurrentIndex == GAME_OBJECT_BATCH_SIZE) {
				u32CurrentIndex = 0;
				gameObjectIter++;
			}
		}
	}

	void mark_game_object_deletable(GameObject *const pGameObject) {
		PRINT_DEBUG("Informing game object ", pGameObject, " to be marked as deletable");
		pGameObject->marked_deletable();
		if (!bRunning || bDeletingAddingGameObjects) {
			PRINT_DEBUG("Deleting game object ", pGameObject, " as the engine is not running and there's nothing to wait for");
			delete pGameObject;
		} else if (pGameObject->bNew) {
			PRINT_DEBUG("Game object ", pGameObject, " is new and will be removed from the queue");
			auto newGameObjectIter = newGameObjects.begin();
			std::advance(newGameObjectIter, pGameObject->u32ListIndex / GAME_OBJECT_BATCH_SIZE);
			(*newGameObjectIter)[pGameObject->u32ListIndex % GAME_OBJECT_BATCH_SIZE] = newGameObjects.back()[u32NewGameObjectCount % GAME_OBJECT_BATCH_SIZE];
			u32NewGameObjectCount--;
		} else {
			PRINT_DEBUG("Enqueued game object ", pGameObject, " in delete-queue");
			if ((u32DeletableGameObjectCount % GAME_OBJECT_BATCH_SIZE) == 0) {
				deletableGameObjects.emplace_back();
				deletableGameObjects.back()[0] = pGameObject;
			} else
				deletableGameObjects.back()[u32DeletableGameObjectCount % GAME_OBJECT_BATCH_SIZE] = pGameObject;
			u32DeletableGameObjectCount++;
		}
	}

	[[nodiscard]]
	uint32_t get_max_game_object_count() {
		return u32MaxGameObjectCount;
	}

	void set_max_game_object_count(const uint32_t u32NewMaxGameObjectCount) {
		if (!bRunning)
			u32MaxGameObjectCount = u32NewMaxGameObjectCount;
		else
			RE_ERROR("The maximum count of game objects cannot be changed, while the engine is running");
	}

	[[nodiscard]]
	uint32_t get_current_game_object_count() {
		return u32CurrentGameObjectCount;
	}

}
