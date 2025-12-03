#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_GPU.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

	Vulkan_TimelineSemaphore::Vulkan_TimelineSemaphore() : Vulkan_TimelineSemaphore(0) {}

	Vulkan_TimelineSemaphore::Vulkan_TimelineSemaphore(const uint64_t u64InitialValue) : vk_hTimelineSemaphore(VK_NULL_HANDLE) {
		PRINT_DEBUG_CLASS("Constructing Vulkan timeline semaphore wrapper");
		const VkSemaphoreTypeCreateInfo vk_timelineSemaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
			.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
			.initialValue = u64InitialValue
		};
		const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = &vk_timelineSemaphoreCreateInfo
		};
		if (vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hTimelineSemaphore) != VK_SUCCESS)
			RE_ERROR("Failed to create a Vulkan timeline semaphore");
	}

	Vulkan_TimelineSemaphore::~Vulkan_TimelineSemaphore() {
		PRINT_DEBUG_CLASS("Destructing Vulkan timeline semaphore wrapper");
		if (!is_valid())
			return;
		PRINT_DEBUG_CLASS("Destroying Vulkan timeline semaphore wrapper ", vk_hTimelineSemaphore);
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
	VkSemaphore Vulkan_TimelineSemaphore::get_timeline_semaphore() const {
		return vk_hTimelineSemaphore;
	}
	
	[[nodiscard]]
	const VkSemaphore* Vulkan_TimelineSemaphore::get_timeline_semaphore_ptr() const {
		return &vk_hTimelineSemaphore;
	}
	
	[[nodiscard]]
	bool Vulkan_TimelineSemaphore::is_valid() const {
		return vk_hTimelineSemaphore != VK_NULL_HANDLE;
	}

	[[nodiscard]]
	Vulkan_TimelineSemaphore::operator VkSemaphore() const {
		return get_timeline_semaphore();
	}
	
	[[nodiscard]]
	Vulkan_TimelineSemaphore::operator const VkSemaphore*() const {
		return get_timeline_semaphore_ptr();
	}

}
