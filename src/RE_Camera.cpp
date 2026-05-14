#include "RE_Internal Header.hpp"
#include "RE_Renderer_Camera.hpp"
#include "RE_List_Camera.hpp"
#include "RE_Main.hpp"
#include "RE_Manager.hpp"

namespace RE {

	void add_new_camera_to_list(Camera *const pCamera) {
		if (u8CurrentCameraCount == u8MaxCameraCount)
			RE_ABORT("Camera pool exhausted due to overallocation");
		if (is_calling_object_active<CALLING_OBJECT_CAMERA>()) {
			PRINT_DEBUG("Enqueuing new camera ", pCamera);
			newCameras[u8NewCameraCount] = pCamera;
			pCamera->u8ListIndex = u8NewCameraCount;
			u8NewCameraCount++;
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
		if (is_calling_object_active<CALLING_OBJECT_CAMERA>())
			RE_WARNING("Camera ", this, " is being removed from the list while the engine is iterating over it. Mark it deletable to avoid potential bugs");
		if (bNew) {
			PRINT_DEBUG_CLASS("Removing camera from queue of new cameras");
			Camera &rReplacement = *newCameras[u8NewCameraCount - 1];
			newCameras[u8ListIndex] = std::addressof(rReplacement);
			rReplacement.u8ListIndex = u8ListIndex;
			u8NewCameraCount--;
		} else {
			PRINT_DEBUG_CLASS("Removing camera from camera list");
			Camera &rReplacement = *cameras[u8CurrentCameraCount - 1];
			cameras[u8ListIndex] = std::addressof(rReplacement);
			rReplacement.u8ListIndex = u8ListIndex;
			u8CurrentCameraCount--;
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
