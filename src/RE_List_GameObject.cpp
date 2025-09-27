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
			add_game_object(pObject);
		newGameObjects.clear();
	}

	void delete_marked_game_objects() {
		bDeletingMarkedGameObjects = true;
		for (GameObject *const pObject : deletableGameObjects)
			delete pObject;
		deletableGameObjects.clear();
		bDeletingMarkedGameObjects = false;
	}

	void add_game_object(GameObject *const pGameObject) {
		size_t batchIndex = 0;
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			if (pBatch->has_space()) {
				pBatch->add(pGameObject);
				break;
			}
			batchIndex++;
		}
		if (batchIndex == gameObjectBatchList.size()) {
			ListBatch_GameObject *const pNewBatch = new ListBatch_GameObject();
			pNewBatch->add(pGameObject);
			gameObjectBatchList.push_back(pNewBatch);
		}
	}
	
	void remove_game_object(const GameObject *const pGameObject) {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			if (pBatch->remove(pGameObject)) {
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
			pBatch->start();
	}

	void update_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			pBatch->update();
	}

	void end_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			pBatch->end();
	}

	bool init_game_object_render_batches() {
		size_t batchesInitializedCount = 0;
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			if (!pBatch->renderBatch.init())
				break;
			else
				batchesInitializedCount++;
		if (batchesInitializedCount < gameObjectBatchList.size()) {
			for (size_t batchesDestroyedCount = 0; batchesDestroyedCount < batchesInitializedCount; batchesDestroyedCount++)
				gameObjectBatchList[batchesDestroyedCount]->renderBatch.destroy();
			return false;
		}
		return true;
	}

	void destroy_game_object_render_batches() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			pBatch->renderBatch.destroy();
	}

	void load_game_object_vertices(bool &rbNeedsRender) {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			pBatch->renderBatch.load_vertices(rbNeedsRender);
	}

	void render_opaque_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			pBatch->renderBatch.render_opaque();
	}

	void render_transparent_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList)
			pBatch->renderBatch.render_transparent();
	}

}
