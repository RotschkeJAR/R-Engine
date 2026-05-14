#ifndef __RE_MANAGER_H__
#define __RE_MANAGER_H__

#include "RE_Internal Header.hpp"

namespace RE {

	enum CallingPhase {
		CALLING_PHASE_NONE = 0,
		CALLING_PHASE_START = 1,
		CALLING_PHASE_UPDATE = 2,
		CALLING_PHASE_END = 3,
		CALLING_PHASE_DELETING = 4
	};
	enum CallingObject {
		CALLING_OBJECT_NONE = 0,
		CALLING_OBJECT_SCENE = 1,
		CALLING_OBJECT_GAME_OBJECT = 2,
		CALLING_OBJECT_CAMERA = 3
	};
	inline uint8_t m8CallingPhaseObject = 0;
	constexpr uint8_t u8CallingObjectBitOffset = 3,
		m8CallingObjectMask = static_cast<uint8_t>(0xFF << u8CallingObjectBitOffset),
		m8CallingPhaseMask = static_cast<uint8_t>(~m8CallingObjectMask);

	template <CallingPhase eCallingPhase>
	inline bool is_calling_phase_active() {
		return (m8CallingPhaseObject & m8CallingPhaseMask) == eCallingPhase;
	}

	template <CallingObject eCallingObject>
	inline bool is_calling_object_active() {
		return (m8CallingPhaseObject & m8CallingObjectMask) == (eCallingObject << u8CallingObjectBitOffset);
	}

	template <CallingPhase eCallingPhase, CallingObject eCallingObject>
	inline bool is_calling_active() {
		return is_calling_phase_active<eCallingPhase>() && is_calling_object_active<eCallingObject>();
	}

	template <CallingPhase eCallingPhase>
	inline void set_calling_phase_active() {
		m8CallingPhaseObject &= m8CallingObjectMask;
		m8CallingPhaseObject |= eCallingPhase;
	}

	template <CallingObject eCallingObject>
	inline void set_calling_object_active() {
		m8CallingPhaseObject &= m8CallingPhaseMask;
		m8CallingPhaseObject |= eCallingObject << u8CallingObjectBitOffset;
	}

	template <CallingPhase eCallingPhase, CallingObject eCallingObject>
	inline void set_calling_active() {
		m8CallingPhaseObject = eCallingPhase | (eCallingObject << u8CallingObjectBitOffset);
	}

	inline void set_calling_inactive() {
		set_calling_active<CALLING_PHASE_NONE, CALLING_OBJECT_NONE>();
	}

	extern Scene *pCurrentScene,
		*pNextScene;

	bool is_object_active(const GameObject* pGameObject);
	void game_logic_update();
	void last_game_logic_update();
	bool are_scenes_present();

}

#endif /* __RE_MANAGER_H__ */
