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
#include <string.h>
#include <math.h>
#include <stdint.h>
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
	
	void error(const char* pcFile, const char* pcFunc, REuint uiLine, const char* pcDetail, bool bTerminate);
	void warning(const char* pcFile, const char* pcFunc, REuint uiLine, const char* pcDetail);
	void note(const char* pcFile, const char* pcFunc, REuint uiLine, const char* pcDetail);
#define FATAL_ERROR(T) error(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T), true)
#define ERROR(T) error(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T), false)
#define WARNING(T) warning(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T))
#define NOTE(T) note(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(T))

	void add_to_stack_trace(const char* pcFile, const char* pcMethod, REuint u32Line, const char* pcDetails);
	void remove_from_stack_trace();
	class SignalGuard {
		public:
			SignalGuard(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetails);
			~SignalGuard();
	};
#define DEFINE_SIGNAL_GUARD_DETAILED(NAME, DETAILS) SignalGuard NAME(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(DETAILS))
#define DEFINE_SIGNAL_GUARD(NAME) DEFINE_SIGNAL_GUARD_DETAILED(NAME, "\0")
#define CATCH_SIGNAL_DETAILED(CMD, DETAILS) do { \
			add_to_stack_trace(__FILE__, __func__, __LINE__, STRIP_QUOTE_MACRO(DETAILS)); \
			CMD; \
			remove_from_stack_trace(); \
		} while (false)
#define CATCH_SIGNAL(CMD) CATCH_SIGNAL_DETAILED(CMD, "\0")

#define SAFE_DELETE(PTR_REF) CATCH_SIGNAL( do { \
			delete (PTR_REF); \
			(PTR_REF) = nullptr; \
		} while (false) )

	template <class... T>
	std::string append_strings(T... strings) {
		std::stringstream ss("");
		(ss << ... << strings);
		return std::string(ss.str());
	}

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
	void print_colored(const char* content, TerminalColor color, bool backgroundColored, bool bold);
	void println_colored(const char* content, TerminalColor color, bool backgroundColored, bool bold);
#define PRINT(MSG) print(append_strings(__FILE__, " (line ", __LINE__, "): ", STRIP_QUOTE_MACRO(MSG)))
#define PRINT_LN(MSG) print(append_strings(__FILE__, " (line ", __LINE__, "): ", STRIP_QUOTE_MACRO(MSG), "\n"))

	std::string convert_wide_chars_to_utf8(const wchar_t* pwcString);
	std::wstring convert_chars_to_wide(const char* pcString);
	std::string get_app_name();

	template <typename T>
	constexpr T nth_root(T n, T value) {
		T result = static_cast<T>(0.0);
		CATCH_SIGNAL(result = std::pow(value, static_cast<T>(1.0) / n));
		return result;
	}

	template <typename... T>
	constexpr REulong gen_bitmask(T... bits) {
		return (... | (1UL << static_cast<REulong>(bits)));
	}

	constexpr REulong gen_bitmask_in_range(REulong u64Begin, REulong u64End) {
		if (u64Begin > u64End) {
			FATAL_ERROR(append_strings("Start (", u64Begin, ") of the range is larger than end (", u64End, ")").c_str());
			return 0UL;
		} else if (u64Begin == u64End)
			return gen_bitmask(u64Begin);
		REulong u64Result = 0UL;
		for (REulong u64CurrentNumber = u64Begin; u64CurrentNumber < u64End; u64CurrentNumber++)
			u64Result |= gen_bitmask(u64CurrentNumber);
		return u64Result;
	}

	template <typename T>
	constexpr bool is_bit_true(T value, T bit) {
		return (value & gen_bitmask<T>(bit)) != static_cast<T>(0);
	}

	template <typename T>
	constexpr bool are_bits_true_in_range(T value, T begin, T end) {
		if (begin > end) {
			FATAL_ERROR(append_strings("Start (", begin, ") of the range is larger than end (", end, ")").c_str());
			return false;
		} else if (begin == end)
			return is_bit_true<T>(value, begin);
		for (T i = begin; i < end; i++)
			if (!is_bit_true<T>(value, i))
				return false;
		return true;
	}

	template <typename T>
	constexpr T set_bit(T& value, T bit, bool bNewState) {
		T targetBit = gen_bitmask<T>(bit);
		if (bNewState)
			value |= targetBit;
		else
			value &= ~targetBit;
		return value;
	}

	template <typename T>
	constexpr T set_bits(T& value, T begin, T end, bool bNewState) {
		if (begin > end) {
			FATAL_ERROR(append_strings("Start (", begin, ") of the range is larger than end (", end, ")").c_str());
			return static_cast<T>(0.0);
		}
		for (T i = begin; i < end; i++)
			set_bit<T>(value, i, bNewState);
		return value;
	}

	template <typename T>
	constexpr std::string bitmask_to_string(T bitmask, bool bWithSpace) {
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
					CATCH_SIGNAL_DETAILED(coords[u32Index] = static_cast<T>(values), append_strings("Index: ", u32Index).c_str());
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

			void copy_from(const Vector& copyVector) {
				if (u32Dimensions != copyVector.get_dimensions()) {
					WARNING(append_strings("Tried to copy values from one vector (", copyVector.get_dimensions(), ") to another (", u32Dimensions, ")").c_str());
					return;
				}
				for (REuint u32Index = 0U; u32Index < u32Dimensions; u32Index++)
					coords[u32Index] = copyVector[u32Index];
			}

			bool equals(Vector& compareVector) const {
				if (u32Dimensions != compareVector.get_dimensions())
					return false;
				for (REuint u32Index = 0U; u32Index < u32Dimensions; u32Index++)
					if (coords[u32Index] != compareVector[u32Index])
						return false;
				return true;
			}

			constexpr REuint get_dimensions() const {
				return u32Dimensions;
			}

			T& operator [](REuint index) {
				if (index >= u32Dimensions)
					FATAL_ERROR(append_strings("Index ", index, " is out of bounds: [0, ", u32Dimensions, "]").c_str());
				return coords[index];
			}

			T operator [](REuint index) const {
				if (index >= u32Dimensions)
					FATAL_ERROR(append_strings("Index ", index, " is out of bounds: [0, ", u32Dimensions, "]").c_str());
				return coords[index];
			}

			void operator =(const Vector& copyVector) {
				copy_from(copyVector);
			}

			bool operator ==(const Vector& compareVector) const {
				return equals(compareVector);
			}

			bool operator !=(const Vector& compareVector) const {
				return !equals(compareVector);
			}

			friend std::ostream& operator <<(std::ostream& stream, const Vector& vector) {
				stream << "(";
				for (REuint i = 0U; i < vector.get_dimensions(); i++) {
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
			RandomNumberGenerator(REuint u32Seed);
			~RandomNumberGenerator();
			void set_seed(REuint newSeed);
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
			bool random_bool(float fChance);
			float random_percentage();
	};

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
	float get_deltaseconds();
	float get_fps_rate();

	void mark_delete(GameObject* pGameObject);
#define MARK_SAFE_DELETE(GAME_OBJECT_POINTER) CATCH_SIGNAL( do { \
		markDelete(GAME_OBJECT_POINTER); \
		GAME_OBJECT_POINTER = nullptr; \
	} while (false) )
	
	void set_next_scene(Scene* pNextScene);
	bool is_next_scene_set();
	Scene* get_current_scene();
	REuint get_current_scene_id();
	bool is_scene_current(REuint u32SceneId);
	Scene* get_next_scene();
	REuint get_next_scene_id();
	bool is_scene_next(REuint u32SceneId);

#ifdef RE_OS_WINDOWS
	void set_hinstance(HINSTANCE win_hInstance);
#endif

}

#endif /* __RE_H__ */
