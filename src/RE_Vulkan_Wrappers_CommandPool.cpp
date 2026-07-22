#include "RE_Vulkan_Wrappers.hpp"

namespace RE {

	bool create_vulkan_command_pool(VkCommandPoolCreateFlags vk_mCreateFlags, uint32_t u32QueueFamilyIndex, VkCommandPool *vk_phCommandPool) {
		PRINT_DEBUG("Creating a Vulkan command pool");
		const VkCommandPoolCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = vk_mCreateFlags,
			.queueFamilyIndex = u32QueueFamilyIndex
		};
		if (vkCreateCommandPool(vk_hDevice, &vk_createInfo, nullptr, vk_phCommandPool) == VK_SUCCESS)
			return true;
		else
			RE_ERROR("Failed to create a Vulkan command pool");
		return false;
	}
	
	Vulkan_CommandPool::Vulkan_CommandPool() : vk_hCommandPool(VK_NULL_HANDLE) {}

	Vulkan_CommandPool::Vulkan_CommandPool(VkCommandPoolCreateFlags vk_mCreateFlags, uint32_t u32QueueFamilyIndex) : Vulkan_CommandPool() {
		PRINT_DEBUG("Constructing Vulkan command pool wrapper");
		create(vk_mCreateFlags, u32QueueFamilyIndex);
	}

	Vulkan_CommandPool::Vulkan_CommandPool(Vulkan_CommandPool &&rrCopy) : vk_hCommandPool(rrCopy.vk_hCommandPool) {
		rrCopy.vk_hCommandPool = VK_NULL_HANDLE;
	}

	Vulkan_CommandPool::~Vulkan_CommandPool() {
		PRINT_DEBUG("Destructing Vulkan command pool wrapper ", vk_hCommandPool);
		destroy();
	}

	bool Vulkan_CommandPool::create(VkCommandPoolCreateFlags vk_mCreateFlags, uint32_t u32QueueFamilyIndex) {
	#ifndef NDEBUG
		if (valid())
			RE_ERROR("Creating another Vulkan command pool wrapper, when the old command pool ", vk_hCommandPool, " hasn't been destroyed yet");
	#endif
		PRINT_DEBUG("Creating Vulkan command pool wrapper");
		return create_vulkan_command_pool(vk_mCreateFlags, u32QueueFamilyIndex, &vk_hCommandPool);
	}
	
	void Vulkan_CommandPool::destroy() {
		PRINT_DEBUG("Destroying Vulkan command pool wrapper ", vk_hCommandPool);
		vkDestroyCommandPool(vk_hDevice, vk_hCommandPool, nullptr);
		vk_hCommandPool = VK_NULL_HANDLE;
	}

	VkCommandPool Vulkan_CommandPool::get() const {
		return vk_hCommandPool;
	}

	const VkCommandPool* Vulkan_CommandPool::get_ptr() const {
		return std::addressof(vk_hCommandPool);
	}

	bool Vulkan_CommandPool::valid() const {
		return vk_hCommandPool != VK_NULL_HANDLE;
	}

	Vulkan_CommandPool::operator VkCommandPool() const {
		return get();
	}

	Vulkan_CommandPool::operator const VkCommandPool*() const {
		return get_ptr();
	}

	Vulkan_CommandPool::operator bool() const {
		return valid();
	}

	VkCommandPool Vulkan_CommandPool::operator()() const {
		return get();
	}

}
