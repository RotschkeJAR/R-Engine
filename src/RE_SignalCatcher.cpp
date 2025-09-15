#include "RE_Internal Header.hpp"

#include <csignal>

namespace RE {

	/**
	 * unused:
	 * 		SIGTERM
	 * 		SIGINT
	 */

	void handle_abort(const int32_t i32SignalId) {
		print("\033[0m\n\033[41mABORTION\nIf the application doesn't terminate, the user has to explicitly kill the process.\n\033[0m");
		std::signal(SIGABRT, SIG_DFL);
		std::raise(SIGABRT);
	}

	void handle_signal(const int32_t i32SignalId) {
		println("Signal code received: ", i32SignalId);
		switch (i32SignalId) {
			[[likely]] case SIGSEGV:
				println("Segmentation violation (Invalid access to memory, e.g. dangling pointer, wild pointer, buffer overflow, array index out of bounds, etc.)");
				break;
			case SIGILL:
				println("Illegal Instruction (The instruction was either corrupted or a non-instruction was used for execution)");
				break;
			case SIGFPE:
				println("Floating-point exception (An arithmetic operation failed, e.g. division by zero, or a number overflowed)");
				break;
			default:
				return;
		}
		print_call_stack_trace();
		std::abort();
	}

	void set_signal_handlers() {
		if (std::signal(SIGSEGV, handle_signal) == SIG_ERR)
			RE_ERROR("Failed to set signal handler for SIGSEGV (segmentation violation)");
		if (std::signal(SIGILL, handle_signal) == SIG_ERR)
			RE_ERROR("Failed to set signal handler for SIGILL (illegal instruction)");
		if (std::signal(SIGFPE, handle_signal) == SIG_ERR)
			RE_ERROR("Failed to set signal handler for SIGFPE (floating-point exception)");
		if (std::signal(SIGABRT, handle_abort) == SIG_ERR)
			RE_ERROR("Failed to set signal handler for SIGABRT (abortion)");
	}

}
