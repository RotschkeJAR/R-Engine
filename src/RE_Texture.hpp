#ifndef __RE_TEXTURE_H__
#define __RE_TEXTURE_H__

#include "RE_Internal Header.hpp"
#include "RE_Vulkan_Instance.hpp"

namespace RE {

	struct VulkanTexture {
		VkImage vk_hImage;
		VkDeviceMemory vk_hImageMemory;
		VkImageView vk_hImageView;
		VkFormat vk_eFormat;
		uint32_t u32Channels;
		uint32_t a2u32Size[2];
	};

}

#endif /* __RE_TEXTURE_H__ */
