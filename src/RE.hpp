#ifndef __RE_H__
#define __RE_H__

#if defined(_WIN64) || defined(_MSC_VER)
# define RE_OS_WINDOWS 1
# define NOGDI
# ifdef _MSC_VER
#  define NOMINMAX
# endif
# define WIN32_LEAN_AND_MEAN /* speeds compilation up */
# include <windows.h>
#elif defined(__linux__)
# define RE_OS_LINUX 1
#else
# warning The targeted OS is unknown to R-Engine
#endif

#include <iostream>
#include <sstream>
#include <cstring>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <random>
#include <limits.h>
#include <array>
#include <variant>
#include <ctime>
#include <iomanip>
#include <optional>
#include <atomic>
#include <chrono>
#include <functional>
#include <thread>
#include <concepts>
#include <numbers>

namespace RE {

	typedef class Context_T final {} *Context;
	Context alloc_context();
	void free_context(Context hContext);

#define STRIP_QUOTE_MACRO(...) __VA_ARGS__

	template <class... T>
	concept Arithmetics = (std::is_arithmetic_v<T> && ...);

	template <class... T>
	concept Integrals = (std::is_integral_v<T> && ...);

	template <class... T>
	concept Floating_Point_Numbers = (std::is_floating_point_v<T> && ...);

	template <class TargetedType, class... T>
	concept Are_Same = (std::is_same_v<T, TargetedType> && ...);

	template <class T, T value1, T value2>
	concept Less_Than = (value1 < value2);

	template <class T, T value1, T value2>
	concept Less_Than_Equal = (value1 <= value2);

	template <class T, T value1, T value2>
	concept Greater_Than = (value1 > value2);

	template <class T, T value1, T value2>
	concept Greater_Than_Equal = (value1 >= value2);

	template <class T, T value1, T value2>
	concept Equal = (value1 == value2);

	template <class T, T value1, T value2>
	concept Not_Equal = (value1 != value2);

	enum TerminalColor {
		RE_TERMINAL_COLOR_BLACK = 0x0,
		RE_TERMINAL_COLOR_RED = 0x1,
		RE_TERMINAL_COLOR_GREEN = 0x2,
		RE_TERMINAL_COLOR_YELLOW = 0x3,
		RE_TERMINAL_COLOR_BLUE = 0x4,
		RE_TERMINAL_COLOR_MAGENTA = 0x5,
		RE_TERMINAL_COLOR_CYAN = 0x6,
		RE_TERMINAL_COLOR_WHITE = 0x7,
		RE_TERMINAL_COLOR_BRIGHT_BLACK = 0x8, /* gray */
		RE_TERMINAL_COLOR_BRIGHT_RED = 0x9,
		RE_TERMINAL_COLOR_BRIGHT_GREEN = 0xA,
		RE_TERMINAL_COLOR_BRIGHT_YELLOW = 0xB,
		RE_TERMINAL_COLOR_BRIGHT_BLUE = 0xC,
		RE_TERMINAL_COLOR_BRIGHT_MAGENTA = 0xD,
		RE_TERMINAL_COLOR_BRIGHT_CYAN = 0xE,
		RE_TERMINAL_COLOR_BRIGHT_WHITE = 0xF,
		RE_TERMINAL_COLOR_MAX_ENUM = 0x10
	};

	enum Input {
		RE_INPUT_UNKNOWN = -0x1,
		RE_INPUT_SCROLL_UP = 0x0,
		RE_INPUT_SCROLL_DOWN = 0x1,
		RE_INPUT_BUTTON_LEFT = 0x2,
		RE_INPUT_BUTTON_RIGHT = 0x3,
		RE_INPUT_BUTTON_MIDDLE = 0x4,
		RE_INPUT_KEY_SPACE = 0x5,
		RE_INPUT_KEY_A = 0x6,
		RE_INPUT_KEY_B = 0x7,
		RE_INPUT_KEY_C = 0x8,
		RE_INPUT_KEY_D = 0x9,
		RE_INPUT_KEY_E = 0xA,
		RE_INPUT_KEY_F = 0xB,
		RE_INPUT_KEY_G = 0xC,
		RE_INPUT_KEY_H = 0xD,
		RE_INPUT_KEY_I = 0xE,
		RE_INPUT_KEY_J = 0xF,
		RE_INPUT_KEY_K = 0x10,
		RE_INPUT_KEY_L = 0x11,
		RE_INPUT_KEY_M = 0x12,
		RE_INPUT_KEY_N = 0x13,
		RE_INPUT_KEY_O = 0x14,
		RE_INPUT_KEY_P = 0x15,
		RE_INPUT_KEY_Q = 0x16,
		RE_INPUT_KEY_R = 0x17,
		RE_INPUT_KEY_S = 0x18,
		RE_INPUT_KEY_T = 0x19,
		RE_INPUT_KEY_U = 0x1A,
		RE_INPUT_KEY_V = 0x1B,
		RE_INPUT_KEY_W = 0x1C,
		RE_INPUT_KEY_X = 0x1D,
		RE_INPUT_KEY_Y = 0x1E,
		RE_INPUT_KEY_Z = 0x1F,
		RE_INPUT_KEY_TOP_0 = 0x20,
		RE_INPUT_KEY_TOP_1 = 0x21,
		RE_INPUT_KEY_TOP_2 = 0x22,
		RE_INPUT_KEY_TOP_3 = 0x23,
		RE_INPUT_KEY_TOP_4 = 0x24,
		RE_INPUT_KEY_TOP_5 = 0x25,
		RE_INPUT_KEY_TOP_6 = 0x26,
		RE_INPUT_KEY_TOP_7 = 0x27,
		RE_INPUT_KEY_TOP_8 = 0x28,
		RE_INPUT_KEY_TOP_9 = 0x29,
		RE_INPUT_KEY_SLASH = 0x2A, /* / */
		RE_INPUT_KEY_BACKSLASH = 0x2B, /* \ */
		RE_INPUT_KEY_COMMA = 0x2C, /* , */
		RE_INPUT_KEY_PERIOD = 0x2D, /* . */
		RE_INPUT_KEY_SEMICOLON = 0x2E, /* ; */
		RE_INPUT_KEY_APOSTROPHE = 0x2F, /* ' */
		RE_INPUT_KEY_ACCENT = 0x30, /* ` */
		RE_INPUT_KEY_BRACKET_LEFT = 0x31, /* [ */
		RE_INPUT_KEY_BRACKET_RIGHT = 0x32, /* ] */
		RE_INPUT_KEY_EQUALS = 0x33, /* = */
		RE_INPUT_KEY_MINUS = 0x34, /* - */
		RE_INPUT_KEY_CTRL_RIGHT = 0x35,
		RE_INPUT_KEY_CTRL_LEFT = 0x36,
		RE_INPUT_KEY_ALT_RIGHT = 0x37, /* AltGr */
		RE_INPUT_KEY_ALT_LEFT = 0x38,
		RE_INPUT_KEY_SHIFT_RIGHT = 0x39,
		RE_INPUT_KEY_SHIFT_LEFT = 0x3A,
		RE_INPUT_KEY_MENU = 0x3B,
		RE_INPUT_KEY_TAB = 0x3C,
		RE_INPUT_KEY_ENTER = 0x3D,
		RE_INPUT_KEY_ESCAPE = 0x3E,
		RE_INPUT_KEY_BACKSPACE = 0x3F,
		RE_INPUT_KEY_ARROW_LEFT = 0x40,
		RE_INPUT_KEY_ARROW_RIGHT = 0x41,
		RE_INPUT_KEY_ARROW_UP = 0x42,
		RE_INPUT_KEY_ARROW_DOWN = 0x43,
		RE_INPUT_KEY_DELETE = 0x44,
		RE_INPUT_KEY_INSERT = 0x45,
		RE_INPUT_KEY_HOME = 0x46,
		RE_INPUT_KEY_END = 0x47,
		RE_INPUT_KEY_PRINT_SCREEN = 0x48,
		RE_INPUT_KEY_SCROLL_LOCK = 0x49,
		RE_INPUT_KEY_PAUSE = 0x4A,
		RE_INPUT_KEY_PAGE_UP = 0x4B,
		RE_INPUT_KEY_PAGE_DOWN = 0x4C,
		RE_INPUT_KEY_F1 = 0x4D,
		RE_INPUT_KEY_F2 = 0x4E,
		RE_INPUT_KEY_F3 = 0x4F,
		RE_INPUT_KEY_F4 = 0x50,
		RE_INPUT_KEY_F5 = 0x51,
		RE_INPUT_KEY_F6 = 0x52,
		RE_INPUT_KEY_F7 = 0x53,
		RE_INPUT_KEY_F8 = 0x54,
		RE_INPUT_KEY_F9 = 0x55,
		RE_INPUT_KEY_F10 = 0x56,
		RE_INPUT_KEY_F11 = 0x57,
		RE_INPUT_KEY_F12 = 0x58,
		RE_INPUT_KEY_F13 = 0x59,
		RE_INPUT_KEY_F14 = 0x5A,
		RE_INPUT_KEY_F15 = 0x5B,
		RE_INPUT_KEY_F16 = 0x5C,
		RE_INPUT_KEY_F17 = 0x5D,
		RE_INPUT_KEY_F18 = 0x5E,
		RE_INPUT_KEY_F19 = 0x5F,
		RE_INPUT_KEY_F20 = 0x60,
		RE_INPUT_KEY_F21 = 0x61,
		RE_INPUT_KEY_F22 = 0x62,
		RE_INPUT_KEY_F23 = 0x63,
		RE_INPUT_KEY_F24 = 0x64,
		RE_INPUT_KEY_F25 = 0x65,
		RE_INPUT_KEY_CAPS_LOCK = 0x66,
		RE_INPUT_KEY_NUMPAD_LOCK = 0x67,
		RE_INPUT_KEY_NUMPAD_0 = 0x68,
		RE_INPUT_KEY_NUMPAD_1 = 0x69,
		RE_INPUT_KEY_NUMPAD_2 = 0x6A,
		RE_INPUT_KEY_NUMPAD_3 = 0x6B,
		RE_INPUT_KEY_NUMPAD_4 = 0x6C,
		RE_INPUT_KEY_NUMPAD_5 = 0x6D,
		RE_INPUT_KEY_NUMPAD_6 = 0x6E,
		RE_INPUT_KEY_NUMPAD_7 = 0x6F,
		RE_INPUT_KEY_NUMPAD_8 = 0x70,
		RE_INPUT_KEY_NUMPAD_9 = 0x71,
		RE_INPUT_KEY_NUMPAD_ADD = 0x72,
		RE_INPUT_KEY_NUMPAD_SUBTRACT = 0x73,
		RE_INPUT_KEY_NUMPAD_MULTIPLY = 0x74,
		RE_INPUT_KEY_NUMPAD_DIVIDE = 0x75,
		RE_INPUT_KEY_NUMPAD_ENTER = 0x76,
		RE_INPUT_KEY_NUMPAD_PERIOD = 0x77,
		/**
		 * Represents a key, that does not exist on US-keyboards:
		 * - UK, DE (next to left shift): less than, (shift) greater than, (right Alt = AltGr) vertical bar/pipe
		 */
		RE_INPUT_KEY_WORLD_1 = 0x78,
		RE_INPUT_MAX_ENUM = 0x79
	};

	enum TextureFilter {
		/**
		 * sharp image / each individual pixel of the texture is clearly visible
		 */
		RE_TEXTURE_FILTER_NEAREST = 0,
		/**
		 * blurry image / smooth transition between pixels
		 */
		RE_TEXTURE_FILTER_LINEAR = 1
	};

	enum TextureRepetition {
		RE_TEXTURE_REPETITION_REPEAT = 0,
		RE_TEXTURE_REPETITION_MIRRORED_REPEAT = 1,
		RE_TEXTURE_REPETITION_CLAMP_TO_EDGE = 2,
		RE_TEXTURE_REPETITION_CLAMP_TO_BORDER = 3
	};

	enum BorderColor {
		RE_BORDER_COLOR_TRANSPARENT = 0,
		RE_BORDER_COLOR_BLACK = 1,
		RE_BORDER_COLOR_WHITE = 2
	};

	enum ScreenPercentageMode {
		RE_SCREEN_PERCENTAGE_MODE_NORMAL,
		RE_SCREEN_PERCENTAGE_MODE_SCALED,
		RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE
	};

	enum MsaaMode {
		RE_MSAA_MODE_1 = 0,
		RE_MSAA_MODE_2 = 1,
		RE_MSAA_MODE_4 = 2,
		RE_MSAA_MODE_8 = 3,
		RE_MSAA_MODE_16 = 4,
		RE_MSAA_MODE_32 = 5,
		RE_MSAA_MODE_64 = 6,
		RE_MSAA_MODE_DISABLED = RE_MSAA_MODE_1
	};

	void set_signal_handlers();

	template <class... T>
	void print(const T... content) {
		const std::ios_base::fmtflags ePreviousSettings = std::cout.setf(std::ios_base::showbase | std::ios_base::boolalpha);
		([&]() {
			if constexpr (std::is_same_v<T, int8_t>)
				std::cout << static_cast<int16_t>(content);
			else if constexpr (std::is_same_v<T, uint8_t>)
				std::cout << static_cast<uint16_t>(content);
			else if constexpr (std::is_same_v<T, std::wstring> || std::is_same_v<T, const wchar_t*> || std::is_same_v<T, wchar_t>)
				std::wcout << content;
			else if constexpr (std::is_same_v<T, char8_t>)
				std::cout << reinterpret_cast<char>(content);
			else if constexpr (std::is_same_v<T, char8_t*> || std::is_same_v<T, const char8_t*>)
				std::cout << reinterpret_cast<const char*>(content);
			else if constexpr (std::is_same_v<T, std::u8string>)
				std::cout << reinterpret_cast<const char*>(content.c_str());
			else
				std::cout << content;
		} (), ...);
		std::cout.flags(ePreviousSettings);
	}
	template <class... T>
	void println(const T... content) {
		print(content..., '\n');
	}
	void print_colored(const std::string &rsContent, TerminalColor eColor, bool bBackgroundColored, bool bBold);
	void println_colored(const std::string &rsContent, TerminalColor eColor, bool bBackgroundColored, bool bBold);
#define PRINT(...) print(__FILE__, " (line ", __LINE__, "): ", STRIP_QUOTE_MACRO(__VA_ARGS__))
#define PRINT_LN(...) PRINT(STRIP_QUOTE_MACRO(__VA_ARGS__), '\n')

#ifndef RE_DISABLE_PRINT_DEBUGS
# define PRINT_DEBUG(...) [&](const char *const pacFile, const char *const pacFunc, const uint32_t u32Line) { \
			time_t currentTime = std::time(0); \
			println("[", std::put_time(std::gmtime(&currentTime), "%d.%b %Y, %H:%M:%S"), "] (", pacFile, ", at line ", u32Line, ", in function \"", pacFunc, "\"): ", STRIP_QUOTE_MACRO(__VA_ARGS__)); \
		} (__FILE__, __func__, __LINE__)
#else
# define PRINT_DEBUG(...)
#endif
#define PRINT_DEBUG_CLASS(...) PRINT_DEBUG("{", this, "} ", __VA_ARGS__)
	
	void abort(const std::string &rsDetail);
	void fatal_error(Context hContext, const std::string &rsDetail);
	void error(Context hContext, const std::string &rsDetail);
	void warning(Context hContext, const std::string &rsDetail);
	void note(Context hContext, const std::string &rsDetail);
#define ABORT(...) RE::abort(append_to_string(STRIP_QUOTE_MACRO(__VA_ARGS__), "\nIn ", __FILE__, ", function \"", __func__, "\", at line ", __LINE__))
#define FATAL_ERROR(CONTEXT_HANDLE, ...) fatal_error(CONTEXT_HANDLE, append_to_string(STRIP_QUOTE_MACRO(__VA_ARGS__), "\nIn ", __FILE__, ", function \"", __func__, "\", at line ", __LINE__))
#define ERROR(...) error(append_to_string(STRIP_QUOTE_MACRO(__VA_ARGS__), "\nIn ", __FILE__, ", function \"", __func__, "\", at line ", __LINE__))
#define WARNING(...) warning(append_to_string(STRIP_QUOTE_MACRO(__VA_ARGS__), "\nIn ", __FILE__, ", function \"", __func__, "\", at line ", __LINE__))
#define NOTE(...) note(append_to_string(STRIP_QUOTE_MACRO(__VA_ARGS__), "\nIn ", __FILE__, ", function \"", __func__, "\", at line ", __LINE__))

#define DELETE_SAFELY(PTR_REF) [&]() { \
			if (!PTR_REF) \
				break; \
			PRINT_DEBUG("Safely deleting ", PTR_REF); \
			delete (PTR_REF); \
			(PTR_REF) = nullptr; \
		} ()
#define DELETE_ARRAY_SAFELY(PTR_REF) [&]() { \
			if (!PTR_REF) \
				break; \
			PRINT_DEBUG("Safely deleting array ", PTR_REF); \
			delete[] (PTR_REF); \
			(PTR_REF) = nullptr; \
		} ()

	template <class T>
	[[nodiscard]]
	const char* resolve_string_class(const T& rString) {
		static_assert(!std::is_same_v<T, std::string_view>, "String views cannot be resolved");
		static_assert(std::is_same_v<T, std::u8string> || std::is_same_v<T, const char*> || std::is_same_v<T, char*>, "This function only accepts string-like datatypes, that support plain characters (wide chars and unicodes except UTF-8 aren't accepted). String views cannot be resolved however");
		if constexpr (std::is_same_v<T, std::u8string>)
			return reinterpret_cast<const char*>(rString.c_str());
		else if constexpr (std::is_same_v<T, const char8_t*> || std::is_same_v<T, char8_t*>)
			return reinterpret_cast<const char*>(rString);
		else if constexpr (std::is_same_v<T, std::string>)
			return rString.c_str();
		else if constexpr (std::is_same_v<T, char*> || std::is_same_v<T, const char*>)
			return rString;
		else
			return nullptr;
	}

	template <class T>
	[[nodiscard]]
	std::string array_to_string(const T (&rArray)[], const size_t arrayLength) {
		std::stringstream sStream("{");
		sStream.setf(std::ios_base::showbase | std::ios_base::boolalpha);
		for (size_t i = 0; i < arrayLength; i++) {
			if (i)
				sStream << ", ";
			if constexpr (std::is_same_v<T, int8_t>)
				sStream << rArray[i];
			else if constexpr (std::is_same_v<T, uint8_t>)
				sStream << rArray[i];
			else
				sStream << rArray[i];
		}
		sStream << "}";
		return sStream.str();
	}

	template <class... T>
	[[nodiscard]]
	std::string append_to_string(const T... strings) {
		std::stringstream sStream;
		sStream.setf(std::ios_base::showbase | std::ios_base::boolalpha);
		([&]() {
			if constexpr (std::is_same_v<T, int8_t>)
				sStream << static_cast<int16_t>(strings);
			else if constexpr (std::is_same_v<T, uint8_t>)
				sStream << static_cast<uint16_t>(strings);
			else
				sStream << strings;
		} (), ...);
		return sStream.str();
	}

	template <class... T>
	[[nodiscard]]
	std::wstring append_to_wstring(const T... strings) {
		std::wstringstream wsStream;
		wsStream.setf(std::ios_base::showbase | std::ios_base::boolalpha);
		([&]() {
			if constexpr (std::is_same_v<T, int8_t>)
				wsStream << static_cast<int16_t>(strings);
			else if constexpr (std::is_same_v<T, uint8_t>)
				wsStream << static_cast<uint16_t>(strings);
			else
				wsStream << strings;
		} (), ...);
		return wsStream.str();
	}

	[[nodiscard]]
	bool is_string_empty(const char *pacString);
	[[nodiscard]]
	size_t get_string_length_safely(const char *pacString);
	[[nodiscard]]
	size_t get_wide_string_length_safely(const wchar_t *pacWideString);
	[[nodiscard]]
	bool are_string_contents_equal(const char *pacString1, const char *pacString2);
	[[nodiscard]]
	size_t get_line_count(const char *pacString);
	[[nodiscard]]
	std::string get_line(const char *pacString, size_t lineIndex);
	[[nodiscard]]
	std::string convert_wide_chars_to_utf8(const wchar_t *pwcString);
	[[nodiscard]]
	std::wstring convert_chars_to_wide(const char *pacString);
	[[nodiscard]]
	std::string get_app_name();

	template <class T> requires Arithmetics<T>
	[[nodiscard]]
	constexpr T nth_root(const T n, const T value) noexcept {
		static_assert(n > static_cast<T>(0.0), "The 0th or negative root is forbidden for causing undefined behaviour");
		if (n <= static_cast<T>(0.0)) {
			FATAL_ERROR("The value of n shouldn't be zero or negative in an nth root");
			return static_cast<T>(0.0);
		}
		return std::pow(value, static_cast<T>(1.0) / n);
	}

	template <class... T> requires Integrals<T...>
	[[nodiscard]]
	constexpr uint64_t gen_bitmask(const T... bits) noexcept {
		return (... | (1UL << static_cast<uint64_t>(bits)));
	}

	[[nodiscard]]
	constexpr uint64_t gen_bitmask_in_range(const uint64_t u64Begin, const uint64_t u64End) noexcept {
		if (u64Begin > u64End) {
			FATAL_ERROR("Start (", u64Begin, ") of the range is larger than end (", u64End, ")");
			return 0;
		} else if (u64Begin == u64End)
			return gen_bitmask(u64Begin);
		uint64_t u64Result = 0;
		for (uint64_t u64CurrentNumber = u64Begin; u64CurrentNumber < u64End; u64CurrentNumber++)
			u64Result |= gen_bitmask(u64CurrentNumber);
		return u64Result;
	}

	template <class T, class... U> requires Arithmetics<T>
	[[nodiscard]]
	constexpr bool are_bits_true(const T value, const U... bits) noexcept {
		const T bitmask = static_cast<T>(gen_bitmask<U...>(bits...));
		return (value & bitmask) == bitmask;
	}

	template <class T> requires Integrals<T>
	[[nodiscard]]
	constexpr bool are_bits_true_in_range(const T value, const T begin, const T end) noexcept {
		if (begin > end) {
			FATAL_ERROR("Start (", begin, ") of the range is larger than end (", end, ")");
			return false;
		}
		for (T i = begin; i < end; i++)
			if (!are_bits_true<T, T>(value, i))
				return false;
		return true;
	}

	template <class T, class... U> requires Integrals<T>
	constexpr T set_bits(T& rValue, const bool bNewState, const U... bits) noexcept {
		const T bitmask = gen_bitmask<U...>(bits...);
		if (bNewState)
			rValue |= bitmask;
		else
			rValue &= ~bitmask;
		return rValue;
	}

	template <class T> requires Integrals<T>
	constexpr T set_bits_in_range(T &rValue, const bool bNewState, const T begin, const T end) noexcept {
		if (begin > end) {
			FATAL_ERROR("Start (", begin, ") of the range is larger than end (", end, ")");
			return rValue;
		}
		for (T i = begin; i < end; i++)
			set_bits<T>(rValue, bNewState, i);
		return rValue;
	}

	template <class T> requires std::integral<T>
	void for_each_bit(const T bitmask, const std::function<void (T bitIndex)> iterFunction) {
		for (T bitIndex = 0; bitIndex < sizeof(T) * 8; bitIndex++)
			if (are_bits_true<T>(bitmask, bitIndex))
				std::invoke(iterFunction, bitIndex);
	}
	
	template <class T> requires Arithmetics<T>
	[[nodiscard]]
	constexpr T sign(const T value) noexcept {
		if constexpr (std::is_unsigned_v<T>)
			return static_cast<T>(0.0) > value;
		else
			return (static_cast<T>(0.0) < value ? 1 : 0) - (value < static_cast<T>(0.0) ? 1 : 0);
	}

	template <class T> requires Arithmetics<T>
	[[nodiscard]]
	constexpr bool multiple_of(const T value, const T multiple) {
		if constexpr (std::is_same_v<T, float>)
			return std::fmodf(value, multiple) == 0.0f;
		else if constexpr (std::is_same_v<T, double>)
			return std::fmod(value, multiple) == 0.0;
		else if constexpr (std::is_same_v<T, long double>)
			return std::fmodl(value, multiple) == 0.0;
		else
			return (value % multiple) == 0;
	}

	template <class T> requires Arithmetics<T>
	[[nodiscard]]
	constexpr T next_multiple(const T value, const T multiple) {
		if (value == static_cast<T>(0.0))
			return multiple;
		if constexpr (std::is_same_v<T, float>)
			return multiple - std::fmodf(value, multiple) + value;
		else if constexpr (std::is_same_v<T, double>)
			return multiple - std::fmod(value, multiple) + value;
		else if constexpr (std::is_same_v<T, long double>)
			return multiple - std::fmodl(value, multiple) + value;
		else
			return multiple - (value % multiple) + value;
	}

	template <class T> requires Arithmetics<T>
	[[nodiscard]]
	constexpr T previous_multiple(const T value, const T multiple) {
		if (value == static_cast<T>(0.0))
			return -multiple;
		if constexpr (std::is_same_v<T, float>)
			return value - std::fmodf(value, multiple);
		else if constexpr (std::is_same_v<T, double>)
			return value - std::fmod(value, multiple);
		else if constexpr (std::is_same_v<T, long double>)
			return value - std::fmodl(value, multiple);
		else
			return value - (value % multiple);
	}

	template <class T> requires std::floating_point<T>
	[[nodiscard]]
	constexpr T degrees_to_radians(const T degrees) noexcept {
		return degrees * (std::numbers::pi / 180.0f);
	}

	template <class T> requires std::floating_point<T>
	[[nodiscard]]
	constexpr T radians_to_degrees(const T radians) noexcept {
		return radians * (180.0f / std::numbers::pi);
	}

	template <class T> requires std::floating_point<T>
	[[nodiscard]]
	constexpr T sin_deg(const T degrees) noexcept {
		return std::sin(degrees_to_radians<T>(degrees));
	}

	template <class T> requires std::floating_point<T>
	[[nodiscard]]
	constexpr T cos_deg(const T degrees) noexcept {
		return std::cos(degrees_to_radians<T>(degrees));
	}

	template <class T> requires std::floating_point<T>
	[[nodiscard]]
	constexpr T tan_deg(const T degrees) noexcept {
		return std::tan(degrees_to_radians<T>(degrees));
	}



	template <class T, size_t dimensionCount> requires Arithmetics<T> && Greater_Than<size_t, dimensionCount, 0>
	class Vector final {
		public:
			std::array<T, dimensionCount> coords;

			Vector() {
				fill(static_cast<T>(0.0));
			}
			template <class P, size_t u32CopyDimensions>
			explicit Vector(const Vector<P, u32CopyDimensions> &rCopyVector) requires Arithmetics<P> && Less_Than_Equal<size_t, u32CopyDimensions, dimensionCount> {
				PRINT_DEBUG_CLASS("Copying coordinates from vector ", &rCopyVector);
				if constexpr (u32CopyDimensions <= dimensionCount) {
					std::copy(rCopyVector.coords.begin(), rCopyVector.coords.end(), coords.begin());
					std::fill(coords.begin() + u32CopyDimensions, coords.end(), static_cast<T>(0.0));
				} else
					std::copy(rCopyVector.begin(), rCopyVector.coords.end() - (u32CopyDimensions - dimensionCount), coords.begin());
			}
			template <class... V>
			Vector(const V... values) requires Arithmetics<V...> && Less_Than_Equal<size_t, sizeof...(V), dimensionCount> {
				PRINT_DEBUG_CLASS("Filling vector with values");
				size_t dimensionIndex = 0;
				([&]() {
					coords[dimensionIndex] = static_cast<T>(values);
					dimensionIndex++;
				} (), ...);
				std::fill(coords.begin() + dimensionIndex, coords.end(), static_cast<T>(0.0));
			}
			~Vector() {}

			[[nodiscard]]
			T sum() const {
				T sum = static_cast<T>(0.0);
				for (const T coord : coords)
					sum += coord;
				return sum;
			}

			[[nodiscard]]
			T area() const {
				T result = static_cast<T>(1.0);
				for (const T coord : coords)
					result *= coord;
				return result;
			}

			[[nodiscard]]
			T length() const {
				return nth_root<T>(static_cast<T>(dimensionCount), sum());
			}

			void fill(const T value) {
				PRINT_DEBUG_CLASS("Filling vector with value ", value);
				coords.fill(value);
			}

			void copy_from(const Vector &rCopyVector) {
				PRINT_DEBUG_CLASS("Copying coordinates from vector ", &rCopyVector);
				if (rCopyVector.dimensions() <= dimensionCount) {
					std::copy(rCopyVector.coords.begin(), rCopyVector.coords.end(), coords.begin());
					std::fill(coords.begin() + rCopyVector.dimensions(), coords.end(), static_cast<T>(0.0));
				} else
					std::copy(rCopyVector.coords.begin(), rCopyVector.coords.end() - (rCopyVector.dimensions() - dimensionCount), coords.begin());
			}

			void copy_from_array(const T *const paArray, const size_t arrayLength) {
				PRINT_DEBUG_CLASS("Copying ", arrayLength, " elements from array ", paArray, " to this vector");
				for (size_t i = 0; i < arrayLength; i++)
					coords[i] = paArray[i];
			}

			[[nodiscard]]
			bool equals(const Vector &rCompareVector) const {
				if (dimensionCount != rCompareVector.dimensions())
					return false;
				for (size_t dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
					if (coords[dimensionIndex] != rCompareVector[dimensionIndex])
						return false;
				return true;
			}

			[[nodiscard]]
			constexpr size_t dimensions() const {
				return dimensionCount;
			}

			T& operator [](const size_t dimensionIndex) {
				if (dimensionIndex >= dimensionCount)
					FATAL_ERROR("Index ", dimensionIndex, " is out of bounds: [0, ", dimensionCount, ")");
				return coords[dimensionIndex];
			}

			[[nodiscard]]
			T operator [](const size_t dimensionIndex) const {
				if (dimensionIndex >= dimensionCount)
					FATAL_ERROR("Index ", dimensionIndex, " is out of bounds: [0, ", dimensionCount, ")");
				return coords[dimensionIndex];
			}

			void operator =(const Vector &rCopyVector) {
				copy_from(rCopyVector);
			}

			[[nodiscard]]
			bool operator ==(const Vector &rCompareVector) const {
				return equals(rCompareVector);
			}

			[[nodiscard]]
			bool operator !=(const Vector &rCompareVector) const {
				return !equals(rCompareVector);
			}

			friend std::ostream& operator <<(std::ostream &rStream, const Vector &rVector) {
				rStream << "(";
				for (size_t i = 0; i < rVector.dimensions(); i++) {
					if (i)
						rStream << ", ";
					rStream << rVector.coords[i];
				}
				rStream << ")";
				return rStream;
			}
	};
	typedef Vector<float, 2> Vector2f;
	typedef Vector<float, 3> Vector3f;
	typedef Vector<float, 4> Vector4f;
	typedef Vector<double, 2> Vector2d;
	typedef Vector<double, 3> Vector3d;
	typedef Vector<double, 4> Vector4d;
	typedef Vector<int32_t, 2> Vector2i;
	typedef Vector<int32_t, 3> Vector3i;
	typedef Vector<int32_t, 4> Vector4i;
	typedef Vector<uint32_t, 2> Vector2u;
	typedef Vector<uint32_t, 3> Vector3u;
	typedef Vector<uint32_t, 4> Vector4u;

	template <size_t numOfThreads = 10> requires Greater_Than<size_t, numOfThreads, 0>
	class Threadpool final {
		private:
			std::array<std::jthread, numOfThreads> threads;
			std::array<uint64_t, numOfThreads> agePerThread;

			size_t wait_for_oldest_thread() {
				const size_t oldThreadIndex = std::min_element(agePerThread.begin(), agePerThread.end()) - agePerThread.begin();
				std::jthread greatestAgeFindThread([&]() {
					agePerThread[oldThreadIndex] = (*std::max_element(agePerThread.begin(), agePerThread.end())) + 1;
				});
				if (threads[oldThreadIndex].joinable())
					threads[oldThreadIndex].join();
				return oldThreadIndex;
			}

		public:
			Threadpool() {}
			~Threadpool() {}

			template <class F, class... paramTypes>
			void execute(F &&rrFunction, paramTypes... params) {
				const size_t oldThreadIndex = wait_for_oldest_thread();
				threads[oldThreadIndex] = std::jthread(rrFunction, params...);
			}

			void move_thread(std::jthread &&rrThread) {
				const size_t oldThreadIndex = wait_for_oldest_thread();
				threads[oldThreadIndex] = rrThread;
			}

			bool joinable() {
				for (std::jthread &rThread : threads)
					if (rThread.joinable())
						return true;
				return false;
			}

			void join() {
				std::jthread ageResetThread([&]() {
					agePerThread.fill(0);
				});
				for (std::jthread &rThread : threads)
					if (rThread.joinable())
						rThread.join();
			}

			size_t free_slots() {
				size_t numOfFreeSlots = 0;
				for (std::jthread &rThread : threads)
					if (!rThread.joinable())
						numOfFreeSlots++;
				return numOfFreeSlots;
			}

			size_t occupied_slots() {
				return get_amount_of_threads() - free_slots();
			}

			consteval size_t get_amount_of_threads() {
				return numOfThreads;
			}
	};

	class Color final {
		public:
			static constexpr uint8_t u8ColorChannelCount = 4;

		private:
			std::array<float, u8ColorChannelCount> a4fChannels;

		public:
			Color();
			Color(float fRed, float fGreen, float fBlue, float fAlpha);
			Color(const Color &rCopyColor);
			~Color();
			float get_channel(uint8_t u8ChannelIndex) const;
			void set_channel(uint8_t u8ChannelIndex, float fNormal);
			void copy_from(const Color &rCopyColor);
			[[nodiscard]]
			bool equals(const Color &rCompareColor) const;

			void set_red(float fRed);
			[[nodiscard]]
			float get_red() const;
			void set_green(float fGreen);
			[[nodiscard]]
			float get_green() const;
			void set_blue(float fBlue);
			[[nodiscard]]
			float get_blue() const;
			void set_alpha(float fAlpha);
			[[nodiscard]]
			float get_alpha() const;

			[[nodiscard]]
			float operator [](uint32_t u32ChannelIndex) const;
			void operator =(const Color &rCopyColor);
			[[nodiscard]]
			bool operator ==(const Color &rCompareColor) const;
			[[nodiscard]]
			bool operator !=(const Color &rCompareColor) const;
	};

	typedef class Texture_T final {} *Texture;
	typedef class SpriteLayout_T final {} *SpriteLayout;
	typedef class Sprite_T final {} *Sprite;
	typedef class Mesh_T final {} *Mesh;

	class SpriteRenderer final {
		public:
			Color color;
			Vector2f textureOffset, textureCoordinates;
			Sprite hSprite;
			Mesh hMesh;

			SpriteRenderer();
			~SpriteRenderer();
	};

	class RandomNumberGenerator final {
		private:
			std::mt19937 rng;

		public:
			RandomNumberGenerator();
			RandomNumberGenerator(size_t seed);
			~RandomNumberGenerator();
			void seed(size_t newSeed);
			void seed_randomly();

			template <class T>
			[[nodiscard]]
			T random(const T &rMin, const T &rMax) requires Arithmetics<T> {
				static_assert(std::is_arithmetic_v<T>, "This function accepts arithmetic datatypes only");
				if constexpr (std::is_integral_v<T>) {
					std::uniform_int_distribution<T> range(rMin, rMax - 1);
					return range(rng);
				} else if constexpr (std::is_floating_point_v<T>) {
					std::uniform_real_distribution<T> range(rMin, rMax);
					return range(rng);
				}
			}
			template <class T>
			[[nodiscard]]
			T random(const T &rMax) requires Arithmetics<T> {
				return random<T>(static_cast<T>(0.0), rMax);
			}
			template <class T>
			[[nodiscard]]
			T random() requires Arithmetics<T> {
				return random<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
			}
			[[nodiscard]]
			bool random_bool();
			[[nodiscard]]
			bool random_bool(double dChance);
			[[nodiscard]]
			double random_percentage();

			template <class T>
			[[nodiscard]]
			T operator ()(const T rMin, const T rMax) requires Arithmetics<T> {
				return random<T>(rMin, rMax);
			}
			template <class T>
			[[nodiscard]]
			T operator ()(const T &rMax) requires Arithmetics<T> {
				return random<T>(rMax);
			}
			template <class T>
			[[nodiscard]]
			T operator ()() requires Arithmetics<T> {
				if constexpr (std::is_same_v<T, bool>)
					return random_bool();
				else
					return random<T>();
			}
			[[nodiscard]]
			bool operator ()(const double &rdChance);
	};

	class Scene {
		public:
			const uint32_t u32Id;

			Scene() = delete;
			Scene(uint32_t u32Id);
			virtual ~Scene();

			bool is_current_scene() const;
			void set_next_scene();
			bool is_next_scene() const;

			virtual void start();
			virtual void update();
			virtual void end();
	};

	class Transform final {
		public:
			Vector3f position, scale;

			Transform();
			Transform(const Vector3f &rPositionCopy);
			Transform(const Vector3f &rPositionCopy, const Vector3f &rScaleCopy);
			Transform(const Transform &rCopyTransform);
			~Transform();
			void reset_position();
			void copy_from(const Transform &rCopyTransform);
			[[nodiscard]]
			bool equals(const Transform &rCompareTransform) const;

			void operator =(const Transform &rCopyTransform);
			[[nodiscard]]
			bool operator ==(const Transform &rCompareTransform) const;
			[[nodiscard]]
			bool operator !=(const Transform &rCompareTransform) const;
	};

	class Camera {
		public:
			Transform transform;

			Vector2f view;
			float fViewDistance;
			bool bIgnoreAspectRatio;

			Camera();
			Camera(const Transform &rTransform);
			Camera(const Vector2f &rView);
			Camera(const Vector2f &rView, float fViewDistance);
			Camera(const Vector2f &rView, float fViewDistance, bool bIgnoreAspectRatio);
			virtual ~Camera();
			virtual void update_before_render();
			void activate();
			void deactivate() const;
			void mark_deletable();
			[[nodiscard]]
			bool equals(const Camera &rCompareCamera) const;

			void operator =(const Camera &rCopyCamera);
			[[nodiscard]]
			bool operator ==(const Camera &rCompareCamera) const;
			[[nodiscard]]
			bool operator !=(const Camera &rCompareCamera) const;
	};

	class GameObject {
		public:
			Transform transform;
			SpriteRenderer spriteRenderer;
			const uint32_t u32OwnId;
			const uint32_t u32SceneParentId;

			GameObject() = delete;
			GameObject(uint32_t u32OwnId, uint32_t u32SceneParentId);
			virtual ~GameObject();

			void mark_deletable();

			virtual void start(Scene *pStartingScene);
			virtual void update(Scene *pCurrentScene);
			virtual void end(Scene *pEndingScene);
	};

	class InputAction final {
		private:
			uint32_t u32KeyScancode;
			Input eInput;

		public:
			InputAction();
			InputAction(Input eInput);
			InputAction(uint32_t u32KeyScancode);
			~InputAction();
			[[nodiscard]]
			bool is_scroll_wheel() const;
			[[nodiscard]]
			bool is_button() const;
			[[nodiscard]]
			bool is_mouse() const;
			[[nodiscard]]
			bool is_key() const;

			// Queries engine to set its input to the next user input
			void update_input();
			[[nodiscard]]
			bool is_updating() const;
			void cancel_update() const;
			[[nodiscard]]
			static bool can_update();

			void change_to_input(Input eInput);
			void change_to_scancode(uint32_t u32NewScancode);

			[[nodiscard]]
			bool is_down() const;
			[[nodiscard]]
			bool was_down() const;
			[[nodiscard]]
			bool is_pressed() const;
			[[nodiscard]]
			bool is_released() const;
			[[nodiscard]]
			bool is_held_down() const;
			void reset_input_state() const;

			[[nodiscard]]
			bool has_valid_input_values() const;
	};

	class ScreenPercentageSettings final {
		public:
			ScreenPercentageMode eMode;
			union {
				float fScale;
				Vector2u constSize;
			};
			TextureFilter eScalingFilter;

			ScreenPercentageSettings();
			ScreenPercentageSettings(float fScale);
			ScreenPercentageSettings(float fScale, TextureFilter eScalingFilter);
			ScreenPercentageSettings(const Vector2u &rConstSize);
			ScreenPercentageSettings(const Vector2u &rConstSize, TextureFilter eScalingFilter);
			ScreenPercentageSettings(ScreenPercentageMode eMode, const std::variant<float, Vector2u> &rSettings);
			ScreenPercentageSettings(ScreenPercentageMode eMode, const std::variant<float, Vector2u> &rSettings, TextureFilter eScalingFilter);
			ScreenPercentageSettings(const ScreenPercentageSettings &rCopy);
			~ScreenPercentageSettings();
			void copy_from(const ScreenPercentageSettings &rCopy);
			[[nodiscard]]
			bool equals(const ScreenPercentageSettings &rCompare) const;
			void operator =(const ScreenPercentageSettings &rCopy);
			[[nodiscard]]
			bool operator ==(const ScreenPercentageSettings &rCompare) const;
			[[nodiscard]]
			bool operator !=(const ScreenPercentageSettings &rCompare) const;
	};

	class SpriteLayoutSettings final {
		public:
			TextureFilter eMagFilter;
			TextureFilter eMinFilter;
			TextureFilter eMipmapFilter;
			TextureRepetition eTextureRepetitionU;
			TextureRepetition eTextureRepetitionV;
			float fMaxAnisotropy; // must be equal or greater than 1, otherwise anisotropic filtering is disabled
			BorderColor eBorderColor;

			SpriteLayoutSettings();
			SpriteLayoutSettings(TextureFilter eMagFilter);
			SpriteLayoutSettings(TextureFilter eMagFilter, TextureFilter eMinFilter);
			SpriteLayoutSettings(TextureFilter eMagFilter, TextureFilter eMinFilter, TextureFilter eMipmapFilter);
			SpriteLayoutSettings(TextureRepetition eTextureRepetitionU);
			SpriteLayoutSettings(TextureRepetition eTextureRepetitionU, TextureRepetition eTextureRepetitionV);
			SpriteLayoutSettings(float fMaxAnisotropy);
			SpriteLayoutSettings(BorderColor eBorderColor);
			SpriteLayoutSettings(TextureFilter eMagFilter, TextureFilter eMinFilter, TextureFilter eMipmapFilter, TextureRepetition eTextureRepetitionU, TextureRepetition eTextureRepetitionV, float fMaxAnisotropy, BorderColor eBorderColor);
			SpriteLayoutSettings(const SpriteLayoutSettings &rCopy);
			~SpriteLayoutSettings();
			void copy_from(const SpriteLayoutSettings &rCopy);
			[[nodiscard]]
			bool equals(const SpriteLayoutSettings &rCompare) const;
			void operator =(const SpriteLayoutSettings &rCopy);
			[[nodiscard]]
			bool operator ==(const SpriteLayoutSettings &rCompare) const;
			[[nodiscard]]
			bool operator !=(const SpriteLayoutSettings &rCompare) const;
	};

	// Window
	void set_window_title(const char *pacNewTitle);
	
	// Console
	void enable_colorful_printing(Context hContext, bool bEnable);
	[[nodiscard]]
	bool is_colorful_printing_enabled(Context hContext);
	void treat_warnings_as_errors(Context hContext, bool bEnable);
	[[nodiscard]]
	bool are_warnings_always_treated_as_errors(Context hContext);
	void make_errors_always_fatal(Context hContext, bool bEnable);
	[[nodiscard]]
	bool are_errors_always_fatal(Context hContext);

	// Cursor input
	[[nodiscard]]
	int32_t get_cursor_position_x();
	[[nodiscard]]
	int32_t get_cursor_position_y();
	[[nodiscard]]
	float get_cursor_normal_position_x();
	[[nodiscard]]
	float get_cursor_normal_position_y();
	void reset_mouse_input();

	// Keyboard input
	[[nodiscard]]
	Input map_scancode_to_input(uint32_t u32Scancode);
	[[nodiscard]]
	uint32_t map_input_to_scancode(Input eInput);
	void reset_keyboard_input();

	// Input
	[[nodiscard]]
	bool is_down(Input eInput, uint32_t u32Scancode);
	[[nodiscard]]
	bool was_down(Input eInput, uint32_t u32Scancode);
	[[nodiscard]]
	bool is_pressed(Input eInput, uint32_t u32Scancode);
	[[nodiscard]]
	bool is_released(Input eInput, uint32_t u32Scancode);
	[[nodiscard]]
	bool is_held_down(Input eInput, uint32_t u32Scancode);
	void reset_input_at(Input eInput, uint32_t u32Scancode);
	void reset_all_input();
	[[nodiscard]]
	constexpr bool is_scroll_input(const Input eInput) {
		return eInput == RE_INPUT_SCROLL_UP || eInput == RE_INPUT_SCROLL_DOWN;
	}
	[[nodiscard]]
	constexpr bool is_button_input(const Input eInput) {
		return eInput >= RE_INPUT_BUTTON_LEFT && eInput <= RE_INPUT_BUTTON_MIDDLE;
	}
	[[nodiscard]]
	constexpr bool is_mouse_input(const Input eInput) {
		return is_scroll_input(eInput) || is_button_input(eInput);
	}
	[[nodiscard]]
	constexpr bool is_key_input(const Input eInput) {
		return eInput >= RE_INPUT_KEY_SPACE && eInput < RE_INPUT_MAX_ENUM;
	}

	// Program execution
	bool execute(Context hContext);
	[[nodiscard]]
	float get_deltaseconds(Context hContext);
	[[nodiscard]]
	float get_fps_rate(Context hContext);
	void set_fps_limit(Context hContext, uint32_t u32MaxFramesPerSecond);
	[[nodiscard]]
	uint32_t get_fps_limit(Context hContext);
	void set_max_lag_time(Context hContext, float fSecondsOfLag);
	[[nodiscard]]
	float get_max_lag_time(Context hContext);
	
	// Manager
	void set_next_scene(Scene *pNextSceneParam);
	[[nodiscard]]
	bool is_next_scene_set();
	[[nodiscard]]
	Scene* get_current_scene();
	[[nodiscard]]
	uint32_t get_current_scene_id();
	[[nodiscard]]
	bool is_scene_current(uint32_t u32SceneId);

	[[nodiscard]]
	Scene* get_next_scene();
	[[nodiscard]]
	uint32_t get_next_scene_id();
	[[nodiscard]]
	bool is_scene_next(uint32_t u32SceneId);

	// Render system
	void enable_vsync(bool bEnableVsync);
	[[nodiscard]]
	bool is_vsync_enabled();

	// Texture loading
	[[nodiscard]]
	Texture alloc_texture_from_binary_data(const uint8_t *pau8TextureBinaries, uint32_t u32Width, uint32_t u32Height, uint32_t u32Channels);
	[[nodiscard]]
	Texture alloc_texture_loading_from_file(const char *pacPathToTextureFile);
	void free_texture(Texture hTexture);
	[[nodiscard]]
	uint32_t get_width_of_texture(Texture hTexture);
	[[nodiscard]]
	uint32_t get_height_of_texture(Texture hTexture);
	void get_extent_of_texture(Texture hTexture, uint32_t *pa2u32Extent);

	// Sprite layout creation
	[[nodiscard]]
	SpriteLayout create_sprite_layout(const SpriteLayoutSettings &rSettings);
	bool change_sprite_layout_settings(SpriteLayout hSpriteLayout, const SpriteLayoutSettings &rNewSettings);
	void destroy_sprite_layout(SpriteLayout hSpriteLayout);
	[[nodiscard]]
	float get_maximum_allowed_anisotropy();

	// Sprite
	[[nodiscard]]
	Sprite create_sprite(Texture hTexture, SpriteLayout hSpriteLayout);
	void change_texture_in_sprite(Sprite hSprite, Texture hTexture);
	void change_layout_in_sprite(Sprite hSprite, SpriteLayout hSpriteLayout);
	void set_default_layout_in_sprite(Sprite hSprite);
	void destroy_sprite(Sprite hSprite);
	[[nodiscard]]
	Texture get_texture_from_sprite(Sprite hSprite);
	[[nodiscard]]
	SpriteLayout get_sprite_layout_from_sprite(Sprite hSprite);

	// Renderer
	void set_screen_percentage_settings(const ScreenPercentageSettings &rNewSettings);
	[[nodiscard]]
	ScreenPercentageSettings get_screen_percentage_settings();
	void set_msaa_mode(MsaaMode eNewMsaaMode);
	[[nodiscard]]
	MsaaMode get_msaa_mode();
	[[nodiscard]]
	bool is_msaa_mode_supported(MsaaMode eMsaaMode);
	void get_supported_msaa_modes(uint8_t u8ListLength, MsaaMode *paeSupportedMsaaModes, uint8_t *pu8SupportedMsaaModeCount);
	[[nodiscard]]
	MsaaMode get_highest_supported_msaa_mode();
	void set_background_color(const Color &rColor);
	void set_background_color(float fRed, float fGreen, float fBlue, float fAlpha);
	[[nodiscard]]
	Color get_background_color();
	[[nodiscard]]
	bool is_sample_shading_enabled();
	void set_sample_shading_rate(float fNewSampleShadingRate);
	[[nodiscard]]
	float get_sample_shading_rate();

#ifdef RE_OS_WINDOWS
	void win64_set_hinstance(HINSTANCE win_hNewInstance);
#endif

}

#endif /* __RE_H__ */
