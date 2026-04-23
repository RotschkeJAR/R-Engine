#include "RE_Vulkan_Wrappers.hpp"

namespace RE {
	
	Vulkan_SparseBuffer::Vulkan_SparseBuffer() : vk_hSparseBuffer(VK_NULL_HANDLE) {}
	Vulkan_SparseBuffer::Vulkan_SparseBuffer(VkBufferCreateFlags vk_eFlags,
					VkDeviceSize vk_size,
					VkBufferUsageFlags vk_eUsages,
					uint32_t u32QueueCount,
					const uint32_t *const pau32Queues,
					VkMemoryPropertyFlags vk_eMemoryPropertyFlags) : vk_hSparseBuffer(VK_NULL_HANDLE) {
		create(vk_eFlags, vk_size, vk_eUsages, u32QueueCount, pau32Queues, vk_eMemoryPropertyFlags);
	}
	Vulkan_SparseBuffer::Vulkan_SparseBuffer(Vulkan_SparseBuffer &&rrCopy) : vk_hSparseBuffer(rrCopy.vk_hSparseBuffer), memories(std::move(rrCopy.memories)) {
		rrCopy.vk_hSparseBuffer = VK_NULL_HANDLE;
	}
	Vulkan_SparseBuffer::~Vulkan_SparseBuffer() {}

	bool Vulkan_SparseBuffer::create(VkBufferCreateFlags vk_eFlags,
					VkDeviceSize vk_size,
					VkBufferUsageFlags vk_eUsages,
					uint32_t u32QueueCount,
					const uint32_t *pau32Queues,
					VkMemoryPropertyFlags vk_eMemoryPropertyFlags) {
		if (create_vulkan_buffer(vk_eFlags | VK_BUFFER_CREATE_SPARSE_BINDING_BIT,
				vk_size,
				vk_eUsages,
				u32QueueCount,
				pau32Queues,
				vk_eMemoryPropertyFlags)) {
			vkDestroyBuffer(vk_hDevice, vk_hSparseBuffer, nullptr);
		}
		return false;
	}

	void Vulkan_SparseBuffer::destroy() {
		vkDestroyBuffer(vk_hDevice, vk_hSparseBuffer, nullptr);
		memories.reset();
	}
	
	VkBuffer Vulkan_SparseBuffer::get() const {
		return vk_hSparseBuffer;
	}
	
	VkBuffer* Vulkan_SparseBuffer::get_ptr() const {
		return std::addressof(vk_hSparseBuffer);
	}
	
	bool Vulkan_SparseBuffer::valid() const {
		return vk_hSparseBuffer != VK_NULL_HANDLE && static_cast<bool>(memories);
	}

	Vulkan_SparseBuffer::operator VkBuffer() const {
		return get();
	}
	
	Vulkan_SparseBuffer::operator const VkBuffer*() const {
		return get_ptr();
	}
	
	Vulkan_SparseBuffer::operator bool() const {
		return valid();
	}
	
	VkBuffer Vulkan_SparseBuffer::operator()() const {
		return vk_hSparseBuffer;
	}

}
