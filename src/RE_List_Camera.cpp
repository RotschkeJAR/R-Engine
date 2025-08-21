#include "RE_List_Camera.hpp"
#include "RE_ListBatch_Camera.hpp"
#include "RE_Main.hpp"

#include <list>

namespace RE {
	
	std::list<ListBatch_Camera*> cameraBatchList;
	std::vector<Camera*> newCameras, deletableCameras;
	bool bDeletingMarkedCameras = false;

	void add_new_cameras() {
		for (Camera *const pCam : newCameras)
			PUSH_TO_CALLSTACKTRACE(add_camera(pCam));
		newCameras.clear();
	}

	void delete_marked_cameras() {
		bDeletingMarkedCameras = true;
		for (Camera *pCam : deletableCameras)
			PUSH_TO_CALLSTACKTRACE_DETAILED(delete pCam, append_to_string("Deleting camera ", pCam).c_str());
		deletableCameras.clear();
		bDeletingMarkedCameras = false;
	}

	void add_camera(Camera *const pCamera) {
		size_t batchIndex = 0U;
		for (ListBatch_Camera *const pBatch : cameraBatchList) {
			if (pBatch->has_space()) {
				PUSH_TO_CALLSTACKTRACE(pBatch->add(pCamera));
				break;
			}
			batchIndex++;
		}
		if (batchIndex == cameraBatchList.size()) {
			ListBatch_Camera *const pNewBatch = PUSH_TO_CALLSTACKTRACE_AND_RETURN(new ListBatch_Camera(), ListBatch_Camera*);
			PUSH_TO_CALLSTACKTRACE(pNewBatch->add(pCamera));
			cameraBatchList.push_back(pNewBatch);
		}
	}

	void remove_camera(const Camera *const pCamera) {
		for (ListBatch_Camera *const pBatch : cameraBatchList)
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(pBatch->remove(pCamera), bool))
				break;
	}

	void mark_camera_deletable(Camera *const pCamera) {
		if (!bRunning || bDeletingMarkedCameras)
			delete pCamera;
		else {
			std::vector<Camera*>::iterator it = std::find(newCameras.begin(), newCameras.end(), pCamera);
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
			PUSH_TO_CALLSTACKTRACE(pBatch->update());
	}

	void end_cameras() {
	}

}
