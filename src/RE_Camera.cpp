#include "RE_Internal Header.hpp"
#include "RE_Renderer.hpp"
#include "RE_List_Camera.hpp"

namespace RE {
	
	Camera::Camera() : scale(1.0f, 1.0f), view(10.0f, 10.0f) {
		newCameras.push_back(this);
	}
	Camera::Camera(const Vector3f &rPosition) : position(rPosition), scale(1.0f, 1.0f), view(10.0f, 10.0f) {
		newCameras.push_back(this);
	}
	Camera::Camera(const Vector3f &rPosition, const Vector2f &rScale) : position(rPosition), scale(rScale), view(10.0f, 10.0f) {
		newCameras.push_back(this);
	}
	Camera::Camera(const Vector3f &rPosition, const Vector2f &rScale, const Vector2f &rView) : position(rPosition), scale(rScale), view(rView) {
		newCameras.push_back(this);
	}
	Camera::~Camera() {
		PRINT_DEBUG("Destructing camera ", this);
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
		PRINT_DEBUG("Marking camera ", this, " deletable");
		mark_camera_deletable(this);
	}
	
	[[nodiscard]]
	bool Camera::has_same_transform(const Camera &rCompareCamera) const {
		return position == rCompareCamera.position 
			&& scale == rCompareCamera.scale 
			&& view == rCompareCamera.view;
	}

	void Camera::operator =(const Camera &rCopyCamera) {
		position = rCopyCamera.position;
		scale = rCopyCamera.scale;
		view = rCopyCamera.view;
	}

	[[nodiscard]]
	bool Camera::operator ==(const Camera &rCompareCamera) const {
		return has_same_transform(rCompareCamera);
	}

	[[nodiscard]]
	bool Camera::operator !=(const Camera &rCompareCamera) const {
		return !has_same_transform(rCompareCamera);
	}

}
