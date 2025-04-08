#include "RE_Rendering_Sync Objects.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Rendering_Semaphore::Rendering_Semaphore() : vk_hSemaphore(VK_NULL_HANDLE) {
		VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {};
		vk_semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if (!CHECK_VK_RESULT(vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hSemaphore)))
			vk_hSemaphore = VK_NULL_HANDLE;
	}

	Rendering_Semaphore::~Rendering_Semaphore() {
		if (!vk_hSemaphore)
			return;
		CATCH_SIGNAL(vkDestroySemaphore(vk_hDevice, vk_hSemaphore, nullptr));
	}

	VkSemaphore Rendering_Semaphore::get_semaphore() const {
		return vk_hSemaphore;
	}
	
	bool Rendering_Semaphore::is_valid() const {
		return vk_hSemaphore != VK_NULL_HANDLE;
	}

	Rendering_Semaphore::operator VkSemaphore() const {
		return this->vk_hSemaphore;
	}



	Rendering_Fence::Rendering_Fence() : vk_hFence(VK_NULL_HANDLE) {
		VkFenceCreateInfo vk_fenceCreateInfo = {};
		vk_fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		if (!CHECK_VK_RESULT(vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, &vk_hFence)))
			vk_hFence = VK_NULL_HANDLE;
	}

	Rendering_Fence::Rendering_Fence(VkFenceCreateFlagBits vk_eFenceCreateFlags) : vk_hFence(VK_NULL_HANDLE) {
		VkFenceCreateInfo vk_fenceCreateInfo = {};
		vk_fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		vk_fenceCreateInfo.flags = vk_eFenceCreateFlags;
		if (!CHECK_VK_RESULT(vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, &vk_hFence)))
			vk_hFence = VK_NULL_HANDLE;
	}

	Rendering_Fence::~Rendering_Fence() {
		if (!vk_hFence)
			return;
		CATCH_SIGNAL(vkDestroyFence(vk_hDevice, vk_hFence, nullptr));
	}

	void Rendering_Fence::wait_for_fence() {
		CATCH_SIGNAL(vkWaitForFences(vk_hDevice, 1U, &vk_hFence, VK_TRUE, std::numeric_limits<uint64_t>::max()));
	}
	
	void Rendering_Fence::reset_fence() {
		CATCH_SIGNAL(vkResetFences(vk_hDevice, 1U, &vk_hFence));
	}
	
	void Rendering_Fence::wait_for_and_reset_fence() {
		wait_for_fence();
		reset_fence();
	}
	
	bool Rendering_Fence::is_fence_signaled() {
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

	VkFence Rendering_Fence::get_fence() const {
		return vk_hFence;
	}
	
	bool Rendering_Fence::is_valid() const {
		return vk_hFence != VK_NULL_HANDLE;
	}

	Rendering_Fence::operator VkFence() const {
		return this->vk_hFence;
	}

}
