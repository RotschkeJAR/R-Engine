#ifndef __RE_CONSOLE_H__
#define __RE_CONSOLE_H__

#include "RE.hpp"

namespace RE {
	
#define RE_FATAL_ERROR(...) ([&]() { \
			add_to_call_stack_trace(__FILE__, __func__, __LINE__, "\0"); \
			error(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)).c_str(), true); \
			remove_from_call_stack_trace(); \
		} ())
#define RE_ERROR(...) ([&]() { \
			add_to_call_stack_trace(__FILE__, __func__, __LINE__, "\0"); \
			error(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)).c_str(), false); \
			remove_from_call_stack_trace(); \
		} ())
#define RE_WARNING(...) ([&]() { \
			add_to_call_stack_trace(__FILE__, __func__, __LINE__, "\0"); \
			warning(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)).c_str()); \
			remove_from_call_stack_trace(); \
		} ())
#define RE_NOTE(...) ([&]() { \
			add_to_call_stack_trace(__FILE__, __func__, __LINE__, "\0"); \
			note(append_to_string("[R-Engine] ", STRIP_QUOTE_MACRO(__VA_ARGS__)).c_str()); \
			remove_from_call_stack_trace(); \
		} ())

}

#endif /* __RE_CONSOLE_H__ */
