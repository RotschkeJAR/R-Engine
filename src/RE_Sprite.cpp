#include "RE.hpp"

namespace RE {

	Sprite::Sprite() : hTexture(nullptr), hSpriteLayout(nullptr) {}
	Sprite::Sprite(const Texture hTexture) : hTexture(hTexture), hSpriteLayout(nullptr) {}
	Sprite::Sprite(const SpriteLayout hSpriteLayout) : hTexture(nullptr), hSpriteLayout(hSpriteLayout) {}
	Sprite::Sprite(const Texture hTexture, const SpriteLayout hSpriteLayout) : hTexture(hTexture), hSpriteLayout(hSpriteLayout) {}
	Sprite::Sprite(const Sprite &rCopy) : Sprite(rCopy.hTexture, rCopy.hSpriteLayout) {}
	Sprite::~Sprite() {}
	
}
