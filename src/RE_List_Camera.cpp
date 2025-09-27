#include "RE_List_Camera.hpp"
#include "RE_ListBatch_Camera.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	std::deque<ListBatch_Camera*> cameraBatchList;
	std::deque<Camera*> newCameras, deletableCameras;
	bool bDeletingMarkedCameras = false;

	void add_new_cameras() {
		for (Camera *const pCam : newCameras)
			add_camera(pCam);
		newCameras.clear();
	}

	void delete_marked_cameras() {
		bDeletingMarkedCameras = true;
		for (Camera *pCam : deletableCameras)
			delete pCam;
		deletableCameras.clear();
		bDeletingMarkedCameras = false;
	}

	void add_camera(Camera *const pCamera) {
		size_t batchIndex = 0;
		for (ListBatch_Camera *const pBatch : cameraBatchList) {
			if (pBatch->has_space()) {
				pBatch->add(pCamera);
				break;
			}
			batchIndex++;
		}
		if (batchIndex == cameraBatchList.size()) {
			ListBatch_Camera *const pNewBatch = new ListBatch_Camera();
			pNewBatch->add(pCamera);
			cameraBatchList.push_back(pNewBatch);
		}
	}

	void remove_camera(const Camera *const pCamera) {
		for (ListBatch_Camera *const pBatch : cameraBatchList)
			if (pBatch->remove(pCamera))
				break;
	}

	void mark_camera_deletable(Camera *const pCamera) {
		if (!bRunning || bDeletingMarkedCameras)
			delete pCamera;
		else {
			std::deque<Camera*>::iterator it = std::find(newCameras.begin(), newCameras.end(), pCamera);
			if (it != newCameras.end()) {
				newCameras.erase(it);
				delete pCamera;
			} else
				deletableCameras.push_back(pCamera);
		}
	}

	void start_cameras() {
	}

	void update_cameras() {
		for (ListBatch_Camera *const pBatch : cameraBatchList)
			pBatch->update();
	}

	void end_cameras() {
	}

}
