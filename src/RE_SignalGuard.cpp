#include "RE.hpp"

namespace RE {
	
	SignalGuard::SignalGuard(const char* pcFile, const char* pcFunc, REuint u32Line, const char* pcDetails) {
		addToStackTrace(pcFile, pcFunc, u32Line, pcDetails);
	}

	SignalGuard::~SignalGuard() {
		removeFromStackTrace();
	}

}