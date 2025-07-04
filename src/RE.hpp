#ifndef __RE_H__
#define __RE_H__

#if defined(_WIN32) || defined(_MSC_VER)
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

namespace RE {

#define STRIP_QUOTE_MACRO(...) __VA_ARGS__

	enum TerminalColor {
		RE_TERMINAL_COLOR_BLACK = 0x00,
		RE_TERMINAL_COLOR_RED = 0x01,
		RE_TERMINAL_COLOR_GREEN = 0x02,
		RE_TERMINAL_COLOR_YELLOW = 0x03,
		RE_TERMINAL_COLOR_BLUE = 0x04,
		RE_TERMINAL_COLOR_MAGENTA = 0x05,
		RE_TERMINAL_COLOR_CYAN = 0x06,
		RE_TERMINAL_COLOR_WHITE = 0x07,
		RE_TERMINAL_COLOR_BRIGHT_BLACK = 0x08, /* gray */
		RE_TERMINAL_COLOR_BRIGHT_RED = 0x09,
		RE_TERMINAL_COLOR_BRIGHT_GREEN = 0x0A,
		RE_TERMINAL_COLOR_BRIGHT_YELLOW = 0x0B,
		RE_TERMINAL_COLOR_BRIGHT_BLUE = 0x0C,
		RE_TERMINAL_COLOR_BRIGHT_MAGENTA = 0x0D,
		RE_TERMINAL_COLOR_BRIGHT_CYAN = 0x0E,
		RE_TERMINAL_COLOR_BRIGHT_WHITE = 0x0F,
		RE_TERMINAL_COLOR_MAX_ENUM = 0x10
	};

	enum Input {
		RE_INPUT_UNKNOWN = -0x01,
		RE_INPUT_SCROLL_UP = 0x00,
		RE_INPUT_SCROLL_DOWN = 0x01,
		RE_INPUT_BUTTON_LEFT = 0x02,
		RE_INPUT_BUTTON_RIGHT = 0x03,
		RE_INPUT_BUTTON_MIDDLE = 0x04,
		RE_INPUT_KEY_SPACE = 0x05,
		RE_INPUT_KEY_A = 0x06,
		RE_INPUT_KEY_B = 0x07,
		RE_INPUT_KEY_C = 0x08,
		RE_INPUT_KEY_D = 0x09,
		RE_INPUT_KEY_E = 0x0A,
		RE_INPUT_KEY_F = 0x0B,
		RE_INPUT_KEY_G = 0x0C,
		RE_INPUT_KEY_H = 0x0D,
		RE_INPUT_KEY_I = 0x0E,
		RE_INPUT_KEY_J = 0x0F,
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

	template <class... T>
	void print(T... content) {
		if constexpr (sizeof...(content) == 0)
			return;
		([&]() {
			if constexpr (std::is_same_v<T, uint8_t>)
				std::cout << static_cast<uint16_t>(content);
			else if constexpr (std::is_same_v<T, int8_t>)
				std::cout << static_cast<int16_t>(content);
			else if constexpr (std::is_same_v<T, wchar_t> || std::is_same_v<T, const wchar_t> || std::is_same_v<T, wchar_t*> || std::is_same_v<T, const wchar_t*> || std::is_same_v<T, std::wstring>)
				std::wcout << content;
			else
				std::cout << content;
		} (), ...);
	}
	template <class... T>
	void println(T... content) {
		print(content..., "\n");
	}
	void print_colored(const char* pcContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold);
	void println_colored(const char* pcContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold);
#define PRINT(...) print(append_to_string(__FILE__, " (line ", __LINE__, "): ", STRIP_QUOTE_MACRO(__VA_ARGS__)))
#define PRINT_LN(...) print(append_to_string(__FILE__, " (line ", __LINE__, "): ", STRIP_QUOTE_MACRO(__VA_ARGS__), "\n"))
	
	void error(const char* pcFile, const char* pcFunc, const uint32_t u32Line, const char* pcDetail, const bool bTerminate);
	void warning(const char* pcFile, const char* pcFunc, const uint32_t u32Line, const char* pcDetail);
	void note(const char* pcFile, const char* pcFunc, const uint32_t u32Line, const char* pcDetail);
#define FATAL_ERROR(T) error(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T), true)
#define ERROR(T) error(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T), false)
#define WARNING(T) warning(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T))
#define NOTE(T) note(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T))
	void enable_colorful_printing(const bool bEnable);
	void treat_warnings_as_errors(const bool bEnable);
	void make_errors_always_fatal(const bool bEnable);
	void show_message_box_on_error(const bool bEnable);
	
	class SignalCatcher final {
		public:
			SignalCatcher();
			~SignalCatcher();
	};

	void add_to_stack_trace(const char* pcFile, const char* pcMethod, const uint32_t u32Line, const char* pcDetails);
	void remove_from_stack_trace();
	class SignalGuard final {
		public:
			SignalGuard(const char* pcFile, const char* pcFunc, const uint32_t u32Line, const char* pcDetails);
			~SignalGuard();
	};
#define DEFINE_SIGNAL_GUARD_DETAILED(NAME, DETAILS) SignalGuard NAME(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(DETAILS))
#define DEFINE_SIGNAL_GUARD(NAME) DEFINE_SIGNAL_GUARD_DETAILED(NAME, "\0")
#define CATCH_SIGNAL_DETAILED(CMD, DETAILS) ([&](const char *const pcFile, const char *const pcFunc, uint32_t u32Line) { \
			add_to_stack_trace(pcFile, pcFunc, u32Line, STRIP_QUOTE_MACRO(DETAILS)); \
			CMD; \
			remove_from_stack_trace(); \
		}) (__FILE__, __func__, __LINE__)
#define CATCH_SIGNAL(CMD) CATCH_SIGNAL_DETAILED(CMD, "\0")
#define CATCH_SIGNAL_AND_RETURN_DETAILED(CMD, RETURN_TYPE, DETAILS) ([&](const char *const pcFile, const char *const pcFunc, uint32_t u32Line) -> RETURN_TYPE { \
			add_to_stack_trace(pcFile, pcFunc, u32Line, STRIP_QUOTE_MACRO(DETAILS)); \
			RETURN_TYPE returnValue = CMD; \
			remove_from_stack_trace(); \
			return returnValue; \
		}) (__FILE__, __func__, __LINE__)
#define CATCH_SIGNAL_AND_RETURN(CMD, RETURN_TYPE) CATCH_SIGNAL_AND_RETURN_DETAILED(CMD, RETURN_TYPE, "\0")

#define DELETE_SAFELY(PTR_REF) CATCH_SIGNAL( do { \
			if (!PTR_REF) \
				break; \
			delete (PTR_REF); \
			(PTR_REF) = nullptr; \
		} while (false) )
#define DELETE_ARRAY_SAFELY(PTR_REF) CATCH_SIGNAL( do { \
			if (!PTR_REF) \
				break; \
			delete[] (PTR_REF); \
			(PTR_REF) = nullptr; \
		} while (false) )

	template <class... T>
	std::string append_to_string(T... strings) {
		std::stringstream ss("");
		(ss << ... << strings);
		return std::string(ss.str());
	}

	template <class... T>
	std::wstring append_to_wstring(T... strings) {
		std::wstringstream wss(L"");
		(wss << ... << strings);
		return std::wstring(wss.str());
	}

	bool is_string_empty(const char *pcString);
	uint32_t get_string_length_safely(const char *pcString);
	bool are_string_contents_equal(const char *pcString1, const char *pcString2);
	uint32_t get_line_count(const char *pcString);
	std::string get_line(const char *pcString, uint32_t u32Line);
	std::string convert_wide_chars_to_utf8(const wchar_t *pwcString);
	std::wstring convert_chars_to_wide(const char *pcString);
	std::string get_app_name();

	template <typename T>
	constexpr T nth_root(const T n, const T value) {
		static_assert(n > 0, "The 0th root is forbidden for causing undefined behaviour");
		if (!n) {
			FATAL_ERROR("The value of n shouldn't be zero in an nth root");
			return static_cast<T>(0.0);
		}
		T result = static_cast<T>(0.0);
		CATCH_SIGNAL(result = std::pow(value, static_cast<T>(1.0) / n));
		return result;
	}

	template <typename T>
	std::string array_to_string(const T arrayToPrint[], const uint32_t u32ArrayLength) {
		std::stringstream ss("");
		ss << '{';
		for (uint32_t u32Index = 0U; u32Index < u32ArrayLength; u32Index++) {
			if (u32Index)
				ss << ", ";
			ss << arrayToPrint[u32Index];
		}
		ss << '}';
		return std::string(ss.str());
	}

	template <typename T>
	std::string hexadecimal_to_string(T number, const bool bCutZeros) {
		DEFINE_SIGNAL_GUARD(sigGuardHexadecimalToString);
		std::stringstream ss("");
		if (number < static_cast<T>(0.0))
			ss << "-";
		ss << "0x";
		if constexpr (std::is_signed<T>::value)
			number = std::abs(number);
		constexpr int32_t i32HexadecimalDigits = sizeof(T) * 8 / 4;
		bool bNumbersPresent = !bCutZeros;
		for (int32_t i32Digit = i32HexadecimalDigits - 1U; i32Digit >= 0; i32Digit--) {
			T base = std::pow<T>(16, static_cast<T>(i32Digit));
			uint32_t u32HexadecimalCharacterIndex = static_cast<uint32_t>(number / base);
			number -= base * u32HexadecimalCharacterIndex;
			if (u32HexadecimalCharacterIndex || bNumbersPresent) {
				bNumbersPresent = true;
				if (u32HexadecimalCharacterIndex < 10U)
					ss << u32HexadecimalCharacterIndex;
				else
					ss << static_cast<char>(u32HexadecimalCharacterIndex - 10U + static_cast<uint32_t>('A'));
			}
		}
		if (!bNumbersPresent)
			ss << "0";
		return std::string(ss.str());
	}

	template <typename T>
	std::string hexadecimal_to_string(const T number) {
		return hexadecimal_to_string<T>(number, true);
	}

	template <typename... T>
	constexpr uint64_t gen_bitmask(T... bits) {
		return (... | (1UL << static_cast<uint64_t>(bits)));
	}

	constexpr uint64_t gen_bitmask_in_range(const uint64_t u64Begin, const uint64_t u64End) {
		if (u64Begin > u64End) {
			FATAL_ERROR(append_to_string("Start (", u64Begin, ") of the range is larger than end (", u64End, ")").c_str());
			return 0UL;
		} else if (u64Begin == u64End)
			return gen_bitmask(u64Begin);
		uint64_t u64Result = 0UL;
		for (uint64_t u64CurrentNumber = u64Begin; u64CurrentNumber < u64End; u64CurrentNumber++)
			u64Result |= gen_bitmask(u64CurrentNumber);
		return u64Result;
	}

	template <typename T>
	constexpr bool is_bit_true(const T value, const T bit) {
		return (value & gen_bitmask<T>(bit)) != static_cast<T>(0);
	}

	template <typename T>
	constexpr bool are_bits_true_in_range(const T value, const T begin, const T end) {
		if (begin > end) {
			FATAL_ERROR(append_to_string("Start (", begin, ") of the range is larger than end (", end, ")").c_str());
			return false;
		} else if (begin == end)
			return is_bit_true<T>(value, begin);
		for (T i = begin; i < end; i++)
			if (!is_bit_true<T>(value, i))
				return false;
		return true;
	}

	template <typename T>
	constexpr T set_bit(T& value, const T bit, const bool bNewState) {
		T targetBit = gen_bitmask<T>(bit);
		if (bNewState)
			value |= targetBit;
		else
			value &= ~targetBit;
		return value;
	}

	template <typename T>
	constexpr T set_bits(T& value, const T begin, const T end, const bool bNewState) {
		if (begin > end) {
			FATAL_ERROR(append_to_string("Start (", begin, ") of the range is larger than end (", end, ")").c_str());
			return value;
		}
		for (T i = begin; i < end; i++)
			set_bit<T>(value, i, bNewState);
		return value;
	}

	template <typename T>
	std::string bitmask_to_string(const T bitmask, const bool bWithSpace) {
		std::stringstream strResult("");
		constexpr T bits = sizeof(T) * 8;
		for (T bit = 0; bit < bits; bit++) {
			if (bWithSpace && bit && (bit % 8) == 0)
				strResult << " ";
			strResult << ((bitmask >> (bits - 1 - bit)) & 1);
		}
		return std::string(strResult.str());
	}

	template <typename T>
	std::string bitmask_to_string(const T bitmask) {
		return bitmask_to_string(bitmask, true);
	}
	
	template <typename T>
	T sign(const T value) {
		return (static_cast<T>(0.0) < value) - (value < static_cast<T>(0.0));
	}

	template <typename T, uint32_t u32Dimensions>
	class Vector final {
		static_assert(u32Dimensions != 0U, "A data-template has zero dimensions");

		public:
			T data[u32Dimensions];

			Vector() : data{} {}
			template <typename P, uint32_t u32CopyDimensions>
			Vector(Vector<P, u32CopyDimensions> &rCopyVector) {
				if (u32Dimensions != u32CopyDimensions)
					WARNING(append_to_string("The coordinates of this vector with ", u32Dimensions, " dimensions were copied from a vector with ", u32CopyDimensions, " dimensions").c_str());
				for (uint32_t u32CoordinateIndex = 0U; u32CoordinateIndex < u32CopyDimensions && u32CoordinateIndex < u32Dimensions; u32CoordinateIndex++)
					data[u32CoordinateIndex] = rCopyVector.data[u32CoordinateIndex];
			}
			template <typename... V>
			Vector(V... values) : data{} {
				uint32_t u32Index = 0U;
				([&]() {
					CATCH_SIGNAL_DETAILED(data[u32Index] = static_cast<T>(values), append_to_string("Index: ", u32Index).c_str());
					u32Index++;
				} (), ...);
			}
			~Vector() {}

			T sum() const {
				T sum = static_cast<T>(0.0);
				for (T coord : data)
					sum += coord;
				return sum;
			}

			T area() const {
				T result = static_cast<T>(1.0);
				for (T coord : data)
					result *= coord;
				return result;
			}

			T length() const {
				T result;
				CATCH_SIGNAL(result = nth_root<T>(static_cast<T>(u32Dimensions), sum()));
				return result;
			}

			void fill(const T value) {
				std::fill(std::begin(data), std::end(data), value);
			}

			void copy_from(const Vector &rCopyVector) {
				if (u32Dimensions != rCopyVector.get_dimensions()) {
					WARNING(append_to_string("Tried to copy values from one vector (", rCopyVector.get_dimensions(), ") to another (", u32Dimensions, "). This process has been terminated").c_str());
					return;
				}
				for (uint32_t u32Index = 0U; u32Index < u32Dimensions; u32Index++)
					data[u32Index] = rCopyVector[u32Index];
			}

			bool equals(const Vector &rCompareVector) const {
				if (u32Dimensions != rCompareVector.get_dimensions())
					return false;
				for (uint32_t u32Index = 0U; u32Index < u32Dimensions; u32Index++)
					if (data[u32Index] != rCompareVector[u32Index])
						return false;
				return true;
			}

			constexpr uint32_t get_dimensions() const {
				return u32Dimensions;
			}

			T& operator [](const uint32_t u32Index) {
				if (u32Index >= u32Dimensions)
					FATAL_ERROR(append_to_string("Index ", u32Index, " is out of bounds: [0, ", u32Dimensions, ")").c_str());
				return data[u32Index];
			}

			T operator [](const uint32_t u32Index) const {
				if (u32Index >= u32Dimensions)
					FATAL_ERROR(append_to_string("Index ", u32Index, " is out of bounds: [0, ", u32Dimensions, ")").c_str());
				return data[u32Index];
			}

			void operator =(const Vector &rCopyVector) {
				copy_from(rCopyVector);
			}

			bool operator ==(const Vector &rCompareVector) const {
				return equals(rCompareVector);
			}

			bool operator !=(const Vector &rCompareVector) const {
				return !equals(rCompareVector);
			}

			friend std::ostream& operator <<(std::ostream &rStream, const Vector &rVector) {
				rStream << "(";
				for (uint32_t i = 0U; i < rVector.get_dimensions(); i++) {
					if (i != 0U)
						rStream << ", ";
					rStream << rVector.data[i];
				}
				rStream << ")";
				return rStream;
			}
	};
	typedef Vector<float, 2U> Vector2f;
	typedef Vector<float, 3U> Vector3f;
	typedef Vector<float, 4U> Vector4f;
	typedef Vector<double, 2U> Vector2d;
	typedef Vector<double, 3U> Vector3d;
	typedef Vector<double, 4U> Vector4d;
	typedef Vector<int32_t, 2U> Vector2i;
	typedef Vector<int32_t, 3U> Vector3i;
	typedef Vector<int32_t, 4U> Vector4i;
	typedef Vector<uint32_t, 2U> Vector2u;
	typedef Vector<uint32_t, 3U> Vector3u;
	typedef Vector<uint32_t, 4U> Vector4u;

	class Color final {
		private:
			float a4fChannels[4];

		public:
			Color();
			Color(Color &rCopyColor);
			~Color();
			void set_channel(const uint8_t u8ChannelIndex, const float fNormal);
			void copy_from(const Color &rCopyColor);
			bool equals(const Color &rCompareColor) const;

			void set_red(const float fRed);
			float get_red() const;
			void set_green(const float fGreen);
			float get_green() const;
			void set_blue(const float fBlue);
			float get_blue() const;
			void set_alpha(const float fAlpha);
			float get_alpha() const;

			float operator [](const uint32_t u32ChannelIndex) const;
			void operator =(const Color &rCopyColor);
			bool operator ==(const Color &rCompareColor) const;
			bool operator !=(const Color &rCompareColor) const;
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
			bool equals(const Transform &rCompareTransform) const;

			void operator =(const Transform &rCopyTransform);
			bool operator ==(const Transform &rCompareTransform) const;
			bool operator !=(const Transform &rCompareTransform) const;
	};

	class SpriteRenderer final {
		public:
			Color color;

			SpriteRenderer();
			~SpriteRenderer();
	};

	class RandomNumberGenerator final {
		private:
			std::mt19937 rng;

		public:
			RandomNumberGenerator();
			RandomNumberGenerator(uint32_t u32Seed);
			~RandomNumberGenerator();
			void set_seed(uint32_t newSeed);

			template <typename T>
			T random(T min, T max) {
				std::uniform_int_distribution<T> range(min, max);
				return range(rng);
			}
			template <typename T>
			T random(T max) {
				return random<T>(static_cast<T>(0.0), max);
			}
			template <typename T>
			T random() {
				return random<T>(std::numeric_limits<T>::max());
			}
			bool random_bool();
			bool random_bool(double dChance);
			double random_percentage();
	};

	class Scene {
		public:
			const uint32_t u32Id;

			Scene() = delete;
			Scene(uint32_t u32Id);
			virtual ~Scene();

			virtual void start();
			virtual void update();
			virtual void end();
	};

	class Camera {
		public:
			Vector3f position;
			Vector2f scale;

			Vector2f view;

			Camera();
			Camera(Vector3f &rPosition);
			Camera(Vector3f &rPosition, Vector2f &rScale);
			Camera(Vector3f &rPosition, Vector2f &rScale, Vector2f &rView);
			~Camera();
			virtual void update();
			void activate();
			void deactivate() const;
			bool has_same_transform(const Camera &rCompareCamera) const;

			void operator =(const Camera &rCopyCamera);
			bool operator ==(const Camera &rCompareCamera) const;
			bool operator !=(const Camera &rCompareCamera) const;
	};

	class GameObject {
		public:
			const uint32_t u32OwnId;
			const uint32_t u32SceneParentId;
			Transform transform;
			SpriteRenderer spriteRenderer;

			GameObject() = delete;
			GameObject(uint32_t u32OwnId, uint32_t u32SceneParentId);
			virtual ~GameObject();

			virtual void start(Scene* pStartingScene);
			virtual void update(Scene* pCurrentScene);
			virtual void end(Scene* pEndingScene);
	};

	class InputAction {
		private:
			uint32_t u32KeyScancode;
			Input eInput;

		public:
			InputAction();
			InputAction(Input eInput);
			InputAction(uint32_t u32KeyScancode);
			~InputAction();
			bool is_scroll_wheel();
			bool is_button();
			bool is_key();

			// Queries engine to set its input to the next user input
			void update_input();
			bool is_updating();
			void cancel_update();
			static bool can_update();

			void change_input(Input eInput);
			void change_scancode(uint32_t u32NewScancode);

			bool is_pressed();
			bool is_down();
			bool is_released();

			bool has_valid_input_values();
	};

	int32_t get_cursor_position_x();
	int32_t get_cursor_position_y();
	float get_cursor_normal_position_x();
	float get_cursor_normal_position_y();

	Input map_scancode_to_input(const uint32_t u32Scancode);
	uint32_t map_input_to_scancode(const Input eInput);

	void execute();
	float get_deltaseconds();
	void set_fps_limit(const uint32_t u32MaxFramesPerSecond);
	float get_fps_rate();

	void mark_deletable(GameObject* pGameObject);
#define MARK_DELETABLE_SAFELY(GAME_OBJECT_POINTER) ([&](GameObject *&rpDeletableGameObject) { \
			CATCH_SIGNAL(mark_deletable(rpDeletableGameObject)); \
			rpDeletableGameObject = nullptr; \
		} (GAME_OBJECT_POINTER))
	
	void set_next_scene(Scene* pNextSceneParam);
	bool is_next_scene_set();
	Scene* get_current_scene();
	uint32_t get_current_scene_id();
	bool is_scene_current(const uint32_t u32SceneId);
	Scene* get_next_scene();
	uint32_t get_next_scene_id();
	bool is_scene_next(const uint32_t u32SceneId);

	void enable_vsync(const bool bEnableVsync);
	bool is_vsync_enabled();

	void set_const_screen_size(const uint32_t u32Width, const uint32_t u32Height);
	void set_screen_percentage(const float fPercentage);
	void reset_screen_size();

#ifdef RE_OS_WINDOWS
	void set_hinstance(HINSTANCE win_hInstance);
#endif

}

#endif /* __RE_H__ */
