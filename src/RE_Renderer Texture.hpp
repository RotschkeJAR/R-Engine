#ifndef __RE_RENDERER_TEXTURE_H__
#define __RE_RENDERER_TEXTURE_H__

#include "RE.hpp"

namespace RE {
	
	void fetch_minimum_allowed_texture_count();
	void reset_texture_counter();
	void submit_sprite(const Sprite *papSprites, int16_t &ri16TextureIndex);
	void submit_sprites_to_descriptor_set(const uint8_t u8FrameInFlightIndex);

}

#endif /* __RE_RENDERER_TEXTURE_H__ */
