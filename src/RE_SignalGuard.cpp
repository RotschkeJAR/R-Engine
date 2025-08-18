#include "RE.hpp"

namespace RE {
	
	SignalGuard::SignalGuard(const char *const pacFile, const char *const pacFunc, uint32_t u32Line, const char *const pacDetails) {
		add_to_call_stack_trace(pacFile, pacFunc, u32Line, pacDetails);
	}

	SignalGuard::~SignalGuard() {
		remove_from_call_stack_trace();
	}

}