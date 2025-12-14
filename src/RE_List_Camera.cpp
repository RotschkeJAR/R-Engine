#include "RE_List_Camera.hpp"
#include "RE_ListBatch_Camera.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	std::deque<ListBatch_Camera*> cameraBatchList;
	std::deque<Camera*> newCameras, deletableCameras;
	bool bDeletingMarkedCameras = false;

	void add_new_cameras() {
		if (newCameras.empty())
			return;
		for (Camera *const pCam : newCameras) {
			PRINT_DEBUG("Adding queued new camera ", pCam);
			add_camera(pCam);
		}
		PRINT_DEBUG("Clearing queue of new cameras");
		newCameras.clear();
	}

	void delete_marked_cameras() {
		if (deletableCameras.empty())
			return;
		bDeletingMarkedCameras = true;
		for (Camera *const pCam : deletableCameras) {
			PRINT_DEBUG("Deleting queued camera ", pCam);
			delete pCam;
		}
		PRINT_DEBUG("Clearing queue of deletable cameras");
		deletableCameras.clear();
		bDeletingMarkedCameras = false;
	}

	void add_camera(Camera *const pCamera) {
		PRINT_DEBUG("Adding camera ", pCamera, " to a batch");
		for (ListBatch_Camera *const pBatch : cameraBatchList) {
			PRINT_DEBUG("Checking camera batch ", pBatch, " for available space");
			if (pBatch->has_space()) {
				PRINT_DEBUG("Batch has space. Adding camera to it");
				pBatch->add(pCamera);
				break;
			}
		}
		PRINT_DEBUG("Adding camera to a new batch");
		ListBatch_Camera *const pNewBatch = new ListBatch_Camera();
		pNewBatch->add(pCamera);
		cameraBatchList.push_back(pNewBatch);
	}

	void remove_camera(const Camera *const pCamera) {
		for (ListBatch_Camera *const pBatch : cameraBatchList) {
			PRINT_DEBUG("Searching for camera ", pCamera, " in batch ", pBatch, " to delete it");
			if (pBatch->remove(pCamera)) {
				PRINT_DEBUG("Removed camera from batch");
				if (pBatch->empty()) {
					PRINT_DEBUG("Batch is empty and will be deleted");
					cameraBatchList.erase(std::find(cameraBatchList.begin(), cameraBatchList.end(), pBatch));
					delete pBatch;
				}
				return;
			}
		}
		RE_ERROR("Camera ", pCamera, " had to be removed from a batch, but was nowhere found");
	}

	void mark_camera_deletable(Camera *const pCamera) {
		if (!bRunning || bDeletingMarkedCameras) {
			PRINT_DEBUG("Deleting camera ", pCamera, " immediatly");
			delete pCamera;
		} else {
			PRINT_DEBUG("Searching for camera ", pCamera, " in new-queue");
			std::deque<Camera*>::iterator it = std::find(newCameras.begin(), newCameras.end(), pCamera);
			if (it != newCameras.end()) {
				PRINT_DEBUG("Camera is new and is being deleted immediatly");
				newCameras.erase(it);
				delete pCamera;
			} else {
				PRINT_DEBUG("Enqueued camera in delete-queue");
				deletableCameras.push_back(pCamera);
			}
		}
	}

	void start_cameras() {
		PRINT_DEBUG("Starting cameras");
	}

	void update_cameras() {
		PRINT_DEBUG("Updating cameras");
	}

	void end_cameras() {
		PRINT_DEBUG("Ending cameras");
	}

}
