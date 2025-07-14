#include "RE_Internal Header.hpp"

namespace RE {

#define MAX_CHANNEL_VALUE 1.0f
#define CLAMP_CHANNEL_VALUE(CHANNEL_FLOAT) std::clamp(CHANNEL_FLOAT, 0.0f, MAX_CHANNEL_VALUE)
	
	Color::Color() : a4fChannels{MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE} {}
	Color::Color(Color &rCopyColor) : a4fChannels{rCopyColor.a4fChannels[0], rCopyColor.a4fChannels[1], rCopyColor.a4fChannels[2], rCopyColor.a4fChannels[3]} {}
	Color::~Color() {}

	void Color::set_channel(const uint8_t u8ChannelIndex, const float fNormal) {
		switch (u8ChannelIndex) {
			case 0U:
			case 1U:
			case 2U:
			case 3U:
				a4fChannels[u8ChannelIndex] = CLAMP_CHANNEL_VALUE(fNormal);
				break;
			default:
				RE_ERROR(append_to_string("The channel index is not within the range [0, 3]: ", u8ChannelIndex).c_str());
				break;
		}
	}

	void Color::copy_from(const Color &rCopyColor) {
		for (uint8_t u8Channel = 0U; u8Channel < 4U; u8Channel++)
			a4fChannels[u8Channel] = rCopyColor.a4fChannels[u8Channel];
	}
	
	[[nodiscard]]
	bool Color::equals(const Color &rCompareColor) const {
		for (uint8_t u8Channel = 0U; u8Channel < 4U; u8Channel++)
			if (a4fChannels[u8Channel] != rCompareColor.a4fChannels[u8Channel])
				return false;
		return true;
	}

	void Color::set_red(const float fRed) {
		a4fChannels[0] = CLAMP_CHANNEL_VALUE(fRed);
	}
	
	[[nodiscard]]
	float Color::get_red() const {
		return a4fChannels[0];
	}
	
	void Color::set_green(const float fGreen) {
		a4fChannels[1] = CLAMP_CHANNEL_VALUE(fGreen);
	}
	
	[[nodiscard]]
	float Color::get_green() const {
		return a4fChannels[1];
	}
	
	void Color::set_blue(const float fBlue) {
		a4fChannels[2] = CLAMP_CHANNEL_VALUE(fBlue);
	}
	
	[[nodiscard]]
	float Color::get_blue() const {
		return a4fChannels[2];
	}
	
	void Color::set_alpha(const float fAlpha) {
		a4fChannels[3] = CLAMP_CHANNEL_VALUE(fAlpha);
	}
	
	[[nodiscard]]
	float Color::get_alpha() const {
		return a4fChannels[3];
	}

	[[nodiscard]]
	float Color::operator [](const uint32_t u32ChannelIndex) const {
		if (u32ChannelIndex < 4)
			return a4fChannels[u32ChannelIndex];
		else {
			RE_ERROR(append_to_string("The channel index range is [0, 3], but yours was ", u32ChannelIndex));
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
