#ifndef __RE_H__
#define __RE_H__

#if defined(_WIN32) || defined(_MSC_VER)
# define RE_OS_WINDOWS
# define NOGDI
# ifdef _MSC_VER
#  define NOMINMAX
# endif
# define WIN32_LEAN_AND_MEAN /* speeds compilation up */
# include <windows.h>
#elif defined(__linux__)
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
#include <random>

typedef int8_t REbyte;
typedef uint8_t REubyte;
typedef int16_t REshort;
typedef uint16_t REushort;
typedef int32_t REint;
typedef uint32_t REuint;
typedef int64_t RElong;
typedef uint64_t REulong;

namespace RE {

#define STRIP_QUOTE_MACRO(T) T

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

	enum Input {
		Unknown = 0x00,
		ScrollUp = 0x01,
		ScrollDown = 0x02,
		LeftButton = 0x03,
		RightButton = 0x04,
		MiddleButton = 0x05,
		Space = 0x06,
		A = 0x07,
		B = 0x08,
		C = 0x09,
		D = 0x0A,
		E = 0x0B,
		F = 0x0C,
		G = 0x0D,
		H = 0x0E,
		I = 0x0F,
		J = 0x10,
		K = 0x11,
		L = 0x12,
		M = 0x13,
		N = 0x14,
		O = 0x15,
		P = 0x16,
		Q = 0x17,
		R = 0x18,
		S = 0x19,
		T = 0x1A,
		U = 0x1B,
		V = 0x1C,
		W = 0x1D,
		X = 0x1E,
		Y = 0x1F,
		Z = 0x20,
		Top_0 = 0x21,
		Top_1 = 0x22,
		Top_2 = 0x23,
		Top_3 = 0x24,
		Top_4 = 0x25,
		Top_5 = 0x26,
		Top_6 = 0x27,
		Top_7 = 0x28,
		Top_8 = 0x29,
		Top_9 = 0x2A,
		Slash = 0x2B, /* / */
		Backslash = 0x2C, /* \ */
		Comma = 0x2D, /* , */
		Period = 0x2E, /* . */
		Semicolon = 0x2F, /* ; */
		Apostrophe = 0x30, /* ' */
		Accent = 0x31, /* ` */
		Left_Bracket = 0x32, /* [ */
		Right_Bracket = 0x33, /* ] */
		Equals = 0x34, /* = */
		Minus = 0x35, /* - */
		Right_Ctrl = 0x36,
		Left_Ctrl = 0x37,
		Right_Alt = 0x38, /* AltGr */
		Left_Alt = 0x39,
		Right_Shift = 0x3A,
		Left_Shift = 0x3B,
		Menu = 0x3C,
		Tab = 0x3D,
		Enter = 0x3E,
		Escape = 0x3F,
		Backspace = 0x40,
		Arrow_Left = 0x41,
		Arrow_Right = 0x42,
		Arrow_Up = 0x43,
		Arrow_Down = 0x44,
		Delete = 0x45,
		Insert = 0x46,
		Home = 0x47,
		End = 0x48,
		Print_Screen = 0x49,
		Scroll_Lock = 0x4A,
		Pause = 0x4B,
		Page_Up = 0x4C,
		Page_Down = 0x4D,
		F1 = 0x4E,
		F2 = 0x4F,
		F3 = 0x50,
		F4 = 0x51,
		F5 = 0x52,
		F6 = 0x53,
		F7 = 0x54,
		F8 = 0x55,
		F9 = 0x56,
		F10 = 0x57,
		F11 = 0x58,
		F12 = 0x59,
		F13 = 0x5A,
		F14 = 0x5B,
		F15 = 0x5C,
		F16 = 0x5D,
		F17 = 0x5E,
		F18 = 0x5F,
		F19 = 0x60,
		F20 = 0x61,
		F21 = 0x62,
		F22 = 0x63,
		F23 = 0x64,
		F24 = 0x65,
		F25 = 0x66,
		Caps_Lock = 0x67,
		Numpad_Lock = 0x68,
		Numpad_0 = 0x69,
		Numpad_1 = 0x6A,
		Numpad_2 = 0x6B,
		Numpad_3 = 0x6C,
		Numpad_4 = 0x6D,
		Numpad_5 = 0x6E,
		Numpad_6 = 0x6F,
		Numpad_7 = 0x70,
		Numpad_8 = 0x71,
		Numpad_9 = 0x72,
		Numpad_Add = 0x73,
		Numpad_Subtract = 0x74,
		Numpad_Multiply = 0x75,
		Numpad_Divide = 0x76,
		Numpad_Enter = 0x77,
		Numpad_Period = 0x78,
		/**
		 * Represents a key, that does not exist on US-keyboards:
		 * - UK, DE: less than, (shift) greater than, (AltGr) vertical bar/pipe
		 */
		World_1 = 0x79
	};

	void addToStackTrace(const char* pcFile, const char* pcMethod, REuint u32Line, const char* pcDetails);
	void removeFromStackTrace();
	class SignalGuard {
		public:
			SignalGuard(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetails);
			~SignalGuard();
	};
#define DEFINE_SIGNAL_GUARD_DETAILED(NAME, DETAILS) SignalGuard NAME(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(DETAILS))
#define DEFINE_SIGNAL_GUARD(NAME) DEFINE_SIGNAL_GUARD_DETAILED(NAME, "\0")
#define CATCH_SIGNAL_DETAILED(CMD, DETAILS) do { \
			addToStackTrace(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(DETAILS)); \
			CMD; \
			removeFromStackTrace(); \
		} while (false)
#define CATCH_SIGNAL(CMD) CATCH_SIGNAL_DETAILED(CMD, "\0")

#define SAFE_DELETE(PTR_REF) CATCH_SIGNAL( do { \
			delete (PTR_REF); \
			(PTR_REF) = nullptr; \
		} while (false) )

	template <class... T>
	void print(T... content) {
		([&]() {
			if constexpr (std::is_same_v<T, REubyte>)
				std::cout << static_cast<REushort>(content);
			else if constexpr (std::is_same_v<T, REbyte>)
				std::cout << static_cast<REshort>(content);
			else if constexpr (std::is_same_v<T, bool>)
				std::cout << (content ? "true" : "false");
			else
				std::cout << content;
		} (), ...);
	}
	template <class... T>
	void println(T... content) {
		print(content..., "\n");
	}
	void printColored(const char* content, TerminalColor color, bool backgroundColored, bool bold);
	void printlnColored(const char* content, TerminalColor color, bool backgroundColored, bool bold);
#define PRINT(MSG) print(appendStrings(__FILE__, " (line ", __LINE__, "): ", STRIP_QUOTE_MACRO(MSG)))
#define PRINT_LN(MSG) print(appendStrings(__FILE__, " (line ", __LINE__, "): ", STRIP_QUOTE_MACRO(MSG), "\n"))

	std::string convertToUTF8(const wchar_t* pwcString);
	std::wstring convertToWide(const char* pcString);
	std::string getAppName();

	template <typename T>
	constexpr T nth_root(T n, T value) {
		T result = static_cast<T>(0.0);
		CATCH_SIGNAL(result = std::pow(value, static_cast<T>(1.0) / n));
		return result;
	}

	template <typename... T>
	constexpr REulong genBitmask(T... bits) {
		return (... | (1L << static_cast<REulong>(bits)));
	}

	constexpr REulong genBitmaskRange(REulong ulBegin, REulong ulEnd) {
		if (ulBegin > ulEnd)
			std::swap(ulBegin, ulEnd);
		else if (ulBegin == ulEnd)
			return genBitmask(ulBegin);
		REulong ulResult = 0L;
		for (REulong ulCurrentNumber = ulBegin; ulCurrentNumber < ulEnd; ulCurrentNumber++)
			ulResult |= genBitmask(ulCurrentNumber);
		return ulResult;
	}

	template <typename T>
	constexpr bool isBitTrue(T value, T bit) {
		return (value & genBitmask<T>(bit)) != static_cast<T>(0);
	}

	template <typename T>
	constexpr bool areBitsTrueRange(T value, T begin, T end) {
		if (begin > end)
			std::swap(begin, end);
		else if (begin == end)
			return isBitTrue<T>(value, begin);
		for (T i = begin; i < end; i++)
			if (!isBitTrue<T>(value, i))
				return false;
		return true;
	}

	template <typename T>
	constexpr T setBit(T& value, T bit, bool bNewState) {
		T targetBit = genBitmask<T>(bit);
		if (bNewState)
			value |= targetBit;
		else
			value &= ~targetBit;
		return value;
	}

	template <typename T>
	constexpr T setBits(T& value, T begin, T end, bool bNewState) {
		for (T i = begin; i < end; i++)
			setBit<T>(value, i, bNewState);
		return value;
	}

	template <typename T>
	constexpr std::string bitmaskToString(T bitmask, bool bWithSpace) {
		std::string strResult("");
		T bits = sizeof(T) * static_cast<T>(8);
		T rightShift = sizeof(T) * static_cast<T>(8) - static_cast<T>(1);
		for (T bit = static_cast<T>(0); bit < bits; bit++) {
			if (bWithSpace && bit % static_cast<T>(8) == static_cast<T>(0) && bit)
				strResult += " ";
			T shiftedNumber = bitmask << bit;
			strResult += ((shiftedNumber >> rightShift) == static_cast<T>(1)) ? "1" : "0";
		}
		return strResult;
	}

	template <class... T>
	std::string appendStrings(T... strings) {
		std::stringstream ss("");
		(ss << ... << strings);
		return std::string(ss.str());
	}
	
	void error(const char* pcFile, const char* pcFunc, REuint uiLine, const char* pcDetail, bool bTerminate);
	void warning(const char* pcFile, const char* pcFunc, REuint uiLine, const char* pcDetail);
	void note(const char* pcFile, const char* pcFunc, REuint uiLine, const char* pcDetail);
#define FATAL_ERROR(T) error(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T), true)
#define ERROR(T) error(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T), false)
#define WARNING(T) warning(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T))
#define NOTE(T) note(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T))

	template <typename T, REuint u32Dimensions>
	class Vector {
		static_assert(u32Dimensions != 0U, "A vector-template has zero dimensions");

		public:
			T coords[u32Dimensions];

			Vector() {
				fill(static_cast<T>(0.0));
			}
			template <typename... V>
			Vector(V... values) {
				fill(static_cast<T>(0.0));
				REuint u32Index = 0U;
				([&]() {
					CATCH_SIGNAL_DETAILED(coords[u32Index] = static_cast<T>(values), appendStrings("Index: ", u32Index).c_str());
					u32Index++;
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
				T result;
				CATCH_SIGNAL(result = nth_root<T>(static_cast<T>(u32Dimensions), sum()));
				return result;
			}

			void fill(T value) {
				std::fill(std::begin(coords), std::end(coords), value);
			}

			constexpr REuint getDimensions() const {
				return u32Dimensions;
			}

			T& operator [](REuint index) {
				if (index >= u32Dimensions)
					FATAL_ERROR(appendStrings("Index ", index, " is out of bounds: [0, ", u32Dimensions, "]").c_str());
				return coords[index];
			}

			T operator [](REuint index) const {
				if (index >= u32Dimensions)
					FATAL_ERROR(appendStrings("Index ", index, " is out of bounds: [0, ", u32Dimensions, "]").c_str());
				return coords[index];
			}

			void operator =(const Vector& copy) {
				if (u32Dimensions != copy.getDimensions())
					return;
				for (REuint u32Index = 0U; u32Index < u32Dimensions; u32Index++)
					coords[u32Index] = copy[u32Index];
			}

			bool operator ==(const Vector& compareVector) const {
				if (u32Dimensions != compareVector.getDimensions())
					return false;
				for (REuint u32Index = 0U; u32Index < u32Dimensions; u32Index++)
					if (coords[u32Index] != compareVector[u32Index])
						return false;
				return true;
			}

			bool operator !=(const Vector& compareVector) const {
				return !(*this == compareVector);
			}

			friend std::ostream& operator <<(std::ostream& stream, const Vector& vector) {
				stream << "(";
				for (REuint i = 0U; i < vector.getDimensions(); i++) {
					if (i != 0U)
						stream << ", ";
					stream << vector.coords[i];
				}
				stream << ")";
				return stream;
			}
	};
	typedef Vector<float, 2U> Vector2f;
	typedef Vector<float, 3U> Vector3f;
	typedef Vector<float, 4U> Vector4f;
	typedef Vector<double, 2U> Vector2d;
	typedef Vector<double, 3U> Vector3d;
	typedef Vector<double, 4U> Vector4d;
	typedef Vector<REint, 2U> Vector2i;
	typedef Vector<REint, 3U> Vector3i;
	typedef Vector<REint, 4U> Vector4i;
	typedef Vector<REuint, 2U> Vector2u;
	typedef Vector<REuint, 3U> Vector3u;
	typedef Vector<REuint, 4U> Vector4u;

	class RandomNumberGenerator {
		private:
			std::mt19937 rng;

		public:
			RandomNumberGenerator();
			RandomNumberGenerator(REuint seed);
			~RandomNumberGenerator();
			REuint random();
			REuint random(REuint max);
			REuint random(REuint min, REuint max);
			bool randomBool();
	};
	typedef RandomNumberGenerator RNG;

	class Scene {
		public:
			const REuint u32Id;

			Scene() = delete;
			Scene(REuint u32Id);
			virtual ~Scene();
			virtual void start();
			virtual void update();
			virtual void end();
	};

	class GameObject {
		public:
			const REuint u32OwnId;
			const REuint u32SceneParentId;

			GameObject() = delete;
			GameObject(REuint u32OwnId, REuint u32SceneParentId);
			virtual ~GameObject();
			virtual void start(Scene* pStartingScene);
			virtual void update(Scene* pCurrentScene);
			virtual void end(Scene* pEndingScene);
	};

	void execute();

	void markDelete(GameObject* pGameObject);
#define MARK_SAFE_DELETE(GAME_OBJECT_POINTER) CATCH_SIGNAL( do { \
		markDelete(GAME_OBJECT_POINTER); \
		GAME_OBJECT_POINTER = nullptr; \
	} while (false) )
	
	void setNextScene(Scene* pNextScene);
	bool isNextSceneSet();
	Scene* getCurrentScene();
	REuint getCurrentSceneId();
	bool isSceneCurrent(REuint u32SceneId);
	Scene* getNextScene();
	REuint getNextSceneId();
	bool isSceneNext(REuint u32SceneId);

#ifdef RE_OS_WINDOWS
	void setHInstance(HINSTANCE win_hInstance);
#endif

}

#endif /* __RE_H__ */
