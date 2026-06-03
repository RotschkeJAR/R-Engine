#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX

	VkBuffer vk_hCursorBuffer;
	VulkanMemory *pCursorBufferMemory;
	CursorShaderData *apCursorShaderData[RE_VK_FRAMES_IN_FLIGHT];

	bool create_cursor_buffers() {
		PRINT_DEBUG("Creating Vulkan buffer for data of the cursor");
		if (create_vulkan_buffer(0,
				next_multiple_inclusive<VkDeviceSize>(sizeof(CursorShaderData), vk_uniformBufferAlignment) + sizeof(CursorShaderData),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				1,
				nullptr,
				&vk_hCursorBuffer))
			return true;
		else
			RE_FATAL_ERROR("Failed to create a Vulkan buffer for data of the cursor");
		return false;
	}

	void destroy_cursor_buffers() {
		PRINT_DEBUG("Destroying Vulkan buffer used for cursor data");
		vkDestroyBuffer(vk_hDevice, vk_hCursorBuffer, nullptr);
	}

#else

	bool create_cursor_buffers() {
		return true;
	}

	void destroy_cursor_buffers() {
	}

#endif

}
