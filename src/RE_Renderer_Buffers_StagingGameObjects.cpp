#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {
	
	VkBuffer vk_hStagingGameObjectsBuffer;
	VulkanMemory *pStagingGameObjectsBufferMemory;
	GameObjectShaderData *paStagingGameObjectsBufferData;

	bool create_staging_game_objects_buffer() {
		PRINT_DEBUG("Creating staging game object buffer in Vulkan");
		if (create_vulkan_buffer(0,
				get_max_game_object_count() * sizeof(GameObjectShaderData),
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				1,
				nullptr,
				&vk_hStagingGameObjectsBuffer))
			return true;
		else
			RE_FATAL_ERROR("Failed to create staging game object buffer in Vulkan");
		return false;
	}

	void destroy_staging_game_objects_buffer() {
		PRINT_DEBUG("Destroying staging game object buffer in Vulkan");
		vkDestroyBuffer(vk_hDevice, vk_hStagingGameObjectsBuffer, nullptr);
	}

}
