#include "RE_ListBatch_Camera.hpp"

namespace RE {
	
	ListBatch_Camera::ListBatch_Camera() : u8Count(0) {
		PRINT_DEBUG_CLASS("Constructing list-batch for cameras");
	}
	ListBatch_Camera::~ListBatch_Camera() {
		PRINT_DEBUG_CLASS("Destructing list-batch for cameras");
	}

	void ListBatch_Camera::add(Camera *const pCamera) {
		PRINT_DEBUG_CLASS("Adding camera ", pCamera, " at array index ", u8Count, ". Count increases to ", u8Count + 1);
		apCameras[u8Count] = pCamera;
		u8Count++;
	}
	
	bool ListBatch_Camera::remove(const Camera *const pCamera) {
		PRINT_DEBUG_CLASS("Looking for camera ", pCamera);
		for (uint8_t u8Index = 0; u8Index < u8Count; u8Index++)
			if (apCameras[u8Index] == pCamera) {
				PRINT_DEBUG_CLASS("Removing camera from batch at array index ", u8Index, ". Count decreases to ", u8Count - 1);
				apCameras[u8Index] = apCameras[u8Count - 1U];
				u8Count--;
				return true;
			}
		PRINT_DEBUG_CLASS("Camera not found here");
		return false;
	}
	
	[[nodiscard]]
	bool ListBatch_Camera::contains(const Camera *const pCamera) const {
		const auto endOfSearch = apCameras.begin() + u8Count;
		return std::find(apCameras.begin(), endOfSearch, pCamera) != endOfSearch;
	}
	
	[[nodiscard]]
	Camera* ListBatch_Camera::get(const uint8_t u8Index) {
		PRINT_DEBUG_CLASS("Returning camera at array index ", u8Index, ", when count is ", u8Count);
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

}
