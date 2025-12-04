#include "RE_Renderer_Descriptor Sets_Internal.hpp"
#include "RE_Renderer_Internal.hpp"

namespace RE {

#define RE_VK_VIEW_MATRIX_SIZE (4 * 4)
#define RE_VK_VIEW_MATRIX_OFFSET 0
#define RE_VK_PROJECTION_MATRIX_SIZE (4 * 4)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE

#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE (RE_VK_VIEW_MATRIX_SIZE + RE_VK_PROJECTION_MATRIX_SIZE)
#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES (RE_VK_CAMERA_UNIFORM_BUFFER_SIZE * sizeof(float))

	const Camera *pActiveCamera = nullptr;
	static VkBuffer vk_hCameraUniformBuffer;
	static VkDeviceMemory vk_hCameraUniformBufferMemory;
	static float *pfCameraUniforms;
	VkDescriptorSetLayout vk_hCameraDescLayout;
	std::array<VkDescriptorSet, RE_VK_FRAMES_IN_FLIGHT> cameraDescSets;
	VkRect2D vk_cameraProjectionOnscreen;
	
	bool create_camera_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan uniform buffer for rendering with view and projection matrices");
		constexpr VkDeviceSize vk_cameraUniformBufferBytes = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES * RE_VK_FRAMES_IN_FLIGHT;
		if (create_vulkan_buffer(0, vk_cameraUniformBufferBytes, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk_hCameraUniformBuffer, &vk_hCameraUniformBufferMemory)) {
			PRINT_DEBUG("Mapping camera uniform buffer's memory ", vk_hCameraUniformBufferMemory);
			if (vkMapMemory(vk_hDevice, vk_hCameraUniformBufferMemory, 0, vk_cameraUniformBufferBytes, 0, reinterpret_cast<void**>(&pfCameraUniforms)) == VK_SUCCESS) {
				std::jthread cameraUniformBufferInitThread([&]() {
					for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
						for (uint8_t u8MatrixElementIndex = 0; u8MatrixElementIndex < RE_VK_VIEW_MATRIX_SIZE; u8MatrixElementIndex++)
							switch (u8MatrixElementIndex) {
								case 0:
								case 5:
								case 10:
								case 15:
									pfCameraUniforms[u8FrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_VIEW_MATRIX_OFFSET + u8MatrixElementIndex] = 1.0f;
									pfCameraUniforms[u8FrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_PROJECTION_MATRIX_OFFSET + u8MatrixElementIndex] = 1.0f;
									break;
								default:
									pfCameraUniforms[u8FrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_VIEW_MATRIX_OFFSET + u8MatrixElementIndex] = 0.0f;
									pfCameraUniforms[u8FrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_PROJECTION_MATRIX_OFFSET + u8MatrixElementIndex] = 0.0f;
									break;
							}
					}
				});
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
							vk_aDescBufferInfos[u8DescriptorSetIndex].offset = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES * u8DescriptorSetIndex;
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
				PRINT_DEBUG("Unmapping memory ", vk_hCameraUniformBufferMemory, " of camera uniform buffer ", vk_hCameraUniformBuffer, " due to failure creating camera descriptor sets");
				vkUnmapMemory(vk_hDevice, vk_hCameraUniformBufferMemory);
			} else
				RE_FATAL_ERROR("Failed mapping camera's uniform buffer's memory ", vk_hCameraUniformBufferMemory);
			PRINT_DEBUG("Destroying camera uniform buffer ", vk_hCameraUniformBuffer, " and its memory ", vk_hCameraUniformBufferMemory, " due to failure creating camera descriptor sets");
			vkDestroyBuffer(vk_hDevice, vk_hCameraUniformBuffer, nullptr);
			vkFreeMemory(vk_hDevice, vk_hCameraUniformBufferMemory, nullptr);
		} else
			RE_FATAL_ERROR("Failed creating Vulkan uniform buffer for the camera");
		return false;
	}

	void destroy_camera_descriptor_sets() {
		PRINT_DEBUG("Destroying Vulkan descriptor set layout ", vk_hCameraDescLayout);
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hCameraDescLayout, nullptr);
		PRINT_DEBUG("Destroying camera uniform buffer ", vk_hCameraUniformBuffer, " and its memory ", vk_hCameraUniformBufferMemory);
		vkUnmapMemory(vk_hDevice, vk_hCameraUniformBufferMemory);
		vkDestroyBuffer(vk_hDevice, vk_hCameraUniformBuffer, nullptr);
		vkFreeMemory(vk_hDevice, vk_hCameraUniformBufferMemory, nullptr);
	}

	void calculate_camera_matrices() {
		vk_cameraProjectionOnscreen.offset.x = 0;
		vk_cameraProjectionOnscreen.offset.y = 0;
		vk_cameraProjectionOnscreen.extent = vk_renderImageSize;
		if (pActiveCamera) {
			for (uint32_t u32DimensionIndex = 0; u32DimensionIndex < pActiveCamera->position.get_dimensions(); u32DimensionIndex++)
				pfCameraUniforms[u8CurrentFrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_VIEW_MATRIX_OFFSET + 3 + 4 * u32DimensionIndex] = pActiveCamera->position[u32DimensionIndex];
			constexpr float fNear = 100.0f,
				fFar = -100.0f,
				fDistance = fFar - fNear;
			const float fLeft = pActiveCamera->position[0] - pActiveCamera->view[0] / 2.0f,
				fRight = pActiveCamera->position[0] + pActiveCamera->view[0] / 2.0f,
				fTop = pActiveCamera->position[1] + pActiveCamera->view[1] / 2.0f,
				fBottom = pActiveCamera->position[1] - pActiveCamera->view[1] / 2.0f,
				fWidth = fRight - fLeft,
				fHeight = fTop - fBottom;
			pfCameraUniforms[u8CurrentFrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_PROJECTION_MATRIX_OFFSET + 0] = 2.0f / fWidth;
			pfCameraUniforms[u8CurrentFrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_PROJECTION_MATRIX_OFFSET + 5] = 2.0f / fHeight;
			pfCameraUniforms[u8CurrentFrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_PROJECTION_MATRIX_OFFSET + 10] = -2.0f / fDistance;
			pfCameraUniforms[u8CurrentFrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_PROJECTION_MATRIX_OFFSET + 3] = -(fRight + fLeft) / fWidth;
			pfCameraUniforms[u8CurrentFrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_PROJECTION_MATRIX_OFFSET + 7] = -(fTop + fBottom) / fHeight;
			pfCameraUniforms[u8CurrentFrameInFlightIndex * RE_VK_CAMERA_UNIFORM_BUFFER_SIZE + RE_VK_PROJECTION_MATRIX_OFFSET + 11] = -(fFar + fNear) / fDistance;
		}
	}

}
