#include "RE_Internal Header.hpp"
#include "RE_Console.hpp"
#include "RE_Main.hpp"
#include "RE_Window_Win64.hpp"

namespace RE {

#define DEFAULT_COLOR "\033[0m"

	bool bPrintColors = true;
	bool bTreatWarningAsError = false;
	bool bErrorAlwaysFatal = false;

	std::string escape_code_to_string(TerminalColor eColor, bool bBackgroundColored, bool bBold) {
		if (!bPrintColors)
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
		println("\t", pcDetail);
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
		if (bTerminate || bErrorAlwaysFatal) {
			bErrorOccured = true;
			println_colored("Terminating...", RE_TERMINAL_COLOR_BRIGHT_BLACK, false, false);
		}
#ifdef RE_OS_WINDOWS
		MessageBoxW(Window::pInstance ? static_cast<Window_Win64*>(Window::pInstance)->get_hwindow() : nullptr, convert_chars_to_wide(pcDetail).c_str(), L"Error", MB_OK | MB_ICONERROR);
#elif defined RE_OS_LINUX
		// TODO: Create message box on Linux, when error occurs
#endif
	}

	void warning(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetail) {
		if (bTreatWarningAsError)
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

}
