#include "RE_List_Camera.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	std::unique_ptr<Camera*[]> newCameras = std::make_unique<Camera*[]>(u8MaxCameraCount),
		deletableCameras = std::make_unique<Camera*[]>(u8MaxCameraCount),
		cameras = std::make_unique<Camera*[]>(u8MaxCameraCount);
	uint8_t u8MaxCameraCount = 8,
		u8CurrentCameraCount = 0,
		u8DeletableCameraCount = 0,
		u8NewCameraCount = 0;

	void delete_and_add_cameras() {
		PRINT_DEBUG("Running the camera deletion and addition procedure");
		while (u8DeletableCameraCount) {
			Camera *const pDeletableCamera = deletableCameras[u8DeletableCameraCount - 1];
			if (u8NewCameraCount) {
				Camera *const pReplacement = newCameras[u8NewCameraCount - 1];
				PRINT_DEBUG("Replacing deletable camera with new, enqueued camera ", pReplacement);
				cameras[pDeletableCamera->u8ListIndex] = pReplacement;
				pReplacement->u8ListIndex = pDeletableCamera->u8ListIndex;
				pReplacement->bNew = pDeletableCamera->bNew;
				u8NewCameraCount--;
			}
			PRINT_DEBUG("Deleting camera ", pDeletableCamera);
			delete pDeletableCamera;
			u8DeletableCameraCount--;
		}
		while (u8NewCameraCount) {
			Camera *const pNewCamera = newCameras[u8NewCameraCount - 1];
			PRINT_DEBUG("Adding enqueued camera ", pNewCamera, " to list");
			add_camera(*pNewCamera);
			u8NewCameraCount--;
		}
	}

	void add_camera(Camera &rCamera) {
		PRINT_DEBUG("Adding camera ", std::addressof(rCamera), " to list");
		cameras[u8CurrentCameraCount] = std::addressof(rCamera);
		rCamera.u8ListIndex = u8CurrentCameraCount;
		rCamera.bNew = false;
		u8CurrentCameraCount++;
	}

	void start_cameras() {
		PRINT_DEBUG("Starting cameras");
		set_calling_active<CALLING_PHASE_START, CALLING_OBJECT_CAMERA>();
	}

	void update_cameras() {
		PRINT_DEBUG("Updating cameras");
		set_calling_active<CALLING_PHASE_UPDATE, CALLING_OBJECT_CAMERA>();
	}

	void end_cameras() {
		PRINT_DEBUG("Ending cameras");
		set_calling_active<CALLING_PHASE_END, CALLING_OBJECT_CAMERA>();
	}

	void mark_camera_deletable(Camera *const pCamera) {
		PRINT_DEBUG("Marking camera ", pCamera, " deletable");
		if (!bRunning || is_calling_phase_active<CALLING_PHASE_DELETING>()) {
			PRINT_DEBUG("Deleting camera ", pCamera);
			delete pCamera;
		} else if (pCamera->bNew) {
			PRINT_DEBUG("Camera ", pCamera, " is new and will be removed from the queue");
			newCameras[pCamera->u8ListIndex] = newCameras[u8NewCameraCount - 1];
			u8NewCameraCount--;
		} else {
			PRINT_DEBUG("Enqueued camera ", pCamera, " for deletion");
			deletableCameras[u8DeletableCameraCount] = pCamera;
			u8DeletableCameraCount++;
		}
	}

	[[nodiscard]]
	uint8_t get_max_camera_count() {
		return u8MaxCameraCount;
	}

	void set_max_camera_count(const uint8_t u8NewMaxCameraCount) {
		if (!bRunning) {
			if (!u8NewMaxCameraCount) {
				RE_ERROR("Maximum count of cameras is not allowed to be zero");
				return;
			}
			PRINT_DEBUG("Setting maximum count of cameras to ", u8NewMaxCameraCount, " and copying contents");
			std::unique_ptr<Camera*[]> newNewCameraList = std::make_unique<Camera*[]>(u8NewMaxCameraCount),
				newDeletableCameraList = std::make_unique<Camera*[]>(u8NewMaxCameraCount),
				newCameraList = std::make_unique<Camera*[]>(u8NewMaxCameraCount);
			std::copy(newCameras.get(), newCameras.get() + std::min(u8MaxCameraCount, u8NewMaxCameraCount), newNewCameraList.get());
			std::copy(deletableCameras.get(), deletableCameras.get() + std::min(u8MaxCameraCount, u8NewMaxCameraCount), newDeletableCameraList.get());
			std::copy(cameras.get(), cameras.get() + std::min(u8MaxCameraCount, u8NewMaxCameraCount), newCameraList.get());
			u8MaxCameraCount = u8NewMaxCameraCount;
			newCameras = std::move(newNewCameraList);
			deletableCameras = std::move(newDeletableCameraList);
			cameras = std::move(newCameraList);
		} else
			RE_ERROR("The maximum count of cameras cannot be changed, while the engine is running");
	}

	[[nodiscard]]
	uint8_t get_current_camera_count() {
		return u8CurrentCameraCount;
	}

}
