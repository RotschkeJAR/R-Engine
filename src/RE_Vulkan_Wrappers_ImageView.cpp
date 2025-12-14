#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {
	
	bool create_vulkan_image_view(const VkImageViewCreateFlags vk_eFlags, 
			const VkImage vk_hImage, 
			const VkImageViewType vk_eType, 
			const VkFormat vk_eFormat, 
			const VkComponentMapping &vk_rComponentMapping, 
			const VkImageSubresourceRange &vk_rSubresourceRange,
			VkImageView *const vk_phImageView) {
		PRINT_DEBUG("Creating a Vulkan image view");
		const VkImageViewCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.flags = vk_eFlags,
			.image = vk_hImage,
			.viewType = vk_eType,
			.format = vk_eFormat,
			.components = vk_rComponentMapping,
			.subresourceRange = vk_rSubresourceRange
		};
		return vkCreateImageView(vk_hDevice, &vk_createInfo, nullptr, vk_phImageView) == VK_SUCCESS;
	}

	Vulkan_ImageView::Vulkan_ImageView() : vk_hImageView(VK_NULL_HANDLE) {}

	Vulkan_ImageView::Vulkan_ImageView(const VkImageViewCreateFlags vk_eFlags, 
			const VkImage vk_hImage, 
			const VkImageViewType vk_eType, 
			const VkFormat vk_eFormat, 
			const VkComponentMapping &vk_rComponentMapping, 
			const VkImageSubresourceRange &vk_rSubresourceRange) : vk_hImageView(VK_NULL_HANDLE) {
		PRINT_DEBUG_CLASS("Constructing Vulkan image view wrapper");
		create(vk_eFlags, vk_hImage, vk_eType, vk_eFormat, vk_rComponentMapping, vk_rSubresourceRange);
	}

	Vulkan_ImageView::Vulkan_ImageView(Vulkan_ImageView &&rrCopy) : vk_hImageView(rrCopy.vk_hImageView) {
		PRINT_DEBUG_CLASS("Constructing Vulkan image view wrapper by moving ownership from another wrapper");
		rrCopy.vk_hImageView = VK_NULL_HANDLE;
	}

	Vulkan_ImageView::~Vulkan_ImageView() {
		PRINT_DEBUG_CLASS("Destructing Vulkan image view");
		if (!valid())
			return;
		destroy();
	}

	bool Vulkan_ImageView::create(const VkImageViewCreateFlags vk_eFlags, 
			const VkImage vk_hImage, 
			const VkImageViewType vk_eType, 
			const VkFormat vk_eFormat, 
			const VkComponentMapping &vk_rComponentMapping, 
			const VkImageSubresourceRange &vk_rSubresourceRange) {
		PRINT_DEBUG_CLASS("Creating Vulkan image view wrapper");
		return create_vulkan_image_view(vk_eFlags, vk_hImage, vk_eType, vk_eFormat, vk_rComponentMapping, vk_rSubresourceRange, &vk_hImageView);
	}
	
	void Vulkan_ImageView::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan image view wrapper");
		vkDestroyImageView(vk_hDevice, vk_hImageView, nullptr);
		vk_hImageView = VK_NULL_HANDLE;
	}

	[[nodiscard]]
	VkImageView Vulkan_ImageView::get() const noexcept {
		return vk_hImageView;
	}

	[[nodiscard]]
	const VkImageView* Vulkan_ImageView::get_ptr() const noexcept {
		return &vk_hImageView;
	}
	
	[[nodiscard]]
	bool Vulkan_ImageView::valid() const noexcept {
		return vk_hImageView != VK_NULL_HANDLE;
	}

	[[nodiscard]]
	Vulkan_ImageView::operator VkImageView() const {
		return get();
	}

	[[nodiscard]]
	Vulkan_ImageView::operator const VkImageView*() const {
		return get_ptr();
	}
	
	[[nodiscard]]
	Vulkan_ImageView::operator bool() const {
		return valid();
	}

}
