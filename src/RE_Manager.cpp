#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {

	Scene *pCurrentScene = nullptr, *pNextScene = nullptr;
	std::vector<GameObject*> gameObjects, deletableGameObjects, newGameObjects;
	Manager *Manager::pInstance = nullptr;
	
	Manager::Manager() {
		if (pInstance) {
			RE_FATAL_ERROR("Another instance of the class \"Manager\" has been constructed");
			return;
		}
		pInstance = this;
	}

	Manager::~Manager() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
		pCurrentScene = nullptr;
		pNextScene = nullptr;
	}

	bool Manager::should_update_object(GameObject* pGameObject) {
		bool bResult;
		CATCH_SIGNAL(bResult = !pGameObject->u32SceneParentId || pGameObject->u32SceneParentId == pCurrentScene->u32Id);
		return bResult;
	}

	void Manager::start_proc() {
		if (!pCurrentScene)
			return;
		CATCH_SIGNAL_DETAILED(pCurrentScene->start(), append_to_string("Scene ID: ", pCurrentScene->u32Id).c_str());
		for (GameObject* pObj : gameObjects)
			if (should_update_object(pObj))
				CATCH_SIGNAL_DETAILED(pObj->start(pCurrentScene), append_to_string("Game Object ID: ", pObj->u32OwnId).c_str());
	}

	void Manager::update_proc() {
		if (!pCurrentScene)
			return;
		CATCH_SIGNAL_DETAILED(pCurrentScene->update(), append_to_string("Scene ID: ", pCurrentScene->u32Id).c_str());
		for (GameObject* pObj : gameObjects)
			if (should_update_object(pObj))
				CATCH_SIGNAL_DETAILED(pObj->update(pCurrentScene), append_to_string("Game Object ID: ", pObj->u32OwnId).c_str());
	}

	void Manager::end_proc() {
		if (!pCurrentScene)
			return;
		CATCH_SIGNAL_DETAILED(pCurrentScene->end(), append_to_string("Scene ID: ", pCurrentScene->u32Id).c_str());
		for (GameObject* pObj : gameObjects)
			if (should_update_object(pObj))
				CATCH_SIGNAL_DETAILED(pObj->end(pCurrentScene), append_to_string("Game Object ID: ", pObj->u32OwnId).c_str());
	}

	void Manager::delete_proc() {
		for (GameObject* deletableGameObject : deletableGameObjects)
			DELETE_SAFELY(deletableGameObject);
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
			RE_ERROR("There is no active scene at the moment");
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
		std::vector<GameObject*>::iterator iteratorGameObject = std::find(std::begin(gameObjects), std::end(gameObjects), pGameObject);
		if (iteratorGameObject == std::end(gameObjects)) {
			iteratorGameObject = std::find(std::begin(newGameObjects), std::end(newGameObjects), pGameObject);
			if (iteratorGameObject != std::end(newGameObjects)) {
				newGameObjects.erase(iteratorGameObject);
				return;
			}
			RE_NOTE(append_to_string("The memory address ", pGameObject, " doesn't point to a game object, that has to be deleted, or is not listed. In case it's been constructed before the main-function had been started executing, it will always be discarded"));
			return;
		} else if (Manager::pInstance)
			deletableGameObjects.push_back(pGameObject);
		else
			DELETE_SAFELY(pGameObject);
	}

	void set_next_scene(Scene* pNextSceneParam) {
		if (!pNextSceneParam)
			return;
		else if (CATCH_SIGNAL_AND_RETURN(!pNextSceneParam->u32Id, bool)) {
			RE_WARNING("A scene has been set for becoming the next one, but its ID is zero and has been therefore discarded");
			return;
		}
		pNextScene = pNextSceneParam;
	}

	bool is_next_scene_set() {
		return pCurrentScene != pNextScene && pNextScene;
	}

	Scene* get_current_scene() {
		return pCurrentScene;
	}

	REuint get_current_scene_id() {
		Scene* pCurrentScene = get_current_scene();
		return pCurrentScene ? CATCH_SIGNAL_AND_RETURN(pCurrentScene->u32Id, uint32_t) : 0U;
	}

	bool is_scene_current(REuint u32SceneId) {
		return get_current_scene() && get_current_scene_id() == u32SceneId;
	}

	Scene* get_next_scene() {
		return pNextScene;
	}

	REuint get_next_scene_id() {
		Scene* pNextScene = get_next_scene();
		return pNextScene ? CATCH_SIGNAL_AND_RETURN(pNextScene->u32Id, uint32_t) : 0U;
	}

	bool is_scene_next(REuint u32SceneId) {
		return get_next_scene() && get_next_scene_id() == u32SceneId;
	}

}
