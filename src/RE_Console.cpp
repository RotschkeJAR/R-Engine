#include "RE_Ext Header.hpp"
#include "RE_Console.hpp"
#include "RE_Main.hpp"

namespace RE {

#define DEFAULT_COLOR "\033[0m"

	bool printColors = true;
	bool treatWarningAsError = false;
	bool errorAlwaysFatal = false;

	std::string ESC_code(TerminalColor color, bool backgroundColored, bool bold) {
		if (!printColors)
			return "";
		unsigned int id = static_cast<unsigned int>(color);
		if (id >= static_cast<unsigned int>(TerminalColor::Bright_Black))
			id += 90 - static_cast<unsigned int>(TerminalColor::Bright_Black);
		else
			id += 30;
		if (backgroundColored)
			id += 10;
		std::string result("\033[");
		if (bold)
			result += "1;";
		result += std::to_string(id);
		result += "m";
		return result;
	}

	void printErrMsg(const char* file, const char* func, unsigned int line, const char* detail) {
		print(" : ");
		printColored(file, TerminalColor::Bright_White, false, true);
		println(" (line ", line, "; in function \"", func, "\")");
		println("\t", detail);
	}


	void printColored(const char* content, TerminalColor color, bool backgroundColored, bool bold) {
		print(ESC_code(color, backgroundColored, bold), content, DEFAULT_COLOR);
	}

	void printlnColored(const char* content, TerminalColor color, bool backgroundColored, bool bold) {
		println(ESC_code(color, backgroundColored, bold), content, DEFAULT_COLOR);
	}
	
	void error(const char* file, const char* func, unsigned int line, const char* detail, bool terminate) {
		printColored("ERROR", TerminalColor::Red, false, false);
		printErrMsg(file, func, line, detail);
		if (terminate || errorAlwaysFatal) {
			errorOccured = true;
			printlnColored("Terminating...", TerminalColor::Bright_Black, false, false);
		}
	}

	void warning(const char* file, const char* func, unsigned int line, const char* detail) {
		if (treatWarningAsError)
			error(file, func, line, detail, false);
		else {
			printColored("WARNING", TerminalColor::Yellow, false, false);
			printErrMsg(file, func, line, detail);
		}
	}

	void note(const char* file, const char* func, unsigned int line, const char* detail) {
		printColored("NOTE", TerminalColor::White, false, false);
		printErrMsg(file, func, line, detail);
	}

}
