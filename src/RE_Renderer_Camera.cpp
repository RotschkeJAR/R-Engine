#include "RE_Renderer_Camera.hpp"
#include "RE_Main.hpp"

namespace RE {

	static Camera *pActiveCamera = nullptr;
	VkRect2D vk_cameraProjectionOnscreen;
	CameraShaderData *apCameraShaderData[RE_VK_FRAMES_IN_FLIGHT];
	
	void init_camera_matrices() {
		PRINT_DEBUG("Initializing camera matrices with default values");
		for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
			for (uint8_t u8MatrixElementIndex = 0; u8MatrixElementIndex < 4 * 4; u8MatrixElementIndex++)
				switch (u8MatrixElementIndex) {
					case 0:
					case 5:
					case 10:
					case 15:
						apCameraShaderData[u8FrameInFlightIndex]->viewMatrix[u8MatrixElementIndex] = 1.0f;
						apCameraShaderData[u8FrameInFlightIndex]->projectionMatrix[u8MatrixElementIndex] = 1.0f;
						break;
					default:
						apCameraShaderData[u8FrameInFlightIndex]->viewMatrix[u8MatrixElementIndex] = 0.0f;
						apCameraShaderData[u8FrameInFlightIndex]->projectionMatrix[u8MatrixElementIndex] = 0.0f;
						break;
				}
		}
		vk_cameraProjectionOnscreen.offset.x = 0;
		vk_cameraProjectionOnscreen.offset.y = 0;
		vk_cameraProjectionOnscreen.extent.width = renderImageSize[0];
		vk_cameraProjectionOnscreen.extent.height = renderImageSize[1];
	}

	void calculate_camera_matrices() {
		if (pActiveCamera) {
			PRINT_DEBUG("Updating camera before transferring its matrices");
			pActiveCamera->update_before_render();
			if (!apCameraBufferMemories[u8CurrentFrameInFlightIndex]->invalidate_mapped_memory(vk_aCameraBufferMemoryOffsets[u8CurrentFrameInFlightIndex], sizeof(CameraShaderData))) {
				RE_FATAL_ERROR("Failed invalidating non-coherent Vulkan memory used for camera uniforms");
				return;
			}
			PRINT_DEBUG("Updating camera matrices");
			apCameraShaderData[u8CurrentFrameInFlightIndex]->viewMatrix[12] = -pActiveCamera->transform.position[0];
			apCameraShaderData[u8CurrentFrameInFlightIndex]->viewMatrix[13] = -pActiveCamera->transform.position[1];
			apCameraShaderData[u8CurrentFrameInFlightIndex]->viewMatrix[14] = -pActiveCamera->transform.position[2];
			apCameraShaderData[u8CurrentFrameInFlightIndex]->projectionMatrix[0] = 1.0f / pActiveCamera->view[0];
			apCameraShaderData[u8CurrentFrameInFlightIndex]->projectionMatrix[5] = -1.0f / pActiveCamera->view[1];
			apCameraShaderData[u8CurrentFrameInFlightIndex]->projectionMatrix[10] = 1.0f / pActiveCamera->f32ViewDistance;
			if (!apCameraBufferMemories[u8CurrentFrameInFlightIndex]->flush_mapped_memory(vk_aCameraBufferMemoryOffsets[u8CurrentFrameInFlightIndex], sizeof(CameraShaderData))) {
				RE_FATAL_ERROR("Failed flushing non-coherent Vulkan memory used for camera uniforms");
				return;
			}
			if (!pActiveCamera->bIgnoreAspectRatio) {
				PRINT_DEBUG("Calculating projection on screen depending on camera's projection's aspect ratio");
				const float f32Factor = std::min(renderImageSize[0] / pActiveCamera->view[0], renderImageSize[1] / pActiveCamera->view[1]);
				vk_cameraProjectionOnscreen.extent.width = static_cast<uint32_t>(std::roundf(pActiveCamera->view[0] * f32Factor));
				vk_cameraProjectionOnscreen.extent.height = static_cast<uint32_t>(std::roundf(pActiveCamera->view[1] * f32Factor));
				vk_cameraProjectionOnscreen.offset.x = (renderImageSize[0] - vk_cameraProjectionOnscreen.extent.width) / 2;
				vk_cameraProjectionOnscreen.offset.y = (renderImageSize[1] - vk_cameraProjectionOnscreen.extent.height) / 2;
				return;
			}
		}
		vk_cameraProjectionOnscreen.offset.x = 0;
		vk_cameraProjectionOnscreen.offset.y = 0;
		vk_cameraProjectionOnscreen.extent.width = renderImageSize[0];
		vk_cameraProjectionOnscreen.extent.height = renderImageSize[1];
	}

	void attach_camera(Camera *const pCam) {
		if (pActiveCamera == pCam)
			return;
		else if (!pCam) {
			PRINT_DEBUG("Dettaching camera and copying recent camera matrices");
			pActiveCamera = nullptr;
			if (!bRunning)
				return;
			const uint8_t u8IndexToCopyFrom = u8CurrentFrameInFlightIndex == 0 ? (RE_VK_FRAMES_IN_FLIGHT - 1) : u8CurrentFrameInFlightIndex;
			for (uint8_t u8CameraUniformBufferIndex = 0; u8CameraUniformBufferIndex < RE_VK_FRAMES_IN_FLIGHT; u8CameraUniformBufferIndex++) {
				if (u8CameraUniformBufferIndex == u8IndexToCopyFrom)
					continue;
				std::memcpy(apCameraShaderData[u8CameraUniformBufferIndex], apCameraShaderData[u8IndexToCopyFrom], sizeof(CameraShaderData));
			}
		} else {
			PRINT_DEBUG("Attaching new camera ", pCam);
			pActiveCamera = pCam;
		}
	}

}
