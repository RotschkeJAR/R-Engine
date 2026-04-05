#include "RE.hpp"

namespace RE {
	
	SpriteLayoutSettings::SpriteLayoutSettings() : SpriteLayoutSettings(0.0f) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureFilter eMagFilter) : SpriteLayoutSettings(eMagFilter, RE_TEXTURE_FILTER_LINEAR) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureFilter eMagFilter, const TextureFilter eMinFilter) : SpriteLayoutSettings(eMagFilter, eMinFilter, RE_TEXTURE_FILTER_LINEAR) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureFilter eMagFilter, const TextureFilter eMinFilter, const TextureFilter eMipmapFilter) : SpriteLayoutSettings(eMagFilter, eMinFilter, eMipmapFilter, RE_TEXTURE_REPETITION_REPEAT, RE_TEXTURE_REPETITION_REPEAT, 0.0f, RE_BORDER_COLOR_BLACK) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureRepetition eTextureRepetitionU) : SpriteLayoutSettings(eTextureRepetitionU, RE_TEXTURE_REPETITION_REPEAT) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureRepetition eTextureRepetitionU, const TextureRepetition eTextureRepetitionV) : SpriteLayoutSettings(RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, eTextureRepetitionU, eTextureRepetitionV, 0.0f, RE_BORDER_COLOR_BLACK) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const float f32MaxAnisotropy) : SpriteLayoutSettings(RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_REPETITION_REPEAT, RE_TEXTURE_REPETITION_REPEAT, f32MaxAnisotropy, RE_BORDER_COLOR_BLACK) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const BorderColor eBorderColor) : SpriteLayoutSettings(RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_FILTER_LINEAR, RE_TEXTURE_REPETITION_REPEAT, RE_TEXTURE_REPETITION_REPEAT, 0.0f, eBorderColor) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const TextureFilter eMagFilter, const TextureFilter eMinFilter, const TextureFilter eMipmapFilter, const TextureRepetition eTextureRepetitionU, const TextureRepetition eTextureRepetitionV, const float f32MaxAnisotropy, const BorderColor eBorderColor) : eMagFilter(eMagFilter), eMinFilter(eMinFilter), eMipmapFilter(eMipmapFilter), eTextureRepetitionU(eTextureRepetitionU), eTextureRepetitionV(eTextureRepetitionV), f32MaxAnisotropy(f32MaxAnisotropy), eBorderColor(eBorderColor) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const SpriteLayoutSettings &rCopy) : SpriteLayoutSettings(rCopy.eMagFilter, rCopy.eMinFilter, rCopy.eMipmapFilter, rCopy.eTextureRepetitionU, rCopy.eTextureRepetitionV, rCopy.f32MaxAnisotropy, rCopy.eBorderColor) {}
	SpriteLayoutSettings::~SpriteLayoutSettings() {}

	void SpriteLayoutSettings::copy_from(const SpriteLayoutSettings &rCopy) {
		eMagFilter = rCopy.eMagFilter;
		eMinFilter = rCopy.eMinFilter;
		eMipmapFilter = rCopy.eMipmapFilter;
		eTextureRepetitionU = rCopy.eTextureRepetitionU;
		eTextureRepetitionV = rCopy.eTextureRepetitionV;
		f32MaxAnisotropy = rCopy.f32MaxAnisotropy;
		eBorderColor = rCopy.eBorderColor;
	}
	
	[[nodiscard]]
	bool SpriteLayoutSettings::equals(const SpriteLayoutSettings &rCompare) const {
		return eMagFilter == rCompare.eMagFilter && eMinFilter == rCompare.eMinFilter && eMipmapFilter == rCompare.eMipmapFilter && eTextureRepetitionU == rCompare.eTextureRepetitionU && eTextureRepetitionV == rCompare.eTextureRepetitionV && f32MaxAnisotropy == rCompare.f32MaxAnisotropy && eBorderColor == rCompare.eBorderColor;
	}
	
	void SpriteLayoutSettings::operator =(const SpriteLayoutSettings &rCopy) {
		copy_from(rCopy);
	}
	
	[[nodiscard]]
	bool SpriteLayoutSettings::operator ==(const SpriteLayoutSettings &rCompare) const {
		return equals(rCompare);
	}
	
	[[nodiscard]]
	bool SpriteLayoutSettings::operator !=(const SpriteLayoutSettings &rCompare) const {
		return !equals(rCompare);
	}

}
