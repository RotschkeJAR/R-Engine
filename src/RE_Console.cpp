#include "RE_Internal.hpp"
#include "RE_Console.hpp"
#include "RE_Main.hpp"

#include <time.h>
#include <csignal>

namespace RE {

#define DEFAULT_COLOR "\033[0m"

	typedef unsigned ConsoleSettingsFlags_t;
	enum ConsoleSettings : ConsoleSettingsFlags_t {
		CONSOLE_SETTINGS_PRINT_COLORS = 0x1,
		CONSOLE_SETTINGS_TREAT_WARNING_AS_ERROR = 0x2,
		CONSOLE_SETTINGS_ERRORS_ALWAYS_FATAL = 0x4
	};

#ifndef NDEBUG
	static unsigned long ulErrorCount = 0,
		ulWarningCount = 0;
	unsigned long u32VulkanErrorCount = 0,
		u32VulkanWarningCount = 0;
#endif
	ConsoleSettingsFlags_t mConsoleSettings = CONSOLE_SETTINGS_PRINT_COLORS;

	static void print_time() {
		std::time_t currentTime = std::time(0);
		print("[", std::put_time(std::gmtime(&currentTime), "%d.%b %Y, %H:%M:%S"), "] ");
	}

	[[nodiscard]]
	static std::string escape_code_to_string(const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		unsigned uId = static_cast<unsigned>(eColor);
		if (uId >= static_cast<unsigned>(RE_TERMINAL_COLOR_BRIGHT_BLACK))
			uId += 90U - static_cast<unsigned>(RE_TERMINAL_COLOR_BRIGHT_BLACK);
		else
			uId += 30U;
		if (bBackgroundColored)
			uId += 10U;
		std::string result("\033[");
		if (bBold)
			result.append("1;");
		result.append(std::format("{}", uId));
		result.append("m");
		return result;
	}

	static void print_error_msg(const std::string &rsDetail) {
		size_t lineBreak = rsDetail.find("\n");
		println(" : ", rsDetail.substr(0, lineBreak));
		while (lineBreak != std::string::npos) {
			lineBreak++;
			const size_t nextLineBreak = rsDetail.find("\n", lineBreak);
			println("                                      ", rsDetail.substr(lineBreak, nextLineBreak != std::string::npos ? (nextLineBreak - lineBreak) : std::string::npos));
			lineBreak = nextLineBreak;
		}
	}

	void print_error_count() {
	#ifndef NDEBUG
		println("Error counter:");
		println("=============================");
		println("Errors:             ", ulErrorCount);
		println("Warnings:           ", ulWarningCount);
		println("Vulkan errors:      ", ulVulkanErrorCount);
		println("Vulkan warnings:    ", ulVulkanWarningCount);
	#endif
	}

	void print_colored(const std::string &rsContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		if ((mConsoleSettings & CONSOLE_SETTINGS_PRINT_COLORS))
			print(escape_code_to_string(eColor, bBackgroundColored, bBold), rsContent, DEFAULT_COLOR);
		else
			print(rsContent);
	}

	void println_colored(const std::string &rsContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		if ((mConsoleSettings & CONSOLE_SETTINGS_PRINT_COLORS))
			println(escape_code_to_string(eColor, bBackgroundColored, bBold), rsContent, DEFAULT_COLOR);
		else
			println(rsContent);
	}

	[[noreturn]]
	void abort(const std::string &rsDetail) {
		print_time();
		print_colored("ABORTION", RE_TERMINAL_COLOR_BRIGHT_BLACK, false, false);
		print("   ");
		print_error_msg(rsDetail);
		print_error_count();
		std::abort();
	}

	void fatal_error(const std::string &rsDetail) {
		print_time();
		print_colored("FATAL ERROR", RE_TERMINAL_COLOR_RED, false, false);
		print_error_msg(rsDetail);
		bErrorOccured = true;
	#ifndef NDEBUG
		ulErrorCount++;
	#endif
	}
	
	void error(const std::string &rsDetail) {
		if ((mConsoleSettings & CONSOLE_SETTINGS_ERRORS_ALWAYS_FATAL)) {
			fatal_error(rsDetail);
			return;
		}
		print_time();
		print_colored("ERROR", RE_TERMINAL_COLOR_BRIGHT_RED, false, false);
		print("      ");
		print_error_msg(rsDetail);
	#ifndef NDEBUG
		ulErrorCount++;
	#endif
	}

	void warning(const std::string &rsDetail) {
		if ((mConsoleSettings & CONSOLE_SETTINGS_TREAT_WARNING_AS_ERROR)) {
			error(rsDetail);
			return;
		}
		print_time();
		print_colored("WARNING", RE_TERMINAL_COLOR_YELLOW, false, false);
		print("    ");
		print_error_msg(rsDetail);
	#ifndef NDEBUG
		ulWarningCount++;
	#endif
	}

	void note(const std::string &rsDetail) {
		print_time();
		print_colored("NOTE", RE_TERMINAL_COLOR_WHITE, false, false);
		print("       ");
		print_error_msg(rsDetail);
	}

	void enable_colorful_printing(const bool bEnable) {
		if (bEnable)
			mConsoleSettings |= CONSOLE_SETTINGS_PRINT_COLORS;
		else
			mConsoleSettings &= ~CONSOLE_SETTINGS_PRINT_COLORS;
	}

	[[nodiscard]]
	bool is_colorful_printing_enabled() {
		return (mConsoleSettings & CONSOLE_SETTINGS_PRINT_COLORS);
	}

	void treat_warnings_as_errors(const bool bEnable) {
		if (bEnable)
			mConsoleSettings |= CONSOLE_SETTINGS_TREAT_WARNING_AS_ERROR;
		else
			mConsoleSettings &= ~CONSOLE_SETTINGS_TREAT_WARNING_AS_ERROR;
	}

	[[nodiscard]]
	bool are_warnings_always_treated_as_errors() {
		return (mConsoleSettings & CONSOLE_SETTINGS_TREAT_WARNING_AS_ERROR);
	}

	void make_errors_always_fatal(const bool bEnable) {
		if (bEnable)
			mConsoleSettings |= CONSOLE_SETTINGS_ERRORS_ALWAYS_FATAL;
		else
			mConsoleSettings &= ~CONSOLE_SETTINGS_ERRORS_ALWAYS_FATAL;
	}

	[[nodiscard]]
	bool are_errors_always_fatal() {
		return (mConsoleSettings & CONSOLE_SETTINGS_ERRORS_ALWAYS_FATAL);
	}

}
