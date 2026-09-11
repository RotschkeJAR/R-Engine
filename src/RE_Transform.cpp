#include "RE_Internal.hpp"

namespace RE {
	
	Transform::Transform() : scale(1.0f, 1.0f, 1.0f) {}
	Transform::Transform(const Transform &rCopy) : position(rCopy.position), scale(rCopy.scale) {}
	Transform::~Transform() {}

	void Transform::reset_position() {
		position.fill(0.0f);
	}

	void Transform::reset_rotation() {
		rotation.reset();
	}

	void Transform::reset_scale() {
		scale.fill(1.0f);
	}

	void Transform::copy_from(const Transform &rCopy) {
		position = rCopy.position;
		rotation = rCopy.rotation;
		scale = rCopy.scale;
	}
	
	[[nodiscard]]
	bool Transform::equals(const Transform &rOther) const {
		return position == rOther.position
				and rotation == rOther.rotation
				and scale == rOther.scale;
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
		return not equals(rOther);
	}

}
