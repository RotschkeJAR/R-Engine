#ifndef __RE_CONSOLE_H__
#define __RE_CONSOLE_H__

namespace RE {

	extern bool printColors;
	extern bool treatWarningAsError;
	extern bool errorAlwaysFatal;

	void printColored(const char* content, TerminalColor color, bool backgroundColored, bool bold);
	void printlnColored(const char* content, TerminalColor color, bool backgroundColored, bool bold);
	
	void error(const char* file, const char* func, unsigned int line, const char* detail, bool terminate);
	void warning(const char* file, const char* func, unsigned int line, const char* detail);
	void note(const char* file, const char* func, unsigned int line, const char* detail);
#define RE_FATAL_ERROR(T) error(__FILE__, __func__, __LINE__, appendStringsA("[R-Engine] ", STRIP_QUOTE(T)).c_str(), true)
#define RE_ERROR(T) error(__FILE__, __func__, __LINE__, appendStringsA("[R-Engine] ", STRIP_QUOTE(T)).c_str(), false)
#define RE_WARNING(T) warning(__FILE__, __func__, __LINE__, appendStringsA("[R-Engine] ", STRIP_QUOTE(T)).c_str())
#define RE_NOTE(T) note(__FILE__, __func__, __LINE__, appendStringsA("[R-Engine] ", STRIP_QUOTE(T)).c_str())

}

#endif /* __RE_CONSOLE_H__ */
