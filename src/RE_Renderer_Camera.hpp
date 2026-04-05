#ifndef __RE_RENDERER_CAMERA_H__
#define __RE_RENDERER_CAMERA_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern VkRect2D vk_cameraProjectionOnscreen;
	extern CameraShaderData *apCameraShaderData[RE_VK_FRAMES_IN_FLIGHT];

	void init_camera_matrices();
	void calculate_camera_matrices();
	void attach_camera(Camera *pCamera);

}

#endif /* __RE_RENDERER_CAMERA_H__ */
