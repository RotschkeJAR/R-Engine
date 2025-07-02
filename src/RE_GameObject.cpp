#include "RE_Internal Header.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	GameObject::GameObject(uint32_t u32OwnId, uint32_t u32SceneParentId) : u32OwnId(u32OwnId), u32SceneParentId(u32SceneParentId) {
		if (bRunning)
			newGameObjects.push_back(this);
		else
			gameObjects.push_back(this);
	}
	GameObject::~GameObject() {
		std::vector<GameObject*>::iterator iterator;
		if (!gameObjects.empty()) {
			iterator = std::find(gameObjects.begin(), gameObjects.end(), this);
			if (iterator == gameObjects.end())
				RE_WARNING(append_to_string("The game object ", this, " has been deleted without being marked deletable before. May skip updating a game object or cause segmentation violation"));
		}
		if (!newGameObjects.empty()) {
			iterator = std::find(newGameObjects.begin(), newGameObjects.end(), this);
			if (iterator == newGameObjects.end())
				newGameObjects.erase(iterator);
		}
		if (!deletableGameObjects.empty()) {
			iterator = std::find(deletableGameObjects.begin(), deletableGameObjects.end(), this);
			if (iterator == deletableGameObjects.end())
				deletableGameObjects.erase(iterator);
		}
	}

	void GameObject::start(Scene* pStartingScene) {}
	void GameObject::update(Scene* pCurrentScene) {}
	void GameObject::end(Scene* pEndingScene) {}

}
