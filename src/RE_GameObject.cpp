#include "RE_Internal Header.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	GameObject::GameObject(uint32_t u32OwnId, uint32_t u32SceneParentId) : u32OwnId(u32OwnId), u32SceneParentId(u32SceneParentId) {
		newGameObjects.push_back(this);
	}
	GameObject::~GameObject() {
		std::vector<GameObject*>::iterator it;
		if (!gameObjects.empty()) {
			it = std::find(gameObjects.begin(), gameObjects.end(), this);
			if (it != gameObjects.end()) {
				if (!bDeletingMarkedGameObjects && bRunning)
					RE_ERROR(append_to_string("The game object ", this, " (ID: ", u32OwnId, ", Scene parent ID: ", u32SceneParentId, ") hasn't been marked deletable. This may cause skipping a game object or segmentation violation"));
				gameObjects.erase(it);
			}
		}
		if (!newGameObjects.empty()) {
			it = std::find(newGameObjects.begin(), newGameObjects.end(), this);
			if (it != newGameObjects.end())
				newGameObjects.erase(it);
		}
		if (!deletableGameObjects.empty() && !bDeletingMarkedGameObjects) {
			it = std::find(deletableGameObjects.begin(), deletableGameObjects.end(), this);
			if (it != deletableGameObjects.end())
				deletableGameObjects.erase(it);
		}
	}

	void GameObject::start(Scene* pStartingScene) {}
	void GameObject::update(Scene* pCurrentScene) {}
	void GameObject::end(Scene* pEndingScene) {}

}
