#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_GPU.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

	Vulkan_Fence::Vulkan_Fence() : Vulkan_Fence(0) {}
	Vulkan_Fence::Vulkan_Fence(const VkFenceCreateFlags vk_eCreateFlags) : vk_hFence(VK_NULL_HANDLE) {
		PRINT_DEBUG_CLASS("Constructing Vulkan fence wrapper");
		const VkFenceCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = vk_eCreateFlags
		};
		if (vkCreateFence(vk_hDevice, &vk_createInfo, nullptr, &vk_hFence) != VK_SUCCESS)
			RE_ERROR("Failed to create Vulkan fence");
	}
	
	Vulkan_Fence::~Vulkan_Fence() {
		PRINT_DEBUG_CLASS("Destructing Vulkan fence wrapper");
		if (!is_valid())
			return;
		PRINT_DEBUG_CLASS("Destroying Vulkan fence wrapper ", vk_hFence);
		vkDestroyFence(vk_hDevice, vk_hFence, nullptr);
	}

	bool Vulkan_Fence::wait_for() const {
		PRINT_DEBUG_CLASS("Waiting for Vulkan fence wrapper ", vk_hFence);
		return vkWaitForFences(vk_hDevice, 1, &vk_hFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	}

	void Vulkan_Fence::reset() const {
		PRINT_DEBUG_CLASS("Reseting Vulkan fence wrapper ", vk_hFence);
		vkResetFences(vk_hDevice, 1, &vk_hFence);
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