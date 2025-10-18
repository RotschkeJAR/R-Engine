#include "RE_Internal Header.hpp"

namespace RE {

	[[nodiscard]]
	bool is_string_empty(const char8_t *const pacString) {
		return !pacString || pacString[0] == u8'\0';
	}

	[[nodiscard]]
	size_t get_string_length_safely(const char8_t *const pacString) {
		return pacString ? std::strlen(pacString) : 0;
	}

	[[nodiscard]]
	size_t get_wide_string_length_safely(const wchar_t *const pacWideString) {
		return pacWideString ? std::wcslen(pacWideString) : 0;
	}

	[[nodiscard]]
	bool are_string_contents_equal(const char8_t *const pacString1, const char8_t *const pacString2) {
		return pacString1 && pacString2 && std::u8string_view(pacString1) == std::u8string_view(pacString2);
	}

	[[nodiscard]]
	size_t get_line_count(const char8_t *const pacString) {
		PRINT_DEBUG("Counting lines in C-style string");
		size_t lineCount = 1;
		const size_t stringLength = get_string_length_safely(pacString);
		for (size_t characterIndex = 0; characterIndex < stringLength; characterIndex++)
			if (pacString[characterIndex] == u8'\n')
				lineCount++;
		return lineCount;
	}

	[[nodiscard]]
	std::u8string get_line(const char8_t *const pacString, const size_t lineIndex) {
		PRINT_DEBUG("Trying to read line ", lineIndex, " from C-style string");
		std::basic_stringstream<char8_t> ss;
		size_t currentLineIndex = 0;
		const size_t stringLength = get_string_length_safely(pacString);
		for (size_t currentCharIndex = 0; currentCharIndex < stringLength && currentLineIndex <= lineIndex; currentCharIndex++)
			if (pacString[currentCharIndex] == u8'\n')
				currentLineIndex++;
			else if (currentLineIndex == lineIndex)
				ss << pacString[currentCharIndex];
		return ss.str();
	}
	
	[[nodiscard]]
	std::u8string convert_wide_chars_to_utf8(const wchar_t *const pawcString) {
		if (!pawcString)
			return u8"";
		PRINT_DEBUG("Converting wide character-string to UTF-8-string");
		return std::format(u8"{}", pawcString);
	}

	[[nodiscard]]
	std::wstring convert_chars_to_wide(const char8_t *const pacString) {
		if (!pacString)
			return L"";
		PRINT_DEBUG("Converting UTF-8-string to wide character-string");
		return std::format(L"{}", pacString);
	}

	[[nodiscard]]
	std::u8string get_app_name() {
		PRINT_DEBUG("Querying application name of this executable");
		std::u8string sAppName;
#ifdef RE_OS_WINDOWS
# define PATH_SIZE 500
		wchar_t awcBuffer[PATH_SIZE];
		if (GetModuleFileNameW(nullptr, awcBuffer, PATH_SIZE))
			sAppName = convert_wide_chars_to_utf8(awcBuffer);
		else
			RE_ERROR("Failed retrieving the file name of the application on Windows");
# undef PATH_SIZE
#elif defined RE_OS_LINUX
		std::ifstream fileStream("/proc/self/comm");
		if (fileStream.is_open())
			fileStream >> sAppName;
		else
			RE_ERROR("Failed opening \"/proc/self/comm\" file for retrieving file name of the application on Linux");
#else
		RE_ERROR("Unknown platform for retrieving file name of the application");
#endif
		return sAppName;
	}

}
