#ifndef __RE_TEXTURE_H__
#define __RE_TEXTURE_H__

#include "RE_Internal Header.hpp"
#include "RE_Vulkan_Instance.hpp"

namespace RE {

	struct TextureContainer {
		VkImage vk_hImage;
		VkDeviceMemory vk_hImageMemory;
		VkImageView vk_hImgView;
		VkSampler vk_hImgSampler;
	};

}

#endif /* __RE_TEXTURE_H__ */
