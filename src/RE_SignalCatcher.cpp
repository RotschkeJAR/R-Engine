#include "RE_Internal Header.hpp"

#include <csignal>

namespace RE {

	/**
	 * unused:
	 * 		SIGABRT (bad practice and causes infinite recursion)
	 * 		SIGTERM
	 * 		SIGINT
	 */

	static SignalCatcher* pInstance = nullptr;
	static bool bSignalAlreadyCaught = false;

	void handle_signal(const int32_t i32SignalId) {
		if (bSignalAlreadyCaught) {
			println_colored("\nThe signal handler has been called again\nIf this application doesn't terminate, you have to kill it manually in your task manager", 
				RE_TERMINAL_COLOR_RED, true, false);
			std::abort();
		}
		bSignalAlreadyCaught = true;
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
				break;
		}
		println("Signal code received: ", i32SignalId);
		print_call_stack_trace();
		std::abort();
	}

	SignalCatcher::SignalCatcher() {
		if (pInstance) {
			RE_WARNING("A signal catcher has already been constructed. The new instance won't be used");
			return;
		}
		pInstance = this;
		std::signal(SIGSEGV, handle_signal);
		std::signal(SIGILL, handle_signal);
		std::signal(SIGFPE, handle_signal);
	}

	SignalCatcher::~SignalCatcher() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
		std::signal(SIGSEGV, SIG_DFL);
		std::signal(SIGILL, SIG_DFL);
		std::signal(SIGFPE, SIG_DFL);
	}

}
