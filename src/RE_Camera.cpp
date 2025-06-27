#include "RE_Internal Header.hpp"
#include "RE_Renderer.hpp"

namespace RE {
	
	Camera::Camera() : scale(1.0f, 1.0f), view(10.0f, 10.0f) {}
	Camera::Camera(Vector3f &rPosition) : position(rPosition), scale(1.0f, 1.0f), view(10.0f, 10.0f) {}
	Camera::Camera(Vector3f &rPosition, Vector2f &rScale) : position(rPosition), scale(rScale), view(10.0f, 10.0f) {}
	Camera::Camera(Vector3f &rPosition, Vector2f &rScale, Vector2f &rView) : position(rPosition), scale(rScale), view(rView) {}
	Camera::~Camera() {
		deactivate();
	}

	void Camera::update() {}

	void Camera::activate() {
		attach_camera(this);
	}

	void Camera::deactivate() const {
		if (pActiveCamera == this)
			attach_camera(nullptr);
	}
	
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
	
	bool Camera::operator ==(const Camera &rCompareCamera) const {
		return has_same_transform(rCompareCamera);
	}
	
	bool Camera::operator !=(const Camera &rCompareCamera) const {
		return !has_same_transform(rCompareCamera);
	}

}
