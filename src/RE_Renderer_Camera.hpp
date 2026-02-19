#ifndef __RE_RENDERER_CAMERA_H__
#define __RE_RENDERER_CAMERA_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {
	
#define RE_VK_VIEW_MATRIX_SIZE (4 * 4)
#define RE_VK_VIEW_MATRIX_OFFSET 0
#define RE_VK_PROJECTION_MATRIX_SIZE (4 * 4)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE
#define RE_VK_CAMERA_MATRICES_SIZE (RE_VK_VIEW_MATRIX_SIZE + RE_VK_PROJECTION_MATRIX_SIZE)
#define RE_VK_CAMERA_MATRICES_SIZE_BYTES (RE_VK_CAMERA_MATRICES_SIZE * sizeof(float))

	extern VkRect2D vk_cameraProjectionOnscreen;
	extern float *apafCameraMatrices[RE_VK_FRAMES_IN_FLIGHT];

	void init_camera_matrices();
	void calculate_camera_matrices();
	void attach_camera(Camera *pCamera);

}

#endif /* __RE_RENDERER_CAMERA_H__ */
