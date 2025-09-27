#ifndef __RE_CONSOLE_H__
#define __RE_CONSOLE_H__

#include "RE.hpp"

namespace RE {
	
#define RE_FATAL_ERROR(...) error(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)), true)
#define RE_ERROR(...) error(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)), false)
#define RE_WARNING(...) warning(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)))
#define RE_NOTE(...) note(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)))

}

#endif /* __RE_CONSOLE_H__ */
