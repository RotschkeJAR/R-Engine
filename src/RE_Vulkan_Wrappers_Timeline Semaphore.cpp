#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {

	bool create_vulkan_timeline_semaphore(const VkSemaphoreCreateFlags vk_eFlags, const uint64_t u64InitialValue, VkSemaphore *const vk_phSemaphore) {
		PRINT_DEBUG("Creating a Vulkan timeline semaphore starting at value ", u64InitialValue);
		const VkSemaphoreTypeCreateInfo vk_timelineSemaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
			.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
			.initialValue = u64InitialValue
		};
		const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = &vk_timelineSemaphoreCreateInfo,
			.flags = vk_eFlags
		};
		if (vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, vk_phSemaphore) == VK_SUCCESS)
			return true;
		RE_ERROR("Failed to create a Vulkan timeline semaphore");
		return false;
	}

	Vulkan_TimelineSemaphore::Vulkan_TimelineSemaphore() : vk_hTimelineSemaphore(VK_NULL_HANDLE) {}

	Vulkan_TimelineSemaphore::Vulkan_TimelineSemaphore(const VkSemaphoreCreateFlags vk_eFlags, const uint64_t u64InitialValue) : vk_hTimelineSemaphore(VK_NULL_HANDLE) {
		PRINT_DEBUG_CLASS("Constructing Vulkan timeline semaphore wrapper");
		create(vk_eFlags, u64InitialValue);
	}

	Vulkan_TimelineSemaphore::Vulkan_TimelineSemaphore(Vulkan_TimelineSemaphore &&rrCopy) : vk_hTimelineSemaphore(rrCopy.vk_hTimelineSemaphore) {
		PRINT_DEBUG_CLASS("Constructing Vulkan timeline semaphore wrapper by moving ownership from another wrapper");
		rrCopy.vk_hTimelineSemaphore = VK_NULL_HANDLE;
	}

	Vulkan_TimelineSemaphore::~Vulkan_TimelineSemaphore() {
		PRINT_DEBUG_CLASS("Destructing Vulkan timeline semaphore wrapper");
		if (!valid())
			return;
		destroy();
	}

	bool Vulkan_TimelineSemaphore::create(const VkSemaphoreCreateFlags vk_eFlags, const uint64_t u64InitialValue) {
		PRINT_DEBUG_CLASS("Creating Vulkan timeline semaphore wrapper");
		return create_vulkan_timeline_semaphore(vk_eFlags, u64InitialValue, &vk_hTimelineSemaphore);
	}
	
	void Vulkan_TimelineSemaphore::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan timeline semaphore wrapper");
		vkDestroySemaphore(vk_hDevice, vk_hTimelineSemaphore, nullptr);
	}

	bool Vulkan_TimelineSemaphore::wait_for_reaching(const uint64_t u64Value) const {
		PRINT_DEBUG_CLASS("Waiting for Vulkan timeline semaphore wrapper ", vk_hTimelineSemaphore, " reaching value ", u64Value);
		const VkSemaphoreWaitInfo vk_waitSemaphoreInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
			.semaphoreCount = 1,
			.pSemaphores = &vk_hTimelineSemaphore,
			.pValues = &u64Value
		};
		return vkWaitSemaphores(vk_hDevice, &vk_waitSemaphoreInfo, std::numeric_limits<uint64_t>::max()) == VK_SUCCESS;
	}
	
	void Vulkan_TimelineSemaphore::set_to(const uint64_t u64Value) const {
		PRINT_DEBUG_CLASS("Signaling Vulkan timeline semaphore wrapper ", vk_hTimelineSemaphore, " to value ", u64Value);
		const VkSemaphoreSignalInfo vk_signalSemaphoreInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO,
			.semaphore = vk_hTimelineSemaphore,
			.value = u64Value
		};
		vkSignalSemaphore(vk_hDevice, &vk_signalSemaphoreInfo);
	}
	
	[[nodiscard]]
	VkSemaphore Vulkan_TimelineSemaphore::get() const noexcept {
		return vk_hTimelineSemaphore;
	}
	
	[[nodiscard]]
	const VkSemaphore* Vulkan_TimelineSemaphore::get_ptr() const noexcept {
		return &vk_hTimelineSemaphore;
	}
	
	[[nodiscard]]
	bool Vulkan_TimelineSemaphore::valid() const noexcept {
		return vk_hTimelineSemaphore != VK_NULL_HANDLE;
	}

	[[nodiscard]]
	Vulkan_TimelineSemaphore::operator VkSemaphore() const {
		return get();
	}
	
	[[nodiscard]]
	Vulkan_TimelineSemaphore::operator const VkSemaphore*() const {
		return get_ptr();
	}

}
