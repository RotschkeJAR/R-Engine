#include "RE_List_GameObject.hpp"
#include "RE_Manager.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	std::deque<GameObject*> newGameObjects, deletableGameObjects;
	std::deque<ListBatch_GameObject*> gameObjectBatchList;
	bool bDeletingMarkedGameObjects = false;

	void add_new_game_objects() {
		for (GameObject *const pObject : newGameObjects)
			PUSH_TO_CALLSTACKTRACE(add_game_object(pObject));
		newGameObjects.clear();
	}

	void delete_marked_game_objects() {
		bDeletingMarkedGameObjects = true;
		for (GameObject *const pObject : deletableGameObjects)
			PUSH_TO_CALLSTACKTRACE_DETAILED(delete pObject, append_to_string("GameObject: ", pObject).c_str());
		deletableGameObjects.clear();
		bDeletingMarkedGameObjects = false;
	}

	void add_game_object(GameObject *const pGameObject) {
		size_t batchIndex = 0;
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			if (pBatch->has_space()) {
				PUSH_TO_CALLSTACKTRACE(pBatch->add(pGameObject));
				break;
			}
			batchIndex++;
		}
		if (batchIndex == gameObjectBatchList.size()) {
			ListBatch_GameObject *const pNewBatch = PUSH_TO_CALLSTACKTRACE_AND_RETURN(new ListBatch_GameObject(), ListBatch_GameObject*);
			PUSH_TO_CALLSTACKTRACE(pNewBatch->add(pGameObject));
			gameObjectBatchList.push_back(pNewBatch);
		}
	}
	
	void remove_game_object(const GameObject *const pGameObject) {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(pBatch->remove(pGameObject), bool)) {
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
			std::deque<GameObject*>::iterator it = std::find(newGameObjects.begin(), newGameObjects.end(), pGameObject);
			if (it != newGameObjects.end()) {
				newGameObjects.erase(it);
				delete pGameObject;
			} else
				deletableGameObjects.push_back(pGameObject);
		}
	}

	void start_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			PUSH_TO_CALLSTACKTRACE(pBatch->start());
	}

	void update_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			PUSH_TO_CALLSTACKTRACE(pBatch->update());
	}

	void end_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			PUSH_TO_CALLSTACKTRACE(pBatch->end());
	}

	bool init_game_object_render_batches() {
		size_t batchesInitializedCount = 0;
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			if (!PUSH_TO_CALLSTACKTRACE_AND_RETURN(pBatch->renderBatch.init(), bool))
				break;
			else
				batchesInitializedCount++;
		if (batchesInitializedCount < gameObjectBatchList.size()) {
			for (size_t batchesDestroyedCount = 0; batchesDestroyedCount < batchesInitializedCount; batchesDestroyedCount++)
				PUSH_TO_CALLSTACKTRACE(gameObjectBatchList[batchesDestroyedCount]->renderBatch.destroy());
			return false;
		}
		return true;
	}

	void destroy_game_object_render_batches() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			PUSH_TO_CALLSTACKTRACE(pBatch->renderBatch.destroy());
	}

	void load_game_object_vertices(bool &rbNeedsRender) {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			PUSH_TO_CALLSTACKTRACE(pBatch->renderBatch.load_vertices(rbNeedsRender));
	}

	void render_opaque_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			PUSH_TO_CALLSTACKTRACE(pBatch->renderBatch.render_opaque());
	}

	void render_transparent_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			PUSH_TO_CALLSTACKTRACE(pBatch->renderBatch.render_transparent());
	}

}
