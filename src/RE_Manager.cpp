#include "RE_Manager_Internal.hpp"
#include "RE_Main.hpp"

namespace RE {

	CallingFlags_t callingPhaseObject = 0;
	Scene *pCurrentScene = nullptr,
		*pNextScene = nullptr;

	static void update_proc() {
		PRINT_DEBUG("Updating current scene ", pCurrentScene);
		SET_CALLING_ACTIVE(CALLING_PHASE_UPDATE, CALLING_OBJECT_SCENE);
		pCurrentScene->update();
		update_game_objects();
		update_cameras();
	}

	static void end_proc() {
		PRINT_DEBUG("Ending current scene ", pCurrentScene);
		SET_CALLING_ACTIVE(CALLING_PHASE_END, CALLING_OBJECT_SCENE);
		pCurrentScene->end();
		end_game_objects();
		end_cameras();
	}

	static void delete_and_add_proc() {
		while (false) {
			delete_and_add_game_objects();
			delete_and_add_cameras();
		}
	}

	bool is_object_active(const GameObject &rGameObject) {
		return !rGameObject.u32SceneParentId || rGameObject.u32SceneParentId == pCurrentScene->u32Id;
	}

	bool init_manager() {
		if (init_manager_game_objects()) {
			if (init_manager_cameras()) {
				return true;
			}
			destroy_manager_game_objects();
		}
		return false;
	}

	void destroy_manager() {
		destroy_manager_cameras();
		destroy_manager_game_objects();
	}

	void game_logic_update() {
		if (pNextScene != pCurrentScene && pNextScene) {
			if (pCurrentScene)
				end_proc();
			delete_and_add_proc();

			PRINT_DEBUG("Switching to and starting new scene ", pNextScene);
			pCurrentScene = pNextScene;
			SET_CALLING_ACTIVE(CALLING_PHASE_START, CALLING_OBJECT_SCENE);
			pCurrentScene->start();
			start_game_objects();
			start_cameras();
		} else if (!pCurrentScene) {
			RE_FATAL_ERROR("There is no active scene at the moment");
			return;
		}
		update_proc();
		delete_and_add_proc();
		SET_CALLING_INACTIVE();
	}

	void last_game_logic_update() {
		if (pCurrentScene)
			end_proc();
		delete_and_add_proc();
		SET_CALLING_INACTIVE();
		pCurrentScene = nullptr;
		pNextScene = nullptr;
	}

	bool are_scenes_present() {
		return pCurrentScene || (!pCurrentScene && pNextScene);
	}

	void set_next_scene(Scene *pNextSceneParam) {
		if (!pNextSceneParam)
			return;
		else if (!pNextSceneParam->u32Id) {
			RE_ERROR("A scene has been set for becoming the next one, but its ID is zero and therefore has been discarded");
			return;
		}
		PRINT_DEBUG("Setting next scene ", pNextSceneParam);
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
		return pCurrentScene ? pCurrentScene->u32Id : 0;
	}

	bool is_scene_current(uint32_t u32SceneId) {
		return u32SceneId && get_current_scene_id() == u32SceneId;
	}

	Scene* get_next_scene() {
		return pNextScene;
	}

	uint32_t get_next_scene_id() {
		Scene *const pNextScene = get_next_scene();
		return pNextScene ? pNextScene->u32Id : 0;
	}

	bool is_scene_next(uint32_t u32SceneId) {
		return u32SceneId && get_next_scene_id() == u32SceneId;
	}

}
