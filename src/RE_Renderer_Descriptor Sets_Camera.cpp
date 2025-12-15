#include "RE_Renderer_Descriptor Sets_Internal.hpp"
#include "RE_Renderer_Internal.hpp"
#include "RE_Main.hpp"

namespace RE {

#define RE_VK_VIEW_MATRIX_SIZE (4 * 4)
#define RE_VK_VIEW_MATRIX_OFFSET 0
#define RE_VK_PROJECTION_MATRIX_SIZE (4 * 4)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE

#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE (RE_VK_VIEW_MATRIX_SIZE + RE_VK_PROJECTION_MATRIX_SIZE)
#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES (RE_VK_CAMERA_UNIFORM_BUFFER_SIZE * sizeof(float))

	static Camera *pActiveCamera = nullptr;
	static VkBuffer vk_hCameraUniformBuffer;
	static VulkanMemory cameraUniformBufferMemory;
	static std::array<float*, RE_VK_FRAMES_IN_FLIGHT> cameraUniforms;
	VkDescriptorSetLayout vk_hCameraDescLayout;
	std::array<VkDescriptorSet, RE_VK_FRAMES_IN_FLIGHT> cameraDescSets;
	VkRect2D vk_cameraProjectionOnscreen;
	
	bool create_camera_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan uniform buffer for rendering with view and projection matrices");
		VkDeviceSize vk_alignmentOffsetBytes = next_multiple<VkDeviceSize>(RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES, vk_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
		if (!multiple_of(vk_alignmentOffsetBytes, sizeof(float)))
			vk_alignmentOffsetBytes = next_multiple<VkDeviceSize>(vk_alignmentOffsetBytes, sizeof(float));
		const VkDeviceSize vk_cameraUniformBufferBytes = (vk_alignmentOffsetBytes * (RE_VK_FRAMES_IN_FLIGHT - 1)) + RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES;
		if (create_vulkan_buffer(
				0,
				vk_cameraUniformBufferBytes,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				1,
				nullptr,
				RE_VK_CPU_RAM,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				&vk_hCameraUniformBuffer,
				&cameraUniformBufferMemory)) {
			PRINT_DEBUG("Mapping camera uniform buffer's memory");
			float *pafCameraUniformBuffer;
			if (cameraUniformBufferMemory.map(0, 0, vk_cameraUniformBufferBytes, reinterpret_cast<void**>(&pafCameraUniformBuffer))) {
				PRINT_DEBUG("Initializing camera matrices with default values");
				cameraUniforms[0] = pafCameraUniformBuffer;
				cameraUniforms[1] = pafCameraUniformBuffer + vk_alignmentOffsetBytes / sizeof(float);
				std::jthread cameraUniformBufferInitThread([&]() {
					for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
						for (uint8_t u8MatrixElementIndex = 0; u8MatrixElementIndex < RE_VK_VIEW_MATRIX_SIZE; u8MatrixElementIndex++)
							switch (u8MatrixElementIndex) {
								case 0:
								case 5:
								case 10:
								case 15:
									cameraUniforms[u8FrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + u8MatrixElementIndex] = 1.0f;
									cameraUniforms[u8FrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + u8MatrixElementIndex] = 1.0f;
									break;
								default:
									cameraUniforms[u8FrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + u8MatrixElementIndex] = 0.0f;
									cameraUniforms[u8FrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + u8MatrixElementIndex] = 0.0f;
									break;
							}
					}
				});
				PRINT_DEBUG("Creating Vulkan descriptor set layout for camera");
				const VkDescriptorSetLayoutBinding vk_camDescBinding = {
					.binding = RE_VK_UNIFORM_BINDING_CAMERA,
					.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
				};
				const VkDescriptorSetLayoutCreateInfo vk_camDescLayoutInfo = {
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
					.bindingCount = 1,
					.pBindings = &vk_camDescBinding
				};
				if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_camDescLayoutInfo, nullptr, &vk_hCameraDescLayout) == VK_SUCCESS) {
					PRINT_DEBUG("Allocating Vulkan descriptor sets for camera");
					std::array<VkDescriptorSetLayout, RE_VK_FRAMES_IN_FLIGHT> camDescLayoutArray;
					camDescLayoutArray.fill(vk_hCameraDescLayout);
					const VkDescriptorSetAllocateInfo vk_camDescSetAllocInfo = {
						.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
						.descriptorPool = vk_hPermanentDescPool,
						.descriptorSetCount = camDescLayoutArray.size(),
						.pSetLayouts = camDescLayoutArray.data()
					};
					if (vkAllocateDescriptorSets(vk_hDevice, &vk_camDescSetAllocInfo, cameraDescSets.data()) == VK_SUCCESS) {
						PRINT_DEBUG("Writing to Vulkan camera descriptor sets");
						VkDescriptorBufferInfo vk_aDescBufferInfos[RE_VK_FRAMES_IN_FLIGHT];
						VkWriteDescriptorSet vk_aWriteCamDescriptorSets[RE_VK_FRAMES_IN_FLIGHT];
						for (uint8_t u8DescriptorSetIndex = 0; u8DescriptorSetIndex < RE_VK_FRAMES_IN_FLIGHT; u8DescriptorSetIndex++) {
							vk_aDescBufferInfos[u8DescriptorSetIndex].buffer = vk_hCameraUniformBuffer;
							vk_aDescBufferInfos[u8DescriptorSetIndex].offset = vk_alignmentOffsetBytes * u8DescriptorSetIndex;
							vk_aDescBufferInfos[u8DescriptorSetIndex].range = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES;
							vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].pNext = nullptr;
							vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].dstSet = cameraDescSets[u8DescriptorSetIndex];
							vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].dstBinding = RE_VK_UNIFORM_BINDING_CAMERA;
							vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].dstArrayElement = 0;
							vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].descriptorCount = 1;
							vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
							vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].pImageInfo = nullptr;
							vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].pBufferInfo = &vk_aDescBufferInfos[u8DescriptorSetIndex];
							vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].pTexelBufferView = nullptr;
						}
						vkUpdateDescriptorSets(vk_hDevice, RE_VK_FRAMES_IN_FLIGHT, vk_aWriteCamDescriptorSets, 0, nullptr);
						return true;
					} else
						RE_FATAL_ERROR("Failed allocating ", RE_VK_FRAMES_IN_FLIGHT, " Vulkan descriptor sets for reading camera's uniform buffer");
					PRINT_DEBUG("Destroying Vulkan descriptor set layout ", vk_hCameraDescLayout, " due to failure allocating camera descriptor sets");
					vkDestroyDescriptorSetLayout(vk_hDevice, vk_hCameraDescLayout, nullptr);
				} else
					RE_FATAL_ERROR("Failed creating Vulkan descriptor set layout for camera uniforms");
				PRINT_DEBUG("Unmapping memory of camera uniform buffer ", vk_hCameraUniformBuffer, " due to failure creating camera descriptor sets");
				cameraUniformBufferMemory.unmap();
			} else
				RE_FATAL_ERROR("Failed mapping camera's uniform buffer's memory");
			PRINT_DEBUG("Destroying camera uniform buffer ", vk_hCameraUniformBuffer, " and its memory due to failure creating camera descriptor sets");
			vkDestroyBuffer(vk_hDevice, vk_hCameraUniformBuffer, nullptr);
			cameraUniformBufferMemory.free();
		} else
			RE_FATAL_ERROR("Failed creating Vulkan uniform buffer for the camera");
		return false;
	}

	void destroy_camera_descriptor_sets() {
		PRINT_DEBUG("Destroying Vulkan descriptor set layout ", vk_hCameraDescLayout);
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hCameraDescLayout, nullptr);
		PRINT_DEBUG("Destroying camera uniform buffer ", vk_hCameraUniformBuffer, " and its memory");
		cameraUniformBufferMemory.unmap();
		vkDestroyBuffer(vk_hDevice, vk_hCameraUniformBuffer, nullptr);
		cameraUniformBufferMemory.free();
	}

	void calculate_camera_matrices() {
		if (pActiveCamera) {
			PRINT_DEBUG("Updating camera before transferring its matrices");
			pActiveCamera->update_before_render();
			PRINT_DEBUG("Updating camera matrices");
			cameraUniforms[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + 12] = -pActiveCamera->transform.position[0];
			cameraUniforms[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + 13] = -pActiveCamera->transform.position[1];
			cameraUniforms[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + 14] = -pActiveCamera->transform.position[2];
			cameraUniforms[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + 0] = 1.0f / pActiveCamera->view[0];
			cameraUniforms[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + 5] = -1.0f / pActiveCamera->view[1];
			cameraUniforms[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + 10] = 1.0f / pActiveCamera->fViewDistance;
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
				std::memcpy(cameraUniforms[u8CameraUniformBufferIndex], cameraUniforms[u8IndexToCopyFrom], RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES);
			}
		} else {
			PRINT_DEBUG("Attaching new camera ", pCam);
			pActiveCamera = pCam;
		}
	}

}
