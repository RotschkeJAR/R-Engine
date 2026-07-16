#include "RE_Manager_Internal.hpp"
#include "RE_Main.hpp"

namespace RE {

#define GAME_OBJECT_MAX_COUNT                    0x100000000
#define GAME_OBJECT_BATCH_SIZE                   4096
#define GAME_OBJECT_MULTITHREADING_THRESHOLD     1

	typedef ListBatch<GameObject*, GAME_OBJECT_BATCH_SIZE> GameObjectListBatch;

	static GameObjectListBatch *pGameObjects = nullptr,
		*pNewGameObjects = nullptr,
		*pDeletableGameObjects = nullptr;
	static GameObjectListBatch *apGameObjectListBatches[GAME_OBJECT_MAX_COUNT / GAME_OBJECT_BATCH_SIZE + bool_to_int(is_multiple_of<uint64_t>(GAME_OBJECT_MAX_COUNT, GAME_OBJECT_BATCH_SIZE))];
	static uint32_t u32GameObjectListBatchCount = 0;

	static_assert(sizeof(apGameObjectListBatches) / sizeof(apGameObjectListBatches[0]) <= std::numeric_limits<decltype(u32GameObjectListBatchCount)>::max());

#define GAME_OBJECT_CALL_PROC(CALL_FUNC) \
		MANAGER_CALL_PROC(GameObject*, GAME_OBJECT_BATCH_SIZE, pGameObjects, apGameObjectListBatches, u32GameObjectListBatchCount, GAME_OBJECT_MULTITHREADING_THRESHOLD, CALL_FUNC)

	bool init_manager_game_objects() {
		return true;
	}

	void destroy_manager_game_objects() {
	}
	
	void delete_and_add_game_objects() {
		SET_CALLING_ACTIVE(CALLING_PHASE_DELETING, CALLING_OBJECT_GAME_OBJECT);
		auto xCurrentBatch = pDeletableGameObjects;
		while (xCurrentBatch) {
			for (uint32_t u32ListIndex = 0; u32ListIndex < xCurrentBatch->u32Count; u32ListIndex++) {
				GameObject *const pDeletableGameObject = xCurrentBatch->aList[u32ListIndex];
				PRINT_DEBUG("Deleting game object ", pDeletableGameObject);
				delete pDeletableGameObject;
			}
			if (!xCurrentBatch->pNext) {
				pDeletableGameObjects = xCurrentBatch;
				break;
			}
			auto xOldBatch = xCurrentBatch;
			PRINT_DEBUG("Deleting empty list batch ", xOldBatch);
			xCurrentBatch = xCurrentBatch->pNext;
			pDeletableGameObjects = xCurrentBatch;
			delete xOldBatch;
		}
		SET_CALLING_PHASE_ACTIVE(CALLING_PHASE_ADDING);
		xCurrentBatch = pNewGameObjects;
		while (xCurrentBatch) {
			for (uint32_t u32ListIndex = 0; u32ListIndex < xCurrentBatch->u32Count; u32ListIndex++) {
				GameObject *const pNewGameObject = xCurrentBatch->aList[u32ListIndex];
				PRINT_DEBUG("Adding new game object ", pNewGameObject);
				if (pGameObjects->u32Count == GAME_OBJECT_BATCH_SIZE) {
					PRINT_DEBUG("Allocating new list batch for game objects");
					GameObjectListBatch *const pNewBatch = new GameObjectListBatch;
					pNewBatch->aList[0] = pNewGameObject;
					pNewBatch->pNext = pGameObjects;
					pNewBatch->u32Count = 1;
					pGameObjects = pNewBatch;
					apGameObjectListBatches[u32GameObjectListBatchCount] = pGameObjects;
				} else {
					pGameObjects->aList[pGameObjects->u32Count] = pNewGameObject;
					pGameObjects->u32Count++;
				}
				pNewGameObject->u64ListIndex = (pGameObjects->u32Count - 1) + static_cast<uint64_t>(u32GameObjectListBatchCount - 1) * GAME_OBJECT_BATCH_SIZE;
				pNewGameObject->bNew = false;
			}
			if (!xCurrentBatch->pNext) {
				pNewGameObjects = xCurrentBatch;
				break;
			}
			PRINT_DEBUG("Deleting empty list batch");
			auto xOldBatch = xCurrentBatch;
			xCurrentBatch = xCurrentBatch->pNext;
			delete xOldBatch;
		}
	}

	void start_game_objects() {
		SET_CALLING_ACTIVE(CALLING_PHASE_START, CALLING_OBJECT_GAME_OBJECT);
		PRINT_DEBUG("Starting game objects");
		GAME_OBJECT_CALL_PROC(start);
	}

	void update_game_objects() {
		SET_CALLING_ACTIVE(CALLING_PHASE_UPDATE, CALLING_OBJECT_GAME_OBJECT);
		PRINT_DEBUG("Updating game objects");
		GAME_OBJECT_CALL_PROC(update);
	}

	void end_game_objects() {
		SET_CALLING_ACTIVE(CALLING_PHASE_END, CALLING_OBJECT_GAME_OBJECT);
		PRINT_DEBUG("Ending game objects");
		GAME_OBJECT_CALL_PROC(end);
	}

	void add_game_object(GameObject *const pNewGameObject) {
	#ifndef NDEBUG
		if (get_current_game_object_count() == GAME_OBJECT_MAX_COUNT)
			RE_ABORT("The maximum amount of game objects (", GAME_OBJECT_MAX_COUNT, " has been surpassed");
	#endif
		if (IS_CALLING_OBJECT_ACTIVE(CALLING_OBJECT_GAME_OBJECT)) {
			switch (GET_ACTIVE_CALLING_PHASE()) {
				case CALLING_PHASE_START:
				case CALLING_PHASE_UPDATE:
				case CALLING_PHASE_END:
					PRINT_DEBUG("Enqueuing new game object ", pNewGameObject);
					if (!pNewGameObjects) {
						pNewGameObjects = new GameObjectListBatch;
						pNewGameObjects->pNext = nullptr;
						pNewGameObjects->u32Count = 0;
					} else if (pNewGameObjects->u32Count == GAME_OBJECT_BATCH_SIZE) {
						PRINT_DEBUG("Allocating new list batch for new game objects");
						auto xNewBatch = new GameObjectListBatch;
						xNewBatch->pNext = pNewGameObjects;
						xNewBatch->u32Count = 0;
						pNewGameObjects = xNewBatch;
					}
					pNewGameObjects->aList[pNewGameObjects->u32Count] = pNewGameObject;
					pNewGameObjects->u32Count++;
					pNewGameObject->bNew = true;
					return;
				default:
					break;
			}
		}
		PRINT_DEBUG("Adding new game object ", pNewGameObject, " without enqueuing");
		if (!pGameObjects) {
			pGameObjects = new GameObjectListBatch;
			pGameObjects->pNext = nullptr;
			pGameObjects->u32Count = 0;
			apGameObjectListBatches[0] = pGameObjects;
			u32GameObjectListBatchCount = 1;
		} else if (pGameObjects->u32Count == GAME_OBJECT_BATCH_SIZE) {
			PRINT_DEBUG("Allocating new list batch for game objects");
			auto xNewBatch = new GameObjectListBatch;
			xNewBatch->pNext = pGameObjects;
			xNewBatch->u32Count = 0;
			pGameObjects = xNewBatch;
			apGameObjectListBatches[u32GameObjectListBatchCount] = xNewBatch;
			u32GameObjectListBatchCount++;
		}
		pGameObjects->aList[pGameObjects->u32Count] = pNewGameObject;
		pGameObjects->u32Count++;
		pNewGameObject->u64ListIndex = (pGameObjects->u32Count - 1) + static_cast<uint64_t>(u32GameObjectListBatchCount - 1) * GAME_OBJECT_BATCH_SIZE;
		pNewGameObject->bNew = false;
	}

	void remove_game_object(GameObject *const pRemovableGameObject) {
		if (pRemovableGameObject->bNew) {
			PRINT_DEBUG("Removing game object ", pRemovableGameObject, " from list containing new game objects");
			auto xCurrentNewBatch = pNewGameObjects;
			while (xCurrentNewBatch) {
				auto xIt = std::find(std::begin(xCurrentNewBatch->aList), std::end(xCurrentNewBatch->aList), pRemovableGameObject);
				if (xIt != std::end(xCurrentNewBatch->aList)) {
					const uint32_t u32NewListIndex = xIt - std::begin(xCurrentNewBatch->aList);
					xCurrentNewBatch->aList[u32NewListIndex] = pNewGameObjects->aList[pNewGameObjects->u32Count - 1];
					pNewGameObjects->u32Count--;
					if (pNewGameObjects->u32Count == 0 && pNewGameObjects->pNext) {
						PRINT_DEBUG("Deleting empty list batch");
						auto xDeletableNewBatch = pNewGameObjects;
						pNewGameObjects = pNewGameObjects->pNext;
						delete xDeletableNewBatch;
					}
					break;
				}
				xCurrentNewBatch = xCurrentNewBatch->pNext;
			}
		} else {
			GameObject **const ppRemovableGameObject = std::addressof(apGameObjectListBatches[pRemovableGameObject->u64ListIndex / GAME_OBJECT_BATCH_SIZE]->aList[pRemovableGameObject->u64ListIndex % GAME_OBJECT_BATCH_SIZE]);
			switch (GET_ACTIVE_CALLING_PHASE()) {
				case CALLING_PHASE_START:
				case CALLING_PHASE_UPDATE:
				case CALLING_PHASE_END:
				#ifndef NDEBUG
					if (IS_CALLING_OBJECT_ACTIVE(CALLING_OBJECT_GAME_OBJECT))
						RE_ABORT("Game object ", pRemovableGameObject, " has to be removed, while the list of game objects is being accessed");
				#endif
					[[fallthrough]];
				case CALLING_PHASE_DELETING:
					if (pNewGameObjects && pNewGameObjects->u32Count) {
						PRINT_DEBUG("Removing game object ", pRemovableGameObject, " and replacing it with a new one");
						GameObject *const pMovedNewGameObject = pNewGameObjects->aList[pNewGameObjects->u32Count - 1];
						*ppRemovableGameObject = pMovedNewGameObject;
						pMovedNewGameObject->u64ListIndex = pRemovableGameObject->u64ListIndex;
						pMovedNewGameObject->bNew = false;
						pNewGameObjects->u32Count--;
						if (pNewGameObjects->u32Count == 0 && pNewGameObjects->pNext) {
							PRINT_DEBUG("Deleting empty list batch");
							auto xDeletableNewBatch = pNewGameObjects;
							pNewGameObjects = pNewGameObjects->pNext;
							delete xDeletableNewBatch;
						}
						break;
					}
					[[fallthrough]];
				default:
					PRINT_DEBUG("Removing game object ", pRemovableGameObject);
					{
						GameObject *const pMovedGameObject = pGameObjects->aList[pGameObjects->u32Count - 1];
						*ppRemovableGameObject = pMovedGameObject;
						pMovedGameObject->u64ListIndex = pRemovableGameObject->u64ListIndex;
					}
					pGameObjects->u32Count--;
					if (pGameObjects->u32Count == 0 && pGameObjects->pNext) {
						PRINT_DEBUG("Deleting empty list batch");
						auto xDeletableBatch = pGameObjects;
						pGameObjects = pGameObjects->pNext;
						delete xDeletableBatch;
					}
					break;
			}
		}
	}

	void mark_game_object_deletable(GameObject *const pDeletableGameObject) {
		switch (GET_ACTIVE_CALLING_PHASE()) {
			case CALLING_PHASE_START:
			case CALLING_PHASE_UPDATE:
			case CALLING_PHASE_END:
				if (IS_CALLING_OBJECT_ACTIVE(CALLING_OBJECT_GAME_OBJECT)) {
					PRINT_DEBUG("Enqueuing game object ", pDeletableGameObject, " for deletion");
					if (!pDeletableGameObjects) {
						pDeletableGameObjects = new GameObjectListBatch;
						pDeletableGameObjects->u32Count = 0;
						pDeletableGameObjects->pNext = nullptr;
					} else if (pDeletableGameObjects->u32Count == GAME_OBJECT_BATCH_SIZE) {
						PRINT_DEBUG("Allocating new list batch for deletable game objects");
						GameObjectListBatch *const pNewBatch = new GameObjectListBatch;
						pNewBatch->u32Count = 0;
						pNewBatch->pNext = pDeletableGameObjects;
						pDeletableGameObjects = pNewBatch;
					}
					pDeletableGameObjects->aList[pDeletableGameObjects->u32Count] = pDeletableGameObject;
					pDeletableGameObjects->u32Count++;
					break;
				}
				[[fallthrough]];
			default:
				PRINT_DEBUG("Deleting game object ", pDeletableGameObject, " immediatly");
				delete pDeletableGameObject;
				break;
		}
	}

	uint64_t get_current_game_object_count() {
		PRINT_DEBUG("Calculating current count of game objects");
		uint64_t u64GameObjectCount = static_cast<uint64_t>(u32GameObjectListBatchCount - 1) * GAME_OBJECT_BATCH_SIZE;
		if (pGameObjects)
			u64GameObjectCount -= static_cast<uint64_t>(GAME_OBJECT_BATCH_SIZE - pGameObjects->u32Count);
		const GameObjectListBatch *pCurrentNewBatch = const_cast<const GameObjectListBatch*>(pNewGameObjects);
		while (pCurrentNewBatch) {
			u64GameObjectCount += static_cast<uint64_t>(pCurrentNewBatch->u32Count);
			pCurrentNewBatch = pCurrentNewBatch->pNext;
		}
		return u64GameObjectCount;
	}

	uint64_t get_max_game_object_count() {
		return static_cast<uint64_t>(GAME_OBJECT_MAX_COUNT);
	}

}
