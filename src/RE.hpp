#ifndef __RE_H__
#define __RE_H__

#if defined(_WIN32) || defined(_MSC_VER)
# define RE_OS_WINDOWS
# define NOGDI
# ifdef _MSC_VER
#  define NOMINMAX
# endif
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

	enum Keyboard {
		Unknown,
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
		Menu,
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
		Numpad_Period,
		/**
		 * Represents a key, that does not exist on US-keyboards:
		 * - UK, DE: less than, (shift) greater than, (AltGr) vertical bar/pipe
		 */
		World_1
	};

	enum MouseButton {
		Left,
		Right,
		Middle
	};

	void addToStackTrace(const char* pcFile, const char* pcMethod, REuint u32Line);
	void removeFromStackTrace();
#define CATCH_SIGNAL(CMD) do { \
			addToStackTrace(__FILE__, __func__, __LINE__); \
			CMD; \
			removeFromStackTrace(); \
		} while (false)

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
		static_assert(u32Dimensions != 0, "A vector-template has zero dimensions");

		public:
			T coords[u32Dimensions];

			Vector() {
				fill(static_cast<T>(0.0));
			}
			template <typename... V>
			Vector(V... values) {
				fill(static_cast<T>(0.0));
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
				for (REuint u32Index = 0; u32Index < u32Dimensions; u32Index++)
					coords[u32Index] = copy[u32Index];
			}

			bool operator ==(const Vector& compareVector) const {
				if (u32Dimensions != compareVector.getDimensions())
					return false;
				for (REuint u32Index = 0; u32Index < u32Dimensions; u32Index++)
					if (coords[u32Index] != compareVector[u32Index])
						return false;
				return true;
			}

			bool operator !=(const Vector& compareVector) const {
				return !(*this == compareVector);
			}

			friend std::ostream& operator <<(std::ostream& stream, const Vector& vector) {
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

	typedef Vector<float, 2> Vector2f;
	typedef Vector<float, 3> Vector3f;
	typedef Vector<float, 4> Vector4f;
	typedef Vector<double, 2> Vector2d;
	typedef Vector<double, 3> Vector3d;
	typedef Vector<double, 4> Vector4d;
	typedef Vector<REint, 2> Vector2i;
	typedef Vector<REint, 3> Vector3i;
	typedef Vector<REint, 4> Vector4i;
	typedef Vector<REuint, 2> Vector2u;
	typedef Vector<REuint, 3> Vector3u;
	typedef Vector<REuint, 4> Vector4u;

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

	REuint scancodeFromKey(Keyboard eKey);
	Keyboard keyFromScancode(REuint u32Scancode);
	bool isKeyDown(Keyboard eKey);
	bool isKeyPressed(Keyboard eKey);
	bool isKeyReleased(Keyboard eKey);
	bool isButtonDown(MouseButton eButton);
	bool isButtonPressed(MouseButton eButton);
	bool isButtonReleased(MouseButton eButton);
	bool isScrolling();
	bool isScrollingUpward();
	bool isScrollingDownward();
	REbyte scrollDirection();
	Vector2i cursorPos();
	REint cursorPosX();
	REint cursorPosY();
	Vector2i cursorDeltaPos();
	REint cursorDeltaPosX();
	REint cursorDeltaPosY();
	Vector2f normalCursorPos();
	float normalCursorPosX();
	float normalCursorPosY();
	Vector2f normalCursorDeltaPos();
	float normalCursorDeltaPosX();
	float normalCursorDeltaPosY();

#ifdef RE_OS_WINDOWS
	void setHInstance(HINSTANCE win_hInstance);
#endif

}

#endif /* __RE_H__ */
