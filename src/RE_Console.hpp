#ifndef __RE_CONSOLE_H__
#define __RE_CONSOLE_H__

#include "RE_Internal Header.hpp"

namespace RE {

	extern bool bPrintColors;
	extern bool bTreatWarningAsError;
	extern bool bErrorAlwaysFatal;

	void print_colored(const char* pcContent, TerminalColor eColor, bool bBackgroundColored, bool bBold);
	void println_colored(const char* pcContent, TerminalColor eColor, bool bBackgroundColored, bool bBold);
	
	void error(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetail, bool bTerminate);
	void warning(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetail);
	void note(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetail);
#define RE_FATAL_ERROR(T) error(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str(), true)
#define RE_ERROR(T) error(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str(), false)
#define RE_WARNING(T) warning(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str())
#define RE_NOTE(T) note(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str())

}

#endif /* __RE_CONSOLE_H__ */
