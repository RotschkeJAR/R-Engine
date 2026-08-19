#include "RE_Internal.hpp"

namespace RE {

	Color::Color() : afChannels{
				fMaxColor,
				fMaxColor,
				fMaxColor,
				fMaxColor} {}
	Color::Color(const float fRed, const float fGreen, const float fBlue, const float fAlpha) : afChannels{
				Color::clamp(fRed),
				Color::clamp(fGreen),
				Color::clamp(fBlue),
				Color::clamp(fAlpha)} {}
	Color::Color(const Color &rCopy) : afChannels{
				rCopy.afChannels[0],
				rCopy.afChannels[1],
				rCopy.afChannels[2],
				rCopy.afChannels[3]} {}
	Color::~Color() {}

	float Color::get_channel(const uint8_t u8ChannelIndex) const {
		if (u8ChannelIndex < uColorChannelCount)
			return afChannels[u8ChannelIndex];
		RE_FATAL_ERROR("The channel index is not within the range [0; ", uColorChannelCount - 1, "]: ", u8ChannelIndex);
		return 0.0f;
	}

	void Color::set_channel(const uint8_t u8ChannelIndex, const float fNormal) {
		if (u8ChannelIndex < uColorChannelCount)
			afChannels[u8ChannelIndex] = Color::clamp(fNormal);
		else
			RE_FATAL_ERROR("The channel index is not within the range [0; ", uColorChannelCount - 1, "]: ", u8ChannelIndex);
	}

	void Color::copy_from(const Color &rCopy) {
		for (uint8_t u8Channel = 0; u8Channel < uColorChannelCount; u8Channel++)
			afChannels[u8Channel] = rCopy.afChannels[u8Channel];
	}
	
	[[nodiscard]]
	bool Color::equals(const Color &rOther) const {
		for (uint8_t u8Channel = 0; u8Channel < uColorChannelCount; u8Channel++)
			if (afChannels[u8Channel] != rOther.afChannels[u8Channel])
				return false;
		return true;
	}

	void Color::set_red(const float fRed) {
		set_channel<0>(fRed);
	}
	
	[[nodiscard]]
	float Color::get_red() const {
		return get_channel<0>();
	}
	
	void Color::set_green(const float fGreen) {
		set_channel<1>(fGreen);
	}
	
	[[nodiscard]]
	float Color::get_green() const {
		return get_channel<1>();
	}
	
	void Color::set_blue(const float fBlue) {
		set_channel<2>(fBlue);
	}
	
	[[nodiscard]]
	float Color::get_blue() const {
		return get_channel<2>();
	}
	
	void Color::set_alpha(const float fAlpha) {
		set_channel<3>(fAlpha);
	}
	
	[[nodiscard]]
	float Color::get_alpha() const {
		return get_channel<3>();
	}

	[[nodiscard]]
	float Color::operator [](const uint32_t u32ChannelIndex) const {
		if (u32ChannelIndex < uColorChannelCount)
			return afChannels[u32ChannelIndex];
		RE_ERROR("The channel index range is [0; ", uColorChannelCount - 1, "], but yours was ", u32ChannelIndex);
		return 0.0f;
	}

	void Color::operator =(const Color &rCopy) {
		copy_from(rCopy);
	}
	
	[[nodiscard]]
	bool Color::operator ==(const Color &rOther) const {
		return equals(rOther);
	}
	
	[[nodiscard]]
	bool Color::operator !=(const Color &rOther) const {
		return equals(rOther);
	}

}
