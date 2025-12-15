#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_List_Camera.hpp"
#include "RE_Vulkan_Device.hpp"

namespace RE {

	Scene *pCurrentScene = nullptr, *pNextScene = nullptr;

	static void update_proc() {
		PRINT_DEBUG("Invoking update-procedure");
		pCurrentScene->update();
		update_game_objects();
		update_cameras();
	}

	static void end_proc() {
		PRINT_DEBUG("Invoking end-procedure");
		pCurrentScene->end();
		end_game_objects();
		end_cameras();
	}

	static void delete_proc() {
		PRINT_DEBUG("Invoking delete-procedure");
		while (!deletableGameObjects.empty() || !deletableCameras.empty()) {
			delete_marked_game_objects();
			delete_marked_cameras();
		}
	}

	static void add_proc() {
		PRINT_DEBUG("Invoking add-procedure");
		while (!newGameObjects.empty() || !newCameras.empty()) {
			add_new_game_objects();
			add_new_cameras();
		}
	}

	[[nodiscard]]
	bool is_object_active(const GameObject *const pGameObject) {
		return !pGameObject->u32SceneParentId || pGameObject->u32SceneParentId == pCurrentScene->u32Id;
	}

	void game_logic_update() {
		PRINT_DEBUG("Updating game's logic");
		if (pNextScene != pCurrentScene && pNextScene) {
			PRINT_DEBUG("Ending current scene ", pCurrentScene);
			if (pCurrentScene)
				end_proc();
			delete_proc();

			PRINT_DEBUG("Switching to and starting new scene ", pNextScene);
			pCurrentScene = pNextScene;
			pCurrentScene->start();
			start_game_objects();
			start_cameras();

			// Start newly added objects
			bool bNewObjectsAdded;
			do {
				for (GameObject *const pObj : newGameObjects)
					if (is_object_active(pObj)) {
						PRINT_DEBUG("Starting and later adding new game object ", pObj);
						pObj->start(pCurrentScene);
					}
				bNewObjectsAdded = !newGameObjects.empty();
				add_proc();
			} while (bNewObjectsAdded);
			PRINT_DEBUG("Finished switching to new scene");
		} else if (!pCurrentScene) {
			RE_ERROR("There is no active scene at the moment");
			return;
		}
		update_proc();
		delete_proc();
		add_proc();
	}

	void last_game_logic_update() {
		PRINT_DEBUG("Ending scene");
		if (pCurrentScene)
			end_proc();
		while (!deletableGameObjects.empty())
			delete_proc();
		pCurrentScene = nullptr;
		pNextScene = nullptr;
		PRINT_DEBUG("Finished ending scene");
	}

	[[nodiscard]]
	bool are_scenes_present() {
		return pCurrentScene || (!pCurrentScene && pNextScene);
	}

	void set_next_scene(Scene *const pNextSceneParam) {
		if (!pNextSceneParam)
			return;
		else if (!pNextSceneParam->u32Id) {
			RE_ERROR("A scene has been set for becoming the next one, but its ID is zero and therefore has been discarded");
			return;
		}
		PRINT_DEBUG("Setting next scene ", pNextSceneParam);
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
		return pCurrentScene ? pCurrentScene->u32Id : 0;
	}

	[[nodiscard]]
	bool is_scene_current(const uint32_t u32SceneId) {
		return u32SceneId && get_current_scene_id() == u32SceneId;
	}

	[[nodiscard]]
	Scene* get_next_scene() {
		return pNextScene;
	}

	[[nodiscard]]
	uint32_t get_next_scene_id() {
		Scene *const pNextScene = get_next_scene();
		return pNextScene ? pNextScene->u32Id : 0;
	}

	[[nodiscard]]
	bool is_scene_next(const uint32_t u32SceneId) {
		return u32SceneId && get_next_scene_id() == u32SceneId;
	}

}
