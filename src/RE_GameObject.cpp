#include "RE_Internal Header.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {
	
	GameObject::GameObject(const uint32_t u32OwnId, const uint32_t u32SceneParentId) : u32ListIndex(-1), bNew(true), u32OwnId(u32OwnId), u32SceneParentId(u32SceneParentId) {
		if (bRunning) {
			if (u32NewGameObjectCount % GAME_OBJECT_BATCH_SIZE == 0) {
				newGameObjects.emplace_back();
				newGameObjects.back()[0] = this;
			} else
				newGameObjects.back()[u32NewGameObjectCount % GAME_OBJECT_BATCH_SIZE] = this;
			u32NewGameObjectCount++;
		} else
			add_game_object(*this);
	}
	GameObject::~GameObject() {
		PRINT_DEBUG_CLASS("Removing game object from list");
		if (u32CurrentGameObjectCount > 1) {
			GameObject &rMovingObject = *gameObjects.back()[u32CurrentGameObjectCount % GAME_OBJECT_BATCH_SIZE];
			gameObjects[rGameObject.u32ListIndex / GAME_OBJECT_BATCH_SIZE][rGameObject.u32ListIndex % GAME_OBJECT_BATCH_SIZE] = std::addressof(rMovingObject);
			rMovingObject.u32ListIndex = rGameObject.u32ListIndex;
		}
		u32CurrentGameObjectCount--;
	}

	void GameObject::marked_deletable() {}

	void GameObject::start(Scene *const pStartingScene) {}
	void GameObject::update(Scene *const pCurrentScene) {}
	void GameObject::end(Scene *const pEndingScene) {}

}
