#ifndef __RE_LIST_CAMERA_H__
#define __RE_LIST_CAMERA_H__

#include "RE_Internal Header.hpp"

namespace RE {

#define CAMERA_BATCH_SIZE 16
	
	extern std::list<Camera*[CAMERA_BATCH_SIZE]> newCameras,
		deletableCameras,
		cameras;
	extern uint32_t u32MaxCameraCount,
		u32CurrentCameraCount,
		u32DeletableCameraCount,
		u32NewCameraCount;
	extern bool bDeletingAddingCameras;

	void delete_and_add_cameras();
	void add_camera(Camera &rCamera);

	void start_cameras();
	void update_cameras();
	void end_cameras();

}

#endif /* __RE_LIST_CAMERA_H__ */
