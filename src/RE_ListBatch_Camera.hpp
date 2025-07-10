#ifndef __RE_LIST_BATCH_CAMERA_H__
#define __RE_LIST_BATCH_CAMERA_H__

#include "RE_Internal Header.hpp"

namespace RE {

#define MAX_CAMERAS_PER_BATCH 100
	
	class ListBatch_Camera final {
		private:
			Camera *apCameras[MAX_CAMERAS_PER_BATCH];
			uint8_t u8Count;

		public:
			ListBatch_Camera();
			~ListBatch_Camera();

			void add(Camera *const pCamera);
			bool remove(const Camera *const pCamera);
			bool contains(const Camera *const pCamera) const;
			Camera* get(const uint8_t u8Index);

			uint8_t size() const;
			bool empty() const;
			bool has_space() const;

			void update();
	};

}

#endif /* __RE_LIST_BATCH_CAMERA_H__ */
