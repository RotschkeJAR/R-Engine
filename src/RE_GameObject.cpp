#include "RE_Internal Header.hpp"
#include "RE_Manager.hpp"

namespace RE {
	
	GameObject::GameObject(REuint u32OwnId, REuint u32SceneParentId) : u32OwnId(u32OwnId), u32SceneParentId(u32SceneParentId) {
		Manager::newGameObjects.push_back(this);
	}
	GameObject::~GameObject() {
		std::vector<GameObject*>::iterator iteratorToGameObject;
		if (!Manager::gameObjects.empty()) {
			iteratorToGameObject = std::find(Manager::gameObjects.begin(), Manager::gameObjects.end(), this);
			if (iteratorToGameObject != Manager::gameObjects.end())
				Manager::gameObjects.erase(iteratorToGameObject);
		}
		if (!Manager::newGameObjects.empty()) {
			iteratorToGameObject = std::find(Manager::newGameObjects.begin(), Manager::newGameObjects.end(), this);
			if (iteratorToGameObject != Manager::newGameObjects.end())
				Manager::newGameObjects.erase(iteratorToGameObject);
		}
		if (!Manager::deletableGameObjects.empty()) {
			iteratorToGameObject = std::find(Manager::deletableGameObjects.begin(), Manager::deletableGameObjects.end(), this);
			if (iteratorToGameObject != Manager::deletableGameObjects.end())
				Manager::deletableGameObjects.erase(iteratorToGameObject);
		}
	}

	void GameObject::start(Scene* pStartingScene) {}
	void GameObject::update(Scene* pCurrentScene) {}
	void GameObject::end(Scene* pEndingScene) {}

}
