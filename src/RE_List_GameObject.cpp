#include "RE_List_GameObject.hpp"
#include "RE_Manager.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	std::vector<GameObject*> newGameObjects, deletableGameObjects;
	std::list<ListBatch_GameObject*> gameObjectBatchList;
	bool bDeletingMarkedGameObjects = false;

	void add_new_game_objects() {
		for (GameObject *const pObject : newGameObjects)
			CATCH_SIGNAL(add_game_object(pObject));
		newGameObjects.clear();
	}

	void delete_marked_game_objects() {
		bDeletingMarkedGameObjects = true;
		for (GameObject *const pObject : deletableGameObjects)
			CATCH_SIGNAL_DETAILED(delete pObject, append_to_string("GameObject: ", pObject).c_str());
		deletableGameObjects.clear();
		bDeletingMarkedGameObjects = false;
	}

	void add_game_object(GameObject *const pGameObject) {
		size_t batchIndex = 0U;
		for (ListBatch_GameObject *pBatch : gameObjectBatchList) {
			if (pBatch->has_space()) {
				CATCH_SIGNAL_DETAILED(pBatch->add(pGameObject), append_to_string("Batch ", pBatch, " at index ", batchIndex).c_str());
				break;
			}
			batchIndex++;
		}
		if (batchIndex == gameObjectBatchList.size()) {
			ListBatch_GameObject *pNewBatch = CATCH_SIGNAL_AND_RETURN(new ListBatch_GameObject(), ListBatch_GameObject*);
			CATCH_SIGNAL(pNewBatch->add(pGameObject));
			gameObjectBatchList.push_back(pNewBatch);
		}
	}
	
	void remove_game_object(const GameObject *const pGameObject) {
		for (ListBatch_GameObject *pBatch : gameObjectBatchList)
			if (CATCH_SIGNAL_AND_RETURN(pBatch->remove(pGameObject), bool)) {
				if (pBatch->empty()) {
					gameObjectBatchList.erase(std::find(gameObjectBatchList.begin(), gameObjectBatchList.end(), pBatch));
					delete pBatch;
				}
				break;
			}
	}

	void mark_game_object_deletable(GameObject *const pGameObject) {
		if (!bRunning || bDeletingMarkedGameObjects)
			delete pGameObject;
		else {
			std::vector<GameObject*>::iterator it = std::find(newGameObjects.begin(), newGameObjects.end(), pGameObject);
			if (it != newGameObjects.end()) {
				newGameObjects.erase(it);
				delete pGameObject;
			} else
				deletableGameObjects.push_back(pGameObject);
		}
	}

	void start_game_objects() {
		for (ListBatch_GameObject *pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->start());
	}

	void update_game_objects() {
		for (ListBatch_GameObject *pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->update());
	}

	void end_game_objects() {
		for (ListBatch_GameObject *pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->end());
	}

	bool init_game_object_render_batches() {
		size_t batchesInitializedCount = 0U;
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			if (!CATCH_SIGNAL_AND_RETURN(pBatch->renderBatch.init(), bool))
				break;
			else
				batchesInitializedCount++;
		if (batchesInitializedCount < gameObjectBatchList.size()) {
			size_t batchesDestroyedCount = 0U;
			for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
				if (batchesDestroyedCount < batchesInitializedCount)
					CATCH_SIGNAL(pBatch->renderBatch.destroy());
				else
					break;
				batchesDestroyedCount++;
			}
			return false;
		}
		return true;
	}

	void destroy_game_object_render_batches() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->renderBatch.destroy());
	}

	void render_opaque_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->renderBatch.render_opaque());
	}

	void render_transparent_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->renderBatch.render_transparent());
	}

}
