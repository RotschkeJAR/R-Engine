#include "RE_Renderer_Camera.hpp"
#include "RE_Main.hpp"

namespace RE {

	static Camera *pActiveCamera = nullptr;
	VkRect2D vk_cameraProjectionOnscreen;
	float *apafCameraMatrices[RE_VK_FRAMES_IN_FLIGHT];
	
	void init_camera_matrices() {
		PRINT_DEBUG("Initializing camera matrices with default values");
		for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
			for (uint8_t u8MatrixElementIndex = 0; u8MatrixElementIndex < 4 * 4; u8MatrixElementIndex++)
				switch (u8MatrixElementIndex) {
					case 0:
					case 5:
					case 10:
					case 15:
						apafCameraMatrices[u8FrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + u8MatrixElementIndex] = 1.0f;
						apafCameraMatrices[u8FrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + u8MatrixElementIndex] = 1.0f;
						break;
					default:
						apafCameraMatrices[u8FrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + u8MatrixElementIndex] = 0.0f;
						apafCameraMatrices[u8FrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + u8MatrixElementIndex] = 0.0f;
						break;
				}
		}
		vk_cameraProjectionOnscreen.offset.x = 0;
		vk_cameraProjectionOnscreen.offset.y = 0;
		vk_cameraProjectionOnscreen.extent = vk_renderImageSize;
	}

	void calculate_camera_matrices() {
		if (pActiveCamera) {
			PRINT_DEBUG("Updating camera before transferring its matrices");
			pActiveCamera->update_before_render();
			const VkMappedMemoryRange vk_memoryRange = {
				.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				.memory = cameraBuffersMemory.get(),
				.offset = vk_aCameraBufferMemoryOffsets[u8CurrentFrameInFlightIndex],
				.size = RE_VK_CAMERA_MATRICES_SIZE_BYTES
			};
			if (!cameraBuffersMemory.cpu_coherent()) {
				PRINT_DEBUG("Invalidating Vulkan memory used for camera uniforms");
				if (vkInvalidateMappedMemoryRanges(vk_hDevice, 1, &vk_memoryRange) != VK_SUCCESS) {
					RE_FATAL_ERROR("Failed invalidating non-coherent Vulkan memory used for camera uniforms");
					return;
				}
			}
			PRINT_DEBUG("Updating camera matrices");
			apafCameraMatrices[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + 12] = -pActiveCamera->transform.position[0];
			apafCameraMatrices[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + 13] = -pActiveCamera->transform.position[1];
			apafCameraMatrices[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + 14] = -pActiveCamera->transform.position[2];
			apafCameraMatrices[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + 0] = 1.0f / pActiveCamera->view[0];
			apafCameraMatrices[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + 5] = -1.0f / pActiveCamera->view[1];
			apafCameraMatrices[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + 10] = 1.0f / pActiveCamera->fViewDistance;
			if (!cameraBuffersMemory.cpu_coherent()) {
				PRINT_DEBUG("Flushing Vulkan memory used for camera uniforms");
				if (vkFlushMappedMemoryRanges(vk_hDevice, 1, &vk_memoryRange) != VK_SUCCESS) {
					RE_FATAL_ERROR("Failed flushing non-coherent Vulkan memory used for camera uniforms");
					return;
				}
			}
			if (!pActiveCamera->bIgnoreAspectRatio) {
				PRINT_DEBUG("Calculating projection on screen depending on camera's projection's aspect ratio");
				const float fFactor = std::min(vk_renderImageSize.width / pActiveCamera->view[0], vk_renderImageSize.height / pActiveCamera->view[1]);
				vk_cameraProjectionOnscreen.extent.width = static_cast<uint32_t>(std::roundf(pActiveCamera->view[0] * fFactor));
				vk_cameraProjectionOnscreen.extent.height = static_cast<uint32_t>(std::roundf(pActiveCamera->view[1] * fFactor));
				vk_cameraProjectionOnscreen.offset.x = (vk_renderImageSize.width - vk_cameraProjectionOnscreen.extent.width) / 2;
				vk_cameraProjectionOnscreen.offset.y = (vk_renderImageSize.height - vk_cameraProjectionOnscreen.extent.height) / 2;
				return;
			}
		}
		vk_cameraProjectionOnscreen.offset.x = 0;
		vk_cameraProjectionOnscreen.offset.y = 0;
		vk_cameraProjectionOnscreen.extent = vk_renderImageSize;
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
				std::memcpy(apafCameraMatrices[u8CameraUniformBufferIndex], apafCameraMatrices[u8IndexToCopyFrom], RE_VK_CAMERA_MATRICES_SIZE_BYTES);
			}
		} else {
			PRINT_DEBUG("Attaching new camera ", pCam);
			pActiveCamera = pCam;
		}
	}

}
