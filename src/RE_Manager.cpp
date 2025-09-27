#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_List_Camera.hpp"
#include "RE_Vulkan_Device.hpp"

namespace RE {

	Scene *pCurrentScene = nullptr, *pNextScene = nullptr;

	static void update_proc() {
		pCurrentScene->update();
		update_game_objects();
		update_cameras();
	}

	static void end_proc() {
		pCurrentScene->end();
		end_game_objects();
		end_cameras();
	}

	static void delete_proc() {
		delete_marked_game_objects();
		delete_marked_cameras();
	}

	static void add_proc() {
		add_new_game_objects();
		add_new_cameras();
	}

	[[nodiscard]]
	bool is_object_active(const GameObject *const pGameObject) {
		return !pGameObject->u32SceneParentId || pGameObject->u32SceneParentId == pCurrentScene->u32Id;
	}

	void game_logic_update() {
		if (pNextScene != pCurrentScene && pNextScene) {
			// End old scene
			WAIT_FOR_IDLE_VULKAN_DEVICE();
			if (pCurrentScene)
				end_proc();
			while (!deletableGameObjects.empty())
				delete_proc();

			pCurrentScene = pNextScene;

			// Start new scene and game objects
			pCurrentScene->start();
			start_game_objects();
			start_cameras();

			// Start newly added objects
			bool bNewObjectsAdded;
			do {
				for (GameObject *const pObj : newGameObjects)
					if (is_object_active(pObj))
						pObj->start(pCurrentScene);
				bNewObjectsAdded = !newGameObjects.empty();
				add_proc();
			} while (bNewObjectsAdded);
		} else if (!pCurrentScene) {
			RE_ERROR("There is no active scene at the moment");
			return;
		}
		update_proc();
		while (!deletableGameObjects.empty())
			delete_proc();
		while (!newGameObjects.empty())
			add_proc();
	}

	void last_game_logic_update() {
		if (pCurrentScene)
			end_proc();
		delete_proc();
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
		else if (!pNextSceneParam->u32Id) {
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
		return pCurrentScene ? pCurrentScene->u32Id : 0;
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
		return pNextScene ? pNextScene->u32Id : 0;
	}

	[[nodiscard]]
	bool is_scene_next(const uint32_t u32SceneId) {
		return get_next_scene() && get_next_scene_id() == u32SceneId;
	}

}
