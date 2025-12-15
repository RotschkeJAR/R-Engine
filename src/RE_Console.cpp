#include "RE_Internal Header.hpp"
#include "RE_Console.hpp"
#include "RE_Main.hpp"

#include <time.h>
#include <csignal>

namespace RE {

#define DEFAULT_COLOR "\033[0m"

	static void print_time() {
		std::time_t currentTime = std::time(0);
		print("[", std::put_time(std::gmtime(&currentTime), "%d.%b %Y, %H:%M:%S"), "] ");
	}

	[[nodiscard]]
	static std::string escape_code_to_string(const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
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
			println("                                      ", rsDetail.substr(lineBreak, nextLineBreak));
			lineBreak = nextLineBreak;
		}
	}

	void print_colored(const std::string &rsContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		if (are_bits_true<uint8_t>(u8ConsoleSettings, PRINT_COLORS))
			print(escape_code_to_string(eColor, bBackgroundColored, bBold), rsContent, DEFAULT_COLOR);
		else
			print(rsContent);
	}

	void println_colored(const std::string &rsContent, const TerminalColor eColor, const bool bBackgroundColored, const bool bBold) {
		if (are_bits_true<uint8_t>(u8ConsoleSettings, PRINT_COLORS))
			println(escape_code_to_string(eColor, bBackgroundColored, bBold), rsContent, DEFAULT_COLOR);
		else
			println(rsContent);
	}

	void abort(const std::string &rsDetail) {
		print_time();
		print_colored("ABORTION", RE_TERMINAL_COLOR_BRIGHT_BLACK, false, false);
		print("   ");
		print_error_msg(rsDetail);
		std::signal(SIGABRT, SIG_DFL);
		std::abort();
	}

	void fatal_error(const Context hContext, const std::string &rsDetail) {
		print_time();
		print_colored("FATAL ERROR", RE_TERMINAL_COLOR_RED, false, false);
		print_error_msg(rsDetail);
		GET_CONTEXT(hContext)->bErrorOccured = true;
	}
	
	void error(const std::string &rsDetail) {
		if (are_bits_true<uint8_t>(u8ConsoleSettings, ERRORS_ALWAYS_FATAL)) {
			fatal_error(rsDetail);
			return;
		}
		print_time();
		print_colored("ERROR", RE_TERMINAL_COLOR_BRIGHT_RED, false, false);
		print("      ");
		print_error_msg(rsDetail);
	}

	void warning(const std::string &rsDetail) {
		if (are_bits_true<uint8_t>(u8ConsoleSettings, TREAT_WARNING_AS_ERROR)) {
			error(rsDetail);
			return;
		}
		print_time();
		print_colored("WARNING", RE_TERMINAL_COLOR_YELLOW, false, false);
		print("    ");
		print_error_msg(rsDetail);
	}

	void note(const std::string &rsDetail) {
		print_time();
		print_colored("NOTE", RE_TERMINAL_COLOR_WHITE, false, false);
		print("       ");
		print_error_msg(rsDetail);
	}

	void enable_colorful_printing(const Context hContext, const bool bEnable) {
		set_bits<uint8_t>(GET_CONTEXT(hContext)->u8ConsoleSettings, bEnable, PRINT_COLORS);
	}

	[[nodiscard]]
	bool is_colorful_printing_enabled(const Context hContext) {
		return are_bits_true<uint8_t>(GET_CONTEXT(hContext)->u8ConsoleSettings, PRINT_COLORS);
	}

	void treat_warnings_as_errors(const Context hContext, const bool bEnable) {
		set_bits<uint8_t>(GET_CONTEXT(hContext)->u8ConsoleSettings, bEnable, TREAT_WARNING_AS_ERROR);
	}

	[[nodiscard]]
	bool are_warnings_always_treated_as_errors(const Context hContext) {
		return are_bits_true<uint8_t>(GET_CONTEXT(hContext)->u8ConsoleSettings, TREAT_WARNING_AS_ERROR);
	}

	void make_errors_always_fatal(const Context hContext, const bool bEnable) {
		set_bits<uint8_t>(GET_CONTEXT(hContext)->u8ConsoleSettings, bEnable, ERRORS_ALWAYS_FATAL);
	}

	[[nodiscard]]
	bool are_errors_always_fatal(const Context hContext) {
		return are_bits_true<uint8_t>(GET_CONTEXT(hContext)->u8ConsoleSettings, ERRORS_ALWAYS_FATAL);
	}

}
