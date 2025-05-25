#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_Buffer::Vulkan_Buffer(const VkDeviceSize vk_sizeInBytes, const VkBufferUsageFlags vk_eUsage, const uint32_t u32QueueTypeCount, const uint32_t *pu32BufferQueueTypeIndices, const VkMemoryPropertyFlags vk_eMemoryProperties) : vk_hBuffer(VK_NULL_HANDLE), vk_hMemory(VK_NULL_HANDLE), vk_sizeInBytes(vk_sizeInBytes) {
		if (!vk_create_buffer(vk_sizeInBytes, vk_eUsage, u32QueueTypeCount, pu32BufferQueueTypeIndices, vk_eMemoryProperties, &vk_hBuffer, &vk_hMemory)) {
			vk_hBuffer = VK_NULL_HANDLE;
			vk_hMemory = VK_NULL_HANDLE;
			return;
		}
	}
	
	Vulkan_Buffer::~Vulkan_Buffer() {
		if (!is_valid())
			return;
		vk_destroy_buffer(vk_hBuffer, vk_hMemory);
	}
	
	bool Vulkan_Buffer::map_memory(void **ppData) const {
		return CHECK_VK_RESULT(vkMapMemory(vk_hDevice, vk_hMemory, 0UL, vk_sizeInBytes, 0, ppData));
	}
	
	void Vulkan_Buffer::unmap_memory() const {
		CATCH_SIGNAL(vkUnmapMemory(vk_hDevice, vk_hMemory));
	}
	
	bool Vulkan_Buffer::is_valid() const {
		return vk_hBuffer && vk_hMemory;
	}
	
	VkBuffer Vulkan_Buffer::get_buffer_handle() const {
		return vk_hBuffer;
	}
	
	VkDeviceMemory Vulkan_Buffer::get_memory_handle() const {
		return vk_hMemory;
	}
	
	VkBuffer const* Vulkan_Buffer::get_buffer_handle_ptr() const {
		return &vk_hBuffer;
	}
	
	VkDeviceMemory const* Vulkan_Buffer::get_memory_handle_ptr() const {
		return &vk_hMemory;
	}

	Vulkan_Buffer::operator VkBuffer() const {
		return get_buffer_handle();
	}
	
	Vulkan_Buffer::operator VkDeviceMemory() const {
		return get_memory_handle();
	}
	
	Vulkan_Buffer::operator VkBuffer const*() const {
		return get_buffer_handle_ptr();
	}
	
	Vulkan_Buffer::operator VkDeviceMemory const*() const {
		return get_memory_handle_ptr();
	}



	Vulkan_Semaphore::Vulkan_Semaphore() : vk_hSemaphore(VK_NULL_HANDLE) {
		if (!vk_create_semaphore(&vk_hSemaphore))
			vk_hSemaphore = VK_NULL_HANDLE;
	}
	
	Vulkan_Semaphore::~Vulkan_Semaphore() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroySemaphore(vk_hDevice, vk_hSemaphore, nullptr));
	}
	
	bool Vulkan_Semaphore::signal() const {
		return vk_signal_semaphores(1U, &vk_hSemaphore);
	}
	
	bool Vulkan_Semaphore::is_valid() const {
		return vk_hSemaphore != VK_NULL_HANDLE;
	}
	
	VkSemaphore Vulkan_Semaphore::get_semaphore_handle() const {
		return vk_hSemaphore;
	}
	
	VkSemaphore const* Vulkan_Semaphore::get_semaphore_handle_ptr() const {
		return &vk_hSemaphore;
	}

	Vulkan_Semaphore::operator VkSemaphore() const {
		return get_semaphore_handle();
	}
	
	Vulkan_Semaphore::operator VkSemaphore const*() const {
		return get_semaphore_handle_ptr();
	}



	Vulkan_Fence::Vulkan_Fence() : vk_hFence(VK_NULL_HANDLE) {
		if (!vk_create_fence(0, &vk_hFence))
			vk_hFence = VK_NULL_HANDLE;
	}
	
	Vulkan_Fence::Vulkan_Fence(const VkFenceCreateFlags vk_eCreateFlags) : vk_hFence(VK_NULL_HANDLE) {
		if (!vk_create_fence(vk_eCreateFlags, &vk_hFence))
			vk_hFence = VK_NULL_HANDLE;
	}
	
	Vulkan_Fence::~Vulkan_Fence() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyFence(vk_hDevice, vk_hFence, nullptr));
	}

	void Vulkan_Fence::wait_for() const {
		vk_wait_for_fence(&vk_hFence);
	}
	
	void Vulkan_Fence::reset() const {
		vk_reset_fence(&vk_hFence);
	}

	void Vulkan_Fence::wait_for_and_reset() const {
		vk_wait_for_and_reset_fence(&vk_hFence);
	}
	
	bool Vulkan_Fence::signal() const {
		return vk_signal_fence(vk_hFence);
	}
	
	bool Vulkan_Fence::is_valid() const {
		return vk_hFence != VK_NULL_HANDLE;
	}
	
	VkFence Vulkan_Fence::get_fence_handle() const {
		return vk_hFence;
	}
	
	VkFence const* Vulkan_Fence::get_fence_handle_ptr() const {
		return &vk_hFence;
	}

	Vulkan_Fence::operator VkFence() const {
		return get_fence_handle();
	}
	
	Vulkan_Fence::operator VkFence const*() const {
		return get_fence_handle_ptr();
	}



	Vulkan_CommandBuffer::Vulkan_CommandBuffer(const VkCommandPool vk_hCommandPool, const VkCommandBufferLevel vk_eCommandBufferLevel) : vk_hCommandBuffer(VK_NULL_HANDLE), vk_hCommandPool(vk_hCommandPool) {
		if (!vk_alloc_command_buffers(vk_hCommandPool, vk_eCommandBufferLevel, 1U, &vk_hCommandBuffer))
			vk_hCommandBuffer = VK_NULL_HANDLE;
	}
	
	Vulkan_CommandBuffer::~Vulkan_CommandBuffer() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkFreeCommandBuffers(vk_hDevice, vk_hCommandPool, 1U, &vk_hCommandBuffer));
	}

	bool Vulkan_CommandBuffer::begin_recording(const VkCommandBufferUsageFlags vk_eUsageFlags, const VkCommandBufferInheritanceInfo *vk_pInheritanceInfo) const {
		return vk_begin_recording_command_buffer(vk_hCommandBuffer, vk_eUsageFlags, vk_pInheritanceInfo);
	}
	
	void Vulkan_CommandBuffer::end_recording() const {
		CATCH_SIGNAL(vkEndCommandBuffer(vk_hCommandBuffer));
	}

	void Vulkan_CommandBuffer::cmd_copy_buffers(const VkBuffer vk_hSrcBuffer, const VkBuffer vk_hDstBuffer, const VkDeviceSize vk_bytesToCopy) const {
		VkBufferCopy vk_copyRegion = {
			.size = vk_bytesToCopy
		};
		CATCH_SIGNAL(vkCmdCopyBuffer(vk_hCommandBuffer, vk_hSrcBuffer, vk_hDstBuffer, 1U, &vk_copyRegion));
	}
	
	bool Vulkan_CommandBuffer::is_valid() const {
		return vk_hCommandBuffer != VK_NULL_HANDLE;
	}
	
	VkCommandBuffer Vulkan_CommandBuffer::get_command_buffer_handle() const {
		return vk_hCommandBuffer;
	}
	
	VkCommandBuffer const* Vulkan_CommandBuffer::get_command_buffer_handle_ptr() const {
		return &vk_hCommandBuffer;
	}

	Vulkan_CommandBuffer::operator VkCommandBuffer() const {
		return get_command_buffer_handle();
	}
	
	Vulkan_CommandBuffer::operator VkCommandBuffer const*() const {
		return get_command_buffer_handle_ptr();
	}

}
