#include "RE.hpp"

namespace RE {
	
	SpriteLayoutSettings::SpriteLayoutSettings() : SpriteLayoutSettings(0.0f) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const Filter eMagFilter) : SpriteLayoutSettings(eMagFilter, RE_FILTER_LINEAR) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const Filter eMagFilter, const Filter eMinFilter) : SpriteLayoutSettings(eMagFilter, eMinFilter, RE_FILTER_LINEAR) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const Filter eMagFilter, const Filter eMinFilter, const Filter eMipmapFilter) : SpriteLayoutSettings(eMagFilter, eMinFilter, eMipmapFilter, RE_REPETITION_REPEAT, RE_REPETITION_REPEAT, 0.0f, RE_BORDER_COLOR_BLACK) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const Repetition eRepetitionU) : SpriteLayoutSettings(eRepetitionU, RE_REPETITION_REPEAT) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const Repetition eRepetitionU, const Repetition eRepetitionV) : SpriteLayoutSettings(RE_FILTER_LINEAR, RE_FILTER_LINEAR, RE_FILTER_LINEAR, eRepetitionU, eRepetitionV, 0.0f, RE_BORDER_COLOR_BLACK) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const float fMaxAnisotropy) : SpriteLayoutSettings(RE_FILTER_LINEAR, RE_FILTER_LINEAR, RE_FILTER_LINEAR, RE_REPETITION_REPEAT, RE_REPETITION_REPEAT, fMaxAnisotropy, RE_BORDER_COLOR_BLACK) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const BorderColor eBorderColor) : SpriteLayoutSettings(RE_FILTER_LINEAR, RE_FILTER_LINEAR, RE_FILTER_LINEAR, RE_REPETITION_REPEAT, RE_REPETITION_REPEAT, 0.0f, eBorderColor) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const Filter eMagFilter, const Filter eMinFilter, const Filter eMipmapFilter, const Repetition eRepetitionU, const Repetition eRepetitionV, const float fMaxAnisotropy, const BorderColor eBorderColor) : eMagFilter(eMagFilter), eMinFilter(eMinFilter), eMipmapFilter(eMipmapFilter), eRepetitionU(eRepetitionU), eRepetitionV(eRepetitionV), fMaxAnisotropy(fMaxAnisotropy), eBorderColor(eBorderColor) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const SpriteLayoutSettings &rCopy) : SpriteLayoutSettings(rCopy.eMagFilter, rCopy.eMinFilter, rCopy.eMipmapFilter, rCopy.eRepetitionU, rCopy.eRepetitionV, rCopy.fMaxAnisotropy, rCopy.eBorderColor) {}
	SpriteLayoutSettings::~SpriteLayoutSettings() {}

	void SpriteLayoutSettings::copy_from(const SpriteLayoutSettings &rCopy) {
		eMagFilter = rCopy.eMagFilter;
		eMinFilter = rCopy.eMinFilter;
		eMipmapFilter = rCopy.eMipmapFilter;
		eRepetitionU = rCopy.eRepetitionU;
		eRepetitionV = rCopy.eRepetitionV;
		fMaxAnisotropy = rCopy.fMaxAnisotropy;
		eBorderColor = rCopy.eBorderColor;
	}
	
	[[nodiscard]]
	bool SpriteLayoutSettings::equals(const SpriteLayoutSettings &rCompare) const {
		return eMagFilter == rCompare.eMagFilter && eMinFilter == rCompare.eMinFilter && eMipmapFilter == rCompare.eMipmapFilter && eRepetitionU == rCompare.eRepetitionU && eRepetitionV == rCompare.eRepetitionV && fMaxAnisotropy == rCompare.fMaxAnisotropy && eBorderColor == rCompare.eBorderColor;
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
