#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {
	
	bool alloc_vulkan_command_buffers(const VkCommandPool vk_hCommandPool, const VkCommandBufferLevel vk_eLevel, const uint32_t u32Count, VkCommandBuffer *const vk_pahCommandBuffers) {
		const VkCommandBufferAllocateInfo vk_allocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = vk_hCommandPool,
			.level = vk_eLevel,
			.commandBufferCount = u32Count
		};
		if (vkAllocateCommandBuffers(vk_hDevice, &vk_allocInfo, vk_pahCommandBuffers) == VK_SUCCESS)
			return true;
		RE_ERROR("Failed to allocate ", u32Count, " Vulkan command buffer(s)");
		return false;
	}

	bool begin_recording_vulkan_command_buffer(
			const VkCommandBuffer vk_hCommandBuffer, 
			const VkCommandBufferUsageFlags vk_eUsage, 
			const VkCommandBufferInheritanceInfo *const vk_pInheritance) {
		const VkCommandBufferBeginInfo vk_beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = vk_eUsage,
			.pInheritanceInfo = vk_pInheritance
		};
		if (vkBeginCommandBuffer(vk_hCommandBuffer, &vk_beginInfo) == VK_SUCCESS)
			return true;
		RE_ERROR("Failed to begin recording Vulkan command buffer ", vk_hCommandBuffer);
		return false;
	}

}
