#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	VkBuffer vk_aahUniformBuffers[RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME][RE_VK_FRAMES_IN_FLIGHT];
	static VulkanMemory uniformBuffersMemory;

	bool create_uniform_buffers(UniformBufferInfo &rUniformBufferInfo) {
		VkMemoryRequirements vk_aaMemoryRequirements[RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME][RE_VK_FRAMES_IN_FLIGHT];
		uint32_t b32SuitableMemories = 0xFFFFFFFF;
		VkDeviceSize vk_requiredMemorySize = 0;
		VkBufferCreateInfo vk_bufferCreateInfo;
		vk_bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vk_bufferCreateInfo.pNext = nullptr;
		vk_bufferCreateInfo.flags = 0;
		vk_bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		uint32_t u32BufferCreateIndex = 0;
		uint8_t u8FrameInFlightCreateIndex = 0;
		std::vector<uint32_t> computeGraphicsQueueCollection;
		if (renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_PROCESSING) != renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_RENDERING)) {
			computeGraphicsQueueCollection.reserve(2);
			computeGraphicsQueueCollection.push_back(renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_PROCESSING));
			computeGraphicsQueueCollection.push_back(renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_RENDERING));
		}
		while (u32BufferCreateIndex < RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME) {
			switch (u32BufferCreateIndex) {
				case RE_VK_CAMERA_UNIFORM_BUFFER_INDEX:
					vk_bufferCreateInfo.size = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES;
					vk_bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
					break;
				case RE_VK_OBJECT_COUNT_UNIFORM_BUFFER_INDEX:
					vk_bufferCreateInfo.size = RE_VK_OBJECT_COUNT_UNIFORM_BUFFER_SIZE_BYTES;
					if (computeGraphicsQueueCollection.empty())
						vk_bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
					else {
						vk_bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
						vk_bufferCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(computeGraphicsQueueCollection.size());
						vk_bufferCreateInfo.pQueueFamilyIndices = computeGraphicsQueueCollection.data();
					}
					break;
				default:
					RE_ABORT("Unknown uniform buffer index ", u32BufferCreateIndex, " within frame at index ", u8FrameInFlightCreateIndex);
			}
			while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				if (vkCreateBuffer(vk_hDevice, &vk_bufferCreateInfo, nullptr, &vk_aahUniformBuffers[u32BufferCreateIndex][u8FrameInFlightCreateIndex]) == VK_SUCCESS) {
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
		if (uniformBuffersMemory.alloc(vk_requiredMemorySize, RE_VK_CPU_RAM, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, b32SuitableMemories)) {
			VkDeviceSize vk_currentByteInMemory = 0;
			for (uint8_t u8FrameInFlightBindingIndex = 0; u8FrameInFlightBindingIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightBindingIndex++)
				for (uint32_t u32BufferBindingIndex = 0; u32BufferBindingIndex < RE_VK_UNIFORM_BUFFER_COUNT_PER_FRAME; u32BufferBindingIndex++) {
					vk_currentByteInMemory = next_multiple<VkDeviceSize>(vk_currentByteInMemory, vk_aaMemoryRequirements[u32BufferBindingIndex][u8FrameInFlightCreateIndex].alignment);
					switch (u32BufferBindingIndex) {
						case RE_VK_CAMERA_UNIFORM_BUFFER_INDEX:
							rUniformBufferInfo.vk_aCameraFirstBytes[u8FrameInFlightBindingIndex] = vk_currentByteInMemory;
							break;
						case RE_VK_OBJECT_COUNT_UNIFORM_BUFFER_INDEX:
							rUniformBufferInfo.vk_aObjectCountFirstBytes[u8FrameInFlightBindingIndex] = vk_currentByteInMemory;
							break;
						default:
							RE_ABORT("Unknown buffer binding index ", u32BufferBindingIndex, " within frame at index ", u8FrameInFlightBindingIndex);
					}
					if (vkBindBufferMemory(vk_hDevice, vk_aahUniformBuffers[u32BufferBindingIndex][u8FrameInFlightCreateIndex], uniformBuffersMemory.get(), vk_currentByteInMemory) != VK_SUCCESS)
						goto DESTROY_UNIFORM_BUFFERS_DUE_TO_FAILURE;
					vk_currentByteInMemory += vk_aaMemoryRequirements[u32BufferBindingIndex][u8FrameInFlightCreateIndex].size;
				}
			if (vkMapMemory(vk_hDevice, uniformBuffersMemory.get(), 0, VK_WHOLE_SIZE, 0, &rUniformBufferInfo.pUniformBuffersContent) == VK_SUCCESS)
				return true;
		}

	DESTROY_UNIFORM_BUFFERS_DUE_TO_FAILURE:
		for (uint32_t u32BufferDestroyIndex = 0; u32BufferDestroyIndex < u32BufferCreateIndex; u32BufferDestroyIndex++)
			for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++)
				vkDestroyBuffer(vk_hDevice, vk_aahUniformBuffers[u32BufferDestroyIndex][u8FrameInFlightDestroyIndex], nullptr);
		if (uniformBuffersMemory.valid())
			uniformBuffersMemory.free();
		return false;
	}

	void destroy_uniform_buffers() {
		for (const VkBuffer (&vk_rahUniformBuffers)[RE_VK_FRAMES_IN_FLIGHT] : vk_aahUniformBuffers)
			for (const VkBuffer vk_hUniformBuffer : vk_rahUniformBuffers)
				vkDestroyBuffer(vk_hDevice, vk_hUniformBuffer, nullptr);
		vkUnmapMemory(vk_hDevice, uniformBuffersMemory.get());
		uniformBuffersMemory.free();
	}

}
