#include "RE_Internal Header.hpp"
#include "RE_Console.hpp"
#include "RE_Main.hpp"
#include "RE_Window_Win64.hpp"

#include <time.h>

namespace RE {

#define DEFAULT_COLOR "\033[0m"

#define PRINT_COLORS 0
#define TREAT_WARNING_AS_ERROR 1
#define ERRORS_ALWAYS_FATAL 2

	uint32_t u32ErrorCount = 0, u32VulkanErrorCount = 0, u32WarningCount = 0, u32VulkanWarningCount = 0;
	uint8_t u8ConsoleSettings = 1 << PRINT_COLORS;

	static void print_time() {
		std::time_t currentTime = std::time(0);
		print("[", std::put_time(std::gmtime(&currentTime), "%d.%b %Y, %H:%M:%S"), "] ");
	}

	[[nodiscard]]
	static std::string escape_code_to_string(const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		if (!are_bits_true<uint8_t>(u8ConsoleSettings, PRINT_COLORS))
			return "";
		uint32_t u32Id = static_cast<uint32_t>(eColor);
		if (u32Id >= static_cast<uint32_t>(RE_TERMINAL_COLOR_BRIGHT_BLACK))
			u32Id += 90 - static_cast<uint32_t>(RE_TERMINAL_COLOR_BRIGHT_BLACK);
		else
			u32Id += 30;
		if (bBackgroundColored)
			u32Id += 10;
		std::string result("\033[");
		if (bBold)
			result.append("1;");
		result.append(std::format("{}", u32Id));
		result.append("m");
		return result;
	}

	static void print_error_msg(const std::string &rsDetail) {
		size_t lineBreak = rsDetail.find("\n");
		println(" : ", rsDetail.substr(0, lineBreak));
		while (lineBreak != std::string::npos) {
			lineBreak++;
			const size_t nextLineBreak = rsDetail.find("\n", lineBreak);
			println("                                  ", rsDetail.substr(lineBreak, nextLineBreak));
			lineBreak = nextLineBreak;
		}
	}

	void print_error_count() {
		println("Error Count:");
		println("===============================");
		println("\tErrors:          ", u32ErrorCount);
		println("\tWarnings:        ", u32WarningCount);
		println("\tVulkan Errors:   ", u32VulkanErrorCount);
		println("\tVulkan Warnings: ", u32VulkanWarningCount);
	}

	void print_colored(const std::string &rsContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		print(escape_code_to_string(eColor, bBackgroundColored, bBold), rsContent, DEFAULT_COLOR);
	}

	void println_colored(const std::string &rsContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		println(escape_code_to_string(eColor, bBackgroundColored, bBold), rsContent, DEFAULT_COLOR);
	}
	
	void error(const std::string &rsDetail, const bool bTerminate) {
		print_time();
		print_colored("ERROR", RE_TERMINAL_COLOR_RED, false, false);
		print("  ");
		print_error_msg(rsDetail);
		const bool bFatal = bTerminate || are_bits_true<uint8_t>(u8ConsoleSettings, ERRORS_ALWAYS_FATAL);
		if (bFatal) {
			bErrorOccured = true;
			println_colored("Terminating...", RE_TERMINAL_COLOR_BRIGHT_BLACK, false, false);
		}
		u32ErrorCount++;
	}

	void warning(const std::string &rsDetail) {
		if (are_bits_true<uint8_t>(u8ConsoleSettings, TREAT_WARNING_AS_ERROR))
			error(rsDetail, false);
		else {
			print_time();
			print_colored("WARNING", RE_TERMINAL_COLOR_YELLOW, false, false);
			print_error_msg(rsDetail);
			u32WarningCount++;
		}
	}

	void note(const std::string &rsDetail) {
		print_time();
		print_colored("NOTE", RE_TERMINAL_COLOR_WHITE, false, false);
		print("   ");
		print_error_msg(rsDetail);
	}

	void enable_colorful_printing(const bool bEnable) {
		set_bits<uint8_t>(u8ConsoleSettings, bEnable, PRINT_COLORS);
	}

	[[nodiscard]]
	bool is_colorful_printing_enabled() {
		return are_bits_true<uint8_t>(u8ConsoleSettings, PRINT_COLORS);
	}

	void treat_warnings_as_errors(const bool bEnable) {
		set_bits<uint8_t>(u8ConsoleSettings, bEnable, TREAT_WARNING_AS_ERROR);
	}

	[[nodiscard]]
	bool are_warnings_always_treated_as_errors() {
		return are_bits_true<uint8_t>(u8ConsoleSettings, TREAT_WARNING_AS_ERROR);
	}

	void make_errors_always_fatal(const bool bEnable) {
		set_bits<uint8_t>(u8ConsoleSettings, bEnable, ERRORS_ALWAYS_FATAL);
	}

	[[nodiscard]]
	bool are_errors_always_made_fatal() {
		return are_bits_true<uint8_t>(u8ConsoleSettings, ERRORS_ALWAYS_FATAL);
	}

}
