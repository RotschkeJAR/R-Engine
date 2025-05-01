#include "RE_Internal Header.hpp"

namespace RE {
	
	Transform::Transform() : scale(1.0f, 1.0f, 1.0f) {}
	Transform::Transform(const Vector3f &rPositionCopy) : position(rPositionCopy), scale(1.0f, 1.0f, 1.0f) {}
	Transform::Transform(const Vector3f &rPositionCopy, const Vector3f &rScaleCopy) : position(rPositionCopy), scale(rScaleCopy) {}
	Transform::Transform(const Transform &rCopyTransform) : position(rCopyTransform.position), scale(rCopyTransform.scale) {}
	Transform::~Transform() {}

	void Transform::reset_position() {
		position.fill(0.0f);
	}

	void Transform::copy_from(const Transform &rCopyTransform) {
		position.copy_from(rCopyTransform.position);
		scale.copy_from(rCopyTransform.scale);
	}
	
	bool Transform::equals(const Transform &rCompareTransform) const {
		return position.equals(rCompareTransform.position) && scale.equals(rCompareTransform.scale);
	}

	void Transform::operator =(const Transform &rCopyTransform) {
		copy_from(rCopyTransform);
	}
	
	bool Transform::operator ==(const Transform &rCompareTransform) const {
		return equals(rCompareTransform);
	}
	
	bool Transform::operator !=(const Transform &rCompareTransform) const {
		return !equals(rCompareTransform);
	}

}
