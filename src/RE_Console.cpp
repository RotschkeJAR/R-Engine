#include "RE_Internal Header.hpp"
#include "RE_Console.hpp"
#include "RE_Main.hpp"
#include "RE_Window_Win64.hpp"

#include <time.h>

namespace RE {

#define DEFAULT_COLOR "\033[0m"

#define PRINT_COLORS 0
#define TREAT_WARNING_AS_ERROR 1
#define ERRORS_ALWAYS_FATAL 2
#define SHOW_MSG_BOX 3
#define VERBOSE_BEHAVIOUR 4
#define LOG_TIME 5

	uint8_t u8ConsoleSettings = (1 << PRINT_COLORS) | (1 << SHOW_MSG_BOX) | (1 << LOG_TIME);

	static void print_time() {
		if (!are_bits_true<uint8_t>(u8ConsoleSettings, LOG_TIME))
			return;
		const std::time_t timePoint = std::time(nullptr);
		const std::string sTimeString(std::ctime(&timePoint));
		print("[", sTimeString.substr(0U, sTimeString.size() - 1U), "] ");
	}

	[[nodiscard]]
	static std::string escape_code_to_string(const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		if (!are_bits_true<uint8_t>(u8ConsoleSettings, PRINT_COLORS))
			return "";
		uint32_t u32Id = static_cast<uint32_t>(eColor);
		if (u32Id >= static_cast<uint32_t>(RE_TERMINAL_COLOR_BRIGHT_BLACK))
			u32Id += 90 - static_cast<uint32_t>(RE_TERMINAL_COLOR_BRIGHT_BLACK);
		else
			u32Id += 30;
		if (bBackgroundColored)
			u32Id += 10;
		std::string result("\033[");
		if (bBold)
			result += "1;";
		result += std::to_string(u32Id);
		result += "m";
		return result;
	}

	static void print_error_msg(const char *const pacFile, const char *const pacFunc, const uint32_t u32Line, const char *const pacDetail) {
		print(" : ");
		print_colored(pacFile, RE_TERMINAL_COLOR_BRIGHT_WHITE, false, true);
		println(" (line ", u32Line, "; in function \"", pacFunc, "\")");
		const size_t msgLineCount = get_line_count(pacDetail);
		for (size_t lineIndex = 0; lineIndex < msgLineCount; lineIndex++)
			println("\t", get_line(pacDetail, lineIndex));
	}

	void print_colored(const char *const pacContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		print(escape_code_to_string(eColor, bBackgroundColored, bBold), pacContent, DEFAULT_COLOR);
	}

	void println_colored(const char *const pacContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		println(escape_code_to_string(eColor, bBackgroundColored, bBold), pacContent, DEFAULT_COLOR);
	}
	
	void error(const char *const pacFile, const char *const pacFunc, const uint32_t u32Line, const char *const pacDetail, const bool bTerminate) {
		print_time();
		print_colored("ERROR", RE_TERMINAL_COLOR_RED, false, false);
		print_error_msg(pacFile, pacFunc, u32Line, pacDetail);
		bool bFatal = bTerminate || are_bits_true<uint8_t>(u8ConsoleSettings, ERRORS_ALWAYS_FATAL);
		if (bFatal) {
			bErrorOccured = true;
			println_colored("Terminating...", RE_TERMINAL_COLOR_BRIGHT_BLACK, false, false);
			if (are_bits_true<uint8_t>(u8ConsoleSettings, SHOW_MSG_BOX)) {
#ifdef RE_OS_WINDOWS
				MessageBoxW(win_hWindow, append_to_wstring(L"In file ", pacFile, L" in function ", pacFunc, L" at line ", u32Line, L"\n", pacDetail).c_str(), L"Fatal Error", MB_OK | MB_ICONERROR);
#elif defined RE_OS_LINUX
				// TODO: Create message box on Linux, when error occurs
#endif
			}
		}
	}

	void warning(const char *const pacFile, const char *const pacFunc, const uint32_t u32Line, const char *const pacDetail) {
		if (are_bits_true<uint8_t>(u8ConsoleSettings, TREAT_WARNING_AS_ERROR))
			error(pacFile, pacFunc, u32Line, pacDetail, false);
		else {
			print_time();
			print_colored("WARNING", RE_TERMINAL_COLOR_YELLOW, false, false);
			print_error_msg(pacFile, pacFunc, u32Line, pacDetail);
		}
	}

	void note(const char *const pacFile, const char *const pacFunc, const uint32_t u32Line, const char *const pacDetail) {
		print_time();
		print_colored("NOTE", RE_TERMINAL_COLOR_WHITE, false, false);
		print_error_msg(pacFile, pacFunc, u32Line, pacDetail);
	}

	void enable_colorful_printing(const bool bEnable) {
		set_bits<uint8_t>(u8ConsoleSettings, bEnable, PRINT_COLORS);
	}

	void treat_warnings_as_errors(const bool bEnable) {
		set_bits<uint8_t>(u8ConsoleSettings, bEnable, TREAT_WARNING_AS_ERROR);
	}

	void make_errors_always_fatal(const bool bEnable) {
		set_bits<uint8_t>(u8ConsoleSettings, bEnable, ERRORS_ALWAYS_FATAL);
	}

	void show_message_box_on_error(const bool bEnable) {
		set_bits<uint8_t>(u8ConsoleSettings, bEnable, SHOW_MSG_BOX);
	}

	void enable_verbosity(const bool bEnable) {
		set_bits<uint8_t>(u8ConsoleSettings, bEnable, VERBOSE_BEHAVIOUR);
	}

	[[nodiscard]]
	bool is_verbose_behaviour_enabled() {
		return are_bits_true<uint8_t>(u8ConsoleSettings, VERBOSE_BEHAVIOUR);
	}

	void enable_time_logging(const bool bEnable) {
		set_bits<uint8_t>(u8ConsoleSettings, bEnable, LOG_TIME);
	}

}
