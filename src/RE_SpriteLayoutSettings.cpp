#include "RE.hpp"

namespace RE {
	
	SpriteLayoutSettings::SpriteLayoutSettings() : eMagFilter(RE_FILTER_LINEAR),
			eMinFilter(RE_FILTER_LINEAR),
			eMipmapFilter(RE_FILTER_LINEAR),
			eRepetitionU(RE_REPETITION_REPEAT),
			eRepetitionV(RE_REPETITION_REPEAT),
			fMaxAnisotropy(0.0f),
			eBorderColor(RE_BORDER_COLOR_BLACK) {}
	SpriteLayoutSettings::SpriteLayoutSettings(const SpriteLayoutSettings &rCopy) : eMagFilter(rCopy.eMagFilter),
			eMinFilter(rCopy.eMinFilter),
			eMipmapFilter(rCopy.eMipmapFilter),
			eRepetitionU(rCopy.eRepetitionU),
			eRepetitionV(rCopy.eRepetitionV),
			fMaxAnisotropy(rCopy.fMaxAnisotropy),
			eBorderColor(rCopy.eBorderColor) {}
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
		return eMagFilter == rCompare.eMagFilter
				and eMinFilter == rCompare.eMinFilter
				and eMipmapFilter == rCompare.eMipmapFilter
				and eRepetitionU == rCompare.eRepetitionU
				and eRepetitionV == rCompare.eRepetitionV
				and fMaxAnisotropy == rCompare.fMaxAnisotropy
				and eBorderColor == rCompare.eBorderColor;
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
		return not equals(rCompare);
	}

}
