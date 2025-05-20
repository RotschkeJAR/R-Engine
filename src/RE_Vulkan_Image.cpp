#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_Image::Vulkan_Image(const VkImageType vk_eType, const VkFormat vk_eFormat, const VkExtent3D vk_imgSizes, const VkImageTiling vk_eTiling, const VkImageUsageFlagBits vk_eUsages, const uint32_t u32QueueTypeCount, const uint32_t *pu32BufferQueueTypeIndices, const VkImageLayout vk_eInitialLayout) : vk_hImage(VK_NULL_HANDLE) {
		std::vector<uint32_t> queueTypeIndices;
		if (u32QueueTypeCount > 1U)
			for (uint32_t u32QueueFamilyIndex = 0U; u32QueueFamilyIndex < u32QueueTypeCount; u32QueueFamilyIndex++)
				if (std::find(queueTypeIndices.begin(), queueTypeIndices.end(), pDeviceQueues[pu32BufferQueueTypeIndices[u32QueueFamilyIndex]]->u32QueueIndex) == queueTypeIndices.end())
					queueTypeIndices.push_back(pDeviceQueues[pu32BufferQueueTypeIndices[u32QueueFamilyIndex]]->u32QueueIndex);
		VkImageCreateInfo vk_imageCreateInfo = {};
		vk_imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		vk_imageCreateInfo.imageType = vk_eType;
		vk_imageCreateInfo.format = vk_eFormat;
		vk_imageCreateInfo.extent = vk_imgSizes;
		vk_imageCreateInfo.mipLevels = 1U;
		vk_imageCreateInfo.arrayLayers = 1U;
		vk_imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		vk_imageCreateInfo.tiling = vk_eTiling;
		vk_imageCreateInfo.usage = vk_eUsages;
		if (queueTypeIndices.size() > 1U) {
			vk_imageCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_imageCreateInfo.queueFamilyIndexCount = queueTypeIndices.size();
			vk_imageCreateInfo.pQueueFamilyIndices = queueTypeIndices.data();
		} else
			vk_imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vk_imageCreateInfo.initialLayout = vk_eInitialLayout;
		if (!CHECK_VK_RESULT(vkCreateImage(vk_hDevice, &vk_imageCreateInfo, nullptr, &vk_hImage))) {
			RE_ERROR("Failed creating image in Vulkan");
			vk_hImage = VK_NULL_HANDLE;
		}
	}

	Vulkan_Image::~Vulkan_Image() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyImage(vk_hDevice, vk_hImage, nullptr));
	}

	VkImage Vulkan_Image::get_image() const {
		return vk_hImage;
	}

	bool Vulkan_Image::is_valid() const {
		return vk_hImage != VK_NULL_HANDLE;
	}

	Vulkan_Image::operator VkImage() const {
		return get_image();
	}

}
