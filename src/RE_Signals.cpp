#include "RE_Signals.hpp"

#include <csignal>

namespace RE {

	struct AppLocation {
		const char *pcFile, *pcMethod, *pcDetails;
		REuint u32Line;
	};

	SignalCatcher* SignalCatcher::pInstance = nullptr;
	std::stack<AppLocation> stackTrace;
	bool signalAlreadyCaught = false;

	void printStackTrace() {
		while (!stackTrace.empty()) {
			AppLocation locationData = stackTrace.top();
			print("\tin file ");
			printColored(locationData.pcFile, TerminalColor::Bright_White, false, false);
			print(", in function ");
			printColored(locationData.pcMethod, TerminalColor::Bright_White, false, false);
			print(", at line ");
			printColored(std::to_string(locationData.u32Line).c_str(), TerminalColor::Bright_White, false, false);
			if (std::strcmp(locationData.pcDetails, "\0") != 0)
				print(appendStrings(": ", locationData.pcDetails));
			println();
			stackTrace.pop();
		}
	}

	void handleSignal(int signalId) {
		if (signalAlreadyCaught) {
			printlnColored("\nThe signal handler has been called again. Terminating instantly", TerminalColor::Red, true, false);
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
				println("Abortion (The program has abnormally exited by calling \"abort\"");
				break;
			case SIGFPE:
				println("Floating-point exception (An arithmetic operation failed or a number overflowed)");
				break;
			default:
				println("Unknown signal received");
				break;
		}
		printStackTrace();
		std::exit(signalId);
	}

	void addToStackTrace(const char* pcFile, const char* pcMethod, REuint u32Line, const char* pcDetails) {
		AppLocation newTrace = {pcFile, pcMethod, pcDetails, u32Line};
		stackTrace.push(newTrace);
	}

	void removeFromStackTrace() {
		stackTrace.pop();
	}

	SignalCatcher::SignalCatcher() {
		if (pInstance) {
			RE_FATAL_ERROR("An object of the class \"SignalCatcher\" has already been constructed");
			return;
		}
		pInstance = this;
		std::signal(SIGSEGV, handleSignal);
		std::signal(SIGILL, handleSignal);
		std::signal(SIGABRT, handleSignal);
		std::signal(SIGFPE, handleSignal);
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

}
