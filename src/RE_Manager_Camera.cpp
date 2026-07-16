#include "RE_Manager_Internal.hpp"
#include "RE_Main.hpp"

namespace RE {

#define CAMERA_MAX_COUNT                   1024
#define CAMERA_BATCH_SIZE                  256
#define CAMERA_MULTITHREADING_THRESHOLD    4

	typedef ListBatch<Camera*, CAMERA_BATCH_SIZE> CameraListBatch;

	static CameraListBatch *pCameras = nullptr,
		*pNewCameras = nullptr,
		*pDeletableCameras = nullptr;
	static CameraListBatch *apCameraListBatches[CAMERA_MAX_COUNT / CAMERA_BATCH_SIZE + bool_to_int(is_multiple_of<uint64_t>(CAMERA_MAX_COUNT, CAMERA_BATCH_SIZE))];
	static unsigned int uCameraListBatchCount = 0;

	static_assert(sizeof(apCameraListBatches) / sizeof(apCameraListBatches[0]) <= std::numeric_limits<decltype(uCameraListBatchCount)>::max());

#define CAMERA_CALL_PROC(CALL_FUNC) \
		MANAGER_CALL_PROC(Camera*, CAMERA_BATCH_SIZE, pCameras, apCameraListBatches, uCameraListBatchCount, CAMERA_MULTITHREADING_THRESHOLD, CALL_FUNC)

	bool init_manager_cameras() {
		return true;
	}

	void destroy_manager_cameras() {
	}

	void delete_and_add_cameras() {
		SET_CALLING_ACTIVE(CALLING_PHASE_DELETING, CALLING_OBJECT_CAMERA);
		auto xCurrentBatch = pDeletableCameras;
		while (xCurrentBatch) {
			for (uint32_t u32ListIndex = 0; u32ListIndex < xCurrentBatch->u32Count; u32ListIndex++) {
				Camera *const pDeletableCamera = xCurrentBatch->aList[u32ListIndex];
				PRINT_DEBUG("Deleting camera ", pDeletableCamera);
				delete pDeletableCamera;
			}
			if (!xCurrentBatch->pNext) {
				pDeletableCameras = xCurrentBatch;
				break;
			}
			PRINT_DEBUG("Deleting empty list batch");
			auto xOldBatch = xCurrentBatch;
			xCurrentBatch = xCurrentBatch->pNext;
			pDeletableCameras = xCurrentBatch;
			delete xOldBatch;
		}
		SET_CALLING_PHASE_ACTIVE(CALLING_PHASE_ADDING);
		xCurrentBatch = pNewCameras;
		while (xCurrentBatch) {
			for (uint32_t u32ListIndex = 0; u32ListIndex < xCurrentBatch->u32Count; u32ListIndex++) {
				Camera *const pNewCamera = xCurrentBatch->aList[u32ListIndex];
				PRINT_DEBUG("Adding new camera ", pNewCamera);
				if (pCameras->u32Count == CAMERA_BATCH_SIZE) {
					PRINT_DEBUG("Allocating new list batch for cameras");
					CameraListBatch *const pNewBatch = new CameraListBatch;
					pNewBatch->aList[0] = pNewCamera;
					pNewBatch->pNext = pCameras;
					pNewBatch->u32Count = 1;
					pCameras = pNewBatch;
					apCameraListBatches[uCameraListBatchCount] = pCameras;
				} else {
					pCameras->aList[pCameras->u32Count] = pNewCamera;
					pCameras->u32Count++;
				}
				pNewCamera->u64ListIndex = (pCameras->u32Count - 1) + static_cast<uint64_t>(uCameraListBatchCount - 1) * CAMERA_BATCH_SIZE;
				pNewCamera->bNew = false;
			}
			if (!xCurrentBatch->pNext) {
				pNewCameras = xCurrentBatch;
				break;
			}
			PRINT_DEBUG("Deleting empty list batch");
			auto xOldBatch = xCurrentBatch;
			xCurrentBatch = xCurrentBatch->pNext;
			delete xOldBatch;
		}
	}

	void start_cameras() {
		SET_CALLING_ACTIVE(CALLING_PHASE_START, CALLING_OBJECT_CAMERA);
	}

	void update_cameras() {
		SET_CALLING_ACTIVE(CALLING_PHASE_UPDATE, CALLING_OBJECT_CAMERA);
	}

	void end_cameras() {
		SET_CALLING_ACTIVE(CALLING_PHASE_END, CALLING_OBJECT_CAMERA);
	}

	void add_camera(Camera *const pNewCamera) {
	#ifndef NDEBUG
		if (get_current_camera_count() == CAMERA_MAX_COUNT)
			RE_ABORT("The maximum amount of cameras (", CAMERA_MAX_COUNT, " has been surpassed");
	#endif
		if (IS_CALLING_OBJECT_ACTIVE(CALLING_OBJECT_CAMERA)) {
			switch (GET_ACTIVE_CALLING_PHASE()) {
				case CALLING_PHASE_START:
				case CALLING_PHASE_UPDATE:
				case CALLING_PHASE_END:
					PRINT_DEBUG("Enqueuing new camera ", pNewCamera);
					if (!pNewCameras) {
						pNewCameras = new CameraListBatch;
						pNewCameras->pNext = nullptr;
						pNewCameras->u32Count = 0;
					} else if (pNewCameras->u32Count == CAMERA_BATCH_SIZE) {
						PRINT_DEBUG("Allocating new list batch for new cameras");
						auto xNewBatch = new CameraListBatch;
						xNewBatch->pNext = pNewCameras;
						xNewBatch->u32Count = 0;
						pNewCameras = xNewBatch;
					}
					pNewCameras->aList[pNewCameras->u32Count] = pNewCamera;
					pNewCameras->u32Count++;
					pNewCamera->bNew = true;
					return;
				default:
					break;
			}
		}
		if (!pCameras) {
			pCameras = new CameraListBatch;
			pCameras->pNext = nullptr;
			pCameras->u32Count = 0;
			apCameraListBatches[0] = pCameras;
			uCameraListBatchCount = 1;
		} else if (pCameras->u32Count == CAMERA_BATCH_SIZE) {
			PRINT_DEBUG("Allocating new list batch for cameras");
			auto xNewBatch = new CameraListBatch;
			xNewBatch->pNext = pCameras;
			xNewBatch->u32Count = 0;
			pCameras = xNewBatch;
			apCameraListBatches[uCameraListBatchCount] = xNewBatch;
			uCameraListBatchCount++;
		}
		pCameras->aList[pCameras->u32Count] = pNewCamera;
		pCameras->u32Count++;
		pNewCamera->u64ListIndex = (pCameras->u32Count - 1) + static_cast<uint64_t>(uCameraListBatchCount - 1) * CAMERA_BATCH_SIZE;
		pNewCamera->bNew = false;
	}

	void remove_camera(Camera *const pRemovableCamera) {
		if (pRemovableCamera->bNew) {
			PRINT_DEBUG("Removing camera ", pRemovableCamera, " from list containing new cameras");
			auto xCurrentNewBatch = pNewCameras;
			while (xCurrentNewBatch) {
				const auto xIt = std::find(std::begin(xCurrentNewBatch->aList), std::end(xCurrentNewBatch->aList), pRemovableCamera);
				if (xIt != std::end(xCurrentNewBatch->aList)) {
					const uint32_t u32NewListIndex = xIt - std::begin(xCurrentNewBatch->aList);
					xCurrentNewBatch->aList[u32NewListIndex] = pNewCameras->aList[pNewCameras->u32Count - 1];
					pNewCameras->u32Count--;
					if (pNewCameras->u32Count == 0 && pNewCameras->pNext) {
						PRINT_DEBUG("Deleting empty list batch");
						auto xDeletableNewBatch = pNewCameras;
						pNewCameras = pNewCameras->pNext;
						delete xDeletableNewBatch;
					}
					break;
				}
				xCurrentNewBatch = xCurrentNewBatch->pNext;
			}
		} else {
			Camera **const ppRemovableCamera = std::addressof(apCameraListBatches[pRemovableCamera->u64ListIndex / CAMERA_BATCH_SIZE]->aList[pRemovableCamera->u64ListIndex % CAMERA_BATCH_SIZE]);
			switch (GET_ACTIVE_CALLING_PHASE()) {
				case CALLING_PHASE_START:
				case CALLING_PHASE_UPDATE:
				case CALLING_PHASE_END:
				#ifndef NDEBUG
					if (IS_CALLING_OBJECT_ACTIVE(CALLING_OBJECT_CAMERA))
						RE_ABORT("Camera ", pRemovableCamera, " has to be removed, while the list of cameras is being accessed");
				#endif
					[[fallthrough]];
				case CALLING_PHASE_DELETING:
					if (pNewCameras && pNewCameras->u32Count) {
						PRINT_DEBUG("Removing camera ", pRemovableCamera, " and replacing it with a new one");
						Camera *const pMovedCamera = pNewCameras->aList[pNewCameras->u32Count - 1];
						*ppRemovableCamera = pMovedCamera;
						pMovedCamera->u64ListIndex = pRemovableCamera->u64ListIndex;
						pMovedCamera->bNew = false;
						pNewCameras->u32Count--;
						if (pNewCameras->u32Count == 0 && pNewCameras->pNext) {
							PRINT_DEBUG("Deleting empty list batch");
							auto xDeletableNewBatch = pNewCameras;
							pNewCameras = pNewCameras->pNext;
							delete xDeletableNewBatch;
						}
						break;
					}
					[[fallthrough]];
				default:
					PRINT_DEBUG("Removing camera ", pRemovableCamera);
					{
						Camera *const pMovedCamera = pCameras->aList[pCameras->u32Count - 1];
						*ppRemovableCamera = pMovedCamera;
						pMovedCamera->u64ListIndex = pRemovableCamera->u64ListIndex;
					}
					pCameras->u32Count--;
					if (pCameras->u32Count == 0 && pCameras->pNext) {
						PRINT_DEBUG("Deleting empty list batch");
						auto xDeletableBatch = pCameras;
						pCameras = pCameras->pNext;
						delete xDeletableBatch;
					}
					break;
			}
		}
	}

	void mark_camera_deletable(Camera *const pDeletableCamera) {
		switch (GET_ACTIVE_CALLING_PHASE()) {
			case CALLING_PHASE_START:
			case CALLING_PHASE_UPDATE:
			case CALLING_PHASE_END:
				if (IS_CALLING_OBJECT_ACTIVE(CALLING_OBJECT_CAMERA)) {
					PRINT_DEBUG("Enqueuing camera ", pDeletableCamera, " for deletion");
					if (!pDeletableCameras) {
						pDeletableCameras = new CameraListBatch;
						pDeletableCameras->u32Count = 0;
						pDeletableCameras->pNext = nullptr;
					} else if (pDeletableCameras->u32Count == CAMERA_BATCH_SIZE) {
						PRINT_DEBUG("Allocating new list batch for deletable cameras");
						CameraListBatch *const pNewBatch = new CameraListBatch;
						pNewBatch->u32Count = 0;
						pNewBatch->pNext = pDeletableCameras;
						pDeletableCameras = pNewBatch;
					}
					pDeletableCameras->aList[pDeletableCameras->u32Count] = pDeletableCamera;
					pDeletableCameras->u32Count++;
					break;
				}
				[[fallthrough]];
			default:
				PRINT_DEBUG("Deleting camera ", pDeletableCamera, " immediatly");
				delete pDeletableCamera;
				break;
		}
	}

	uint64_t get_current_camera_count() {
		PRINT_DEBUG("Calculating current count of cameras");
		uint64_t u64CameraCount = static_cast<uint64_t>(uCameraListBatchCount - 1) * CAMERA_BATCH_SIZE;
		if (pCameras)
			u64CameraCount -= static_cast<uint64_t>(CAMERA_BATCH_SIZE - pCameras->u32Count);
		const CameraListBatch *pCurrentBatch = pNewCameras;
		while (pCurrentBatch) {
			u64CameraCount += static_cast<uint64_t>(pCurrentBatch->u32Count);
			pCurrentBatch = pCurrentBatch->pNext;
		}
		return u64CameraCount;
	}

	uint64_t get_max_camera_count() {
		return static_cast<uint64_t>(CAMERA_MAX_COUNT);
	}

}
