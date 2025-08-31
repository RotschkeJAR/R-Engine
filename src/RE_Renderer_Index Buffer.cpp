#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	VkBuffer vk_hRectIndexBuffer;
	VkDeviceMemory vk_hRectIndexBufferMemory;

	bool create_rect_index_buffer() {
		return true;
	}

	void destroy_rect_index_buffer() {
		vkFreeMemory(vk_hDevice, vk_hRectIndexBufferMemory, nullptr);
		vkDestroyBuffer(vk_hDevice, vk_hRectIndexBuffer, nullptr);
	}

}
