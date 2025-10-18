#include "RE_Internal Header.hpp"

#include <csignal>

namespace RE {

	/**
	 * unused:
	 * 		SIGTERM
	 * 		SIGINT
	 */

	void handle_abort(const int32_t i32SignalId) {
		print("\nABORTION\nIf the application doesn't terminate, the user has to explicitly kill the process.\n");
		if (std::signal(SIGABRT, SIG_DFL) == SIG_ERR)
			println("Failed reassigning the signal handler for SIGABRT (abortion). This might lead to infinite recursion");
		std::abort();
	}

	void handle_signal(const int32_t i32SignalId) {
		println("Signal code received: ", i32SignalId);
		switch (i32SignalId) {
			[[likely]] case SIGSEGV:
				println("Segmentation violation (Invalid access to memory, e.g. dangling pointer, wild pointer, buffer overflow, array index out of bounds, etc.)");
				break;
			case SIGILL:
				println("Illegal Instruction (The CPU-instruction was either corrupted or a non-CPU-instruction was used for execution)");
				break;
			case SIGFPE:
				println("Floating-point exception (An arithmetic operation with floating-point numbers failed, e.g. division by zero, or a number overflowed)");
				break;
			default:
				return;
		}
		std::abort();
	}

	void set_signal_handlers() {
		PRINT_DEBUG("Setting signal handlers");
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
