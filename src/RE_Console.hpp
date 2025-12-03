#ifndef __RE_CONSOLE_H__
#define __RE_CONSOLE_H__

#include "RE.hpp"

namespace RE {

	extern uint32_t u32VulkanErrorCount, u32VulkanWarningCount;

	void print_error_count();
	
#define RE_FATAL_ERROR(...) error(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__), " (in ", __FILE__, ", in function ", __func__, " at line ", __LINE__, ")"), true)
#define RE_ERROR(...) error(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__), " (in ", __FILE__, ", in function ", __func__, " at line ", __LINE__, ")"), false)
#define RE_WARNING(...) warning(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__), " (in ", __FILE__, ", in function ", __func__, " at line ", __LINE__, ")"))
#define RE_NOTE(...) note(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__), " (in ", __FILE__, ", in function ", __func__, " at line ", __LINE__, ")"))

}

#endif /* __RE_CONSOLE_H__ */
