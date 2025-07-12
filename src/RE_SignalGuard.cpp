#include "RE.hpp"

namespace RE {
	
	SignalGuard::SignalGuard(const char *const pcFile, const char *const pcFunc, uint32_t u32Line, const char *const pcDetails) {
		add_to_stack_trace(pcFile, pcFunc, u32Line, pcDetails);
	}

	SignalGuard::~SignalGuard() {
		remove_from_stack_trace();
	}

}