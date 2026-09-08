#include "RE_Internal.hpp"

namespace RE {

#define RED     0
#define GREEN   1
#define BLUE    2
#define ALPHA   3

	Color::Color(float fRed, float fGreen, float fBlue, float fAlpha) : afChannels{
				Color::clamp(fRed),
				Color::clamp(fGreen),
				Color::clamp(fBlue),
				Color::clamp(fAlpha)} {}
	Color::Color(const Color &rCopy) : afChannels{
				rCopy.afChannels[RED],
				rCopy.afChannels[GREEN],
				rCopy.afChannels[BLUE],
				rCopy.afChannels[ALPHA]} {}
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
		for (unsigned uChannelIndex = 0; uChannelIndex < uColorChannelCount; uChannelIndex++)
			afChannels[uChannelIndex] = rCopy.afChannels[uChannelIndex];
	}
	
	[[nodiscard]]
	bool Color::equals(const Color &rOther) const {
		for (unsigned uChannelIndex = 0; uChannelIndex < uColorChannelCount; uChannelIndex++)
			if (afChannels[uChannelIndex] != rOther.afChannels[uChannelIndex])
				return false;
		return true;
	}

	void Color::set_red(const float fRed) {
		set_channel<RED>(fRed);
	}
	
	[[nodiscard]]
	float Color::get_red() const {
		return get_channel<RED>();
	}
	
	void Color::set_green(const float fGreen) {
		set_channel<GREEN>(fGreen);
	}
	
	[[nodiscard]]
	float Color::get_green() const {
		return get_channel<GREEN>();
	}
	
	void Color::set_blue(const float fBlue) {
		set_channel<BLUE>(fBlue);
	}
	
	[[nodiscard]]
	float Color::get_blue() const {
		return get_channel<BLUE>();
	}
	
	void Color::set_alpha(const float fAlpha) {
		set_channel<ALPHA>(fAlpha);
	}
	
	[[nodiscard]]
	float Color::get_alpha() const {
		return get_channel<ALPHA>();
	}

	[[nodiscard]]
	float Color::operator [](const uint8_t u8ChannelIndex) const {
		if (u8ChannelIndex < uColorChannelCount)
			return afChannels[u8ChannelIndex];
		RE_ERROR("The channel index range is [0; ", uColorChannelCount - 1, "], but yours was ", u8ChannelIndex);
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
		return not equals(rOther);
	}

}
