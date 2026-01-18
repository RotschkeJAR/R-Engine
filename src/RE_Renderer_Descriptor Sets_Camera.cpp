#include "RE_Renderer_Descriptor Sets_Internal.hpp"
#include "RE_Main.hpp"

namespace RE {

	static Camera *pActiveCamera = nullptr;
	VkDescriptorSet vk_ahCameraDescSets[RE_VK_FRAMES_IN_FLIGHT];
	VkRect2D vk_cameraProjectionOnscreen;
	
	bool create_camera_descriptor_sets() {
		PRINT_DEBUG("Initializing camera matrices with default values");
		std::jthread cameraUniformBufferInitThread([&]() {
			for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
				for (uint8_t u8MatrixElementIndex = 0; u8MatrixElementIndex < RE_VK_VIEW_MATRIX_SIZE; u8MatrixElementIndex++)
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
		});
		PRINT_DEBUG("Allocating Vulkan descriptor sets for camera");
		std::array<VkDescriptorSetLayout, RE_VK_FRAMES_IN_FLIGHT> camDescLayoutArray;
		camDescLayoutArray.fill(vk_hCameraDescSetLayout);
		const VkDescriptorSetAllocateInfo vk_camDescSetAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = vk_hPermanentDescPool,
			.descriptorSetCount = camDescLayoutArray.size(),
			.pSetLayouts = camDescLayoutArray.data()
		};
		if (vkAllocateDescriptorSets(vk_hDevice, &vk_camDescSetAllocInfo, vk_ahCameraDescSets) == VK_SUCCESS) {
			PRINT_DEBUG("Writing to Vulkan camera descriptor sets");
			VkDescriptorBufferInfo vk_aDescBufferInfos[RE_VK_FRAMES_IN_FLIGHT];
			VkWriteDescriptorSet vk_aWriteCamDescriptorSets[RE_VK_FRAMES_IN_FLIGHT];
			for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
				vk_aDescBufferInfos[u8FrameInFlightIndex].buffer = vk_aahUniformBuffers[RE_VK_CAMERA_UNIFORM_BUFFER_INDEX][u8FrameInFlightIndex];
				vk_aDescBufferInfos[u8FrameInFlightIndex].offset = 0;
				vk_aDescBufferInfos[u8FrameInFlightIndex].range = VK_WHOLE_SIZE;
				vk_aWriteCamDescriptorSets[u8FrameInFlightIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				vk_aWriteCamDescriptorSets[u8FrameInFlightIndex].pNext = nullptr;
				vk_aWriteCamDescriptorSets[u8FrameInFlightIndex].dstSet = vk_ahCameraDescSets[u8FrameInFlightIndex];
				vk_aWriteCamDescriptorSets[u8FrameInFlightIndex].dstBinding = RE_VK_UNIFORM_BINDING_CAMERA;
				vk_aWriteCamDescriptorSets[u8FrameInFlightIndex].dstArrayElement = 0;
				vk_aWriteCamDescriptorSets[u8FrameInFlightIndex].descriptorCount = 1;
				vk_aWriteCamDescriptorSets[u8FrameInFlightIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				vk_aWriteCamDescriptorSets[u8FrameInFlightIndex].pImageInfo = nullptr;
				vk_aWriteCamDescriptorSets[u8FrameInFlightIndex].pBufferInfo = &vk_aDescBufferInfos[u8FrameInFlightIndex];
				vk_aWriteCamDescriptorSets[u8FrameInFlightIndex].pTexelBufferView = nullptr;
			}
			vkUpdateDescriptorSets(vk_hDevice, RE_VK_FRAMES_IN_FLIGHT, vk_aWriteCamDescriptorSets, 0, nullptr);
			return true;
		} else
			RE_FATAL_ERROR("Failed allocating ", RE_VK_FRAMES_IN_FLIGHT, " Vulkan descriptor sets for reading camera's uniform buffer");
		return false;
	}

	void destroy_camera_descriptor_sets() {
	}

	void calculate_camera_matrices() {
		if (pActiveCamera) {
			PRINT_DEBUG("Updating camera before transferring its matrices");
			pActiveCamera->update_before_render();
			const VkMappedMemoryRange vk_memoryRange = {
				.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				.memory = uniformBuffersMemory.get(),
				.offset = vk_aaUniformByteOffsets[RE_VK_CAMERA_UNIFORM_BUFFER_INDEX][u8CurrentFrameInFlightIndex],
				.size = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES
			};
			if (!uniformBuffersMemory.cpu_coherent()) {
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
			if (!uniformBuffersMemory.cpu_coherent()) {
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

	void attach_camera(Camera *pCam) {
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
				std::memcpy(apafCameraMatrices[u8CameraUniformBufferIndex], apafCameraMatrices[u8IndexToCopyFrom], RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES);
			}
		} else {
			PRINT_DEBUG("Attaching new camera ", pCam);
			pActiveCamera = pCam;
		}
	}

}
