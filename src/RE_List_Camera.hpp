#ifndef __RE_LIST_CAMERA_H__
#define __RE_LIST_CAMERA_H__

#include "RE_Internal Header.hpp"

namespace RE {

#define CAMERA_BATCH_SIZE 16
	
	extern std::list<Camera*[CAMERA_BATCH_SIZE]> newCameras,
		deletableCameras,
		cameras;

	void add_new_cameras();
	void delete_marked_cameras();
	void add_camera(Camera *pCamera);
	void remove_camera(const Camera *pCamera);
	void mark_camera_deletable(Camera *pCamera);

	void start_cameras();
	void update_cameras();
	void end_cameras();

}

#endif /* __RE_LIST_CAMERA_H__ */
