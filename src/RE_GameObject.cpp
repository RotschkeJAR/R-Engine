#include "RE_Ext Header.hpp"
#include "RE_Manager.hpp"

namespace RE {
	
	GameObject::GameObject(REuint u32OwnId, REuint u32SceneParentId) : u32OwnId(u32OwnId), u32SceneParentId(u32SceneParentId) {
		if (Manager::pInstance)
			Manager::pInstance->newGameObjects.push_back(this);
		else
			Manager::gameObjects.push_back(this);
	}
	GameObject::~GameObject() {
		std::vector<GameObject*>::iterator iteratorToGameObject = std::find(std::begin(Manager::gameObjects), std::end(Manager::gameObjects), this);
		if (iteratorToGameObject != std::end(Manager::gameObjects))
			Manager::gameObjects.erase(iteratorToGameObject);
		else
			RE_NOTE("A recently deleted game object wasn't in the game engine's list");
		if (Manager::pInstance && !Manager::pInstance->newGameObjects.empty()) {
			iteratorToGameObject = std::find(std::begin(Manager::pInstance->newGameObjects), std::end(Manager::pInstance->newGameObjects), this);
			if (iteratorToGameObject != std::end(Manager::pInstance->newGameObjects))
				Manager::pInstance->newGameObjects.erase(iteratorToGameObject);
		}
	}

	void GameObject::start(Scene* pStartingScene) {}
	void GameObject::update(Scene* pCurrentScene) {}
	void GameObject::end(Scene* pEndingScene) {}

}
