#include "RE_Internal Header.hpp"

namespace RE {
	
	Transform::Transform() : scale(1.0f, 1.0f, 1.0f) {}
	Transform::Transform(const Vector3f &positionCopy) : position(positionCopy), scale(1.0f, 1.0f, 1.0f) {}
	Transform::Transform(const Vector3f &positionCopy, const Vector3f &scaleCopy) : position(positionCopy), scale(scaleCopy) {}
	Transform::Transform(const Transform &copyTransform) : position(copyTransform.position), scale(copyTransform.scale) {}
	Transform::~Transform() {}

	void Transform::reset_position() {
		position.fill(0.0f);
	}

	void Transform::copy_from(const Transform &copyTransform) {
		position.copy_from(copyTransform.position);
		scale.copy_from(copyTransform.scale);
	}
	
	bool Transform::equals(const Transform &compareTransform) const {
		return position.equals(compareTransform.position) && scale.equals(compareTransform.scale);
	}

	void Transform::operator =(const Transform &copyTransform) {
		copy_from(copyTransform);
	}
	
	bool Transform::operator ==(const Transform &compareTransform) const {
		return equals(compareTransform);
	}
	
	bool Transform::operator !=(const Transform &compareTransform) const {
		return !equals(compareTransform);
	}

}
