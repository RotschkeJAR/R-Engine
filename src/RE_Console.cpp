#include "RE_Internal Header.hpp"
#include "RE_Console.hpp"
#include "RE_Main.hpp"

namespace RE {

#define DEFAULT_COLOR "\033[0m"

	bool bPrintColors = true;
	bool bTreatWarningAsError = false;
	bool bErrorAlwaysFatal = false;

	std::string escape_code_to_string(TerminalColor eColor, bool bBackgroundColored, bool bBold) {
		if (!bPrintColors)
			return "";
		unsigned int uiId = static_cast<unsigned int>(eColor);
		if (uiId >= static_cast<unsigned int>(RE_TERMINAL_COLOR_BRIGHT_BLACK))
			uiId += 90 - static_cast<unsigned int>(RE_TERMINAL_COLOR_BRIGHT_BLACK);
		else
			uiId += 30;
		if (bBackgroundColored)
			uiId += 10;
		std::string result("\033[");
		if (bBold)
			result += "1;";
		result += std::to_string(uiId);
		result += "m";
		return result;
	}

	void print_error_msg(const char* pFile, const char* pFunc, unsigned int uiLine, const char* pDetail) {
		print(" : ");
		print_colored(pFile, RE_TERMINAL_COLOR_BRIGHT_WHITE, false, true);
		println(" (line ", uiLine, "; in function \"", pFunc, "\")");
		println("\t", pDetail);
	}


	void print_colored(const char* pContent, TerminalColor eColor, bool bBackgroundColored, bool bBold) {
		print(escape_code_to_string(eColor, bBackgroundColored, bBold), pContent, DEFAULT_COLOR);
	}

	void println_colored(const char* pContent, TerminalColor eColor, bool bBackgroundColored, bool bBold) {
		println(escape_code_to_string(eColor, bBackgroundColored, bBold), pContent, DEFAULT_COLOR);
	}
	
	void error(const char* pFile, const char* pFunc, unsigned int uiLine, const char* pDetail, bool bTerminate) {
		print_colored("ERROR", RE_TERMINAL_COLOR_RED, false, false);
		print_error_msg(pFile, pFunc, uiLine, pDetail);
		if (bTerminate || bErrorAlwaysFatal) {
			bErrorOccured = true;
			println_colored("Terminating...", RE_TERMINAL_COLOR_BRIGHT_BLACK, false, false);
		}
	}

	void warning(const char* pFile, const char* pFunc, unsigned int uiLine, const char* pDetail) {
		if (bTreatWarningAsError)
			error(pFile, pFunc, uiLine, pDetail, false);
		else {
			print_colored("WARNING", RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, false);
			print_error_msg(pFile, pFunc, uiLine, pDetail);
		}
	}

	void note(const char* pFile, const char* pFunc, unsigned int uiLine, const char* pDetail) {
		print_colored("NOTE", RE_TERMINAL_COLOR_WHITE, false, false);
		print_error_msg(pFile, pFunc, uiLine, pDetail);
	}

}
