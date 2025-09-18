#include "RE.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	ScreenPercentageSettings::ScreenPercentageSettings() : eMode(RE_SCREEN_PERCENTAGE_MODE_NORMAL) {}
	ScreenPercentageSettings::ScreenPercentageSettings(const ScreenPercentageMode eMode) : eMode(eMode) {
		switch (eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				fScale = 1.0f;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				constSize = windowSize;
				break;
			default:
				RE_WARNING("Unknown screen percentage mode ", std::hex, eMode, std::dec, ". Falling back to normal");
				this->eMode = RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				break;
		}
	}
	ScreenPercentageSettings::ScreenPercentageSettings(const float fScale) : eMode(RE_SCREEN_PERCENTAGE_MODE_SCALED), fScale(fScale) {}
	ScreenPercentageSettings::ScreenPercentageSettings(const Vector2u &rConstSize) : eMode(RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE), constSize(rConstSize) {}
	ScreenPercentageSettings::ScreenPercentageSettings(ScreenPercentageMode eMode, const std::variant<float, Vector2u> &rSettings) {
		switch (eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				if (std::holds_alternative<float>(rSettings))
					fScale = std::get<float>(rSettings);
				else {
					RE_WARNING("Screen percentage mode is set to 'scaled', but the setting-union doesn't hold the mode's corresponding value. Falling back to normal mode");
					eMode = RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				}
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				if (std::holds_alternative<Vector2u>(rSettings))
					constSize = std::get<Vector2u>(rSettings);
				else {
					RE_WARNING("Screen percentage mode is set to 'const size', but the setting-union doesn't hold the mode's corresponding value. Falling back to normal mode");
					eMode = RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				}
				break;
			default:
				RE_WARNING("Unknown screen percentage mode ", std::hex, eMode, std::dec, ". Falling back to normal");
				eMode = RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				break;
		}
		this->eMode = eMode;
	}
	ScreenPercentageSettings::ScreenPercentageSettings(const ScreenPercentageSettings &rCopy) : eMode(rCopy.eMode) {
		switch (rCopy.eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				fScale = rCopy.fScale;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				constSize = rCopy.constSize;
				break;
			default:
				RE_WARNING("The screen percentage mode ", std::hex, rCopy.eMode, std::dec, " of the 'screen percentage setting'-struct is invalid and makes copying insufficient. Falling back to normal mode");
				eMode = RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				return;
		}
		eMode = rCopy.eMode;
	}
	ScreenPercentageSettings::~ScreenPercentageSettings() {}

	void ScreenPercentageSettings::copy_from(const ScreenPercentageSettings &rCopy) {
		switch (rCopy.eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				fScale = rCopy.fScale;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				constSize = rCopy.constSize;
				break;
			default:
				RE_WARNING("The screen percentage mode ", std::hex, rCopy.eMode, std::dec, " is invalid, which makes further copying insufficient");
				return;
		}
		eMode = rCopy.eMode;
	}

	[[nodiscard]]
	bool ScreenPercentageSettings::equals(const ScreenPercentageSettings &rCompare) const {
		if (eMode != rCompare.eMode)
			return false;
		switch (eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				return true;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				return fScale == rCompare.fScale;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				return constSize == rCompare.constSize;
			default:
				RE_WARNING("The 'screen percentage setting'-struct itself contains an invalid screen percentage mode ", std::hex, eMode, std::dec, ", which makes further comparing insufficient");
				return false;
		}
	}
	
	void ScreenPercentageSettings::operator =(const ScreenPercentageSettings &rCopy) {
		PUSH_TO_CALLSTACKTRACE(copy_from(rCopy));
	}
	
	[[nodiscard]]
	bool ScreenPercentageSettings::operator ==(const ScreenPercentageSettings &rCompare) const {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(equals(rCompare), bool);
	}
	
	[[nodiscard]]
	bool ScreenPercentageSettings::operator !=(const ScreenPercentageSettings &rCompare) const {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(!equals(rCompare), bool);
	}

}