#ifndef __RE_LIST_CAMERA_H__
#define __RE_LIST_CAMERA_H__

#include "RE_Internal Header.hpp"

namespace RE {
	
	extern std::vector<Camera*> newCameras, deletableCameras;

	void add_new_cameras();
	void delete_marked_cameras();
	void add_camera(Camera *const pCamera);
	void remove_camera(const Camera *const pCamera);
	void mark_camera_deletable(Camera *const pCamera);

	void start_cameras();
	void update_cameras();
	void end_cameras();

}

#endif /* __RE_LIST_CAMERA_H__ */
