#ifndef __RE_CONSOLE_H__
#define __RE_CONSOLE_H__

#include "RE.hpp"

namespace RE {
	
#define RE_FATAL_ERROR(...) error(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)).c_str(), true)
#define RE_ERROR(...) error(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)).c_str(), false)
#define RE_WARNING(...) warning(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)).c_str())
#define RE_NOTE(...) note(__FILE__, __func__, __LINE__, append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)).c_str())

}

#endif /* __RE_CONSOLE_H__ */
