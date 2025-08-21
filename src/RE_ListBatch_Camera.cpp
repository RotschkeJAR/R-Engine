#include "RE_ListBatch_Camera.hpp"

namespace RE {
	
	ListBatch_Camera::ListBatch_Camera() : u8Count(0U) {}
	ListBatch_Camera::~ListBatch_Camera() {}

	void ListBatch_Camera::add(Camera *const pCamera) {
		apCameras[u8Count] = pCamera;
		u8Count++;
	}
	
	bool ListBatch_Camera::remove(const Camera *const pCamera) {
		for (uint8_t u8Index = 0U; u8Index < u8Count; u8Index++)
			if (apCameras[u8Index] == pCamera) {
				apCameras[u8Index] = apCameras[u8Count - 1U];
				u8Count--;
				return true;
			}
		return false;
	}
	
	[[nodiscard]]
	bool ListBatch_Camera::contains(const Camera *const pCamera) const {
		for (uint8_t u8Index = 0U; u8Index < u8Count; u8Index++)
			if (apCameras[u8Index] == pCamera)
				return true;
		return false;
	}
	
	[[nodiscard]]
	Camera* ListBatch_Camera::get(const uint8_t u8Index) {
		return apCameras[u8Index];
	}

	[[nodiscard]]
	uint8_t ListBatch_Camera::size() const {
		return u8Count;
	}
	
	[[nodiscard]]
	bool ListBatch_Camera::empty() const {
		return !u8Count;
	}
	
	[[nodiscard]]
	bool ListBatch_Camera::has_space() const {
		return u8Count < MAX_CAMERAS_PER_BATCH;
	}

	void ListBatch_Camera::update() {
		for (uint8_t u8Index = 0U; u8Index < u8Count; u8Index++)
			PUSH_TO_CALLSTACKTRACE_DETAILED(apCameras[u8Index]->update(), append_to_string("Camera ", apCameras[u8Index], ", Index: ", u8Index).c_str());
	}

}
