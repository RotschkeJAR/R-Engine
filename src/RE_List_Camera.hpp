#ifndef __RE_LIST_CAMERA_H__
#define __RE_LIST_CAMERA_H__

#include "RE_Internal Header.hpp"

namespace RE {
	
	extern std::unique_ptr<Camera*[]> newCameras,
		deletableCameras,
		cameras;
	extern uint8_t u8MaxCameraCount,
		u8CurrentCameraCount,
		u8DeletableCameraCount,
		u8NewCameraCount;

	void delete_and_add_cameras();
	void add_camera(Camera &rCamera);

	void start_cameras();
	void update_cameras();
	void end_cameras();

}

#endif /* __RE_LIST_CAMERA_H__ */
