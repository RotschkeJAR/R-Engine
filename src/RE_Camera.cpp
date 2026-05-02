#include "RE_Internal Header.hpp"
#include "RE_Renderer_Camera.hpp"
#include "RE_List_Camera.hpp"

namespace RE {
	
	Camera::Camera() : Camera(Transform()) {}
	Camera::Camera(const Transform &rTransform) : transform(rTransform), view(100.0f, 100.0f), f32ViewDistance(100.0f), bIgnoreAspectRatio(false) {
		newCameras.push_back(this);
	}
	Camera::Camera(const Vector2f &rView) : Camera(rView, 100.0f) {}
	Camera::Camera(const Vector2f &rView, const float f32ViewDistance) : Camera(rView, f32ViewDistance, false) {}
	Camera::Camera(const Vector2f &rView, const float f32ViewDistance, const bool bIgnoreAspectRatio) : view(rView), f32ViewDistance(f32ViewDistance), bIgnoreAspectRatio(bIgnoreAspectRatio) {
		newCameras.push_back(this);
	}
	Camera::Camera(const Camera &rCopy) : Camera(rCopy.view, rCopy.f32ViewDistance, rCopy.bIgnoreAspectRatio) {}
	Camera::~Camera() {
		PRINT_DEBUG_CLASS("Destructing camera");
		deactivate();
		remove_camera(this);
	}

	void Camera::update_before_render() {}

	void Camera::activate() {
		PRINT_DEBUG_CLASS("Activating this camera");
		attach_camera(this);
	}

	void Camera::deactivate() const {
		PRINT_DEBUG_CLASS("Deactivating this camera");
		attach_camera(nullptr);
	}

	void Camera::mark_deletable() {
		PRINT_DEBUG_CLASS("Marking camera deletable");
		mark_camera_deletable(this);
	}

	void Camera::copy_from(const Camera &rOther) {
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
