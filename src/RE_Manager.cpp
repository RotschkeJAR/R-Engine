#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {

	Scene *pCurrentScene = nullptr, *pNextScene = nullptr;

	static void update_proc() {
		CATCH_SIGNAL_DETAILED(pCurrentScene->update(), append_to_string("Scene ID: ", pCurrentScene->u32Id).c_str());
		CATCH_SIGNAL(update_game_objects());
	}

	static void end_proc() {
		CATCH_SIGNAL_DETAILED(pCurrentScene->end(), append_to_string("Ending scene ", pCurrentScene, ", ID: ", pCurrentScene->u32Id).c_str());
		CATCH_SIGNAL(end_game_objects());
	}

	static void delete_proc() {
		CATCH_SIGNAL(delete_marked_game_objects());
	}

	static void add_proc() {
		CATCH_SIGNAL(add_new_game_objects());
	}

	bool is_object_active(const GameObject *pGameObject) {
		return CATCH_SIGNAL_AND_RETURN(!pGameObject->u32SceneParentId || pGameObject->u32SceneParentId == pCurrentScene->u32Id, bool);
	}

	void game_logic_update() {
		if (pNextScene != pCurrentScene && pNextScene) {
			// End old scene
			if (pCurrentScene)
				CATCH_SIGNAL(end_proc());
			while (!deletableGameObjects.empty())
				CATCH_SIGNAL(delete_proc());

			pCurrentScene = pNextScene;

			// Start new scene and game objects
			CATCH_SIGNAL_DETAILED(pCurrentScene->start(), append_to_string("Starting scene ", pCurrentScene, ", ID: ", pCurrentScene->u32Id).c_str());
			CATCH_SIGNAL(start_game_objects());

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
		while (!deletableGameObjects.empty())
			CATCH_SIGNAL(delete_proc());
		while (!newGameObjects.empty())
			CATCH_SIGNAL(add_proc());
	}

	void last_game_logic_update() {
		if (pCurrentScene)
			CATCH_SIGNAL(end_proc());
		CATCH_SIGNAL(delete_proc());
		pCurrentScene = nullptr;
		pNextScene = nullptr;
	}

	bool are_scenes_present() {
		return pCurrentScene || (!pCurrentScene && pNextScene);
	}

	void set_next_scene(Scene *const pNextSceneParam) {
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
		Scene *const pCurrentScene = get_current_scene();
		return pCurrentScene ? CATCH_SIGNAL_AND_RETURN(pCurrentScene->u32Id, uint32_t) : 0U;
	}

	bool is_scene_current(const uint32_t u32SceneId) {
		return get_current_scene() && get_current_scene_id() == u32SceneId;
	}

	Scene* get_next_scene() {
		return pNextScene;
	}

	uint32_t get_next_scene_id() {
		Scene *const pNextScene = get_next_scene();
		return pNextScene ? CATCH_SIGNAL_AND_RETURN(pNextScene->u32Id, uint32_t) : 0U;
	}

	bool is_scene_next(const uint32_t u32SceneId) {
		return get_next_scene() && get_next_scene_id() == u32SceneId;
	}

}
