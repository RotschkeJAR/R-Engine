#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {

	Scene *Manager::pCurrentScene = nullptr, *Manager::pNextScene = nullptr;
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

	bool Manager::should_update_object(GameObject* pGameObject) {
		return !pGameObject->u32SceneParentId || pGameObject->u32SceneParentId == pCurrentScene->u32Id;
	}

	void Manager::start_proc() {
		if (!pCurrentScene)
			return;
		CATCH_SIGNAL(pCurrentScene->start());
		for (GameObject* pObj : gameObjects)
			if (should_update_object(pObj))
				CATCH_SIGNAL(pObj->start(pCurrentScene));
	}

	void Manager::update_proc() {
		if (!pCurrentScene)
			return;
		CATCH_SIGNAL(pCurrentScene->update());
		for (GameObject* pObj : gameObjects)
			if (should_update_object(pObj))
				CATCH_SIGNAL(pObj->update(pCurrentScene));
	}

	void Manager::end_proc() {
		if (!pCurrentScene)
			return;
		CATCH_SIGNAL(pCurrentScene->end());
		for (GameObject* pObj : gameObjects)
			if (should_update_object(pObj))
				CATCH_SIGNAL(pObj->end(pCurrentScene));
	}

	void Manager::delete_proc() {
		for (GameObject* deletableGameObject : deletableGameObjects)
			CATCH_SIGNAL(delete deletableGameObject);
		deletableGameObjects.clear();
	}

	void Manager::add_proc() {
		for (GameObject* newGameObject : newGameObjects)
			gameObjects.push_back(newGameObject);
		newGameObjects.clear();
	}

	void Manager::game_logic_update() {
		if (pNextScene != pCurrentScene && pNextScene) {
			// Switch scene
			CATCH_SIGNAL(end_proc());
			CATCH_SIGNAL(delete_proc());
			pCurrentScene = pNextScene;
			CATCH_SIGNAL(start_proc());
			CATCH_SIGNAL(add_proc());
		} else if (!pCurrentScene) {
			RE_FATAL_ERROR("There is no active scene at the moment");
			return;
		}
		CATCH_SIGNAL(update_proc());
		CATCH_SIGNAL(delete_proc());
		CATCH_SIGNAL(add_proc());
	}

	void Manager::last_game_logic_update() {
		CATCH_SIGNAL(end_proc());
		CATCH_SIGNAL(delete_proc());
	}

	bool Manager::is_game_valid() {
		return pCurrentScene || pNextScene;
	}

	void mark_delete(GameObject* pGameObject) {
		std::vector<GameObject*>::iterator iteratorGameObject = std::find(std::begin(Manager::gameObjects), std::end(Manager::gameObjects), pGameObject);
		if (iteratorGameObject == std::end(Manager::gameObjects)) {
			if (Manager::pInstance) {
				iteratorGameObject = std::find(std::begin(Manager::pInstance->newGameObjects), std::end(Manager::pInstance->newGameObjects), pGameObject);
				if (iteratorGameObject != std::end(Manager::pInstance->newGameObjects)) {
					Manager::pInstance->newGameObjects.erase(iteratorGameObject);
					return;
				}
			}
			RE_NOTE(append_strings("The memory address ", pGameObject, " doesn't point to a game object, that has to be deleted, or is not listed. In case it's constructed before the main-function is executed, it will always be discarded"));
			return;
		} else if (Manager::pInstance)
			Manager::pInstance->deletableGameObjects.push_back(pGameObject);
		else
			CATCH_SIGNAL(delete pGameObject);
	}

	void set_next_scene(Scene* pNextScene) {
		DEFINE_SIGNAL_GUARD(sigGuardSetNextSceneFunc);
		if (!pNextScene)
			return;
		else if (!pNextScene->u32Id) {
			RE_WARNING("A scene has been set for becoming the next one, but its ID is zero and has been therefore discarded");
			return;
		}
		Manager::pNextScene = pNextScene;
	}

	bool is_next_scene_set() {
		return Manager::pCurrentScene != Manager::pNextScene && Manager::pNextScene;
	}

	Scene* get_current_scene() {
		return Manager::pCurrentScene;
	}

	REuint get_current_scene_id() {
		Scene* pCurrentScene = get_current_scene();
		return pCurrentScene ? pCurrentScene->u32Id : 0U;
	}

	bool is_scene_current(REuint u32SceneId) {
		return get_current_scene() && get_current_scene_id() == u32SceneId;
	}

	Scene* get_next_scene() {
		return Manager::pNextScene;
	}

	REuint get_next_scene_id() {
		Scene* pNextScene = get_next_scene();
		return pNextScene ? pNextScene->u32Id : 0U;
	}

	bool is_scene_next(REuint u32SceneId) {
		return get_next_scene() && get_next_scene_id() == u32SceneId;
	}

}
