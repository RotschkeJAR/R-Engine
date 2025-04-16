#include "RE_Internal Header.hpp"
#include "RE_Console.hpp"
#include "RE_Main.hpp"
#include "RE_Window.hpp"

namespace RE {

#define DEFAULT_COLOR "\033[0m"

#define PRINT_COLORS 0
#define TREAT_WARNING_AS_ERROR 1
#define ERRORS_ALWAYS_FATAL 2
#define SHOW_MSG_BOX 3

	REubyte u8ConsoleSettings = 0b00001001U;

	std::string escape_code_to_string(TerminalColor eColor, bool bBackgroundColored, bool bBold) {
		if (!is_bit_true<REubyte>(u8ConsoleSettings, PRINT_COLORS))
			return "";
		REuint u32Id = static_cast<REuint>(eColor);
		if (u32Id >= static_cast<REuint>(RE_TERMINAL_COLOR_BRIGHT_BLACK))
			u32Id += 90 - static_cast<REuint>(RE_TERMINAL_COLOR_BRIGHT_BLACK);
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

	void print_error_msg(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetail) {
		print(" : ");
		print_colored(pcFile, RE_TERMINAL_COLOR_BRIGHT_WHITE, false, true);
		println(" (line ", u32Line, "; in function \"", pcFunc, "\")");
		const REuint u32MsgLineCount = get_line_count(pcDetail);
		for (REuint u32Line = 0U; u32Line < u32MsgLineCount; u32Line++)
			println("\t", get_line(pcDetail, u32Line));
	}

	void print_colored(const char* pcContent, TerminalColor eColor, bool bBackgroundColored, bool bBold) {
		print(escape_code_to_string(eColor, bBackgroundColored, bBold), pcContent, DEFAULT_COLOR);
	}

	void println_colored(const char* pcContent, TerminalColor eColor, bool bBackgroundColored, bool bBold) {
		println(escape_code_to_string(eColor, bBackgroundColored, bBold), pcContent, DEFAULT_COLOR);
	}
	
	void error(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetail, bool bTerminate) {
		print_colored("ERROR", RE_TERMINAL_COLOR_RED, false, false);
		print_error_msg(pcFile, pcFunc, u32Line, pcDetail);
		bool bFatal = bTerminate || is_bit_true<REubyte>(u8ConsoleSettings, ERRORS_ALWAYS_FATAL);
		if (bFatal) {
			bErrorOccured = true;
			println_colored("Terminating...", RE_TERMINAL_COLOR_BRIGHT_BLACK, false, false);
		}
		if (is_bit_true<REubyte>(u8ConsoleSettings, SHOW_MSG_BOX)) {
#ifdef RE_OS_WINDOWS
			MessageBoxW(Window::pInstance ? static_cast<Window_Win64*>(Window::pInstance)->get_hwindow() : nullptr, append_to_wstring(L"in file ", pcFile, L" in function ", pcFunc, L" at line ", u32Line, L"\n", pcDetail).c_str(), bFatal ? L"Fatal Error" : L"Error", MB_OK | MB_ICONERROR);
#elif defined RE_OS_LINUX
			// TODO: Create message box on Linux, when error occurs
#endif
		}
	}

	void warning(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetail) {
		if (is_bit_true<REubyte>(u8ConsoleSettings, TREAT_WARNING_AS_ERROR))
			error(pcFile, pcFunc, u32Line, pcDetail, false);
		else {
			print_colored("WARNING", RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, false);
			print_error_msg(pcFile, pcFunc, u32Line, pcDetail);
		}
	}

	void note(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetail) {
		print_colored("NOTE", RE_TERMINAL_COLOR_WHITE, false, false);
		print_error_msg(pcFile, pcFunc, u32Line, pcDetail);
	}

	void enable_colorful_printing(bool bEnable) {
		set_bit<REubyte>(u8ConsoleSettings, PRINT_COLORS, bEnable);
	}

	void treat_warnings_as_errors(bool bEnable) {
		set_bit<REubyte>(u8ConsoleSettings, TREAT_WARNING_AS_ERROR, bEnable);
	}

	void make_errors_always_fatal(bool bEnable) {
		set_bit<REubyte>(u8ConsoleSettings, ERRORS_ALWAYS_FATAL, bEnable);
	}

	void show_message_box_on_error(bool bEnable) {
		set_bit<REubyte>(u8ConsoleSettings, SHOW_MSG_BOX, bEnable);
	}

}
