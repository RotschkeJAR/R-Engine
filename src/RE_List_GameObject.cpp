#include "RE_List_GameObject.hpp"
#include "RE_Manager.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	std::deque<GameObject*> newGameObjects, deletableGameObjects;
	std::deque<ListBatch_GameObject*> gameObjectBatchList;
	bool bDeletingMarkedGameObjects = false;

	void add_new_game_objects() {
		if (newGameObjects.empty())
			return;
		for (GameObject *const pObject : newGameObjects) {
			PRINT_DEBUG("Adding queued new game object ", pObject);
			add_game_object(pObject);
		}
		PRINT_DEBUG("Clearing queue of new game objects");
		newGameObjects.clear();
	}

	void delete_marked_game_objects() {
		if (deletableGameObjects.empty())
			return;
		bDeletingMarkedGameObjects = true;
		for (GameObject *const pObject : deletableGameObjects) {
			PRINT_DEBUG("Deleting game object ", pObject);
			delete pObject;
		}
		PRINT_DEBUG("Clearing queue of deletable game objects");
		deletableGameObjects.clear();
		bDeletingMarkedGameObjects = false;
	}

	void add_game_object(GameObject *const pGameObject) {
		PRINT_DEBUG("Adding game object ", pGameObject, " to a batch");
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			PRINT_DEBUG("Checking batch ", pBatch, " for available space");
			if (pBatch->has_space()) {
				PRINT_DEBUG("Batch has space. Adding game object to it");
				pBatch->add(pGameObject);
				return;
			}
		}
		PRINT_DEBUG("Adding game object to a new batch");
		ListBatch_GameObject *const pNewBatch = new ListBatch_GameObject();
		pNewBatch->add(pGameObject);
		gameObjectBatchList.push_back(pNewBatch);
	}
	
	void remove_game_object(const GameObject *const pGameObject) {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			PRINT_DEBUG("Searching for game object ", pGameObject, " in batch ", pBatch, " to delete it");
			if (pBatch->remove(pGameObject)) {
				PRINT_DEBUG("Removed game object from batch");
				if (pBatch->empty()) {
					PRINT_DEBUG("Batch is empty and will be deleted");
					gameObjectBatchList.erase(std::find(gameObjectBatchList.begin(), gameObjectBatchList.end(), pBatch));
					delete pBatch;
				}
				return;
			}
		}
		RE_ERROR("Game object ", pGameObject, " had to be removed from a batch, but was nowhere found");
	}

	void mark_game_object_deletable(GameObject *const pGameObject) {
		if (!bRunning || bDeletingMarkedGameObjects) {
			PRINT_DEBUG("Deleting game object ", pGameObject, " immediatly");
			delete pGameObject;
		} else {
			PRINT_DEBUG("Searching for game object ", pGameObject, " in new-queue");
			std::deque<GameObject*>::iterator it = std::find(newGameObjects.begin(), newGameObjects.end(), pGameObject);
			if (it != newGameObjects.end()) {
				PRINT_DEBUG("Game object is new and is being deleted immediatly");
				newGameObjects.erase(it);
				delete pGameObject;
			} else {
				PRINT_DEBUG("Enqueued game object in delete-queue");
				deletableGameObjects.push_back(pGameObject);
			}
		}
	}

	void start_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			PRINT_DEBUG("Starting game objects in batch ", pBatch);
			pBatch->start();
		}
	}

	void update_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			PRINT_DEBUG("Updating game objects in batch ", pBatch);
			pBatch->update();
		}
	}

	void end_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			PRINT_DEBUG("Ending game objects in batch ", pBatch);
			pBatch->end();
		}
	}

	bool init_game_object_render_batches() {
		size_t batchesInitializedCount = 0;
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			PRINT_DEBUG("Initializing game object batch ", pBatch, " for rendering");
			if (!pBatch->renderBatch.init())
				break;
			else
				batchesInitializedCount++;
		}
		if (batchesInitializedCount < gameObjectBatchList.size()) {
			for (size_t batchesDestroyedCount = 0; batchesDestroyedCount < batchesInitializedCount; batchesDestroyedCount++) {
				PRINT_DEBUG("Destroying game object batch ", gameObjectBatchList[batchesDestroyedCount], " at index ", batchesDestroyedCount, " for rendering due to failure initializing another batch");
				gameObjectBatchList[batchesDestroyedCount]->renderBatch.destroy();
			}
			return false;
		}
		return true;
	}

	void destroy_game_object_render_batches() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			PRINT_DEBUG("Destroying game object batch ", pBatch, " for rendering");
			pBatch->renderBatch.destroy();
		}
	}

	void load_game_object_vertices(bool &rbNeedsRender) {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			PRINT_DEBUG("Loading game object vertices in batch ", pBatch);
			pBatch->renderBatch.load_vertices(rbNeedsRender);
		}
	}

	void render_opaque_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			PRINT_DEBUG("Rendering opaque game objects in batch ", pBatch);
			pBatch->renderBatch.render_opaque();
		}
	}

	void render_transparent_game_objects() {
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			PRINT_DEBUG("Rendering transparent game objects in batch ", pBatch);
			pBatch->renderBatch.render_transparent();
		}
	}

}
