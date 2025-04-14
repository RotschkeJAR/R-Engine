#include "RE_Vulkan_Sync Objects.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_Semaphore::Vulkan_Semaphore() : vk_hSemaphore(VK_NULL_HANDLE) {
		VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {};
		vk_semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if (!CHECK_VK_RESULT(vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hSemaphore)))
			vk_hSemaphore = VK_NULL_HANDLE;
	}

	Vulkan_Semaphore::~Vulkan_Semaphore() {
		if (!vk_hSemaphore)
			return;
		CATCH_SIGNAL(vkDestroySemaphore(vk_hDevice, vk_hSemaphore, nullptr));
	}

	VkSemaphore Vulkan_Semaphore::get_semaphore() const {
		return vk_hSemaphore;
	}
	
	bool Vulkan_Semaphore::is_valid() const {
		return vk_hSemaphore != VK_NULL_HANDLE;
	}

	Vulkan_Semaphore::operator VkSemaphore() const {
		return this->vk_hSemaphore;
	}



	Vulkan_Fence::Vulkan_Fence() : vk_hFence(VK_NULL_HANDLE) {
		VkFenceCreateInfo vk_fenceCreateInfo = {};
		vk_fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		if (!CHECK_VK_RESULT(vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, &vk_hFence)))
			vk_hFence = VK_NULL_HANDLE;
	}

	Vulkan_Fence::Vulkan_Fence(VkFenceCreateFlagBits vk_eFenceCreateFlags) : vk_hFence(VK_NULL_HANDLE) {
		VkFenceCreateInfo vk_fenceCreateInfo = {};
		vk_fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		vk_fenceCreateInfo.flags = vk_eFenceCreateFlags;
		if (!CHECK_VK_RESULT(vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, &vk_hFence)))
			vk_hFence = VK_NULL_HANDLE;
	}

	Vulkan_Fence::~Vulkan_Fence() {
		if (!vk_hFence)
			return;
		CATCH_SIGNAL(vkDestroyFence(vk_hDevice, vk_hFence, nullptr));
	}

	void Vulkan_Fence::wait_for_fence() const {
		CATCH_SIGNAL(vkWaitForFences(vk_hDevice, 1U, &vk_hFence, VK_TRUE, std::numeric_limits<uint64_t>::max()));
	}
	
	void Vulkan_Fence::reset_fence() const {
		CATCH_SIGNAL(vkResetFences(vk_hDevice, 1U, &vk_hFence));
	}
	
	void Vulkan_Fence::wait_for_and_reset_fence() const {
		wait_for_fence();
		reset_fence();
	}
	
	bool Vulkan_Fence::is_fence_signaled() const {
		const VkResult vk_eFenceState = CATCH_SIGNAL_AND_RETURN(vkGetFenceStatus(vk_hDevice, vk_hFence), VkResult);
		switch (vk_eFenceState) {
			case VK_SUCCESS:
				return true;
			case VK_NOT_READY:
				return false;
			default:
				CHECK_VK_RESULT(vk_eFenceState);
				return false;
		}
	}

	VkFence Vulkan_Fence::get_fence() const {
		return vk_hFence;
	}
	
	bool Vulkan_Fence::is_valid() const {
		return vk_hFence != VK_NULL_HANDLE;
	}

	Vulkan_Fence::operator VkFence() const {
		return this->vk_hFence;
	}

}
