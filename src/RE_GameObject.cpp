#include "RE_Internal Header.hpp"
#include "RE_Manager.hpp"

namespace RE {
	
	GameObject::GameObject(REuint u32OwnId, REuint u32SceneParentId) : u32OwnId(u32OwnId), u32SceneParentId(u32SceneParentId) {
		newGameObjects.push_back(this);
	}
	GameObject::~GameObject() {
		std::vector<GameObject*>::iterator iteratorToGameObject;
		if (!gameObjects.empty()) {
			iteratorToGameObject = std::find(gameObjects.begin(), gameObjects.end(), this);
			if (iteratorToGameObject != gameObjects.end())
				gameObjects.erase(iteratorToGameObject);
		}
		if (!newGameObjects.empty()) {
			iteratorToGameObject = std::find(newGameObjects.begin(), newGameObjects.end(), this);
			if (iteratorToGameObject != newGameObjects.end())
				newGameObjects.erase(iteratorToGameObject);
		}
		if (!deletableGameObjects.empty()) {
			iteratorToGameObject = std::find(deletableGameObjects.begin(), deletableGameObjects.end(), this);
			if (iteratorToGameObject != deletableGameObjects.end())
				deletableGameObjects.erase(iteratorToGameObject);
		}
	}

	void GameObject::start(Scene* pStartingScene) {}
	void GameObject::update(Scene* pCurrentScene) {}
	void GameObject::end(Scene* pEndingScene) {}

}
