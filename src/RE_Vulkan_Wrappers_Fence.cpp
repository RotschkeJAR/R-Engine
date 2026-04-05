#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {

	bool create_vulkan_fence(const VkFenceCreateFlags vk_mFlags, VkFence *const vk_phFence) {
		PRINT_DEBUG("Creating a Vulkan fence");
		const VkFenceCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = vk_mFlags
		};
		if (vkCreateFence(vk_hDevice, &vk_createInfo, nullptr, vk_phFence) == VK_SUCCESS)
			return true;
		RE_ERROR("Failed to create a Vulkan fence");
		return false;
	}

	Vulkan_Fence::Vulkan_Fence() : vk_hFence(VK_NULL_HANDLE) {}

	Vulkan_Fence::Vulkan_Fence(const VkFenceCreateFlags vk_mFlags) : Vulkan_Fence() {
		PRINT_DEBUG_CLASS("Constructing Vulkan fence wrapper");
		create(vk_mFlags);
	}

	Vulkan_Fence::Vulkan_Fence(Vulkan_Fence &&rrCopy) : vk_hFence(rrCopy.vk_hFence) {
		PRINT_DEBUG_CLASS("Constructing Vulkan fence wrapper by moving ownership from another wrapper");
		rrCopy.vk_hFence = VK_NULL_HANDLE;
	}
	
	Vulkan_Fence::~Vulkan_Fence() {
		PRINT_DEBUG_CLASS("Destructing Vulkan fence wrapper");
		destroy();
	}

	bool Vulkan_Fence::create(const VkFenceCreateFlags vk_mFlags) {
#ifndef RE_DISABLE_PRINT_DEBUGS
		if (valid())
			RE_ERROR("Creating another Vulkan fence wrapper, when the old fence ", vk_hFence, " hasn't been destroyed yet");
#endif
		PRINT_DEBUG_CLASS("Creating Vulkan fence wrapper");
		return create_vulkan_fence(vk_mFlags, &vk_hFence);
	}

	void Vulkan_Fence::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan fence wrapper");
		vkDestroyFence(vk_hDevice, vk_hFence, nullptr);
		vk_hFence = VK_NULL_HANDLE;
	}

	VkResult Vulkan_Fence::wait_for(const uint64_t u64Timeout) const {
		PRINT_DEBUG_CLASS("Waiting for Vulkan fence wrapper for ", u64Timeout, " nanoseconds");
		return vkWaitForFences(vk_hDevice, 1, &vk_hFence, VK_TRUE, u64Timeout);
	}

	void Vulkan_Fence::reset() const {
		PRINT_DEBUG_CLASS("Reseting Vulkan fence wrapper");
		vkResetFences(vk_hDevice, 1, &vk_hFence);
	}

	VkFence Vulkan_Fence::get() const {
		return vk_hFence;
	}

	const VkFence* Vulkan_Fence::get_ptr() const {
		return &vk_hFence;
	}
	
	bool Vulkan_Fence::valid() const {
		return vk_hFence != VK_NULL_HANDLE;
	}

	Vulkan_Fence::operator VkFence() const {
		return get();
	}

	Vulkan_Fence::operator const VkFence*() const {
		return get_ptr();
	}

	Vulkan_Fence::operator bool() const {
		return valid();
	}

	VkFence Vulkan_Fence::operator()() const {
		return get();
	}

}