#ifndef __RE_TEXTURE_H__
#define __RE_TEXTURE_H__

#include "RE_Internal Header.hpp"
#include "RE_GPU.hpp"

namespace RE {

	struct VulkanTexture final {
		VkImage vk_hImage;
		VulkanMemory imageMemory;
		VkImageView vk_hImageView;
		VkFormat vk_eFormat;
		uint32_t a2u32Size[2];
	};

}

#endif /* __RE_TEXTURE_H__ */
