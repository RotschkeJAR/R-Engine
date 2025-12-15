#include "RE_Internal Header.hpp"
#include "RE_Renderer.hpp"
#include "RE_List_Camera.hpp"

namespace RE {
	
	Camera::Camera() : Camera(Transform()) {}
	Camera::Camera(const Transform &rTransform) : transform(rTransform), view(100.0f, 100.0f), fViewDistance(100.0f), bIgnoreAspectRatio(false) {
		newCameras.push_back(this);
	}
	Camera::Camera(const Vector2f &rView) : Camera(rView, 100.0f) {}
	Camera::Camera(const Vector2f &rView, const float fViewDistance) : Camera(rView, fViewDistance, false) {}
	Camera::Camera(const Vector2f &rView, const float fViewDistance, const bool bIgnoreAspectRatio) : view(rView), fViewDistance(fViewDistance), bIgnoreAspectRatio(bIgnoreAspectRatio) {
		newCameras.push_back(this);
	}
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
	
	[[nodiscard]]
	bool Camera::equals(const Camera &rCompareCamera) const {
		return transform == rCompareCamera.transform
			&& view == rCompareCamera.view
			&& fViewDistance == rCompareCamera.fViewDistance
			&& bIgnoreAspectRatio == rCompareCamera.bIgnoreAspectRatio;
	}

	void Camera::operator =(const Camera &rCopyCamera) {
		transform = rCopyCamera.transform;
		view = rCopyCamera.view;
		fViewDistance = rCopyCamera.fViewDistance;
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
