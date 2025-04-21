#include "RE_Internal Header.hpp"

namespace RE {
	
	Transform::Transform() : scale(1.0f, 1.0f, 1.0f) {}
	Transform::Transform(const Vector3f &positionCopy) : position(positionCopy[0], positionCopy[1], positionCopy[2]), scale(1.0f, 1.0f, 1.0f) {}
	Transform::Transform(const Vector3f &positionCopy, const Vector3f &scaleCopy) : position(positionCopy[0], positionCopy[1], positionCopy[2]), scale(scaleCopy[0], scaleCopy[1], scaleCopy[2]) {}
	Transform::~Transform() {}

}
