#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	VkBuffer vk_aahUniformBuffers[RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME][RE_VK_FRAMES_IN_FLIGHT];
	VulkanMemory uniformBuffersMemory;
	VkDeviceSize vk_aaUniformByteOffsets[RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME][RE_VK_FRAMES_IN_FLIGHT];
	float *apafCameraMatrices[RE_VK_FRAMES_IN_FLIGHT];

	bool create_uniform_buffers() {
		PRINT_DEBUG("Querying queues the uniform buffers will be used in");
		VkMemoryRequirements vk_aaMemoryRequirements[RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME][RE_VK_FRAMES_IN_FLIGHT];
		VkDeviceSize vk_requiredMemorySize = 0;
		uint32_t b32SuitableMemories = 0xFFFFFFFF;
		VkBufferCreateInfo vk_bufferCreateInfo;
		vk_bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vk_bufferCreateInfo.pNext = nullptr;
		vk_bufferCreateInfo.flags = 0;
		vk_bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		uint32_t u32BufferCreateIndex = 0;
		uint8_t u8FrameInFlightCreateIndex = 0;
		constexpr uint32_t au32FunctionIndices[] = {RENDER_TASK_SUBINDEX_PROCESSING, RENDER_TASK_SUBINDEX_RENDERING};
		const VulkanQueueCollection queueCollection = renderTasks[0].queues_of_functions(au32FunctionIndices, sizeof(au32FunctionIndices) / sizeof(au32FunctionIndices[0]));
		while (u32BufferCreateIndex < RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME) {
			switch (u32BufferCreateIndex) {
				case RE_VK_CAMERA_UNIFORM_BUFFER_INDEX:
					vk_bufferCreateInfo.size = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES;
					vk_bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
					break;
				default:
					RE_ABORT("Unknown uniform buffer index ", u32BufferCreateIndex, " within frame at index ", u8FrameInFlightCreateIndex);
			}
			while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				PRINT_DEBUG("Creating uniform buffer in Vulkan at index ", u32BufferCreateIndex, " and frame-in-flight ", u8FrameInFlightCreateIndex);
				if (vkCreateBuffer(vk_hDevice, &vk_bufferCreateInfo, nullptr, &vk_aahUniformBuffers[u32BufferCreateIndex][u8FrameInFlightCreateIndex]) == VK_SUCCESS) {
					PRINT_DEBUG("Receiving memory requirements of recently created uniform buffer ", vk_aahUniformBuffers[u32BufferCreateIndex][u8FrameInFlightCreateIndex], " at index ", u32BufferCreateIndex, " and frame-in-flight ", u8FrameInFlightCreateIndex);
					vkGetBufferMemoryRequirements(vk_hDevice, vk_aahUniformBuffers[u32BufferCreateIndex][u8FrameInFlightCreateIndex], &vk_aaMemoryRequirements[u32BufferCreateIndex][u8FrameInFlightCreateIndex]);
					vk_requiredMemorySize = next_multiple<VkDeviceSize>(vk_requiredMemorySize, vk_aaMemoryRequirements[u32BufferCreateIndex][u8FrameInFlightCreateIndex].alignment);
					vk_requiredMemorySize += vk_aaMemoryRequirements[u32BufferCreateIndex][u8FrameInFlightCreateIndex].size;
					b32SuitableMemories &= vk_aaMemoryRequirements[u32BufferCreateIndex][u8FrameInFlightCreateIndex].memoryTypeBits;
					if (!b32SuitableMemories) {
						RE_FATAL_ERROR("Failed to determine an exact memory type for uniform buffers in Vulkan");
						vkDestroyBuffer(vk_hDevice, vk_aahUniformBuffers[u32BufferCreateIndex][u8FrameInFlightCreateIndex], nullptr);
						goto DESTROY_UNIFORM_BUFFERS_DUE_TO_FAILURE;
					}
					u8FrameInFlightCreateIndex++;
				} else {
					RE_FATAL_ERROR("Failed to create a uniform buffer in Vulkan at index ", u32BufferCreateIndex, " for frame at index ", u8FrameInFlightCreateIndex);
					goto DESTROY_UNIFORM_BUFFERS_DUE_TO_FAILURE;
				}
			}
			u8FrameInFlightCreateIndex = 0;
			u32BufferCreateIndex++;
		}
		PRINT_DEBUG("Allocating Vulkan memory for uniform buffers");
		if (uniformBuffersMemory.alloc(vk_requiredMemorySize, RE_VK_CPU_RAM, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, b32SuitableMemories)) {
			VkDeviceSize vk_currentByteInMemory = 0;
			for (uint8_t u8FrameInFlightBindingIndex = 0; u8FrameInFlightBindingIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightBindingIndex++)
				for (uint32_t u32BufferBindingIndex = 0; u32BufferBindingIndex < RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME; u32BufferBindingIndex++) {
					PRINT_DEBUG("Binding memory to uniform buffer at index ", u32BufferBindingIndex, " and frame-in-flight ", u8FrameInFlightBindingIndex);
					vk_currentByteInMemory = next_multiple<VkDeviceSize>(vk_currentByteInMemory, vk_aaMemoryRequirements[u32BufferBindingIndex][u8FrameInFlightBindingIndex].alignment);
					vk_aaUniformByteOffsets[u32BufferBindingIndex][u8FrameInFlightBindingIndex] = vk_currentByteInMemory;
					if (vkBindBufferMemory(vk_hDevice, vk_aahUniformBuffers[u32BufferBindingIndex][u8FrameInFlightBindingIndex], uniformBuffersMemory.get(), vk_currentByteInMemory) != VK_SUCCESS) {
						RE_FATAL_ERROR("Failed mapping memory for uniform buffers");
						goto DESTROY_UNIFORM_BUFFERS_DUE_TO_FAILURE;
					}
					vk_currentByteInMemory += vk_aaMemoryRequirements[u32BufferBindingIndex][u8FrameInFlightBindingIndex].size;
				}
			PRINT_DEBUG("Mapping Vulkan memory bound to uniforms");
			void *pMemoryContent;
			if (vkMapMemory(vk_hDevice, uniformBuffersMemory.get(), 0, VK_WHOLE_SIZE, 0, &pMemoryContent) == VK_SUCCESS) {
				PRINT_DEBUG("Calculating pointer to uniform buffer content");
				for (uint8_t u8FrameInFlightMappingIndex = 0; u8FrameInFlightMappingIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightMappingIndex++)
					apafCameraMatrices[u8FrameInFlightMappingIndex] = reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(pMemoryContent) + vk_aaUniformByteOffsets[RE_VK_CAMERA_UNIFORM_BUFFER_INDEX][u8FrameInFlightMappingIndex]);
				return true;
			} else
				RE_FATAL_ERROR("Failed mapping Vulkan memory to access uniform buffers' contents");
		} else
			RE_FATAL_ERROR("Failed allocating Vulkan memory for uniform buffers");

	DESTROY_UNIFORM_BUFFERS_DUE_TO_FAILURE:
		for (uint32_t u32BufferDestroyIndex = 0; u32BufferDestroyIndex < u32BufferCreateIndex; u32BufferDestroyIndex++)
			for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++) {
				PRINT_DEBUG("Destroying Vulkan buffer at index ", u32BufferDestroyIndex, " and frame-in-flight ", u8FrameInFlightDestroyIndex, " due to failure creating uniform buffers");
				vkDestroyBuffer(vk_hDevice, vk_aahUniformBuffers[u32BufferDestroyIndex][u8FrameInFlightDestroyIndex], nullptr);
			}
		if (uniformBuffersMemory.valid()) {
			PRINT_DEBUG("Freeing Vulkan memory used for uniform buffers due to them being all destroyed");
			uniformBuffersMemory.free();
		}
		return false;
	}

	void destroy_uniform_buffers() {
		for (uint32_t u32BufferDestroyIndex = 0; u32BufferDestroyIndex < RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME; u32BufferDestroyIndex++)
			for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightDestroyIndex++) {
				PRINT_DEBUG("Destroying uniform buffer at index ", u32BufferDestroyIndex, " and frame-in-flight ", u8FrameInFlightDestroyIndex);
				vkDestroyBuffer(vk_hDevice, vk_aahUniformBuffers[u32BufferDestroyIndex][u8FrameInFlightDestroyIndex], nullptr);
			}
		PRINT_DEBUG("Unmapping and freeing Vulkan memory used for uniform buffers");
		vkUnmapMemory(vk_hDevice, uniformBuffersMemory.get());
		uniformBuffersMemory.free();
	}

}
