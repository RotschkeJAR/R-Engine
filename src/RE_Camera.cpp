#include "RE_Internal Header.hpp"
#include "RE_Renderer.hpp"

namespace RE {
	
	Camera::Camera() : scale(10.0f, 10.0f) {}
	Camera::Camera(Vector3f &rPosition) : position(rPosition), scale(10.0f, 10.0f) {}
	Camera::Camera(Vector3f &rPosition, Vector2f &rScale) : position(rPosition), scale(rScale) {}
	Camera::~Camera() {
		deactivate();
	}

	void Camera::update() {}

	void Camera::activate() {
		pActiveCamera = this;
		Renderer::pInstance->calculate_render_area();
	}

	void Camera::deactivate() const {
		if (pActiveCamera == this)
			pActiveCamera = nullptr;
	}
	
	bool Camera::has_same_transform(const Camera &rCompareCamera) const {
		return position == rCompareCamera.position && scale == rCompareCamera.scale;
	}

	void Camera::operator =(const Camera &rCopyCamera) {
		position = rCopyCamera.position;
		scale = rCopyCamera.scale;
	}
	
	bool Camera::operator ==(const Camera &rCompareCamera) const {
		return has_same_transform(rCompareCamera);
	}
	
	bool Camera::operator !=(const Camera &rCompareCamera) const {
		return !has_same_transform(rCompareCamera);
	}

}
