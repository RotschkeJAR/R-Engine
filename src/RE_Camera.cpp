#include "RE_Internal Header.hpp"
#include "RE_Renderer_Camera.hpp"
#include "RE_List_Camera.hpp"
#include "RE_Main.hpp"

namespace RE {

	static void add_new_camera_to_list(Camera *const pCamera) {
		if (u32CurrentCameraCount == u32MaxCameraCount)
			RE_ABORT("Camera pool exhausted due to overallocation");
		if (bRunning || !bDeletingAddingCameras) {
			PRINT_DEBUG("Enqueuing new camera ", pCamera);
			if ((u32NewCameraCount % CAMERA_BATCH_SIZE) == 0) {
				newCameras.emplace_back();
				newCameras.back()[0] = pCamera;
			} else
				newCameras.back()[u32NewCameraCount % CAMERA_BATCH_SIZE] = pCamera;
			pCamera->u32ListIndex = u32NewCameraCount;
			u32NewCameraCount++;
		} else {
			PRINT_DEBUG("Directly adding new camera ", pCamera);
			add_camera(*pCamera);
		}
	}
	
	Camera::Camera() : Camera(Transform()) {}
	Camera::Camera(const Transform &rTransform) : bNew(true), transform(rTransform), view(100.0f, 100.0f), f32ViewDistance(100.0f), bIgnoreAspectRatio(false) {
		add_new_camera_to_list(this);
	}
	Camera::Camera(const Vector2f &rView) : Camera(rView, 100.0f) {}
	Camera::Camera(const Vector2f &rView, const float f32ViewDistance) : Camera(rView, f32ViewDistance, false) {}
	Camera::Camera(const Vector2f &rView, const float f32ViewDistance, const bool bIgnoreAspectRatio) : bNew(true), view(rView), f32ViewDistance(f32ViewDistance), bIgnoreAspectRatio(bIgnoreAspectRatio) {
		add_new_camera_to_list(this);
	}
	Camera::Camera(const Camera &rCopy) : Camera(rCopy.view, rCopy.f32ViewDistance, rCopy.bIgnoreAspectRatio) {
		transform = rCopy.transform;
	}
	Camera::~Camera() {
		PRINT_DEBUG_CLASS("Destructing camera");
		deactivate();
		if (bNew) {
			PRINT_DEBUG_CLASS("Removing camera from queue of new cameras");
			Camera &rCamera = *newCameras.back()[(u32NewCameraCount - 1) % CAMERA_BATCH_SIZE];
			auto newCameraIter = newCameras.begin();
			std::advance(newCameraIter, u32ListIndex / CAMERA_BATCH_SIZE);
			(*newCameraIter)[u32ListIndex % CAMERA_BATCH_SIZE] = std::addressof(rCamera);
			rCamera.u32ListIndex = u32ListIndex;
			u32NewCameraCount--;
		} else {
			PRINT_DEBUG_CLASS("Removing camera from camera list");
			Camera &rCamera = *cameras.back()[(u32CurrentCameraCount - 1) % CAMERA_BATCH_SIZE];
			auto cameraIter = cameras.begin();
			std::advance(cameraIter, u32ListIndex / CAMERA_BATCH_SIZE);
			(*cameraIter)[u32ListIndex % CAMERA_BATCH_SIZE] = std::addressof(rCamera);
			rCamera.u32ListIndex = u32ListIndex;
			u32CurrentCameraCount--;
		}
	}

	void Camera::update_before_render() {}

	void Camera::activate() {
		PRINT_DEBUG_CLASS("Activating this camera");
		attach_camera(this);
	}

	void Camera::deactivate() const {
		if (get_active_camera() != this)
			return;
		PRINT_DEBUG_CLASS("Deactivating this camera");
		attach_camera(nullptr);
	}

	void Camera::copy_from(const Camera &rOther) {
		PRINT_DEBUG_CLASS("Copying camera data from ", std::addressof(rOther));
		transform = rOther.transform;
		view = rOther.view;
		f32ViewDistance = rOther.f32ViewDistance;
		bIgnoreAspectRatio = rOther.bIgnoreAspectRatio;
	}
	
	[[nodiscard]]
	bool Camera::equals(const Camera &rOther) const {
		return transform == rOther.transform
			&& view == rOther.view
			&& f32ViewDistance == rOther.f32ViewDistance
			&& bIgnoreAspectRatio == rOther.bIgnoreAspectRatio;
	}

	void Camera::operator =(const Camera &rOther) {
		copy_from(rOther);
	}

	[[nodiscard]]
	bool Camera::operator ==(const Camera &rOther) const {
		return equals(rOther);
	}

	[[nodiscard]]
	bool Camera::operator !=(const Camera &rOther) const {
		return !equals(rOther);
	}

}
