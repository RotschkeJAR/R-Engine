#include "RE_Internal Header.hpp"

namespace RE {
	
	Color::Color() : channels{1.0f, 1.0f, 1.0f, 1.0f} {}
	Color::~Color() {}

	void Color::copy_from(const Color &copyColor) {
		for (uint8_t u8Channel = 0U; u8Channel < 4U; u8Channel++)
			channels[u8Channel] = copyColor.channels[u8Channel];
	}
	
	bool Color::equals(const Color &compareColor) const {
		for (uint8_t u8Channel = 0U; u8Channel < 4U; u8Channel++)
			if (channels[u8Channel] != compareColor.channels[u8Channel])
				return false;
		return true;
	}

	float Color::operator [](const uint32_t u32ChannelIndex) const {
		if (u32ChannelIndex < 4)
			return channels[u32ChannelIndex];
		else {
			RE_ERROR(append_to_string("The channels index range is [0, 3], but yours was ", u32ChannelIndex));
			return 0.0f;
		}
	}

	void Color::operator =(const Color &copyColor) {
		copy_from(copyColor);
	}
	
	bool Color::operator ==(const Color &compareColor) const {
		return equals(compareColor);
	}
	
	bool Color::operator !=(const Color &compareColor) const {
		return equals(compareColor);
	}

}
