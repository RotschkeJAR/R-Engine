#ifndef __RE_H__
#define __RE_H__

#if (defined _WIN32 || defined _MSC_VER)
# define RE_OS_WINDOWS
#elif defined __linux__
# define RE_OS_LINUX
#else
# error The targeted OS is unknown to R-Engine
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdint>

typedef int8_t REbyte;
typedef uint8_t REubyte;
typedef int16_t REshort;
typedef uint16_t REushort;
typedef int32_t REint;
typedef uint32_t REuint;
typedef int64_t RElong;
typedef uint64_t REulong;

namespace RE {

#define STRIP_QUOTE(T) T

	inline std::ostream& operator <<(std::ostream& stream, REubyte& var) {
		stream << static_cast<unsigned>(var);
		return stream;
	}
	inline std::ostream& operator <<(std::ostream& stream, REushort& var) {
		stream << static_cast<unsigned>(var);
		return stream;
	}

	enum TerminalColor {
		Black,
		Red,
		Green,
		Yellow,
		Blue,
		Magenta,
		Cyan,
		White,
		Bright_Black, /* gray */
		Bright_Red,
		Bright_Green,
		Bright_Yellow,
		Bright_Blue,
		Bright_Magenta,
		Bright_Cyan,
		Bright_White
	};

	enum Key {
		Space,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		Top_0,
		Top_1,
		Top_2,
		Top_3,
		Top_4,
		Top_5,
		Top_6,
		Top_7,
		Top_8,
		Top_9,
		Slash, /* / */
		Backslash, /* \ */
		Comma, /* , */
		Period, /* . */
		Semicolon, /* ; */
		Apostrophe, /* ' */
		Accent, /* ` */
		Hashtag, /* # */
		Left_Bracket, /* [ */
		Right_Bracket, /* ] */
		Equals, /* = */
		Minus, /* - */
		Right_Ctrl,
		Left_Ctrl,
		Right_Alt, /* ALT-Gr */
		Left_Alt,
		Right_Shift,
		Left_Shift,
		Right_Menu,
		Left_Menu,
		Tab,
		Enter,
		Escape,
		Backspace,
		Arrow_Left,
		Arrow_Right,
		Arrow_Up,
		Arrow_Down,
		Delete,
		Insert,
		Home,
		End,
		Print_Screen,
		Scroll_Lock,
		Pause,
		Page_Up,
		Page_Down,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		F25,
		Caps_Lock,
		Numpad_Lock,
		Numpad_0,
		Numpad_1,
		Numpad_2,
		Numpad_3,
		Numpad_4,
		Numpad_5,
		Numpad_6,
		Numpad_7,
		Numpad_8,
		Numpad_9,
		Numpad_Add,
		Numpad_Subtract,
		Numpad_Multiply,
		Numpad_Divide,
		Numpad_Enter,
		Numpad_Period
	};

	template <typename... T>
	void print(T... content) {
		(std::cout << ... << content);
	}
	template <typename... T>
	void println(T... content) {
		(std::cout << ... << content);
		std::cout << '\n';
	}
	void printColored(const char* content, TerminalColor color, bool backgroundColored, bool bold);
	void printlnColored(const char* content, TerminalColor color, bool backgroundColored, bool bold);

	template <typename T>
	T nth_root(T n, T value) {
		return std::pow(value, static_cast<T>(1.0) / n);
	}

	template <typename... T>
	REulong genBitmask(T... bits) {
		return (... | (1L << static_cast<REulong>(bits)));
	}

	constexpr REulong genBitmaskRange(REubyte begin, REubyte end) {
		if (begin > end)
			std::swap(begin, end);
		else if (begin == end)
			return (1L << static_cast<REulong>(begin));
		REulong result = 0L;
		for (REubyte i = begin; i < end; i++)
			result |= 1L << static_cast<REulong>(i);
		return result;
	}

	template <typename T>
	std::string bitmaskToString(T bitmask) {
		std::string result("");
		REulong bits = sizeof(T) * 8L;
		for (REulong bit = 0L; bit < bits; bit++)
			result += (((static_cast<REulong>(bitmask) << bit) >> (bits - 1L)) == 1L) ? "1" : "0";
		return result;
	}

	template <typename... T>
	std::string appendStringsA(T... args) {
		std::stringstream ss("");
		(ss << ... << args);
		return ss.str();
	}

	template <typename... T>
	std::wstring appendStringsW(T... args) {
		std::wstringstream wss(L"");
		(wss << ... << args);
		return wss.str();
	}
	
	void error(const char* file, const char* func, REuint line, const char* detail, bool terminate);
	void warning(const char* file, const char* func, REuint line, const char* detail);
	void note(const char* file, const char* func, REuint line, const char* detail);
#define FATAL_ERROR(T) error(__FILE__, __func__, __LINE__, STRIP_QUOTE(T), true)
#define ERROR(T) error(__FILE__, __func__, __LINE__, STRIP_QUOTE(T), false)
#define WARNING(T) warning(__FILE__, __func__, __LINE__, STRIP_QUOTE(T))
#define NOTE(T) note(__FILE__, __func__, __LINE__, STRIP_QUOTE(T))

	template <typename T, REuint dimensions>
	class Vector {
		public:
			T coords[dimensions];

			Vector() {
				std::fill(std::begin(coords), std::begin(coords) + dimensions, 0);
			}
			~Vector() {}

			T sum() {
				T sum = static_cast<T>(0.0);
				for (T coord : coords)
					sum += coord;
				return sum;
			}

			T area() {
				T result = static_cast<T>(1.0);
				for (T coord : coords)
					result *= coord;
				return result;
			}

			T length() {
				return nth_root<T>(static_cast<T>(dimensions), sum());
			}

			constexpr REuint getDimension() {
				return dimensions;
			}

			T& operator[](REuint index) {
				if (index >= dimensions) {
					FATAL_ERROR(appendStringsA("Index ", index, " is out of bounds: [0, ", dimensions, "]").c_str());
					return coords[0];
				}
				return coords[index];
			}
	};

	void execute();

}

#endif /* __RE_H__ */
