#include "RE_Renderer_Descriptor Sets_Internal.hpp"
#include "RE_Renderer_Internal.hpp"

namespace RE {

#define RE_VK_VIEW_MATRIX_SIZE (4 * 4)
#define RE_VK_VIEW_MATRIX_OFFSET 0
#define RE_VK_PROJECTION_MATRIX_SIZE (4 * 4)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE

#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE (RE_VK_VIEW_MATRIX_SIZE + RE_VK_PROJECTION_MATRIX_SIZE)
#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES (RE_VK_CAMERA_UNIFORM_BUFFER_SIZE * sizeof(float))

	static VkBuffer vk_hCameraUniformBuffer;
	static VkDeviceMemory vk_hCameraUniformBufferMemory;
	static float *pfCameraUniforms;
	VkDescriptorSetLayout vk_hCameraDescriptorSetLayout;
	static VkDescriptorPool vk_hCameraDescriptorPool;
	VkDescriptorSet vk_ahCameraDescriptorSets[RE_VK_FRAMES_IN_FLIGHT];
	
	bool create_camera_descriptor_sets() {
		PRINT_DEBUG("Creating Vulkan uniform buffer for rendering with view and projection matrices");
		constexpr VkDeviceSize vk_cameraUniformBufferBytes = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES * RE_VK_FRAMES_IN_FLIGHT;
		if (create_vulkan_buffer(vk_cameraUniformBufferBytes, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk_hCameraUniformBuffer, &vk_hCameraUniformBufferMemory)) {
			PRINT_DEBUG("Mapping camera uniform buffer's memory ", vk_hCameraUniformBufferMemory);
			if (vkMapMemory(vk_hDevice, vk_hCameraUniformBufferMemory, 0, vk_cameraUniformBufferBytes, 0, reinterpret_cast<void**>(&pfCameraUniforms)) == VK_SUCCESS) {
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
				PRINT_DEBUG("Creating Vulkan descriptor set layout for camera uniforms");
				constexpr VkDescriptorSetLayoutBinding vk_aCameraUniformBufferBindings[] = {
					{
						.binding = 0,
						.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						.descriptorCount = 1,
						.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
						.pImmutableSamplers = nullptr
					}
				};
				const VkDescriptorSetLayoutCreateInfo vk_cameraDescriptorLayoutCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
					.bindingCount = sizeof(vk_aCameraUniformBufferBindings) / sizeof(vk_aCameraUniformBufferBindings[0]),
					.pBindings = vk_aCameraUniformBufferBindings
				};
				if (vkCreateDescriptorSetLayout(vk_hDevice, &vk_cameraDescriptorLayoutCreateInfo, nullptr, &vk_hCameraDescriptorSetLayout) == VK_SUCCESS) {
					PRINT_DEBUG("Creating Vulkan descriptor pool for camera uniforms");
					constexpr VkDescriptorPoolSize vk_aDescPoolSizes[] = {
						{
							.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
							.descriptorCount = RE_VK_FRAMES_IN_FLIGHT
						}
					};
					const VkDescriptorPoolCreateInfo vk_camDescPoolCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
						.maxSets = RE_VK_FRAMES_IN_FLIGHT,
						.poolSizeCount = sizeof(vk_aDescPoolSizes) / sizeof(vk_aDescPoolSizes[0]),
						.pPoolSizes = vk_aDescPoolSizes
					};
					if (vkCreateDescriptorPool(vk_hDevice, &vk_camDescPoolCreateInfo, nullptr, &vk_hCameraDescriptorPool) == VK_SUCCESS) {
						PRINT_DEBUG("Allocating Vulkan descriptor sets for camera uniforms");
						VkDescriptorSetLayout vk_aCamDescSetLayouts[RE_VK_FRAMES_IN_FLIGHT];
						std::fill(std::begin(vk_aCamDescSetLayouts), std::end(vk_aCamDescSetLayouts), vk_hCameraDescriptorSetLayout);
						const VkDescriptorSetAllocateInfo vk_camDescSetAllocInfo = {
							.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
							.descriptorPool = vk_hCameraDescriptorPool,
							.descriptorSetCount = RE_VK_FRAMES_IN_FLIGHT,
							.pSetLayouts = vk_aCamDescSetLayouts
						};
						if (vkAllocateDescriptorSets(vk_hDevice, &vk_camDescSetAllocInfo, vk_ahCameraDescriptorSets) == VK_SUCCESS) {
							PRINT_DEBUG("Writing to Vulkan camera descriptor sets");
							VkDescriptorBufferInfo vk_aDescBufferInfos[RE_VK_FRAMES_IN_FLIGHT];
							VkWriteDescriptorSet vk_aWriteCamDescriptorSets[RE_VK_FRAMES_IN_FLIGHT];
							for (uint8_t u8DescriptorSetIndex = 0; u8DescriptorSetIndex < RE_VK_FRAMES_IN_FLIGHT; u8DescriptorSetIndex++) {
								vk_aDescBufferInfos[u8DescriptorSetIndex].buffer = vk_hCameraUniformBuffer;
								vk_aDescBufferInfos[u8DescriptorSetIndex].offset = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES * u8DescriptorSetIndex;
								vk_aDescBufferInfos[u8DescriptorSetIndex].range = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES;
								vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
								vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].pNext = nullptr;
								vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].dstSet = vk_ahCameraDescriptorSets[u8DescriptorSetIndex];
								vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].dstBinding = 0;
								vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].dstArrayElement = 0;
								vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].descriptorCount = 1;
								vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
								vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].pImageInfo = nullptr;
								vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].pBufferInfo = &vk_aDescBufferInfos[u8DescriptorSetIndex];
								vk_aWriteCamDescriptorSets[u8DescriptorSetIndex].pTexelBufferView = nullptr;
							}
							vkUpdateDescriptorSets(vk_hDevice, 1, vk_aWriteCamDescriptorSets, 0, nullptr);
							return true;
						} else
							RE_FATAL_ERROR("Failed allocating Vulkan descriptor sets in pool ", vk_hCameraDescriptorPool, " for camera uniform buffers");
						PRINT_DEBUG("Destroying Vulkan descriptor pool ", vk_hCameraDescriptorPool, " for camera due to failure allocating its sets");
						vkDestroyDescriptorPool(vk_hDevice, vk_hCameraDescriptorPool, nullptr);
					} else
						RE_FATAL_ERROR("Failed creating Vulkan descriptor set pool for allocating camera descriptor sets");
					PRINT_DEBUG("Destroying Vulkan camera descriptor set layout ", vk_hCameraDescriptorSetLayout, " due to failure allocating camera descriptor sets");
					vkDestroyDescriptorSetLayout(vk_hDevice, vk_hCameraDescriptorSetLayout, nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout for camera descriptor sets");
				PRINT_DEBUG("Unmapping camera uniform buffer's memory ", vk_hCameraUniformBufferMemory, " due to failure creating camera descriptor sets");
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
		PRINT_DEBUG("Destroying Vulkan descriptor pool ", vk_hCameraDescriptorPool, " for camera");
		vkDestroyDescriptorPool(vk_hDevice, vk_hCameraDescriptorPool, nullptr);
		PRINT_DEBUG("Destroying Vulkan camera descriptor set layout ", vk_hCameraDescriptorSetLayout);
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hCameraDescriptorSetLayout, nullptr);
		PRINT_DEBUG("Destroying camera uniform buffer ", vk_hCameraUniformBuffer, " and its memory ", vk_hCameraUniformBufferMemory);
		vkUnmapMemory(vk_hDevice, vk_hCameraUniformBufferMemory);
		vkDestroyBuffer(vk_hDevice, vk_hCameraUniformBuffer, nullptr);
		vkFreeMemory(vk_hDevice, vk_hCameraUniformBufferMemory, nullptr);
	}

}
