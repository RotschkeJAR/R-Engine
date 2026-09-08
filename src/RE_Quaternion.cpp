#include "RE_Internal.hpp"

namespace RE {

#define PITCH   0
#define YAW     1
#define ROLL    2

	Quaternion::Quaternion(float fPitch, float fYaw, float fRoll) : a3fRotation{
				fPitch,
				fYaw,
				fRoll
			} {}
	Quaternion::Quaternion(const Quaternion &rCopy) : a3fRotation{
				rCopy.a3fRotation[PITCH],
				rCopy.a3fRotation[YAW],
				rCopy.a3fRotation[ROLL]
			} {}
	Quaternion::~Quaternion() {}

	void Quaternion::set_pitch(float fNewPitch) {
		a3fRotation[PITCH] = fNewPitch;
	}
	
	float Quaternion::get_pitch() const {
		return a3fRotation[PITCH];
	}
	
	void Quaternion::set_yaw(float fNewYaw) {
		a3fRotation[YAW] = fNewYaw;
	}
	
	float Quaternion::get_yaw() const {
		return a3fRotation[YAW];
	}
	
	void Quaternion::set_roll(float fNewRoll) {
		a3fRotation[ROLL] = fNewRoll;
	}
	
	float Quaternion::get_roll() const {
		return a3fRotation[ROLL];
	}
	
	void Quaternion::copy_from(const Quaternion &rCopy) {
		for (unsigned i = 0; i < uDimensionCount; i++)
			a3fRotation[i] = rCopy.a3fRotation[i];
	}
	
	[[nodiscard]]
	bool Quaternion::equals(const Quaternion &rOther) const {
		for (unsigned i = 0; i < uDimensionCount; i++)
			if (a3fRotation[i] == rOther.a3fRotation[i])
				return true;
		return false;
	}

	void Quaternion::operator =(const Quaternion &rCopy) {
		copy_from(rCopy);
	}
	
	[[nodiscard]]
	bool Quaternion::operator ==(const Quaternion &rOther) const {
		return equals(rOther);
	}
	
	[[nodiscard]]
	bool Quaternion::operator !=(const Quaternion &rOther) const {
		return not equals(rOther);
	}

}