#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_List_Camera.hpp"

namespace RE {

	Scene *pCurrentScene = nullptr,
		*pNextScene = nullptr;

	static void update_proc() {
		PRINT_DEBUG("Invoking update-procedure");
		set_calling_active<CALLING_PHASE_UPDATE, CALLING_OBJECT_SCENE>();
		pCurrentScene->update();
		update_game_objects();
		update_cameras();
	}

	static void end_proc() {
		PRINT_DEBUG("Invoking end-procedure");
		set_calling_active<CALLING_PHASE_END, CALLING_OBJECT_SCENE>();
		pCurrentScene->end();
		end_game_objects();
		end_cameras();
	}

	static void delete_and_add_proc() {
		PRINT_DEBUG("Invoking delete-procedure");
		set_calling_active<CALLING_PHASE_DELETING, CALLING_OBJECT_NONE>();
		while (u32NewGameObjectCount
				|| u32DeletableGameObjectCount
				|| u8NewCameraCount
				|| u8DeletableCameraCount) {
			delete_and_add_game_objects();
			delete_and_add_cameras();
		}
	}

	bool is_object_active(const GameObject &rGameObject) {
		return !rGameObject.u32SceneParentId || rGameObject.u32SceneParentId == pCurrentScene->u32Id;
	}

	void game_logic_update() {
		PRINT_DEBUG("Updating game's logic");
		if (pNextScene != pCurrentScene && pNextScene) {
			PRINT_DEBUG("Ending current scene ", pCurrentScene);
			if (pCurrentScene)
				end_proc();
			delete_and_add_proc();

			PRINT_DEBUG("Switching to and starting new scene ", pNextScene);
			pCurrentScene = pNextScene;
			set_calling_active<CALLING_PHASE_START, CALLING_OBJECT_SCENE>();
			pCurrentScene->start();
			start_game_objects();
			start_cameras();
			PRINT_DEBUG("Finished switching to new scene");
		} else if (!pCurrentScene) {
			RE_FATAL_ERROR("There is no active scene at the moment");
			return;
		}
		update_proc();
		delete_and_add_proc();
	}

	void last_game_logic_update() {
		PRINT_DEBUG("Ending scene");
		if (pCurrentScene)
			end_proc();
		delete_and_add_proc();
		pCurrentScene = nullptr;
		pNextScene = nullptr;
		PRINT_DEBUG("Finished ending scene");
	}

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
