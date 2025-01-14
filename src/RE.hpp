#ifndef __RE_H__
#define __RE_H__

#if (defined _WIN32 || defined _MSC_VER)
# define RE_OS_WINDOWS
#elif defined __linux__
# define RE_OS_LINUX
#else
# warning The targeted OS is unknown to R-Engine
#endif

#include <iostream>
#include <sstream>
#include <cstring>
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

	enum Keyboard {
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
		Right_Alt, /* AltGr */
		Left_Alt,
		Right_Shift,
		Left_Shift,
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

	enum MouseButton {
		Left,
		Right,
		Middle
	};

	template <typename... T>
	void print(T... content) {
		([&]() {
			if constexpr (std::is_same_v<T, REubyte>)
				std::cout << static_cast<unsigned>(content);
			else if constexpr (std::is_same_v<T, REbyte>)
				std::cout << static_cast<signed>(content);
			else
				std::cout << content;
		} (), ...);
	}
	template <typename... T>
	void println(T... content) {
		print(content..., "\n");
	}
	void printColored(const char* content, TerminalColor color, bool backgroundColored, bool bold);
	void printlnColored(const char* content, TerminalColor color, bool backgroundColored, bool bold);

	std::string convertToUTF8(const wchar_t* wstring);
	std::wstring convertToWide(const char* string);

	template <typename T>
	constexpr T nth_root(T n, T value) {
		return std::pow(value, static_cast<T>(1.0) / n);
	}

	template <typename... T>
	constexpr REulong genBitmask(T... bits) {
		return (... | (1L << static_cast<REulong>(bits)));
	}

	constexpr REulong genBitmaskRange(REulong begin, REulong end) {
		if (begin > end)
			std::swap(begin, end);
		else if (begin == end)
			return genBitmask(begin);
		REulong result = 0L;
		for (REulong i = begin; i < end; i++)
			result |= genBitmask(i);
		return result;
	}

	template <typename T>
	constexpr std::string bitmaskToString(T bitmask, bool withSpace) {
		std::string result("");
		T bits = sizeof(T) * static_cast<T>(8);
		T rightShift = sizeof(T) * static_cast<T>(8) - static_cast<T>(1);
		for (T bit = static_cast<T>(0); bit < bits; bit++) {
			if (withSpace && bit % static_cast<T>(8) == static_cast<T>(0) && bit)
				result += " ";
			T shiftedNumber = bitmask << bit;
			result += ((shiftedNumber >> rightShift) == static_cast<T>(1)) ? "1" : "0";
		}
		return result;
	}

	template <typename... T>
	std::string appendStrings(T... strings) {
		std::stringstream ss("");
		(ss << ... << strings);
		return std::string(ss.str());
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
				static_assert(dimensions != 0, "A vector has zero dimensions");
				fill(static_cast<T>(0.0));
			}
			template <typename... V>
			Vector(V... values) {
				static_assert(dimensions != 0, "A vector has zero dimensions");
				REuint index = 0;
				([&]() {
					coords[index] = static_cast<T>(values);
					index++;
				} (), ...);
			}
			~Vector() {}

			T sum() const {
				T sum = static_cast<T>(0.0);
				for (T coord : coords)
					sum += coord;
				return sum;
			}

			T area() const {
				T result = static_cast<T>(1.0);
				for (T coord : coords)
					result *= coord;
				return result;
			}

			T length() const {
				return nth_root<T>(static_cast<T>(dimensions), sum());
			}

			void fill(T value) {
				std::fill(std::begin(coords), std::end(coords), value);
			}

			constexpr REuint getDimensions() const {
				return dimensions;
			}

			T& operator[](REuint index) {
				if (index >= dimensions) {
					FATAL_ERROR(appendStrings("Index ", index, " is out of bounds: [0, ", dimensions, "]").c_str());
					return coords[0];
				}
				return coords[index];
			}

			T operator[](REuint index) const {
				if (index >= dimensions) {
					FATAL_ERROR(appendStrings("Index ", index, " is out of bounds: [0, ", dimensions, "]").c_str());
					return coords[0];
				}
				return coords[index];
			}

			friend std::ostream& operator<<(std::ostream& stream, const Vector& vector) {
				stream << "(";
				for (REuint i = 0; i < vector.getDimensions(); i++) {
					if (i != 0)
						stream << ", ";
					stream << vector.coords[i];
				}
				stream << ")";
				return stream;
			}
	};

	void execute();

	REint scancodeFromKey(Keyboard key);
	Keyboard keyFromScancode(REint scancode);
	bool isKeyDown(Keyboard key);
	bool isKeyPressed(Keyboard key);
	bool isKeyReleased(Keyboard key);
	bool isButtonDown(MouseButton button);
	bool isButtonPressed(MouseButton button);
	bool isButtonReleased(MouseButton button);
	bool isScrolling();
	bool isScrollingUpward();
	bool isScrollingDownward();
	REbyte scrollDirection();
	Vector<REint, 2> cursorPos();
	REint cursorPosX();
	REint cursorPosY();
	Vector<REint, 2> cursorDeltaPos();
	REint cursorDeltaPosX();
	REint cursorDeltaPosY();
	Vector<float, 2> normalCursorPos();
	float normalCursorPosX();
	float normalCursorPosY();
	Vector<float, 2> normalCursorDeltaPos();
	float normalCursorDeltaPosX();
	float normalCursorDeltaPosY();

}

#endif /* __RE_H__ */
