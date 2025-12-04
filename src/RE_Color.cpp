#include "RE_Internal Header.hpp"

namespace RE {

#define MAX_CHANNEL_VALUE 1.0f
#define CLAMP_CHANNEL_VALUE(CHANNEL_FLOAT) std::clamp(CHANNEL_FLOAT, 0.0f, MAX_CHANNEL_VALUE)
	
	Color::Color() : a4fChannels{MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE} {}
	Color::Color(float fRed, float fGreen, float fBlue, float fAlpha) : a4fChannels{CLAMP_CHANNEL_VALUE(fRed), CLAMP_CHANNEL_VALUE(fGreen), CLAMP_CHANNEL_VALUE(fBlue), CLAMP_CHANNEL_VALUE(fAlpha)} {}
	Color::Color(const Color &rCopyColor) : a4fChannels{rCopyColor.a4fChannels[0], rCopyColor.a4fChannels[1], rCopyColor.a4fChannels[2], rCopyColor.a4fChannels[3]} {}
	Color::~Color() {}

	float Color::get_channel(const uint8_t u8ChannelIndex) const {
		if (u8ChannelIndex < u8ColorChannelCount)
			return a4fChannels[u8ChannelIndex];
		else
			RE_FATAL_ERROR("The channel index is not within the range [0, 3]: ", u8ChannelIndex);
		return 0.0f;
	}

	void Color::set_channel(const uint8_t u8ChannelIndex, const float fNormal) {
		if (u8ChannelIndex < u8ColorChannelCount)
			a4fChannels[u8ChannelIndex] = CLAMP_CHANNEL_VALUE(fNormal);
		else
			RE_FATAL_ERROR("The channel index is not within the range [0, 3]: ", u8ChannelIndex);
	}

	void Color::copy_from(const Color &rCopyColor) {
		for (uint8_t u8Channel = 0; u8Channel < u8ColorChannelCount; u8Channel++)
			a4fChannels[u8Channel] = rCopyColor.a4fChannels[u8Channel];
	}
	
	[[nodiscard]]
	bool Color::equals(const Color &rCompareColor) const {
		for (uint8_t u8Channel = 0; u8Channel < u8ColorChannelCount; u8Channel++)
			if (a4fChannels[u8Channel] != rCompareColor.a4fChannels[u8Channel])
				return false;
		return true;
	}

	void Color::set_red(const float fRed) {
		set_channel(0, fRed);
	}
	
	[[nodiscard]]
	float Color::get_red() const {
		return get_channel(0);
	}
	
	void Color::set_green(const float fGreen) {
		set_channel(1, fGreen);
	}
	
	[[nodiscard]]
	float Color::get_green() const {
		return get_channel(1);
	}
	
	void Color::set_blue(const float fBlue) {
		set_channel(2, fBlue);
	}
	
	[[nodiscard]]
	float Color::get_blue() const {
		return get_channel(2);
	}
	
	void Color::set_alpha(const float fAlpha) {
		set_channel(3, fAlpha);
	}
	
	[[nodiscard]]
	float Color::get_alpha() const {
		return get_channel(3);
	}

	[[nodiscard]]
	float Color::operator [](const uint32_t u32ChannelIndex) const {
		if (u32ChannelIndex < u8ColorChannelCount)
			return a4fChannels[u32ChannelIndex];
		else {
			RE_ERROR("The channel index range is [0, 3], but yours was ", u32ChannelIndex);
			return 0.0f;
		}
	}

	void Color::operator =(const Color &rCopyColor) {
		copy_from(rCopyColor);
	}
	
	[[nodiscard]]
	bool Color::operator ==(const Color &rCompareColor) const {
		return equals(rCompareColor);
	}
	
	[[nodiscard]]
	bool Color::operator !=(const Color &rCompareColor) const {
		return equals(rCompareColor);
	}

}
