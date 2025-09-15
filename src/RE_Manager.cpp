#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_List_Camera.hpp"
#include "RE_Vulkan_Device.hpp"

namespace RE {

	Scene *pCurrentScene = nullptr, *pNextScene = nullptr;

	static void update_proc() {
		PUSH_TO_CALLSTACKTRACE_DETAILED(pCurrentScene->update(), append_to_string("Scene ", pCurrentScene, ", ID: ", pCurrentScene->u32Id));
		PUSH_TO_CALLSTACKTRACE(update_game_objects());
		PUSH_TO_CALLSTACKTRACE(update_cameras());
	}

	static void end_proc() {
		PUSH_TO_CALLSTACKTRACE_DETAILED(pCurrentScene->end(), append_to_string("Scene ", pCurrentScene, ", ID: ", pCurrentScene->u32Id));
		PUSH_TO_CALLSTACKTRACE(end_game_objects());
		PUSH_TO_CALLSTACKTRACE(end_cameras());
	}

	static void delete_proc() {
		PUSH_TO_CALLSTACKTRACE(delete_marked_game_objects());
		PUSH_TO_CALLSTACKTRACE(delete_marked_cameras());
	}

	static void add_proc() {
		PUSH_TO_CALLSTACKTRACE(add_new_game_objects());
		PUSH_TO_CALLSTACKTRACE(add_new_cameras());
	}

	[[nodiscard]]
	bool is_object_active(const GameObject *const pGameObject) {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(!pGameObject->u32SceneParentId || pGameObject->u32SceneParentId == pCurrentScene->u32Id, bool);
	}

	void game_logic_update() {
		if (pNextScene != pCurrentScene && pNextScene) {
			// End old scene
			WAIT_FOR_IDLE_VULKAN_DEVICE();
			if (pCurrentScene)
				PUSH_TO_CALLSTACKTRACE(end_proc());
			while (!deletableGameObjects.empty())
				PUSH_TO_CALLSTACKTRACE(delete_proc());

			pCurrentScene = pNextScene;

			// Start new scene and game objects
			PUSH_TO_CALLSTACKTRACE_DETAILED(pCurrentScene->start(), append_to_string("Scene ", pCurrentScene, ", ID: ", pCurrentScene->u32Id));
			PUSH_TO_CALLSTACKTRACE(start_game_objects());
			PUSH_TO_CALLSTACKTRACE(start_cameras());

			// Start newly added objects
			bool bNewObjectsAdded;
			do {
				for (GameObject *const pObj : newGameObjects)
					if (is_object_active(pObj))
						PUSH_TO_CALLSTACKTRACE_DETAILED(pObj->start(pCurrentScene), append_to_string("Starting new game object ", pObj, ", ID: ", pObj->u32OwnId));
				bNewObjectsAdded = !newGameObjects.empty();
				PUSH_TO_CALLSTACKTRACE(add_proc());
			} while (bNewObjectsAdded);
		} else if (!pCurrentScene) {
			RE_ERROR("There is no active scene at the moment");
			return;
		}
		PUSH_TO_CALLSTACKTRACE(update_proc());
		while (!deletableGameObjects.empty())
			PUSH_TO_CALLSTACKTRACE(delete_proc());
		while (!newGameObjects.empty())
			PUSH_TO_CALLSTACKTRACE(add_proc());
	}

	void last_game_logic_update() {
		if (pCurrentScene)
			PUSH_TO_CALLSTACKTRACE(end_proc());
		PUSH_TO_CALLSTACKTRACE(delete_proc());
		pCurrentScene = nullptr;
		pNextScene = nullptr;
	}

	[[nodiscard]]
	bool are_scenes_present() {
		return pCurrentScene || (!pCurrentScene && pNextScene);
	}

	void set_next_scene(Scene *const pNextSceneParam) {
		if (!pNextSceneParam)
			return;
		else if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(!pNextSceneParam->u32Id, bool)) {
			RE_WARNING("A scene has been set for becoming the next one, but its ID is zero and therefore has been discarded");
			return;
		}
		pNextScene = pNextSceneParam;
	}

	[[nodiscard]]
	bool is_next_scene_set() {
		return pCurrentScene != pNextScene && pNextScene;
	}

	[[nodiscard]]
	Scene* get_current_scene() {
		return pCurrentScene;
	}

	[[nodiscard]]
	uint32_t get_current_scene_id() {
		Scene *const pCurrentScene = get_current_scene();
		return pCurrentScene ? PUSH_TO_CALLSTACKTRACE_AND_RETURN(pCurrentScene->u32Id, uint32_t) : 0;
	}

	[[nodiscard]]
	bool is_scene_current(const uint32_t u32SceneId) {
		return get_current_scene() && get_current_scene_id() == u32SceneId;
	}

	[[nodiscard]]
	Scene* get_next_scene() {
		return pNextScene;
	}

	[[nodiscard]]
	uint32_t get_next_scene_id() {
		Scene *const pNextScene = get_next_scene();
		return pNextScene ? PUSH_TO_CALLSTACKTRACE_AND_RETURN(pNextScene->u32Id, uint32_t) : 0;
	}

	[[nodiscard]]
	bool is_scene_next(const uint32_t u32SceneId) {
		return get_next_scene() && get_next_scene_id() == u32SceneId;
	}

}
