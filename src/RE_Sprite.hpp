#ifndef __RE_SPRITE_H__
#define __RE_SPRITE_H__

#include "RE_Texture.hpp"
#include "RE_SpriteLayout.hpp"

#include <deque>

namespace RE {
	
	struct VulkanSprite final {
		VulkanTexture *pTexture;
		VulkanSpriteLayout *pLayout;
		uint16_t u16UniformIndex;
	};

	extern std::deque<VulkanSprite*> vulkanSprites;

}

#endif /* __RE_SPRITE_H__ */
