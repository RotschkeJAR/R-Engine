#include "RE_Vulkan_Wrappers.hpp"

namespace RE {
	
	bool create_vulkan_semaphore(const VkSemaphoreCreateFlags vk_mFlags, VkSemaphore *const vk_phSemaphore) {
		PRINT_DEBUG("Creating a Vulkan semaphore");
		const VkSemaphoreCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.flags = vk_mFlags
		};
		if (vkCreateSemaphore(vk_hDevice, &vk_createInfo, nullptr, vk_phSemaphore) == VK_SUCCESS)
			return true;
		RE_ERROR("Failed to create a Vulkan semaphore");
		return false;
	}

	Vulkan_Semaphore::Vulkan_Semaphore() : vk_hSemaphore(VK_NULL_HANDLE) {}

	Vulkan_Semaphore::Vulkan_Semaphore(const VkSemaphoreCreateFlags vk_mFlags) : Vulkan_Semaphore() {
		PRINT_DEBUG_CLASS("Constructing Vulkan semaphore wrapper");
		create(vk_mFlags);
	}

	Vulkan_Semaphore::Vulkan_Semaphore(Vulkan_Semaphore &&rrCopy) : vk_hSemaphore(rrCopy.vk_hSemaphore) {
		PRINT_DEBUG_CLASS("Constructing Vulkan semaphore wrapper by moving ownership from another wrapper");
		rrCopy.vk_hSemaphore = VK_NULL_HANDLE;
	}

	Vulkan_Semaphore::~Vulkan_Semaphore() {
		PRINT_DEBUG_CLASS("Deconstructing Vulkan semaphore wrapper");
		destroy();
	}

	bool Vulkan_Semaphore::create(const VkSemaphoreCreateFlags vk_mFlags) {
#ifndef RE_DISABLE_PRINT_DEBUGS
		if (valid())
			RE_ERROR("Creating another Vulkan semaphore wrapper, when the old semaphore ", vk_hSemaphore, " hasn't been destroyed yet");
#endif
		PRINT_DEBUG_CLASS("Creating Vulkan semaphore wrapper");
		return create_vulkan_semaphore(vk_mFlags, &vk_hSemaphore);
	}
	
	void Vulkan_Semaphore::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan semaphore wrapper");
		vkDestroySemaphore(vk_hDevice, vk_hSemaphore, nullptr);
		vk_hSemaphore = VK_NULL_HANDLE;
	}

	VkSemaphore Vulkan_Semaphore::get() const {
		return vk_hSemaphore;
	}
	
	const VkSemaphore* Vulkan_Semaphore::get_ptr() const {
		return &vk_hSemaphore;
	}
	
	bool Vulkan_Semaphore::valid() const {
		return vk_hSemaphore != VK_NULL_HANDLE;
	}

	Vulkan_Semaphore::operator VkSemaphore() const {
		return get();
	}
	
	Vulkan_Semaphore::operator const VkSemaphore*() const {
		return get_ptr();
	}
	
	Vulkan_Semaphore::operator bool() const {
		return valid();
	}

	VkSemaphore Vulkan_Semaphore::operator()() const {
		return get();
	}

}
