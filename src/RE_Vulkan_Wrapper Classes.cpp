#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Render System.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {
	
	Vulkan_Buffer::Vulkan_Buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *const pu32Queues, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags) : vk_hBuffer(VK_NULL_HANDLE), vk_hMemory(VK_NULL_HANDLE), vk_size(vk_size) {
		PUSH_TO_CALLSTACKTRACE(create_vulkan_buffer(vk_size, vk_eUsages, u32QueueCount, pu32Queues, vk_eMemoryPropertyFlags, &vk_hBuffer, &vk_hMemory));
	}
	
	Vulkan_Buffer::~Vulkan_Buffer() {
		if (!is_valid())
			return;
		vkFreeMemory(vk_hDevice, vk_hMemory, nullptr);
		vkDestroyBuffer(vk_hDevice, vk_hBuffer, nullptr);
	}

	bool Vulkan_Buffer::map_memory(void **ppData) const {
		return vkMapMemory(vk_hDevice, vk_hMemory, 0UL, vk_size, 0, ppData);
	}

	void Vulkan_Buffer::unmap_memory() const {
		vkUnmapMemory(vk_hDevice, vk_hMemory);
	}

	[[nodiscard]]
	VkBuffer Vulkan_Buffer::get_buffer() const {
		return vk_hBuffer;
	}

	[[nodiscard]]
	const VkBuffer* Vulkan_Buffer::get_buffer_ptr() const {
		return &vk_hBuffer;
	}
	
	[[nodiscard]]
	VkDeviceMemory Vulkan_Buffer::get_memory() const {
		return vk_hMemory;
	}

	[[nodiscard]]
	bool Vulkan_Buffer::is_valid() const {
		return vk_hBuffer && vk_hMemory;
	}

	[[nodiscard]]
	Vulkan_Buffer::operator VkBuffer() const {
		return get_buffer();
	}

	[[nodiscard]]
	Vulkan_Buffer::operator const VkBuffer*() const {
		return get_buffer_ptr();
	}

	[[nodiscard]]
	Vulkan_Buffer::operator VkDeviceMemory() const {
		return get_memory();
	}


	
	Vulkan_CommandBuffer::Vulkan_CommandBuffer(const VkCommandPool vk_hCommandPool, const VkCommandBufferLevel vk_eCommandBufferLevel) : vk_hCommandBuffer(VK_NULL_HANDLE), vk_hCommandPool(vk_hCommandPool) {
		const VkCommandBufferAllocateInfo vk_commandBufferAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = vk_hCommandPool,
			.level = vk_eCommandBufferLevel,
			.commandBufferCount = 1U
		};
		if (vkAllocateCommandBuffers(vk_hDevice, &vk_commandBufferAllocInfo, &vk_hCommandBuffer) != VK_SUCCESS)
			RE_ERROR("Failed to allocate a new command buffer in command pool ", vk_hCommandPool);
	}
	
	Vulkan_CommandBuffer::~Vulkan_CommandBuffer() {
		if(!is_valid())
			return;
		vkFreeCommandBuffers(vk_hDevice, vk_hCommandPool, 1U, &vk_hCommandBuffer);
	}

	bool Vulkan_CommandBuffer::begin_recording(const VkCommandBufferUsageFlags vk_eUsageFlags, const VkCommandBufferInheritanceInfo *const vk_pInheritanceInfo) const {
		const VkCommandBufferBeginInfo vk_beginRecordInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = vk_eUsageFlags,
			.pInheritanceInfo = vk_pInheritanceInfo
		};
		const bool bSuccess = vkBeginCommandBuffer(vk_hCommandBuffer, &vk_beginRecordInfo) == VK_SUCCESS;
		if (!bSuccess)
			RE_ERROR("Failed to begin recording command buffer ", vk_hCommandBuffer);
		return bSuccess;
	}

	bool Vulkan_CommandBuffer::end_recording() const {
		const bool bSuccess = vkEndCommandBuffer(vk_hCommandBuffer) == VK_SUCCESS;
		if (!bSuccess)
			RE_ERROR("Failed to finish recording command buffer ", vk_hCommandBuffer);
		return bSuccess;
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


	
	Vulkan_Fence::Vulkan_Fence() : Vulkan_Fence(0) {}
	Vulkan_Fence::Vulkan_Fence(const VkFenceCreateFlags vk_eCreateFlags) : vk_hFence(VK_NULL_HANDLE) {
		const VkFenceCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = vk_eCreateFlags
		};
		if (vkCreateFence(vk_hDevice, &vk_createInfo, nullptr, &vk_hFence) != VK_SUCCESS)
			RE_ERROR("Failed to create Vulkan fence");
	}
	
	Vulkan_Fence::~Vulkan_Fence() {
		if (!is_valid())
			return;
		vkDestroyFence(vk_hDevice, vk_hFence, nullptr);
	}

	bool Vulkan_Fence::wait_for() const {
		return vkWaitForFences(vk_hDevice, 1, &vk_hFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	}

	[[nodiscard]]
	VkFence Vulkan_Fence::get_fence() const {
		return vk_hFence;
	}

	[[nodiscard]]
	const VkFence* Vulkan_Fence::get_fence_ptr() const {
		return &vk_hFence;
	}
	
	[[nodiscard]]
	bool Vulkan_Fence::is_valid() const {
		return vk_hFence != VK_NULL_HANDLE;
	}

	[[nodiscard]]
	Vulkan_Fence::operator VkFence() const {
		return get_fence();
	}

	[[nodiscard]]
	Vulkan_Fence::operator const VkFence*() const {
		return get_fence_ptr();
	}

}
