#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX

	VkBuffer vk_hWindowFrameBuffer;
	VulkanMemory *pWindowFrameBufferMemory;
	WindowFrameUniformData *pWindowFrameUniformData = nullptr;
	VkDrawIndirectCommand *pIndirectDrawWindowTitle = nullptr;

	bool create_cursor_buffers() {
		PRINT_DEBUG("Creating Vulkan buffer for data of the cursor");
		if (create_vulkan_buffer(0,
				sizeof(WindowFrameUniformData) + sizeof(VkDrawIndirectCommand),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
				1,
				nullptr,
				&vk_hWindowFrameBuffer))
			return true;
		else
			RE_FATAL_ERROR("Failed to create a Vulkan buffer for data of the cursor");
		return false;
	}

	void destroy_cursor_buffers() {
		PRINT_DEBUG("Destroying Vulkan buffer used for cursor data");
		vkDestroyBuffer(vk_hDevice, vk_hWindowFrameBuffer, nullptr);
		pWindowFrameUniformData = nullptr;
	}

#else

	bool create_cursor_buffers() {
		return true;
	}

	void destroy_cursor_buffers() {
	}

#endif

}
