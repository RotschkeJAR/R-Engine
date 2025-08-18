#include "RE_Internal Header.hpp"

#include <csignal>

namespace RE {

	SignalCatcher* pInstance = nullptr;
	bool bSignalAlreadyCaught = false;

	void handle_signal(const int32_t i32SignalId) {
		if (bSignalAlreadyCaught) {
			println_colored("\nThe signal handler has been called again. Killing itself...\nIf this application doesn't terminate, you have to kill it manually", RE_TERMINAL_COLOR_RED, true, false);
#ifdef RE_OS_WINDOWS
			TerminateProcess(GetCurrentProcess(), static_cast<UINT>(i32SignalId));
#elif defined RE_OS_LINUX
			std::raise(SIGKILL);
#endif
			return;
		}
		bSignalAlreadyCaught = true;
		switch (i32SignalId) {
			case SIGSEGV:
				println("Segmentation violation (Invalid access to memory, e.g. dangling pointer, wild pointer, buffer overflow, array index out of bounds, etc.)");
				break;
			case SIGILL:
				println("Illegal Instruction (The instruction was either corrupted or a non-instruction was used for execution)");
				break;
			case SIGABRT:
				println("Abortion (The program has abnormally exited by calling \"abort\", which most likely happens for errors)");
				break;
			case SIGFPE:
				println("Floating-point exception (An arithmetic operation failed, e.g. division by zero, or a number overflowed)");
				break;
			case SIGTERM:
				println("Program terminated");
				std::exit(SIGTERM);
			case SIGINT:
				println("Extern interruption");
				std::exit(SIGINT);
			default:
				println("Unknown signal received");
				break;
		}
		print_call_stack_trace();
		std::exit(i32SignalId);
	}

	SignalCatcher::SignalCatcher() {
		if (pInstance)
			return;
		pInstance = this;
		std::signal(SIGSEGV, handle_signal);
		std::signal(SIGILL, handle_signal);
		std::signal(SIGABRT, handle_signal);
		std::signal(SIGFPE, handle_signal);
		std::signal(SIGTERM, handle_signal);
		std::signal(SIGINT, handle_signal);
	}

	SignalCatcher::~SignalCatcher() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
		std::signal(SIGSEGV, SIG_DFL);
		std::signal(SIGILL, SIG_DFL);
		std::signal(SIGABRT, SIG_DFL);
		std::signal(SIGFPE, SIG_DFL);
		std::signal(SIGTERM, SIG_DFL);
		std::signal(SIGINT, SIG_DFL);
	}

}
