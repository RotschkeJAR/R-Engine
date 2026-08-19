#ifndef __RE_RENDERER_CAMERA_H__
#define __RE_RENDERER_CAMERA_H__ 1

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern VkRect2D vk_cameraProjectionOnscreen;

	void init_camera_matrices();
	void calculate_camera_matrices();

}

#endif /* __RE_RENDERER_CAMERA_H__ */
