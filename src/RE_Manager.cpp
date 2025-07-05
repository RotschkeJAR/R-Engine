#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_Renderer.hpp"

namespace RE {

	Scene *pCurrentScene = nullptr, *pNextScene = nullptr;
	std::vector<GameObject*> gameObjects, deletableGameObjects, newGameObjects;
	bool bDeletingMarkedGameObjects = false;

	static void update_proc() {
		CATCH_SIGNAL_DETAILED(pCurrentScene->update(), append_to_string("Scene ID: ", pCurrentScene->u32Id).c_str());
		for (GameObject *pObj : gameObjects)
			if (is_object_active(pObj))
				CATCH_SIGNAL_DETAILED(pObj->update(pCurrentScene), append_to_string("Game Object ", pObj, ", ID: ", pObj->u32OwnId).c_str());
	}

	static void end_proc() {
		CATCH_SIGNAL_DETAILED(pCurrentScene->end(), append_to_string("Ending scene ", pCurrentScene, ", ID: ", pCurrentScene->u32Id).c_str());
		for (GameObject *pObj : gameObjects)
			if (is_object_active(pObj))
				CATCH_SIGNAL_DETAILED(pObj->end(pCurrentScene), append_to_string("Ending game object ", pObj, ", ID: ", pObj->u32OwnId).c_str());
	}

	static void delete_proc() {
		bDeletingMarkedGameObjects = true;
		for (GameObject *pObj : deletableGameObjects)
			delete pObj;
		deletableGameObjects.clear();
		bDeletingMarkedGameObjects = false;
	}

	static void add_proc() {
		gameObjects.insert(gameObjects.end(), newGameObjects.begin(), newGameObjects.end());
		newGameObjects.clear();
	}

	bool is_object_active(const GameObject *pGameObject) {
		return CATCH_SIGNAL_AND_RETURN(!pGameObject->u32SceneParentId || pGameObject->u32SceneParentId == pCurrentScene->u32Id, bool);
	}

	void game_logic_update() {
		if (pNextScene != pCurrentScene && pNextScene) {
			// End old scene
			if (pCurrentScene)
				CATCH_SIGNAL(end_proc());
			CATCH_SIGNAL(delete_proc());

			pCurrentScene = pNextScene;

			// Start new scene and game objects
			CATCH_SIGNAL_DETAILED(pCurrentScene->start(), append_to_string("Starting scene ", pCurrentScene, ", ID: ", pCurrentScene->u32Id).c_str());
			for (GameObject *pObj : gameObjects)
				if (is_object_active(pObj))
					CATCH_SIGNAL_DETAILED(pObj->start(pCurrentScene), append_to_string("Starting game object ", pObj, ", ID: ", pObj->u32OwnId).c_str());

			// Start newly added game objects
			bool bNewGameObjectsAdded;
			do {
				for (GameObject *pObj : newGameObjects)
					if (is_object_active(pObj))
						CATCH_SIGNAL_DETAILED(pObj->start(pCurrentScene), append_to_string("Starting new game object ", pObj, ", ID: ", pObj->u32OwnId).c_str());
				bNewGameObjectsAdded = !newGameObjects.empty();
				CATCH_SIGNAL(add_proc());
			} while (bNewGameObjectsAdded);
		} else if (!pCurrentScene) {
			RE_ERROR("There is no active scene at the moment");
			return;
		}
		CATCH_SIGNAL(update_proc());
		CATCH_SIGNAL(delete_proc());
		CATCH_SIGNAL(add_proc());
	}

	void last_game_logic_update() {
		if (pCurrentScene)
			CATCH_SIGNAL(end_proc());
		CATCH_SIGNAL(delete_proc());
		pCurrentScene = nullptr;
		pNextScene = nullptr;
	}

	bool is_game_valid() {
		return pCurrentScene || (!pCurrentScene && pNextScene);
	}

	void mark_deletable(GameObject *pGameObject) {
		if (!bRunning)
			delete pGameObject;
		else {
			std::vector<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), pGameObject);
			if (it == gameObjects.end()) {
				it = std::find(newGameObjects.begin(), newGameObjects.end(), pGameObject);
				if (it != std::end(newGameObjects))
					newGameObjects.erase(it);
				else
					RE_NOTE(append_to_string("The game object ", pGameObject, " is not in the game objects list. In case it had been constructed before the main-function started executing, it is always be discarded"));
			} else
				deletableGameObjects.push_back(pGameObject);
		}
	}

	void set_next_scene(Scene *pNextSceneParam) {
		if (!pNextSceneParam)
			return;
		else if (CATCH_SIGNAL_AND_RETURN(!pNextSceneParam->u32Id, bool)) {
			RE_WARNING("A scene has been set for becoming the next one, but its ID is zero and therefore has been discarded");
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

	uint32_t get_current_scene_id() {
		Scene *pCurrentScene = get_current_scene();
		return pCurrentScene ? CATCH_SIGNAL_AND_RETURN(pCurrentScene->u32Id, uint32_t) : 0U;
	}

	bool is_scene_current(const uint32_t u32SceneId) {
		return get_current_scene() && get_current_scene_id() == u32SceneId;
	}

	Scene* get_next_scene() {
		return pNextScene;
	}

	uint32_t get_next_scene_id() {
		Scene *pNextScene = get_next_scene();
		return pNextScene ? CATCH_SIGNAL_AND_RETURN(pNextScene->u32Id, uint32_t) : 0U;
	}

	bool is_scene_next(const uint32_t u32SceneId) {
		return get_next_scene() && get_next_scene_id() == u32SceneId;
	}

}
