#ifndef __RE_CONSOLE_H__
#define __RE_CONSOLE_H__

#include "RE_Ext Header.hpp"

namespace RE {

	extern bool bPrintColors;
	extern bool bTreatWarningAsError;
	extern bool bErrorAlwaysFatal;

	void printColored(const char* pContent, TerminalColor eColor, bool bBackgroundColored, bool bBold);
	void printlnColored(const char* pContent, TerminalColor eColor, bool bBackgroundColored, bool bBold);
	
	void error(const char* pFile, const char* pFunc, unsigned int pLine, const char* pDetail, bool bTerminate);
	void warning(const char* pFile, const char* pFunc, unsigned int pLine, const char* pDetail);
	void note(const char* pFile, const char* pFunc, unsigned int pLine, const char* pDetail);
#define RE_FATAL_ERROR(T) error(__FILE__, __func__, __LINE__, appendStrings("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str(), true)
#define RE_ERROR(T) error(__FILE__, __func__, __LINE__, appendStrings("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str(), false)
#define RE_WARNING(T) warning(__FILE__, __func__, __LINE__, appendStrings("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str())
#define RE_NOTE(T) note(__FILE__, __func__, __LINE__, appendStrings("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str())

}

#endif /* __RE_CONSOLE_H__ */
