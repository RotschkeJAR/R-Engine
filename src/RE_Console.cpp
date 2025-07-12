#include "RE_Internal Header.hpp"
#include "RE_Console.hpp"
#include "RE_Main.hpp"
#include "RE_Window.hpp"

#include <time.h>

namespace RE {

#define DEFAULT_COLOR "\033[0m"

#define PRINT_COLORS 0
#define TREAT_WARNING_AS_ERROR 1
#define ERRORS_ALWAYS_FATAL 2
#define SHOW_MSG_BOX 3
#define VERBOSE_BEHAVIOUR 4
#define LOG_TIME 5

	uint8_t u8ConsoleSettings = (1U << PRINT_COLORS) | (1U << SHOW_MSG_BOX) | (1U << LOG_TIME);

	static void print_time() {
		if (!is_bit_true<uint8_t>(u8ConsoleSettings, LOG_TIME))
			return;
		const std::time_t timePoint = std::time(nullptr);
		const std::string sTimeString(std::ctime(&timePoint));
		print("[", sTimeString.substr(0U, sTimeString.size() - 1U), "] ");
	}

	static std::string escape_code_to_string(const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		if (!is_bit_true<uint8_t>(u8ConsoleSettings, PRINT_COLORS))
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

	static void print_error_msg(const char *const pcFile, const char *const pcFunc, const uint32_t u32Line, const char *const pcDetail) {
		print(" : ");
		print_colored(pcFile, RE_TERMINAL_COLOR_BRIGHT_WHITE, false, true);
		println(" (line ", u32Line, "; in function \"", pcFunc, "\")");
		const uint32_t u32MsgLineCount = get_line_count(pcDetail);
		for (uint32_t u32Line = 0U; u32Line < u32MsgLineCount; u32Line++)
			println("\t", get_line(pcDetail, u32Line));
	}

	void print_colored(const char *const pcContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		print(escape_code_to_string(eColor, bBackgroundColored, bBold), pcContent, DEFAULT_COLOR);
	}

	void println_colored(const char *const pcContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		println(escape_code_to_string(eColor, bBackgroundColored, bBold), pcContent, DEFAULT_COLOR);
	}
	
	void error(const char *const pcFile, const char *const pcFunc, const uint32_t u32Line, const char *const pcDetail, const bool bTerminate) {
		print_time();
		print_colored("ERROR", RE_TERMINAL_COLOR_RED, false, false);
		print_error_msg(pcFile, pcFunc, u32Line, pcDetail);
		bool bFatal = bTerminate || is_bit_true<uint8_t>(u8ConsoleSettings, ERRORS_ALWAYS_FATAL);
		if (bFatal) {
			bErrorOccured = true;
			println_colored("Terminating...", RE_TERMINAL_COLOR_BRIGHT_BLACK, false, false);
			if (is_bit_true<uint8_t>(u8ConsoleSettings, SHOW_MSG_BOX)) {
#ifdef RE_OS_WINDOWS
				MessageBoxW(Window::pInstance ? static_cast<Window_Win64*>(Window::pInstance)->get_hwindow() : nullptr, append_to_wstring(L"In file ", pcFile, L" in function ", pcFunc, L" at line ", u32Line, L"\n", pcDetail).c_str(), L"Fatal Error", MB_OK | MB_ICONERROR);
#elif defined RE_OS_LINUX
				// TODO: Create message box on Linux, when error occurs
#endif
			}
		}
	}

	void warning(const char *const pcFile, const char *const pcFunc, const uint32_t u32Line, const char *const pcDetail) {
		if (is_bit_true<uint8_t>(u8ConsoleSettings, TREAT_WARNING_AS_ERROR))
			error(pcFile, pcFunc, u32Line, pcDetail, false);
		else {
			print_time();
			print_colored("WARNING", RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, false);
			print_error_msg(pcFile, pcFunc, u32Line, pcDetail);
		}
	}

	void note(const char *const pcFile, const char *const pcFunc, const uint32_t u32Line, const char *const pcDetail) {
		print_time();
		print_colored("NOTE", RE_TERMINAL_COLOR_WHITE, false, false);
		print_error_msg(pcFile, pcFunc, u32Line, pcDetail);
	}

	void enable_colorful_printing(const bool bEnable) {
		set_bit<uint8_t>(u8ConsoleSettings, PRINT_COLORS, bEnable);
	}

	void treat_warnings_as_errors(const bool bEnable) {
		set_bit<uint8_t>(u8ConsoleSettings, TREAT_WARNING_AS_ERROR, bEnable);
	}

	void make_errors_always_fatal(const bool bEnable) {
		set_bit<uint8_t>(u8ConsoleSettings, ERRORS_ALWAYS_FATAL, bEnable);
	}

	void show_message_box_on_error(const bool bEnable) {
		set_bit<uint8_t>(u8ConsoleSettings, SHOW_MSG_BOX, bEnable);
	}

	void enable_verbosity(const bool bEnable) {
		set_bit<uint8_t>(u8ConsoleSettings, VERBOSE_BEHAVIOUR, bEnable);
	}

	bool is_verbose_behaviour_enabled() {
		return is_bit_true<uint8_t>(u8ConsoleSettings, VERBOSE_BEHAVIOUR);
	}

	void enable_time_logging(const bool bEnable) {
		set_bit<uint8_t>(u8ConsoleSettings, LOG_TIME, bEnable);
	}

}
