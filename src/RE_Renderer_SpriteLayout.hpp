#ifndef __RE_SPRITE_LAYOUT_H__
#define __RE_SPRITE_LAYOUT_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	struct VulkanSpriteLayout final {
		VkSampler vk_hSampler;
	};

	bool init_renderer_sprite_layouts();
	void destroy_renderer_sprite_layout();
	uint16_t get_index_of_sprite_layout(const VulkanSpriteLayout *pSpriteLayout);

}

#endif /* __RE_SPRITE_LAYOUT_H__ */
