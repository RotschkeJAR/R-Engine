#include "RE_Internal Header.hpp"
#include "RE_Renderer.hpp"
#include "RE_List_Camera.hpp"

namespace RE {
	
	Camera::Camera() : Camera(Transform()) {}
	Camera::Camera(const Transform &rTransform) : transform(rTransform), view(10.0f, 10.0f), bIgnoreAspectRatio(false) {}
	Camera::Camera(const Vector2f &rView) : Camera(rView, false) {}
	Camera::Camera(const Vector2f &rView, const bool bIgnoreAspectRatio) : view(rView), bIgnoreAspectRatio(bIgnoreAspectRatio) {
		newCameras.push_back(this);
	}
	Camera::~Camera() {
		PRINT_DEBUG_CLASS("Destructing camera");
		deactivate();
		remove_camera(this);
	}

	void Camera::update() {}

	void Camera::activate() const {
		attach_camera(this);
	}

	void Camera::deactivate() const {
		if (pActiveCamera == this)
			attach_camera(nullptr);
	}

	void Camera::mark_deletable() {
		PRINT_DEBUG_CLASS("Marking camera deletable");
		mark_camera_deletable(this);
	}
	
	[[nodiscard]]
	bool Camera::equals(const Camera &rCompareCamera) const {
		return transform == rCompareCamera.transform
			&& view == rCompareCamera.view
			&& bIgnoreAspectRatio == rCompareCamera.bIgnoreAspectRatio;
	}

	void Camera::operator =(const Camera &rCopyCamera) {
		transform = rCopyCamera.transform;
		view = rCopyCamera.view;
		bIgnoreAspectRatio = rCopyCamera.bIgnoreAspectRatio;
	}

	[[nodiscard]]
	bool Camera::operator ==(const Camera &rCompareCamera) const {
		return equals(rCompareCamera);
	}

	[[nodiscard]]
	bool Camera::operator !=(const Camera &rCompareCamera) const {
		return !equals(rCompareCamera);
	}

}
