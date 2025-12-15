#include "RE_Vulkan_Wrappers.hpp"

namespace RE {
	
	bool create_vulkan_semaphore(VkSemaphoreCreateFlags vk_eFlags, VkSemaphore *vk_phSemaphore) {
		PRINT_DEBUG("Creating a Vulkan semaphore");
		const VkSemaphoreCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.flags = vk_eFlags
		};
		if (vkCreateSemaphore(vk_hDevice, &vk_createInfo, nullptr, vk_phSemaphore) == VK_SUCCESS)
			return true;
		RE_ERROR("Failed to create a Vulkan semaphore");
		return false;
	}

	Vulkan_Semaphore::Vulkan_Semaphore() : vk_hSemaphore(VK_NULL_HANDLE) {}

	Vulkan_Semaphore::Vulkan_Semaphore(VkSemaphoreCreateFlags vk_eFlags) : vk_hSemaphore(VK_NULL_HANDLE) {
		PRINT_DEBUG_CLASS("Constructing Vulkan semaphore wrapper");
		create(vk_eFlags);
	}

	Vulkan_Semaphore::Vulkan_Semaphore(Vulkan_Semaphore &&rrCopy) : vk_hSemaphore(rrCopy.vk_hSemaphore) {
		PRINT_DEBUG_CLASS("Constructing Vulkan semaphore wrapper by moving ownership from another wrapper");
		rrCopy.vk_hSemaphore = VK_NULL_HANDLE;
	}

	Vulkan_Semaphore::~Vulkan_Semaphore() {
		PRINT_DEBUG_CLASS("Deconstructing Vulkan semaphore wrapper");
		if (!valid())
			return;
		destroy();
	}

	bool Vulkan_Semaphore::create(VkSemaphoreCreateFlags vk_eFlags) {
		PRINT_DEBUG_CLASS("Creating Vulkan semaphore wrapper");
		return create_vulkan_semaphore(vk_eFlags, &vk_hSemaphore);
	}
	
	void Vulkan_Semaphore::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan semaphore wrapper");
		vkDestroySemaphore(vk_hDevice, vk_hSemaphore, nullptr);
		vk_hSemaphore = VK_NULL_HANDLE;
	}

	[[nodiscard]]
	VkSemaphore Vulkan_Semaphore::get() const noexcept {
		return vk_hSemaphore;
	}
	
	[[nodiscard]]
	const VkSemaphore* Vulkan_Semaphore::get_ptr() const noexcept {
		return &vk_hSemaphore;
	}
	
	[[nodiscard]]
	bool Vulkan_Semaphore::valid() const noexcept {
		return vk_hSemaphore != VK_NULL_HANDLE;
	}

	[[nodiscard]]
	Vulkan_Semaphore::operator VkSemaphore() const {
		return get();
	}
	
	[[nodiscard]]
	Vulkan_Semaphore::operator const VkSemaphore*() const {
		return get_ptr();
	}
	
	[[nodiscard]]
	Vulkan_Semaphore::operator bool() const {
		return valid();
	}

}
