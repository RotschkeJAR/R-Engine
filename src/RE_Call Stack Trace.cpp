#include "RE_Internal Header.hpp"

namespace RE {
	
	struct AppLocation final {
		const char *pacFile, *pacFunc, *pacDetails;
		uint32_t u32Line;

		AppLocation(const char *const pacFile, const char *const pacFunc, const uint32_t u32Line, const char *const pacDetails) : pacFile(pacFile), pacFunc(pacFunc), pacDetails(pacDetails), u32Line(u32Line) {}
		~AppLocation() {}
	};

	std::deque<AppLocation> callStackTrace;

	void add_to_call_stack_trace(const char *const pacFile, const char *const pacFunc, const uint32_t u32Line, const char *const pacDetails) {
		callStackTrace.emplace_back(pacFile, pacFunc, u32Line, pacDetails);
	}

	void remove_from_call_stack_trace() {
		callStackTrace.pop_back();
	}

	void print_call_stack_trace() {
		std::for_each(callStackTrace.rbegin(), callStackTrace.rend(), [](const AppLocation &appLocation) {
			print("\tin file ");
			print_colored(appLocation.pacFile, RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
			print(", in function ");
			print_colored(appLocation.pacFunc, RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
			print(", at line ");
			print_colored(append_to_string(appLocation.u32Line).c_str(), RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
			if (std::strcmp(appLocation.pacDetails, "\0") != 0) {
				print(": ");
				println_colored(appLocation.pacDetails, RE_TERMINAL_COLOR_BRIGHT_WHITE, false, false);
			} else
				println();
		});
	}

}
