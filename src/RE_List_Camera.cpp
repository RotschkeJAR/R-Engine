#include "RE_List_Camera.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	std::list<std::array<Camera*, CAMERA_BATCH_SIZE>> newCameras,
		deletableCameras,
		cameras;
	uint32_t u32MaxCameraCount = CAMERA_BATCH_SIZE,
		u32CurrentCameraCount = 0,
		u32DeletableCameraCount = 0,
		u32NewCameraCount = 0;
	bool bDeletingAddingCameras = false;

	void delete_and_add_cameras() {
		PRINT_DEBUG("Running the camera deletion and addition procedure");
		bDeletingAddingCameras = true;
		while (u32DeletableCameraCount) {
			Camera *const pDeletableCamera = deletableCameras.back()[u32DeletableCameraCount % CAMERA_BATCH_SIZE];
			if (u32NewCameraCount) {
				Camera *const pReplacement = newCameras.back()[(u32NewCameraCount - 1) % CAMERA_BATCH_SIZE];
				PRINT_DEBUG("Replacing deletable camera with new, enqueued camera ", pReplacement);
				auto newCameraIter = cameras.begin();
				std::advance(newCameraIter, pDeletableCamera->u32ListIndex / CAMERA_BATCH_SIZE);
				(*newCameraIter)[pDeletableCamera->u32ListIndex % CAMERA_BATCH_SIZE] = pReplacement;
				pReplacement->u32ListIndex = pDeletableCamera->u32ListIndex;
				pReplacement->bNew = pDeletableCamera->bNew;
				u32NewCameraCount--;
				if ((u32NewCameraCount % CAMERA_BATCH_SIZE) == 0)
					newCameras.pop_back();
			}
			PRINT_DEBUG("Deleting camera ", pDeletableCamera);
			delete pDeletableCamera;
			u32DeletableCameraCount--;
			if ((u32DeletableCameraCount % CAMERA_BATCH_SIZE) == 0)
				deletableCameras.pop_back();
		}
		while (u32NewCameraCount) {
			Camera *const pNewCamera = newCameras.back()[(u32NewCameraCount - 1) % CAMERA_BATCH_SIZE];
			PRINT_DEBUG("Adding enqueued camera ", pNewCamera, " to list");
			add_camera(*pNewCamera);
			u32NewCameraCount--;
			if ((u32NewCameraCount % CAMERA_BATCH_SIZE) == 0)
				newCameras.pop_back();
		}
		bDeletingAddingCameras = false;
	}

	void add_camera(Camera &rCamera) {
		PRINT_DEBUG("Adding camera ", std::addressof(rCamera), " to list");
		if ((u32CurrentCameraCount % CAMERA_BATCH_SIZE) == 0) {
			cameras.emplace_back();
			cameras.back()[0] = std::addressof(rCamera);
		} else
			cameras.back()[u32CurrentCameraCount % CAMERA_BATCH_SIZE] = std::addressof(rCamera);
		rCamera.u32ListIndex = u32CurrentCameraCount;
		rCamera.bNew = false;
		u32CurrentCameraCount++;
	}

	void start_cameras() {
		PRINT_DEBUG("Starting cameras");
		eCallingPhase = CALLING_PHASE_CAMERA_START;
	}

	void update_cameras() {
		PRINT_DEBUG("Updating cameras");
		eCallingPhase = CALLING_PHASE_CAMERA_UPDATE;
	}

	void end_cameras() {
		PRINT_DEBUG("Ending cameras");
		eCallingPhase = CALLING_PHASE_CAMERA_END;
	}

	void mark_camera_deletable(Camera *const pCamera) {
		PRINT_DEBUG("Marking camera ", pCamera, " deletable");
		if (!bRunning || bDeletingAddingCameras) {
			PRINT_DEBUG("Deleting camera ", pCamera);
			delete pCamera;
		} else if (pCamera->bNew) {
			PRINT_DEBUG("Camera ", pCamera, " is new and will be removed from the queue");
			auto newCameraIter = newCameras.begin();
			std::advance(newCameraIter, pCamera->u32ListIndex / CAMERA_BATCH_SIZE);
			(*newCameraIter)[pCamera->u32ListIndex % CAMERA_BATCH_SIZE] = newCameras.back()[(u32NewCameraCount - 1) % CAMERA_BATCH_SIZE];
			u32NewCameraCount--;
		} else {
			PRINT_DEBUG("Enqueued camera ", pCamera, " for deletion");
			if ((u32DeletableCameraCount % CAMERA_BATCH_SIZE) == 0) {
				deletableCameras.emplace_back();
				deletableCameras.back()[0] = pCamera;
			} else
				deletableCameras.back()[u32DeletableCameraCount % CAMERA_BATCH_SIZE] = pCamera;
			u32DeletableCameraCount++;
		}
	}

	[[nodiscard]]
	uint32_t get_max_camera_count() {
		return u32MaxCameraCount;
	}

	void set_max_camera_count(const uint32_t u32NewMaxCameraCount) {
		if (!bRunning) {
			PRINT_DEBUG("Setting maximum count of cameras to ", u32NewMaxCameraCount);
			u32MaxCameraCount = u32NewMaxCameraCount;
		} else
			RE_ERROR("The maximum count of cameras cannot be changed, while the engine is running");
	}

	[[nodiscard]]
	uint32_t get_current_camera_count() {
		return u32CurrentCameraCount;
	}

}
