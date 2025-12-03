#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_GPU.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

	Vulkan_CommandBuffer::Vulkan_CommandBuffer(const VkCommandPool vk_hCommandPool, const VkCommandBufferLevel vk_eCommandBufferLevel) : vk_hCommandBuffer(VK_NULL_HANDLE), vk_hCommandPool(vk_hCommandPool) {
		PRINT_DEBUG_CLASS("Constructing Vulkan command buffer wrapper");
		/*const VkCommandBufferAllocateInfo vk_commandBufferAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = vk_hCommandPool,
			.level = vk_eCommandBufferLevel,
			.commandBufferCount = 1U
		};
		if (vkAllocateCommandBuffers(vk_hDevice, &vk_commandBufferAllocInfo, &vk_hCommandBuffer) != VK_SUCCESS)
			RE_ERROR("Failed to allocate a new command buffer in command pool ", vk_hCommandPool);*/
	}
	
	Vulkan_CommandBuffer::~Vulkan_CommandBuffer() {
		PRINT_DEBUG_CLASS("Destructing Vulkan command buffer wrapper");
		if (!is_valid())
			return;
		PRINT_DEBUG_CLASS("Destroying Vulkan command buffer wrapper ", vk_hCommandBuffer);
		//vkFreeCommandBuffers(vk_hDevice, vk_hCommandPool, 1U, &vk_hCommandBuffer);
	}

	bool Vulkan_CommandBuffer::begin_recording(const VkCommandBufferUsageFlags vk_eUsageFlags, const VkCommandBufferInheritanceInfo *const vk_pInheritanceInfo) const {
		PRINT_DEBUG_CLASS("Beginning to record Vulkan command buffer wrapper ", vk_hCommandBuffer);
		/*const VkCommandBufferBeginInfo vk_beginRecordInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = vk_eUsageFlags,
			.pInheritanceInfo = vk_pInheritanceInfo
		};
		const bool bSuccess = vkBeginCommandBuffer(vk_hCommandBuffer, &vk_beginRecordInfo) == VK_SUCCESS;
		if (!bSuccess)
			RE_ERROR("Failed to begin recording command buffer ", vk_hCommandBuffer);
		return bSuccess;*/
		return true;
	}

	bool Vulkan_CommandBuffer::end_recording() const {
		PRINT_DEBUG_CLASS("Finishing to record Vulkan buffer wrapper ", vk_hCommandBuffer);
		/*const bool bSuccess = vkEndCommandBuffer(vk_hCommandBuffer) == VK_SUCCESS;
		if (!bSuccess)
			RE_ERROR("Failed to finish recording command buffer ", vk_hCommandBuffer);
		return bSuccess;*/
		return true;
	}

	[[nodiscard]]
	VkCommandBuffer Vulkan_CommandBuffer::get_command_buffer() const {
		return vk_hCommandBuffer;
	}

	[[nodiscard]]
	const VkCommandBuffer* Vulkan_CommandBuffer::get_command_buffer_ptr() const {
		return &vk_hCommandBuffer;
	}

	[[nodiscard]]
	bool Vulkan_CommandBuffer::is_valid() const {
		return vk_hCommandPool && vk_hCommandBuffer;
	}

	[[nodiscard]]
	Vulkan_CommandBuffer::operator VkCommandBuffer() const {
		return get_command_buffer();
	}

	[[nodiscard]]
	Vulkan_CommandBuffer::operator const VkCommandBuffer*() const {
		return get_command_buffer_ptr();
	}

}
