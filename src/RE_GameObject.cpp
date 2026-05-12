#include "RE_Internal Header.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {
	
	GameObject::GameObject(const uint32_t u32OwnId, const uint32_t u32SceneParentId) : bNew(true), u32OwnId(u32OwnId), u32SceneParentId(u32SceneParentId) {
		if (u32CurrentGameObjectCount == u32MaxGameObjectCount)
			RE_ABORT("Game object pool exhausted due to overallocation");
		if (bRunning && !bDeletingAddingGameObjects) {
			PRINT_DEBUG_CLASS("Enqueuing new game object");
			if ((u32NewGameObjectCount % GAME_OBJECT_BATCH_SIZE) == 0) {
				newGameObjects.emplace_back();
				newGameObjects.back()[0] = this;
			} else
				newGameObjects.back()[u32NewGameObjectCount % GAME_OBJECT_BATCH_SIZE] = this;
			u32ListIndex = u32NewGameObjectCount;
			u32NewGameObjectCount++;
		} else {
			PRINT_DEBUG_CLASS("Directly adding new game object");
			add_game_object(*this);
		}
	}
	GameObject::~GameObject() {
		PRINT_DEBUG_CLASS("Destructing game object");
		if (bNew) {
			PRINT_DEBUG_CLASS("Removing game object from queue of new game objects");
			GameObject &rMovingObject = *newGameObjects.back()[(u32NewGameObjectCount - 1) % GAME_OBJECT_BATCH_SIZE];
			auto newGameObjectIter = newGameObjects.begin();
			std::advance(newGameObjectIter, u32ListIndex / GAME_OBJECT_BATCH_SIZE);
			(*newGameObjectIter)[u32ListIndex % GAME_OBJECT_BATCH_SIZE] = std::addressof(rMovingObject);
			rMovingObject.u32ListIndex = u32ListIndex;
			u32NewGameObjectCount--;
		} else {
			PRINT_DEBUG_CLASS("Removing game object from game object list");
			GameObject &rMovingObject = *gameObjects.back()[(u32CurrentGameObjectCount - 1) % GAME_OBJECT_BATCH_SIZE];
			auto gameObjectIter = gameObjects.begin();
			std::advance(gameObjectIter, u32ListIndex / GAME_OBJECT_BATCH_SIZE);
			(*gameObjectIter)[u32ListIndex % GAME_OBJECT_BATCH_SIZE] = std::addressof(rMovingObject);
			rMovingObject.u32ListIndex = u32ListIndex;
			u32CurrentGameObjectCount--;
		}
	}

	void GameObject::marked_deletable() {}

	void GameObject::start(Scene *const pStartingScene) {}
	void GameObject::update(Scene *const pCurrentScene) {}
	void GameObject::end(Scene *const pEndingScene) {}

}
