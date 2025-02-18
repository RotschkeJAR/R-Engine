#include "RE_Ext Header.hpp"
#include "RE_Console.hpp"
#include "RE_Main.hpp"

namespace RE {

#define DEFAULT_COLOR "\033[0m"

	bool bPrintColors = true;
	bool bTreatWarningAsError = false;
	bool bErrorAlwaysFatal = false;

	std::string ESC_code(TerminalColor eColor, bool bBackgroundColored, bool bBold) {
		if (!bPrintColors)
			return "";
		unsigned int uiId = static_cast<unsigned int>(eColor);
		if (uiId >= static_cast<unsigned int>(TerminalColor::Bright_Black))
			uiId += 90 - static_cast<unsigned int>(TerminalColor::Bright_Black);
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

	void printErrMsg(const char* pFile, const char* pFunc, unsigned int uiLine, const char* pDetail) {
		print(" : ");
		printColored(pFile, TerminalColor::Bright_White, false, true);
		println(" (line ", uiLine, "; in function \"", pFunc, "\")");
		println("\t", pDetail);
	}


	void printColored(const char* pContent, TerminalColor eColor, bool bBackgroundColored, bool bBold) {
		print(ESC_code(eColor, bBackgroundColored, bBold), pContent, DEFAULT_COLOR);
	}

	void printlnColored(const char* pContent, TerminalColor eColor, bool bBackgroundColored, bool bBold) {
		println(ESC_code(eColor, bBackgroundColored, bBold), pContent, DEFAULT_COLOR);
	}
	
	void error(const char* pFile, const char* pFunc, unsigned int uiLine, const char* pDetail, bool bTerminate) {
		printColored("ERROR", TerminalColor::Red, false, false);
		printErrMsg(pFile, pFunc, uiLine, pDetail);
		if (bTerminate || bErrorAlwaysFatal) {
			bErrorOccured = true;
			printlnColored("Terminating...", TerminalColor::Bright_Black, false, false);
		}
	}

	void warning(const char* pFile, const char* pFunc, unsigned int uiLine, const char* pDetail) {
		if (bTreatWarningAsError)
			error(pFile, pFunc, uiLine, pDetail, false);
		else {
			printColored("WARNING", TerminalColor::Bright_Yellow, false, false);
			printErrMsg(pFile, pFunc, uiLine, pDetail);
		}
	}

	void note(const char* pFile, const char* pFunc, unsigned int uiLine, const char* pDetail) {
		printColored("NOTE", TerminalColor::White, false, false);
		printErrMsg(pFile, pFunc, uiLine, pDetail);
	}

}
