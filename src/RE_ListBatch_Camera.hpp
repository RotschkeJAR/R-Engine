#ifndef __RE_LIST_BATCH_CAMERA_H__
#define __RE_LIST_BATCH_CAMERA_H__

#include "RE_Internal Header.hpp"

namespace RE {

#define MAX_CAMERAS_PER_BATCH std::numeric_limits<uint8_t>::max()
	
	class ListBatch_Camera final {
		private:
			std::array<Camera*, MAX_CAMERAS_PER_BATCH> apCameras;
			uint8_t u8Count;

		public:
			ListBatch_Camera();
			~ListBatch_Camera();

			void add(Camera *pCamera);
			bool remove(const Camera *pCamera);
			[[nodiscard]]
			bool contains(const Camera *pCamera) const;
			[[nodiscard]]
			Camera* get(uint8_t u8Index);

			[[nodiscard]]
			uint8_t size() const;
			[[nodiscard]]
			bool empty() const;
			[[nodiscard]]
			bool has_space() const;

			void update();
	};

}

#endif /* __RE_LIST_BATCH_CAMERA_H__ */
