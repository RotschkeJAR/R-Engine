#include "RE_Internal Header.hpp"

namespace RE {
	
	Transform::Transform() : scale(1.0f, 1.0f, 1.0f) {}
	Transform::Transform(const Vector3f &rPosition) : position(rPosition), scale(1.0f, 1.0f, 1.0f) {}
	Transform::Transform(const Vector3f &rPosition, const Vector3f &rScale) : position(rPosition), scale(rScale) {}
	Transform::Transform(const Transform &rCopy) : position(rCopy.position), scale(rCopy.scale) {}
	Transform::~Transform() {}

	void Transform::reset_position() {
		position.fill(0.0f);
	}

	void Transform::copy_from(const Transform &rCopy) {
		position.copy_from(rCopy.position);
		scale.copy_from(rCopy.scale);
	}
	
	[[nodiscard]]
	bool Transform::equals(const Transform &rOther) const {
		return position.equals(rOther.position) && scale.equals(rOther.scale);
	}

	void Transform::operator =(const Transform &rCopy) {
		copy_from(rCopy);
	}
	
	[[nodiscard]]
	bool Transform::operator ==(const Transform &rOther) const {
		return equals(rOther);
	}
	
	[[nodiscard]]
	bool Transform::operator !=(const Transform &rOther) const {
		return !equals(rOther);
	}

}
