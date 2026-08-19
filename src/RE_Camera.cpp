#include "RE_Internal.hpp"
#include "RE_Renderer.hpp"
#include "RE_Main.hpp"
#include "RE_Manager.hpp"

namespace RE {
	
	Camera::Camera() : view(20.0f, 15.0f), fViewDistance(10.0f), bIgnoreAspectRatio(false) {
		add_camera(this);
	}

	Camera::Camera(const Camera &rCopy) : transform(rCopy.transform), view(rCopy.view), fViewDistance(rCopy.fViewDistance), bIgnoreAspectRatio(rCopy.bIgnoreAspectRatio) {
		add_camera(this);
	}

	Camera::~Camera() {
		deactivate();
		remove_camera(this);
	}

	void Camera::update_before_render() {}

	void Camera::activate() {
		attach_camera(this);
	}

	void Camera::deactivate() const {
		if (get_active_camera() != this)
			return;
		attach_camera(nullptr);
	}

	void Camera::copy_from(const Camera &rOther) {
		PRINT_DEBUG_CLASS("Copying camera data from ", std::addressof(rOther));
		transform = rOther.transform;
		view = rOther.view;
		fViewDistance = rOther.fViewDistance;
		bIgnoreAspectRatio = rOther.bIgnoreAspectRatio;
	}
	
	bool Camera::equals(const Camera &rOther) const {
		return transform == rOther.transform
				&& view == rOther.view
				&& fViewDistance == rOther.fViewDistance
				&& bIgnoreAspectRatio == rOther.bIgnoreAspectRatio;
	}

	void Camera::operator =(const Camera &rOther) {
		copy_from(rOther);
	}

	bool Camera::operator ==(const Camera &rOther) const {
		return equals(rOther);
	}

	bool Camera::operator !=(const Camera &rOther) const {
		return !equals(rOther);
	}

}
