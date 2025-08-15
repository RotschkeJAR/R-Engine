#include "RE.hpp"

namespace RE {

	Sprite::Sprite() : hTexture(nullptr), hSpriteLayout(nullptr) {}
	Sprite::Sprite(Texture hTexture) : hTexture(hTexture), hSpriteLayout(nullptr) {}
	Sprite::Sprite(SpriteLayout hSpriteLayout) : hTexture(nullptr), hSpriteLayout(hSpriteLayout) {}
	Sprite::Sprite(Texture hTexture, SpriteLayout hSpriteLayout) : hTexture(hTexture), hSpriteLayout(hSpriteLayout) {}
	
}
