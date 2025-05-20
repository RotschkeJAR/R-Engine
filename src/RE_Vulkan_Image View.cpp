#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_ImageView::Vulkan_ImageView(const VkImage vk_hImage, const VkImageViewType vk_eType, const VkFormat vk_eFormat, const VkComponentMapping vk_componentMapping, const VkImageSubresourceRange vk_subresourceRange) : vk_hImageView(VK_NULL_HANDLE) {
		VkImageViewCreateInfo vk_imageViewCreateInfo = {};
		vk_imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vk_imageViewCreateInfo.image = vk_hImage;
		vk_imageViewCreateInfo.viewType = vk_eType;
		vk_imageViewCreateInfo.format = vk_eFormat;
		vk_imageViewCreateInfo.components = vk_componentMapping;
		vk_imageViewCreateInfo.subresourceRange = vk_subresourceRange;
		if (!CHECK_VK_RESULT(vkCreateImageView(vk_hDevice, &vk_imageViewCreateInfo, nullptr, &vk_hImageView))) {
			RE_ERROR("Failed creating image view in Vulkan");
			vk_hImageView = VK_NULL_HANDLE;
		}
	}
	
	Vulkan_ImageView::~Vulkan_ImageView() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyImageView(vk_hDevice, vk_hImageView, nullptr));
	}
	
	VkImageView Vulkan_ImageView::get_image_view() const {
		return vk_hImageView;
	}
	
	bool Vulkan_ImageView::is_valid() const {
		return vk_hImageView != VK_NULL_HANDLE;
	}

	Vulkan_ImageView::operator VkImageView() const {
		return get_image_view();
	}

}
