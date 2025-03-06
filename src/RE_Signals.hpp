#ifndef __RE_SIGNALS_H__
#define __RE_SIGNALS_H__

#include "RE_Internal Header.hpp"

namespace RE {
	
	class SignalCatcher {
		public:
			static SignalCatcher* pInstance;

			SignalCatcher();
			~SignalCatcher();
	};

	void add_to_stack_trace(const char* pcFile, const char* pcMethod, REuint u32Line, const char* pcDetails);
	void remove_from_stack_trace();

}

#endif /* __RE_SIGNALS_H__ */
