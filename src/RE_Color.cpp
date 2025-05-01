#include "RE_Internal Header.hpp"

namespace RE {
	
	Color::Color() : channels{1.0f, 1.0f, 1.0f, 1.0f} {}
	Color::Color(Color &rCopyColor) : channels{rCopyColor.channels[0], rCopyColor.channels[1], rCopyColor.channels[2], rCopyColor.channels[3]} {}
	Color::~Color() {}

	void Color::set_channel(const uint8_t u8ChannelIndex, const float fNormal) {
		switch (u8ChannelIndex) {
			case 0U:
			case 1U:
			case 2U:
			case 3U:
				channels[u8ChannelIndex] = std::clamp(fNormal, 0.0f, 1.0f);
				break;
			default:
				RE_ERROR(append_to_string("The channel index is not within the range [0, 3]: ", u8ChannelIndex).c_str());
				break;
		}
	}

	void Color::copy_from(const Color &rCopyColor) {
		for (uint8_t u8Channel = 0U; u8Channel < 4U; u8Channel++)
			channels[u8Channel] = rCopyColor.channels[u8Channel];
	}
	
	bool Color::equals(const Color &rCompareColor) const {
		for (uint8_t u8Channel = 0U; u8Channel < 4U; u8Channel++)
			if (channels[u8Channel] != rCompareColor.channels[u8Channel])
				return false;
		return true;
	}

	float Color::operator [](const uint32_t u32ChannelIndex) const {
		if (u32ChannelIndex < 4)
			return channels[u32ChannelIndex];
		else {
			RE_ERROR(append_to_string("The channel index range is [0, 3], but yours was ", u32ChannelIndex));
			return 0.0f;
		}
	}

	void Color::operator =(const Color &rCopyColor) {
		copy_from(rCopyColor);
	}
	
	bool Color::operator ==(const Color &rCompareColor) const {
		return equals(rCompareColor);
	}
	
	bool Color::operator !=(const Color &rCompareColor) const {
		return equals(rCompareColor);
	}

}
