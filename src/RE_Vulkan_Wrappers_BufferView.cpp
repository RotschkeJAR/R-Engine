#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {
	
	bool create_vulkan_buffer_view(
			VkBufferViewCreateFlags vk_eFlags, 
			VkBuffer vk_hBuffer, 
			VkFormat vk_eFormat, 
			VkDeviceSize vk_offset, 
			VkDeviceSize vk_range, 
			VkBufferView *vk_phBufferView) {
		PRINT_DEBUG("Creating a Vulkan buffer view");
		const VkBufferViewCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
			.flags = vk_eFlags,
			.buffer = vk_hBuffer,
			.format = vk_eFormat,
			.offset = vk_offset,
			.range = vk_range
		};
		if (vkCreateBufferView(vk_hDevice, &vk_createInfo, nullptr, vk_phBufferView) == VK_SUCCESS)
			return true;
		RE_ERROR("Failed to create a Vulkan timeline semaphore");
		return false;
	}

	Vulkan_BufferView::Vulkan_BufferView() : vk_hBufferView(VK_NULL_HANDLE) {}
	
	Vulkan_BufferView::Vulkan_BufferView(
			VkBufferViewCreateFlags vk_eFlags, 
			VkBuffer vk_hBuffer, 
			VkFormat vk_eFormat, 
			VkDeviceSize vk_offset, 
			VkDeviceSize vk_range) : Vulkan_BufferView() {
		PRINT_DEBUG_CLASS("Constructing Vulkan buffer view wrapper");
		create(vk_eFlags, vk_hBuffer, vk_eFormat, vk_offset, vk_range);
	}
	
	Vulkan_BufferView::Vulkan_BufferView(Vulkan_BufferView &&rrCopy) : vk_hBufferView(rrCopy.vk_hBufferView) {
		PRINT_DEBUG_CLASS("Constructing Vulkan buffer view wrapper by moving ownership from another wrapper");
		rrCopy.vk_hBufferView = VK_NULL_HANDLE;
	}

	Vulkan_BufferView::~Vulkan_BufferView() {
		PRINT_DEBUG_CLASS("Destructing Vulkan buffer view wrapper");
		destroy();
	}

	bool Vulkan_BufferView::create(
			VkBufferViewCreateFlags vk_eFlags, 
			VkBuffer vk_hBuffer, 
			VkFormat vk_eFormat, 
			VkDeviceSize vk_offset, 
			VkDeviceSize vk_range) {
	#ifndef NDEBUG
		if (valid())
			RE_ERROR("Creating another Vulkan buffer view wrapper, when the old Vulkan buffer view ", vk_hBufferView, " hasn't been destroyed yet");
	#endif
		PRINT_DEBUG_CLASS("Creating Vulkan buffer view wrapper");
		return create_vulkan_buffer_view(vk_eFlags, vk_hBuffer, vk_eFormat, vk_offset, vk_range, &vk_hBufferView);
	}

	void Vulkan_BufferView::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan buffer view");
		vkDestroyBufferView(vk_hDevice, vk_hBufferView, nullptr);
		vk_hBufferView = VK_NULL_HANDLE;
	}

	VkBufferView Vulkan_BufferView::get() const {
		return vk_hBufferView;
	}

	const VkBufferView* Vulkan_BufferView::get_ptr() const {
		return &vk_hBufferView;
	}

	bool Vulkan_BufferView::valid() const {
		return vk_hBufferView != VK_NULL_HANDLE;
	}

	Vulkan_BufferView::operator VkBufferView() const {
		return get();
	}

	Vulkan_BufferView::operator const VkBufferView*() const {
		return get_ptr();
	}

	Vulkan_BufferView::operator bool() const {
		return valid();
	}

	VkBufferView Vulkan_BufferView::operator()() const {
		return get();
	}

}
