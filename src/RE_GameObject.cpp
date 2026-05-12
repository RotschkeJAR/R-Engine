#include "RE_Internal Header.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {
	
	GameObject::GameObject(const uint32_t u32OwnId, const uint32_t u32SceneParentId) : u32ListIndex(-1), bNew(true), u32OwnId(u32OwnId), u32SceneParentId(u32SceneParentId) {
		if (u32CurrentGameObjectCount == u32MaxGameObjectCount)
			RE_ABORT("");
		if (bRunning && !bDeletingAddingGameObjects) {
			if ((u32NewGameObjectCount % GAME_OBJECT_BATCH_SIZE) == 0) {
				newGameObjects.emplace_back();
				newGameObjects.back()[0] = this;
			} else
				newGameObjects.back()[u32NewGameObjectCount % GAME_OBJECT_BATCH_SIZE] = this;
			u32ListIndex = u32NewGameObjectCount;
			u32NewGameObjectCount++;
		} else
			add_game_object(*this);
	}
	GameObject::~GameObject() {
		if (bDeletingAddingGameObjects)
			return;
		RE_WARNING("Removing game object, when game objects aren't being added or deleted. Mark it as deletable to avoid potential bugs");
		if (bNew) {
			GameObject &rMovingObject = *newGameObjects.back()[u32NewGameObjectCount % GAME_OBJECT_BATCH_SIZE];
			auto newGameObjectIter = newGameObjects.begin();
			std::advance(newGameObjectIter, u32ListIndex / GAME_OBJECT_BATCH_SIZE);
			(*newGameObjectIter)[u32ListIndex % GAME_OBJECT_BATCH_SIZE] = std::addressof(rMovingObject);
			rMovingObject.u32ListIndex = u32ListIndex;
			u32NewGameObjectCount--;
		} else {
			GameObject &rMovingObject = *gameObjects.back()[u32CurrentGameObjectCount % GAME_OBJECT_BATCH_SIZE];
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
