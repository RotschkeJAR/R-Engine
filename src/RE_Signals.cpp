#include "RE_Internal Header.hpp"

#include <csignal>

namespace RE {

	struct AppLocation {
		const char *pcFile, *pcMethod, *pcDetails;
		REuint u32Line;
	};

	SignalCatcher* pInstance = nullptr;
	std::stack<AppLocation> stackTrace;
	bool signalAlreadyCaught = false;

	void handle_signal(int signalId) {
		if (signalAlreadyCaught) {
			println_colored("\nThe signal handler has been called again. Terminating instantly", RE_TERMINAL_COLOR_RED, true, false);
			std::exit(signalId);
		}
		signalAlreadyCaught = true;
		switch (signalId) {
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
			default:
				println("Unknown signal received");
				break;
		}
		while (!stackTrace.empty()) { // prints stack trace
			AppLocation locationData = stackTrace.top();
			print("\tin file ");
			print_colored(locationData.pcFile, RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
			print(", in function ");
			print_colored(locationData.pcMethod, RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
			print(", at line ");
			print_colored(std::to_string(locationData.u32Line).c_str(), RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
			if (std::strcmp(locationData.pcDetails, "\0") != 0)
				print(append_to_string(": ", locationData.pcDetails));
			println();
			stackTrace.pop();
		}
		std::exit(signalId);
	}

	SignalCatcher::SignalCatcher() {
		if (pInstance)
			return;
		pInstance = this;
		std::signal(SIGSEGV, handle_signal);
		std::signal(SIGILL, handle_signal);
		std::signal(SIGABRT, handle_signal);
		std::signal(SIGFPE, handle_signal);
	}

	SignalCatcher::~SignalCatcher() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
		std::signal(SIGSEGV, SIG_DFL);
		std::signal(SIGILL, SIG_DFL);
		std::signal(SIGABRT, SIG_DFL);
		std::signal(SIGFPE, SIG_DFL);
	}

	void add_to_stack_trace(const char* pcFile, const char* pcMethod, REuint u32Line, const char* pcDetails) {
		AppLocation newTrace = {};
		newTrace.pcFile = pcFile;
		newTrace.pcMethod = pcMethod;
		newTrace.pcDetails = pcDetails;
		newTrace.u32Line = u32Line;
		stackTrace.push(newTrace);
	}

	void remove_from_stack_trace() {
		stackTrace.pop();
	}

}
