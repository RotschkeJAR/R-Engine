#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {

	Scene *Manager::pCurrentScene, *Manager::pNextScene;
	std::vector<GameObject*> Manager::gameObjects;
	Manager* Manager::pInstance = nullptr;
	
	Manager::Manager() {
		if (pInstance) {
			RE_FATAL_ERROR("Another instance of the class \"Manager\" has been constructed");
			return;
		}
		Manager::pInstance = this;
	}

	Manager::~Manager() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
		pCurrentScene = nullptr;
		pNextScene = nullptr;
	}

	void Manager::startProc() {
		if (!pCurrentScene)
			return;
		pCurrentScene->start();
		for (GameObject* obj : gameObjects)
			if (obj->u32SceneParentId && obj->u32SceneParentId == pCurrentScene->u32Id)
				obj->start(pCurrentScene);
	}

	void Manager::updateProc() {
		if (!pCurrentScene)
			return;
		pCurrentScene->update();
		for (GameObject* obj : gameObjects)
			if (obj->u32SceneParentId && obj->u32SceneParentId == pCurrentScene->u32Id)
				obj->update(pCurrentScene);
	}

	void Manager::endProc() {
		if (!pCurrentScene)
			return;
		pCurrentScene->end();
		for (GameObject* obj : gameObjects)
			if (obj->u32SceneParentId && obj->u32SceneParentId == pCurrentScene->u32Id)
				obj->end(pCurrentScene);
	}

	void Manager::deleteProc() {
		for (GameObject* deletableGameObject : deletableGameObjects)
			delete deletableGameObject;
		deletableGameObjects.clear();
	}

	void Manager::addProc() {
		for (GameObject* newGameObject : newGameObjects)
			gameObjects.push_back(newGameObject);
		newGameObjects.clear();
	}


	void Manager::gameLogicUpdate() {
		if (pNextScene != pCurrentScene && pNextScene) {
			// Switch scene
			endProc();
			deleteProc();
			pCurrentScene = pNextScene;
			addProc();
			startProc();
		} else if (!pCurrentScene) {
			RE_FATAL_ERROR("There is no active scene at the moment");
			return;
		}
		updateProc();
		deleteProc();
		addProc();
	}

	void Manager::lastGameLogicUpdate() {
		endProc();
		deleteProc();
	}

	bool Manager::isGameValid() {
		return pCurrentScene || pNextScene;
	}

	void markDelete(GameObject* pGameObject) {
		std::vector<GameObject*>::iterator iteratorGameObject = std::find(std::begin(Manager::gameObjects), std::end(Manager::gameObjects), pGameObject);
		if (iteratorGameObject == std::end(Manager::gameObjects)) {
			RE_NOTE(appendStrings("The memory address ", pGameObject, " doesn't point to a game object, that has to be deleted, or is not listed. In case it's been constructed before the main-function was executed, it will always be discarded"));
			return;
		}
		if (Manager::pInstance)
			Manager::pInstance->deletableGameObjects.push_back(pGameObject);
		else
			delete pGameObject;
	}

	void setNextScene(Scene* pNextScene) {
		if (!pNextScene)
			return;
		else if (!pNextScene->u32Id) {
			RE_WARNING("A scene has been set for becoming the next one, but its ID is zero and has been therefore discarded");
			return;
		}
		Manager::pNextScene = pNextScene;
	}

	bool isNextSceneSet() {
		return Manager::pCurrentScene != Manager::pNextScene && Manager::pNextScene;
	}

	Scene* getCurrentScene() {
		return Manager::pCurrentScene;
	}

	REuint getCurrentSceneId() {
		Scene* pCurrentScene = getCurrentScene();
		return pCurrentScene ? pCurrentScene->u32Id : static_cast<REuint>(0);
	}

	bool isSceneCurrent(REuint u32SceneId) {
		return getCurrentSceneId() == u32SceneId && getCurrentScene();
	}

	Scene* getNextScene() {
		return Manager::pNextScene;
	}

	REuint getNextSceneId() {
		Scene* pNextScene = getNextScene();
		return pNextScene ? pNextScene->u32Id : static_cast<REuint>(0);
	}

	bool isSceneNext(REuint u32SceneId) {
		return getNextSceneId() == u32SceneId && getNextScene();
	}

}
