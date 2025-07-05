#ifndef __RE_CONSOLE_H__
#define __RE_CONSOLE_H__

#include "RE_Internal Header.hpp"

namespace RE {

	void print_colored(const char* pcContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold);
	void println_colored(const char* pcContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold);
	
	void error(const char* pcFile, const char* pcFunc, const uint32_t u32Line, const char* pcDetail, const bool bTerminate);
	void warning(const char* pcFile, const char* pcFunc, const uint32_t u32Line, const char* pcDetail);
	void note(const char* pcFile, const char* pcFunc, const uint32_t u32Line, const char* pcDetail);
#define RE_FATAL_ERROR(T) error(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str(), true)
#define RE_ERROR(T) error(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str(), false)
#define RE_WARNING(T) warning(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str())
#define RE_NOTE(T) note(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(T)).c_str())

	void enable_colorful_printing(const bool bEnable);
	void treat_warnings_as_errors(const bool bEnable);
	void make_errors_always_fatal(const bool bEnable);
	void show_message_box_on_error(const bool bEnable);
	void enable_verbosity(const bool bEnable);
	bool is_verbose_behaviour_enabled();

}

#endif /* __RE_CONSOLE_H__ */
