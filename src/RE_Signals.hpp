#ifndef __RE_SIGNALS_H__
#define __RE_SIGNALS_H__

#include "RE_Ext Header.hpp"

namespace RE {
	
	class SignalCatcher {
		public:
			static SignalCatcher* pInstance;

			SignalCatcher();
			~SignalCatcher();
	};

	void addToStackTrace(const char* pcFile, const char* pcMethod, REuint u32Line, const char* pcDetails);
	void removeFromStackTrace();

}

#endif /* __RE_SIGNALS_H__ */
