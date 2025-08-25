#include "RE.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	ScreenPercentageSettings::ScreenPercentageSettings() : eMode(RE_SCREEN_PERCENTAGE_MODE_NORMAL), settings(1.0f) {}
	ScreenPercentageSettings::ScreenPercentageSettings(const ScreenPercentageMode eMode) : eMode(eMode), settings() {
		switch (eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				settings = 1.0f;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				settings.emplace<Vector2u>(windowSize);
				break;
			default:
				RE_WARNING("Unknown screen percentage mode ", hexadecimal_to_string<size_t>(eMode), ". Falling back to normal");
				this->eMode = RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				settings = 1.0f;
				break;
		}
	}
	ScreenPercentageSettings::ScreenPercentageSettings(const float fScale) : eMode(RE_SCREEN_PERCENTAGE_MODE_SCALED), settings(fScale) {}
	ScreenPercentageSettings::ScreenPercentageSettings(const Vector2u &rConstSize) : eMode(RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE), settings(rConstSize) {}
	ScreenPercentageSettings::ScreenPercentageSettings(ScreenPercentageMode eMode, const std::variant<float, Vector2u> &rSettings) {
		switch (eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				settings = 1.0f;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				if (std::holds_alternative<float>(rSettings))
					settings = std::get<float>(rSettings);
				else {
					RE_WARNING("Screen percentage mode is set to 'scaled', but the setting-union doesn't hold the mode's corresponding value. Setting it to 1.0 and falling back to normal mode");
					settings = 1.0f;
				}
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				if (std::holds_alternative<Vector2u>(rSettings))
					settings.emplace<Vector2u>(std::get<Vector2u>(rSettings));
				else {
					RE_WARNING("Screen percentage mode is set to 'const size', but the setting-union doesn't hold the mode's corresponding value. Using the window size instead");
					settings.emplace<Vector2u>(windowSize);
				}
				break;
			default:
				RE_WARNING("Unknown screen percentage mode ", hexadecimal_to_string<size_t>(eMode), ". Falling back to normal");
				eMode = RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				settings = 1.0f;
				break;
		}
		this->eMode = eMode;
	}
	ScreenPercentageSettings::ScreenPercentageSettings(const ScreenPercentageSettings &rCopy) : ScreenPercentageSettings(rCopy.eMode, rCopy.settings) {}
	ScreenPercentageSettings::~ScreenPercentageSettings() {}

}