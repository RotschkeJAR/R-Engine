#include "RE_Renderer_Mesh_Internal.hpp"

namespace RE {
	
	static VulkanMemory permanentMeshMemory;

	bool alloc_memory_for_permanent_meshes() {
		PRINT_DEBUG("Fetching memory requirements of both mesh buffers");
		VkMemoryRequirements vk_aMemoryRequirements[PERMANENT_MESH_COUNT];
		vkGetBufferMemoryRequirements(vk_hDevice, square2D.vk_hMeshBuffer, &vk_aMemoryRequirements[0]);
		vkGetBufferMemoryRequirements(vk_hDevice, square3D.vk_hMeshBuffer, &vk_aMemoryRequirements[1]);
		VkDeviceSize vk_size = 0;
		for (const VkMemoryRequirements &vk_rMemoryRequirements : vk_aMemoryRequirements)
			vk_size = next_multiple_inclusive<VkDeviceSize>(vk_size, vk_rMemoryRequirements.alignment) + vk_rMemoryRequirements.size;
		PRINT_DEBUG("Allocating memory for both mesh buffers in Vulkan");
		if (permanentMeshMemory.alloc(
				vk_size,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				vk_aMemoryRequirements[0].memoryTypeBits & vk_aMemoryRequirements[1].memoryTypeBits) == VK_SUCCESS) {
			PRINT_DEBUG("Binding memory to both mesh buffers in Vulkan");
			const VkBindBufferMemoryInfo vk_aBindMemories[PERMANENT_MESH_COUNT] = {
				{
					.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
					.pNext = nullptr,
					.buffer = square2D.vk_hMeshBuffer,
					.memory = permanentMeshMemory(),
					.memoryOffset = 0
				}, {
					.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
					.pNext = nullptr,
					.buffer = square3D.vk_hMeshBuffer,
					.memory = permanentMeshMemory(),
					.memoryOffset = next_multiple_inclusive<VkDeviceSize>(vk_aBindMemories[0].memoryOffset + vk_aMemoryRequirements[0].size, vk_aMemoryRequirements[1].alignment)
				}
			};
			if (vkBindBufferMemory2(vk_hDevice, PERMANENT_MESH_COUNT, vk_aBindMemories) == VK_SUCCESS) {
				return true; 
			} else
				RE_FATAL_ERROR("Failed to bind memory to the permanent meshes");
			PRINT_DEBUG("Freeing memory used for the permanent mesh buffers due to failure binding them");
			permanentMeshMemory.free();
		} else
			RE_FATAL_ERROR("Failed to allocate memory for permanent meshes");
		return false;
	}

	void free_memory_for_permanent_meshes() {
		PRINT_DEBUG("Freeing memory of permanent mesh buffers in Vulkan");
		permanentMeshMemory.free();
	}

}
