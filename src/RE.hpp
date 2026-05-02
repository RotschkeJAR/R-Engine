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
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>
#include <concepts>
#include <numbers>
#include <bit>

/**
 *   Uncomment this, if you want to disable PRINT_DEBUG and PRINT_DEBUG_CLASS to
 * filter debug output and get better performance while the terminal is active.
 */
//#define RE_DISABLE_DEBUGGING

namespace RE {

#define STRIP_QUOTE_MACRO(...) __VA_ARGS__



//================ Concepts

	template <class... T>
	concept ArePointers = (std::is_pointer_v<T> && ...);

	template <class... T>
	concept AreArithmetics = (std::is_arithmetic_v<T> && ...);

	template <class... T>
	concept AreIntegrals = (std::is_integral_v<T> && ...);

	template <class... T>
	concept AreFloatingPointNumbers = (std::is_floating_point_v<T> && ...);

	template <class TargetedType, class... T>
	concept AreSame = (std::is_same_v<T, TargetedType> && ...);

	template <class... T>
	concept AreComparable = requires (T... values) {
		{ (values == ...) };
	};



//================ Enumerations

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

	enum DepthPrecission {
		RE_DEPTH_PRECISSION_LOW = 0,
		RE_DEPTH_PRECISSION_MODERATE = 1,
		RE_DEPTH_PRECISSION_HIGH = 2
	};



//================ Signals

	void set_signal_handlers();



//================ Console output

	template <class... T>
	void print(const T... content) {
		const auto previousSettings = std::cout.setf(std::ios_base::showbase | std::ios_base::boolalpha);
		([&]() {
			if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>)
				std::cout << static_cast<int16_t>(content);
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
		std::cout.flags(previousSettings);
	}
	template <class... T>
	void println(const T... content) {
		print(content..., '\n');
	}
	void print_colored(const std::string &rsContent, TerminalColor eColor, bool bBackgroundColored, bool bBold);
	void println_colored(const std::string &rsContent, TerminalColor eColor, bool bBackgroundColored, bool bBold);
#define PRINT(...) print(__FILE__, " (line ", __LINE__, "): ", STRIP_QUOTE_MACRO(__VA_ARGS__))
#define PRINT_LN(...) PRINT(STRIP_QUOTE_MACRO(__VA_ARGS__), '\n')

#ifndef RE_DISABLE_DEBUGING
# define PRINT_DEBUG(...) [&](const char *const pacFile, const char *const pacFunc, const uint32_t u32Line) { \
			time_t currentTime = std::time(0); \
			println("[", std::put_time(std::gmtime(&currentTime), "%d.%b %Y, %H:%M:%S"), "] (", pacFile, ", at line ", u32Line, ", in function \"", pacFunc, "\"): ", STRIP_QUOTE_MACRO(__VA_ARGS__)); \
		} (__FILE__, __func__, __LINE__)
#else
# define PRINT_DEBUG(...)
#endif
#define PRINT_DEBUG_CLASS(...) PRINT_DEBUG("{", this, "} ", __VA_ARGS__)
	
	[[noreturn]]
	void abort(const std::string &rsDetail);
	void fatal_error(const std::string &rsDetail);
	void error(const std::string &rsDetail);
	void warning(const std::string &rsDetail);
	void note(const std::string &rsDetail);
#define ABORT(...) RE::abort(append_to_string(STRIP_QUOTE_MACRO(__VA_ARGS__), "\nIn ", __FILE__, ", function \"", __func__, "\", at line ", __LINE__))
#define FATAL_ERROR(...) fatal_error(append_to_string(STRIP_QUOTE_MACRO(__VA_ARGS__), "\nIn ", __FILE__, ", function \"", __func__, "\", at line ", __LINE__))
#define ERROR(...) error(append_to_string(STRIP_QUOTE_MACRO(__VA_ARGS__), "\nIn ", __FILE__, ", function \"", __func__, "\", at line ", __LINE__))
#define WARNING(...) warning(append_to_string(STRIP_QUOTE_MACRO(__VA_ARGS__), "\nIn ", __FILE__, ", function \"", __func__, "\", at line ", __LINE__))
#define NOTE(...) note(append_to_string(STRIP_QUOTE_MACRO(__VA_ARGS__), "\nIn ", __FILE__, ", function \"", __func__, "\", at line ", __LINE__))



//================ Memory

#define DELETE_SAFELY(PTR_REF) do { \
			if (!PTR_REF) \
				break; \
			PRINT_DEBUG("Safely deleting ", PTR_REF); \
			delete (PTR_REF); \
			(PTR_REF) = nullptr; \
		} while (false)
#define DELETE_ARRAY_SAFELY(PTR_REF) do { \
			if (!PTR_REF) \
				break; \
			PRINT_DEBUG("Safely deleting array ", PTR_REF); \
			delete[] (PTR_REF); \
			(PTR_REF) = nullptr; \
		} while (false)

	template <class T = void*> requires (std::is_pointer_v<T>)
	T not_null(const T pointer) {
		if (!pointer)
			RE::abort("Didn't pass the 'not null'-check");
		return pointer;
	}

	// Safe alternative to 'std::malloc'
	void* safe_malloc(size_t size);
	// Safe alternative to 'std::align'
	void* safe_align(size_t alignment, size_t size, void *&rpPtr, size_t &rSpace);
	// Improved version of 'std::align'
	void* align_2(size_t alignment, size_t size, void *&rpPtr, size_t &rSpace);
	// Improved version of 'safe_align'
	void* safe_align_2(size_t alignment, size_t size, void *&rpPointer, size_t &rSpace);



//================ Strings

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



//================ Arithmetic/Logic Utilities

	template <class T> requires std::is_arithmetic_v<T>
	[[nodiscard]]
	constexpr T nth_root(const T n, const T value) {
		if (n <= 0) {
			FATAL_ERROR("The value of 'n' shouldn't be zero or negative in an nth root");
			return static_cast<T>(0);
		}
		return std::pow(value, static_cast<T>(1.0) / n);
	}

	template <class... T> requires AreIntegrals<T...>
	[[nodiscard]]
	constexpr uint64_t gen_bitmask(const T... bits) {
		return (... | (1UL << static_cast<uint64_t>(bits)));
	}

	[[nodiscard]]
	constexpr uint64_t gen_bitmask_in_range(const uint64_t u64Begin, const uint64_t u64End) {
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

	template <class T, class... U> requires AreIntegrals<T, U...>
	[[nodiscard]]
	constexpr bool are_bits_true(const T value, const U... bits) {
		const T bitmask = static_cast<T>(gen_bitmask<U...>(bits...));
		return (value & bitmask) == bitmask;
	}

	template <class T> requires std::integral<T>
	[[nodiscard]]
	constexpr bool are_bits_true_in_range(const T value, const T begin, const T end) {
		if (begin > end) {
			FATAL_ERROR("Start (", begin, ") of the range is larger than end (", end, ")");
			return false;
		}
		for (T i = begin; i < end; i++)
			if (!are_bits_true<T, T>(value, i))
				return false;
		return true;
	}

	template <class T, class... U> requires AreIntegrals<T, U...>
	[[nodiscard]]
	constexpr bool are_bitmasks_true(const T mValue, const U... mBitmasks) {
		return (((mValue & mBitmasks) == mBitmasks) && ...);
	}

	template <class T> requires std::integral<T>
	void get_indices_for_true_bits(const T mValue, uint8_t &ru8IndexCount, std::unique_ptr<uint8_t[]> &rIndices) {
		ru8IndexCount = static_cast<uint8_t>(std::popcount<T>(mValue));
		rIndices = std::make_unique<uint8_t[]>(ru8IndexCount);
		uint8_t u8NextIndexEntry = 0;
		for (T bitIndex = 0; bitIndex < sizeof(T) * 8; bitIndex++)
			if ((mValue & (1 << bitIndex)) != 0) {
				rIndices[u8NextIndexEntry] = static_cast<uint8_t>(bitIndex);
				u8NextIndexEntry++;
				if (u8NextIndexEntry >= ru8IndexCount)
					break;
			}
	}

	template <class T, class... U> requires AreIntegrals<T, U...>
	constexpr T set_bits(T& rValue, const bool bNewState, const U... bits) {
		const T bitmask = gen_bitmask<U...>(bits...);
		if (bNewState)
			rValue |= bitmask;
		else
			rValue &= ~bitmask;
		return rValue;
	}

	template <class T> requires std::integral<T>
	constexpr T set_bits_in_range(T &rValue, const bool bNewState, const T begin, const T end) {
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
	
	template <class T> requires std::is_arithmetic_v<T>
	[[nodiscard]]
	constexpr T sign(const T value) {
		if constexpr (std::is_unsigned_v<T>)
			return value ? 1 : 0;
		else
			return (value > 0 ? 1 : 0) - (value < 0 ? 1 : 0);
	}

	template <class T> requires std::is_arithmetic_v<T>
	[[nodiscard]]
	constexpr bool is_multiple_of(const T value, const T multiple) {
		if (!multiple)
			return !value;
		if constexpr (std::is_same_v<float, T>)
			return std::truncf(value / multiple) * multiple == value;
		else if constexpr (std::is_same_v<float, T>)
			return std::trunc(value / multiple) * multiple == value;
		else if constexpr (std::is_same_v<float, T>)
			return std::truncl(value / multiple) * multiple == value;
		else
			return (value / multiple) * multiple == value;
	}

	// If value is a multiple, the returned number is the next after the value
	template <class T> requires std::is_arithmetic_v<T>
	[[nodiscard]]
	constexpr T next_multiple(const T value, const T multiple) {
		if (!multiple)
			return static_cast<T>(0);
		T currentFactor = value / multiple;
		if constexpr (std::is_same_v<float, T>)
			currentFactor = std::truncf(currentFactor);
		else if constexpr (std::is_same_v<double, T>)
			currentFactor = std::trunc(currentFactor);
		else if constexpr (std::is_same_v<long double, T>)
			currentFactor = std::truncl(currentFactor);
		if (sign<T>(value) >= 0 && sign<T>(multiple))
			return (currentFactor + 1) * multiple;
		else
			return currentFactor * multiple;
	}

	// If value is a multiple, the returned number is the previous before the value
	template <class T> requires std::is_arithmetic_v<T>
	[[nodiscard]]
	constexpr T previous_multiple(const T value, const T multiple) {
		if (!multiple)
			return static_cast<T>(0);
		if (!value)
			return -multiple;
		T currentFactor = value / multiple;
		if constexpr (std::is_same_v<float, T>)
			currentFactor = std::truncf(currentFactor);
		else if constexpr (std::is_same_v<double, T>)
			currentFactor = std::trunc(currentFactor);
		else if constexpr (std::is_same_v<long double, T>)
			currentFactor = std::truncl(currentFactor);
		if (sign<T>(value) == sign<T>(multiple))
			return (currentFactor - 1) * multiple;
		else
			return currentFactor * multiple;
	}

	template <class T> requires std::is_arithmetic_v<T>
	[[nodiscard]]
	constexpr T next_multiple_inclusive(const T value, const T multiple) {
		return is_multiple_of<T>(value, multiple) ? value : next_multiple<T>(value, multiple);
	}

	template <class T> requires std::is_arithmetic_v<T>
	[[nodiscard]]
	constexpr T previous_multiple_inclusive(const T value, const T multiple) {
		return is_multiple_of<T>(value, multiple) ? value : previous_multiple<T>(value, multiple);
	}

	template <class T> requires std::floating_point<T>
	[[nodiscard]]
	constexpr T degrees_to_radians(const T degrees) {
		return degrees * (std::numbers::pi / 180.0f);
	}

	template <class T> requires std::floating_point<T>
	[[nodiscard]]
	constexpr T radians_to_degrees(const T radians) {
		return radians * (180.0f / std::numbers::pi);
	}

	template <class T> requires std::floating_point<T>
	[[nodiscard]]
	constexpr T sin_deg(const T degrees) {
		return std::sin(degrees_to_radians<T>(degrees));
	}

	template <class T> requires std::floating_point<T>
	[[nodiscard]]
	constexpr T cos_deg(const T degrees) {
		return std::cos(degrees_to_radians<T>(degrees));
	}

	template <class T> requires std::floating_point<T>
	[[nodiscard]]
	constexpr T tan_deg(const T degrees) {
		return std::tan(degrees_to_radians<T>(degrees));
	}



//================ Containers

	class Bitset final {
		private:
			std::unique_ptr<uint8_t[]> bitArray;
			size_t bitSize;

		public:
			class BitReference final {
				private:
					uint8_t *pm8Bitmask,
						u8BitIndex;

				public:
					BitReference() = delete;
					BitReference(uint8_t *pm8Bitmask, uint8_t u8BitIndex);
					BitReference(const BitReference &rCopy);
					BitReference(const BitReference &&rrCopy) = delete;
					~BitReference();

					void flip();

					operator bool() const;
					void operator =(bool bNewValue);
					bool operator ==(const BitReference &rOther) const;
					bool operator !=(const BitReference &rOther) const;
					friend std::ostream& operator <<(std::ostream &rStream, const BitReference &rBitReference);
			};

			Bitset();
			Bitset(size_t bitSize, bool bInitialState = false);
			Bitset(const Bitset &rCopy) = delete;
			Bitset(Bitset &rrCopy);
			~Bitset();

			void fill(bool bNewState);
			void swap(Bitset &rOther);
			void resize(size_t newBitSize, bool bInitialState = false);
			void clear();
			size_t size() const;
			bool empty() const;
			BitReference at(size_t index);

			BitReference operator [](size_t index);

			friend std::ostream& operator <<(std::ostream &rStream, const Bitset &Bitset);
	};

	template <class T, size_t dimensionCount> requires std::is_arithmetic_v<T> && (dimensionCount > 0)
	class Vector final {
		private:
			T aCoords[dimensionCount];

		public:
			using type = T;

			explicit Vector(const T initialValue = 0) {
				fill(initialValue);
			}
			
			Vector(const Vector<T, dimensionCount> &rCopy) {
				copy_from(rCopy);
			}
			
			Vector(const Vector<T, dimensionCount> &&rrCopy) {
				copy_from(rrCopy);
			}

			template <class... U>
			explicit Vector(const U... values) requires AreArithmetics<U...> && (sizeof...(U) <= dimensionCount) {
				PRINT_DEBUG_CLASS("Filling vector with values");
				size_t dimensionIndex = 0;
				([&]() {
					aCoords[dimensionIndex] = static_cast<T>(values);
					dimensionIndex++;
				} (), ...);
				std::fill(std::begin(aCoords) + dimensionIndex, std::end(aCoords), static_cast<T>(0));
			}

			~Vector() {}

			[[nodiscard]]
			T sum() const {
				T sum = 0;
				for (const T coord : aCoords)
					sum += coord;
				return sum;
			}

			[[nodiscard]]
			T area() const {
				T result = 1;
				for (const T coord : aCoords)
					result *= coord;
				return result;
			}

			[[nodiscard]]
			T length() const {
				return nth_root<T>(static_cast<T>(dimensionCount), sum());
			}

			[[nodiscard]]
			Vector<T, dimensionCount> add(const Vector<T, dimensionCount> &rOther) const {
				Vector<T, dimensionCount> result;
				for (size_t i = 0; i < dimensionCount; i++)
					result.aCoords[i] = aCoords[i] + rOther.aCoords[i];
				return result;
			}

			[[nodiscard]]
			Vector<T, dimensionCount> subtract(const Vector<T, dimensionCount> &rOther) const {
				Vector<T, dimensionCount> result;
				for (size_t i = 0; i < dimensionCount; i++)
					result.aCoords[i] = aCoords[i] - rOther.aCoords[i];
				return result;
			}

			[[nodiscard]]
			T dot_product(const Vector<T, dimensionCount> &rOther) const {
				T result = 0;
				for (size_t i = 0; i < dimensionCount; i++)
					result += aCoords[i] * rOther.aCoords[i];
				return result;
			}

			[[nodiscard]]
			Vector<T, dimensionCount> cross_product(const Vector<T, dimensionCount> &rOther) const requires (dimensionCount == 3) {
				return Vector<T, dimensionCount>(aCoords[1] * rOther.aCoords[2] - aCoords[2] * rOther.aCoords[1],
												aCoords[2] * rOther.aCoords[0] - aCoords[0] * rOther.aCoords[2],
												aCoords[0] * rOther.aCoords[1] - aCoords[1] * rOther.aCoords[0]);
			}

			typedef bool (*compareFunc_t)(T valueToCompare);

			[[nodiscard]]
			bool all_of(const compareFunc_t compareFunction) const {
				return std::all_of(std::begin(aCoords), std::end(aCoords), compareFunction);
			}

			[[nodiscard]]
			bool any_of(const compareFunc_t compareFunction) const {
				return std::any_of(std::begin(aCoords), std::end(aCoords), compareFunction);
			}

			[[nodiscard]]
			bool none_of(const compareFunc_t compareFunction) const {
				return std::none_of(std::begin(aCoords), std::end(aCoords), compareFunction);
			}

			[[nodiscard]]
			T max() const {
				return *std::max_element(std::begin(aCoords), std::end(aCoords));
			}

			[[nodiscard]]
			T min() const {
				return *std::min_element(std::begin(aCoords), std::end(aCoords));
			}

			[[nodiscard]]
			T average() const {
				return sum() / static_cast<T>(dimensionCount);
			}

			void fill(const T value) {
				PRINT_DEBUG_CLASS("Filling vector with value ", value);
				std::fill(std::begin(aCoords), std::end(aCoords), value);
			}

			void copy_from(const Vector<T, dimensionCount> &rCopy) {
				PRINT_DEBUG_CLASS("Copying coordinates from vector ", &rCopy);
				std::copy(std::begin(rCopy.aCoords), std::end(rCopy.aCoords), std::begin(aCoords));
			}

			void copy_from_array(const T *const paArray, const size_t arrayLength) {
				PRINT_DEBUG_CLASS("Copying ", arrayLength, " elements from array ", paArray, " to this vector");
				for (size_t i = 0; i < arrayLength && i < dimensionCount; i++)
					aCoords[i] = paArray[i];
			}

			[[nodiscard]]
			bool equals(const Vector<T, dimensionCount> &rOther) const {
				for (size_t dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
					if (aCoords[dimensionIndex] != rOther[dimensionIndex])
						return false;
				return true;
			}

			[[nodiscard]]
			T& at(const size_t dimensionIndex) {
				if (dimensionIndex >= dimensionCount)
					FATAL_ERROR("Index ", dimensionIndex, " is out of bounds: [0, ", dimensionCount, ")");
				return aCoords[dimensionIndex];
			}

			[[nodiscard]]
			T at(const size_t dimensionIndex) const {
				if (dimensionIndex >= dimensionCount)
					FATAL_ERROR("Index ", dimensionIndex, " is out of bounds: [0, ", dimensionCount, ")");
				return aCoords[dimensionIndex];
			}

			[[nodiscard]]
			consteval size_t dimensions() const {
				return dimensionCount;
			}

			[[nodiscard]]
			Vector<T, dimensionCount> operator +(const Vector<T, dimensionCount> &rOther) const {
				return add(rOther);
			}

			[[nodiscard]]
			Vector<T, dimensionCount> operator -(const Vector<T, dimensionCount> &rOther) const {
				return subtract(rOther);
			}

			[[nodiscard]]
			T& operator [](const size_t dimensionIndex) {
				return aCoords[dimensionIndex];
			}

			[[nodiscard]]
			T operator [](const size_t dimensionIndex) const {
				return aCoords[dimensionIndex];
			}

			void operator =(const Vector<T, dimensionCount> &rCopy) {
				copy_from(rCopy);
			}

			[[nodiscard]]
			bool operator ==(const Vector<T, dimensionCount> &rOther) const {
				return equals(rOther);
			}

			[[nodiscard]]
			bool operator !=(const Vector<T, dimensionCount> &rOther) const {
				return !equals(rOther);
			}

			friend std::ostream& operator <<(std::ostream &rStream, const Vector<T, dimensionCount> &rVector) {
				rStream << '(';
				for (size_t i = 0; i < dimensionCount; i++) {
					if (i)
						rStream << ", ";
					if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>)
						rStream << static_cast<int16_t>(rVector.aCoords[i]);
					else
						rStream << rVector.aCoords[i];
				}
				rStream << ')';
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

	template <size_t numOfThreads = 10> requires (numOfThreads > 0)
	class Threadpool final {
		private:
			std::mutex hMutex;
			std::array<std::jthread, numOfThreads> threads;
			std::array<uint64_t, numOfThreads> agePerThread;

			size_t join_oldest_thread_and_occupy() {
				std::lock_guard<std::mutex> lockGuardMute(hMutex);
				const size_t oldThreadIndex = *std::min_element(agePerThread.begin(), agePerThread.end());
				if (threads[oldThreadIndex].joinable())
					threads[oldThreadIndex].join();
				agePerThread[oldThreadIndex] = (*std::max_element(agePerThread.begin(), agePerThread.end())) + 1;
				return oldThreadIndex;
			}

		public:
			Threadpool() {
				agePerThread.fill(0);
			}
			Threadpool(Threadpool &rCopy) = delete;
			Threadpool(Threadpool &&rrCopy) : threads(std::move(rrCopy.threads)), agePerThread(std::move(rrCopy.agePerThread)) {
				rrCopy.agePerThread.fill(0);
			}
			~Threadpool() {}

			template <class F, class... paramTypes> requires std::invocable<F, paramTypes...>
			void execute(F &&rrFunction, paramTypes... params) {
				const size_t oldThreadIndex = join_oldest_thread_and_occupy();
				threads[oldThreadIndex] = std::jthread(rrFunction, params...);
			}

			void move_thread(std::jthread &&rrThread) {
				const size_t oldThreadIndex = join_oldest_thread_and_occupy();
				threads[oldThreadIndex] = rrThread;
			}

			bool joinable() {
				for (std::jthread &rThread : threads)
					if (rThread.joinable())
						return true;
				return false;
			}

			void join() {
				for (std::jthread &rThread : threads)
					if (rThread.joinable())
						rThread.join();
				agePerThread.fill(0);
			}

			size_t free_slots() {
				size_t numOfFreeSlots = 0;
				for (std::jthread &rThread : threads)
					if (!rThread.joinable())
						numOfFreeSlots++;
				return numOfFreeSlots;
			}

			size_t occupied_slots() {
				return numOfThreads - free_slots();
			}

			consteval size_t amount_of_threads() {
				return numOfThreads;
			}
	};

	class RandomNumberGenerator final {
		private:
			std::mt19937 rng;

		public:
			RandomNumberGenerator();
			explicit RandomNumberGenerator(size_t seed);
			RandomNumberGenerator(const RandomNumberGenerator &rCopy);
			RandomNumberGenerator(RandomNumberGenerator &&rrCopy) = delete;
			~RandomNumberGenerator();

			void seed(size_t newSeed);
			size_t seed_randomly();

			template <class T>
			[[nodiscard]]
			T random(const T min, const T max) requires std::is_arithmetic_v<T> {
				if constexpr (std::is_integral_v<T>) {
					std::uniform_int_distribution<T> range(min, max - 1);
					return range(rng);
				} else if constexpr (std::is_floating_point_v<T>) {
					std::uniform_real_distribution<T> range(min, max);
					return range(rng);
				} else {
					T uninitialized;
					return uninitialized;
				}
			}
			template <class T>
			[[nodiscard]]
			T random(const T max) requires std::is_arithmetic_v<T> {
				return random<T>(static_cast<T>(0.0), max);
			}
			template <class T>
			[[nodiscard]]
			T random() requires std::is_arithmetic_v<T> {
				return random<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
			}
			[[nodiscard]]
			double random_normal();
			[[nodiscard]]
			bool random_bool(double f64Chance = 0.5);

			template <class T>
			[[nodiscard]]
			T operator ()(const T min, const T max) requires std::is_arithmetic_v<T> {
				return random<T>(min, max);
			}
			template <class T>
			[[nodiscard]]
			T operator ()(const T max) requires std::is_arithmetic_v<T> {
				return random<T>(max);
			}
			template <class T>
			[[nodiscard]]
			T operator ()() requires std::is_arithmetic_v<T> || std::same_as<T, bool> {
				if constexpr (std::is_same_v<T, bool>)
					return random_bool();
				else
					return random<T>();
			}
	};



//================ Engine

	class Color final {
		public:
			static constexpr uint8_t u8ColorChannelCount = 4;
			static constexpr float f32MinColor = 0.0f,
				f32MaxColor = 1.0f;

			[[nodiscard]]
			static constexpr float clamp(const float f32ChannelValue) {
				return std::clamp(f32ChannelValue, f32MinColor, f32MaxColor);
			}

		private:
			float afChannels[u8ColorChannelCount];

		public:
			Color();
			Color(float f32Red, float f32Green, float f32Blue, float f32Alpha);
			Color(const Color &rCopy);
			Color(const Color &&rrCopy) = delete;
			~Color();
			
			[[nodiscard]]
			float get_channel(uint8_t u8ChannelIndex) const;
			template <uint8_t u8ChannelIndex> requires (u8ChannelIndex < u8ColorChannelCount)
			[[nodiscard]]
			float get_channel() const {
				return afChannels[u8ChannelIndex];
			}
			void set_channel(uint8_t u8ChannelIndex, float f32Normal);
			template <uint8_t u8ChannelIndex> requires (u8ChannelIndex < u8ColorChannelCount)
			void set_channel(const float f32Normal) {
				afChannels[u8ChannelIndex] = this->clamp(f32Normal);
			}

			void copy_from(const Color &rCopy);
			[[nodiscard]]
			bool equals(const Color &rOther) const;

			void set_red(float f32Red);
			[[nodiscard]]
			float get_red() const;
			void set_green(float f32Green);
			[[nodiscard]]
			float get_green() const;
			void set_blue(float f32Blue);
			[[nodiscard]]
			float get_blue() const;
			void set_alpha(float f32Alpha);
			[[nodiscard]]
			float get_alpha() const;

			[[nodiscard]]
			float operator [](uint32_t u32ChannelIndex) const;
			void operator =(const Color &rCopy);
			[[nodiscard]]
			bool operator ==(const Color &rOther) const;
			[[nodiscard]]
			bool operator !=(const Color &rOther) const;
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

	class Scene {
		public:
			const uint32_t u32Id;

			Scene() = delete;
			explicit Scene(uint32_t u32Id);
			Scene(const Scene &rCopy) = delete;
			Scene(const Scene &&rrCopy) = delete;
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
			explicit Transform(const Vector3f &rPosition);
			Transform(const Vector3f &rPosition, const Vector3f &rScale);
			Transform(const Transform &rCopy);
			Transform(const Transform &&rrCopy) = delete;
			~Transform();
			void reset_position();
			void copy_from(const Transform &rCopy);
			[[nodiscard]]
			bool equals(const Transform &rOther) const;

			void operator =(const Transform &rCopy);
			[[nodiscard]]
			bool operator ==(const Transform &rOther) const;
			[[nodiscard]]
			bool operator !=(const Transform &rOther) const;
	};

	class Camera {
		public:
			Transform transform;

			Vector2f view;
			float f32ViewDistance;
			bool bIgnoreAspectRatio;

			Camera();
			explicit Camera(const Transform &rTransform);
			explicit Camera(const Vector2f &rView);
			Camera(const Vector2f &rView, float f32ViewDistance);
			Camera(const Vector2f &rView, float f32ViewDistance, bool bIgnoreAspectRatio);
			Camera(const Camera &rCopy);
			Camera(const Camera &&rrCopy) = delete;
			virtual ~Camera();
			virtual void update_before_render();
			void activate();
			void deactivate() const;
			void mark_deletable();
			void copy_from(const Camera &rOther);
			[[nodiscard]]
			bool equals(const Camera &rOther) const;

			void operator =(const Camera &rCopy);
			[[nodiscard]]
			bool operator ==(const Camera &rOther) const;
			[[nodiscard]]
			bool operator !=(const Camera &rOther) const;
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
			explicit InputAction(Input eInput);
			explicit InputAction(uint32_t u32KeyScancode);
			InputAction(const InputAction &rCopy);
			InputAction(const InputAction &&rrCopy) = delete;
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
				float f32Scale;
				Vector2u constSize;
			};
			TextureFilter eScalingFilter;

			ScreenPercentageSettings();
			explicit ScreenPercentageSettings(float f32Scale);
			ScreenPercentageSettings(float f32Scale, TextureFilter eScalingFilter);
			explicit ScreenPercentageSettings(const Vector2u &rConstSize);
			ScreenPercentageSettings(const Vector2u &rConstSize, TextureFilter eScalingFilter);
			ScreenPercentageSettings(ScreenPercentageMode eMode, const std::variant<float, Vector2u> &rSettings);
			ScreenPercentageSettings(ScreenPercentageMode eMode, const std::variant<float, Vector2u> &rSettings, TextureFilter eScalingFilter);
			ScreenPercentageSettings(const ScreenPercentageSettings &rCopy);
			ScreenPercentageSettings(const ScreenPercentageSettings &&rrCopy) = delete;
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
			float f32MaxAnisotropy; // must be equal or greater than 1, otherwise anisotropic filtering is disabled
			BorderColor eBorderColor;

			SpriteLayoutSettings();
			explicit SpriteLayoutSettings(TextureFilter eMagFilter);
			SpriteLayoutSettings(TextureFilter eMagFilter, TextureFilter eMinFilter);
			SpriteLayoutSettings(TextureFilter eMagFilter, TextureFilter eMinFilter, TextureFilter eMipmapFilter);
			SpriteLayoutSettings(TextureRepetition eTextureRepetitionU);
			SpriteLayoutSettings(TextureRepetition eTextureRepetitionU, TextureRepetition eTextureRepetitionV);
			explicit SpriteLayoutSettings(float f32MaxAnisotropy);
			explicit SpriteLayoutSettings(BorderColor eBorderColor);
			SpriteLayoutSettings(TextureFilter eMagFilter, TextureFilter eMinFilter, TextureFilter eMipmapFilter, TextureRepetition eTextureRepetitionU, TextureRepetition eTextureRepetitionV, float f32MaxAnisotropy, BorderColor eBorderColor);
			SpriteLayoutSettings(const SpriteLayoutSettings &rCopy);
			SpriteLayoutSettings(const SpriteLayoutSettings &&rrCopy) = delete;
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
	void set_fullscreen(bool bNewFullscreen);
	bool is_fullscreen();
	void set_window_title(const char *pacNewTitle);
	
	// Console
	void enable_colorful_printing(bool bEnable);
	[[nodiscard]]
	bool is_colorful_printing_enabled();
	void treat_warnings_as_errors(bool bEnable);
	[[nodiscard]]
	bool are_warnings_always_treated_as_errors();
	void make_errors_always_fatal(bool bEnable);
	[[nodiscard]]
	bool are_errors_always_fatal();

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
	bool is_down(Input eInput, uint32_t u32Scancode = 0);
	[[nodiscard]]
	bool was_down(Input eInput, uint32_t u32Scancode = 0);
	[[nodiscard]]
	bool is_pressed(Input eInput, uint32_t u32Scancode = 0);
	[[nodiscard]]
	bool is_released(Input eInput, uint32_t u32Scancode = 0);
	[[nodiscard]]
	bool is_held_down(Input eInput, uint32_t u32Scancode = 0);
	void reset_input_at(Input eInput, uint32_t u32Scancode = 0);
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
	bool execute();
	[[nodiscard]]
	float get_deltaseconds();
	[[nodiscard]]
	float get_fps_rate();
	void set_fps_limit(uint32_t u32MaxFramesPerSecond);
	[[nodiscard]]
	uint32_t get_fps_limit();
	void set_max_lag_time(float f32MaxSecondsOfLag);
	[[nodiscard]]
	float get_max_lag_time();
	void set_max_exhaustion_time(float f32MaxSecondsOfExhaustion);
	[[nodiscard]]
	float get_max_exhaustion_time();
	
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

	// Game Objects
	[[nodiscard]]
	size_t amount_of_game_objects();

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
	void get_extent_of_texture(Texture hTexture, uint32_t (&ra2u32Extent)[2]);

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
	void set_screen_percentage_settings(ScreenPercentageSettings newSettings);
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
	void set_background_color(float f32Red, float f32Green, float f32Blue, float f32Alpha);
	[[nodiscard]]
	Color get_background_color();
	[[nodiscard]]
	bool is_sample_shading_enabled();
	void set_sample_shading_rate(float f32NewSampleShadingRate);
	[[nodiscard]]
	float get_sample_shading_rate();

#ifdef RE_OS_WINDOWS
	void win64_set_hinstance(HINSTANCE win_hNewInstance);
#endif

}

#endif /* __RE_H__ */
