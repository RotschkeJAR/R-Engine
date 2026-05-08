#ifndef __RE_TEXTURE_H__
#define __RE_TEXTURE_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	struct VulkanTexture final {
		VulkanMemory imageMemory;
		VkImage vk_hImage;
		VkImageView vk_hImageView;
		VkFormat vk_eFormat;
		uint32_t a2u32Size[2];
	};

	bool init_renderer_textures();
	void destroy_renderer_textures();
	uint16_t get_index_of_texture(const VulkanTexture *pTexture);

}

#endif /* __RE_TEXTURE_H__ */
