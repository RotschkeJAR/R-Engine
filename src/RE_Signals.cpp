#include "RE_Signals.hpp"

#include <csignal>

namespace RE {

	struct AppLocation {
		const char *pcFile, *pcMethod;
		REuint u32Line;
	};

	SignalCatcher* SignalCatcher::pInstance = nullptr;
	std::stack<AppLocation> stackTrace;

	void printStackTrace() {
		while (!stackTrace.empty()) {
			AppLocation locationData = stackTrace.top();
			print("in file ");
			printColored(locationData.pcFile, TerminalColor::Bright_White, false, false);
			print(", in function ");
			printColored(locationData.pcMethod, TerminalColor::Bright_White, false, false);
			print(", at line ");
			std::string strLine = std::to_string(locationData.u32Line);
			printlnColored(strLine.c_str(), TerminalColor::Bright_White, false, false);
			stackTrace.pop();
		}
	}

	void handleSignal(int signalId) {
		switch (signalId) {
			case SIGSEGV:
				println("Segmentation fault (Invalid access to memory, e.g. dangling pointer, wild pointer, buffer overflow, array index out of bounds, etc.)");
				break;
			case SIGBUS:
				println("Bus error (Invalid access to memory)");
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
		exit(signalId);
	}

	void addToStackTrace(const char* pcFile, const char* pcMethod, REuint u32Line) {
		AppLocation newTrace = {pcFile, pcMethod, u32Line};
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
		signal(SIGSEGV, handleSignal);
		signal(SIGBUS, handleSignal);
		signal(SIGILL, handleSignal);
		signal(SIGABRT, handleSignal);
		signal(SIGFPE, handleSignal);
	}

	SignalCatcher::~SignalCatcher() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
		signal(SIGSEGV, SIG_DFL);
		signal(SIGBUS, SIG_DFL);
		signal(SIGILL, SIG_DFL);
		signal(SIGABRT, SIG_DFL);
		signal(SIGFPE, SIG_DFL);
	}

}
