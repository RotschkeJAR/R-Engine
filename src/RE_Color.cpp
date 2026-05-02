#include "RE_Internal Header.hpp"

namespace RE {

	Color::Color() : afChannels{
				f32MaxColor,
				f32MaxColor,
				f32MaxColor,
				f32MaxColor} {}
	Color::Color(const float f32Red, const float f32Green, const float f32Blue, const float f32Alpha) : afChannels{
				Color::clamp(f32Red),
				Color::clamp(f32Green),
				Color::clamp(f32Blue),
				Color::clamp(f32Alpha)} {}
	Color::Color(const Color &rCopy) : afChannels{
				rCopy.afChannels[0],
				rCopy.afChannels[1],
				rCopy.afChannels[2],
				rCopy.afChannels[3]} {}
	Color::~Color() {}

	float Color::get_channel(const uint8_t u8ChannelIndex) const {
		if (u8ChannelIndex < u8ColorChannelCount)
			return afChannels[u8ChannelIndex];
		RE_FATAL_ERROR("The channel index is not within the range [0; ", u8ColorChannelCount - 1, "]: ", u8ChannelIndex);
		return 0.0f;
	}

	void Color::set_channel(const uint8_t u8ChannelIndex, const float f32Normal) {
		if (u8ChannelIndex < u8ColorChannelCount)
			afChannels[u8ChannelIndex] = Color::clamp(f32Normal);
		else
			RE_FATAL_ERROR("The channel index is not within the range [0; ", u8ColorChannelCount - 1, "]: ", u8ChannelIndex);
	}

	void Color::copy_from(const Color &rCopy) {
		for (uint8_t u8Channel = 0; u8Channel < u8ColorChannelCount; u8Channel++)
			afChannels[u8Channel] = rCopy.afChannels[u8Channel];
	}
	
	[[nodiscard]]
	bool Color::equals(const Color &rOther) const {
		for (uint8_t u8Channel = 0; u8Channel < u8ColorChannelCount; u8Channel++)
			if (afChannels[u8Channel] != rOther.afChannels[u8Channel])
				return false;
		return true;
	}

	void Color::set_red(const float f32Red) {
		set_channel<0>(f32Red);
	}
	
	[[nodiscard]]
	float Color::get_red() const {
		return get_channel<0>();
	}
	
	void Color::set_green(const float f32Green) {
		set_channel<1>(f32Green);
	}
	
	[[nodiscard]]
	float Color::get_green() const {
		return get_channel<1>();
	}
	
	void Color::set_blue(const float f32Blue) {
		set_channel<2>(f32Blue);
	}
	
	[[nodiscard]]
	float Color::get_blue() const {
		return get_channel<2>();
	}
	
	void Color::set_alpha(const float f32Alpha) {
		set_channel<3>(f32Alpha);
	}
	
	[[nodiscard]]
	float Color::get_alpha() const {
		return get_channel<3>();
	}

	[[nodiscard]]
	float Color::operator [](const uint32_t u32ChannelIndex) const {
		if (u32ChannelIndex < u8ColorChannelCount)
			return afChannels[u32ChannelIndex];
		RE_ERROR("The channel index range is [0; ", u8ColorChannelCount - 1, "], but yours was ", u32ChannelIndex);
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
