#include "RE.hpp"

namespace RE {
	
	SpriteLayoutSettings::SpriteLayoutSettings() : SpriteLayoutSettings(0.0f) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureFilter eMagFilter) : SpriteLayoutSettings(eMagFilter, RE_TEXTURE_FILTER_LINEAR) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureFilter eMagFilter, const TextureFilter eMinFilter) : SpriteLayoutSettings(eMagFilter, eMinFilter, RE_TEXTURE_FILTER_LINEAR) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureFilter eMagFilter, const TextureFilter eMinFilter, const TextureFilter eMipmapFilter) : SpriteLayoutSettings(eMagFilter, eMinFilter, eMipmapFilter, RE_TEXTURE_REPETITION_REPEAT, RE_TEXTURE_REPETITION_REPEAT, 0.0f, RE_BORDER_COLOR_BLACK) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureRepetition eTextureRepetitionU) : SpriteLayoutSettings(eTextureRepetitionU, RE_TEXTURE_REPETITION_REPEAT) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureRepetition eTextureRepetitionU, const TextureRepetition eTextureRepetitionV) : SpriteLayoutSettings(RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, eTextureRepetitionU, eTextureRepetitionV, 0.0f, RE_BORDER_COLOR_BLACK) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const float fMaxAnisotropy) : SpriteLayoutSettings(RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_REPETITION_REPEAT, RE_TEXTURE_REPETITION_REPEAT, fMaxAnisotropy, RE_BORDER_COLOR_BLACK) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const BorderColor eBorderColor) : SpriteLayoutSettings(RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_REPETITION_REPEAT, RE_TEXTURE_REPETITION_REPEAT, 0.0f, eBorderColor) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureFilter eMagFilter, const TextureFilter eMinFilter, const TextureFilter eMipmapFilter, const TextureRepetition eTextureRepetitionU, const TextureRepetition eTextureRepetitionV, const float fMaxAnisotropy, const BorderColor eBorderColor) : eMagFilter(eMagFilter), eMinFilter(eMinFilter), eMipmapFilter(eMipmapFilter), eTextureRepetitionU(eTextureRepetitionU), eTextureRepetitionV(eTextureRepetitionV), fMaxAnisotropy(fMaxAnisotropy), eBorderColor(eBorderColor) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const SpriteLayoutSettings &rCopy) : SpriteLayoutSettings(rCopy.eMagFilter, rCopy.eMinFilter, rCopy.eMipmapFilter, rCopy.eTextureRepetitionU, rCopy.eTextureRepetitionV, rCopy.fMaxAnisotropy, rCopy.eBorderColor) {}
	SpriteLayoutSettings::~SpriteLayoutSettings() {}

}
