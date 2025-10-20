#include "RE_Internal Header.hpp"

namespace RE {

	[[nodiscard]]
	bool is_string_empty(const char *const pacString) {
		return !pacString || pacString[0] == '\0';
	}

	[[nodiscard]]
	size_t get_string_length_safely(const char *const pacString) {
		return pacString ? std::strlen(pacString) : 0;
	}

	[[nodiscard]]
	size_t get_wide_string_length_safely(const wchar_t *const pacWideString) {
		return pacWideString ? std::wcslen(pacWideString) : 0;
	}

	[[nodiscard]]
	bool are_string_contents_equal(const char *const pacString1, const char *const pacString2) {
		return pacString1 && pacString2 && std::string_view(pacString1) == std::string_view(pacString2);
	}

	[[nodiscard]]
	size_t get_line_count(const char *const pacString) {
		const size_t length = get_string_length_safely(pacString);
		if (!length)
			return 0;
		PRINT_DEBUG("Counting lines in C-style string");
		size_t lineCount = 1;
		for (size_t characterIndex = 0; characterIndex < length; characterIndex++)
			if (pacString[characterIndex] == '\n')
				lineCount++;
		return lineCount;
	}

	[[nodiscard]]
	std::string get_line(const char *const pacString, const size_t lineIndex) {
		const size_t length = get_string_length_safely(pacString);
		if (!length)
			return "";
		PRINT_DEBUG("Trying to read line ", lineIndex, " from C-style string");
		std::stringstream ss;
		size_t currentLineIndex = 0;
		for (size_t currentCharIndex = 0; currentCharIndex < length && currentLineIndex <= lineIndex; currentCharIndex++)
			if (pacString[currentCharIndex] == '\n')
				currentLineIndex++;
			else if (currentLineIndex == lineIndex)
				ss << pacString[currentCharIndex];
		return ss.str();
	}
	
	[[nodiscard]]
	std::string convert_wide_chars_to_utf8(const wchar_t *const pawcString) {
		const size_t length = get_wide_string_length_safely(pawcString) * 4;
		if (!length)
			return "";
		PRINT_DEBUG("Converting wide character-string to UTF-8-string");
		std::string sResult(length, '\0');
		std::wcstombs(sResult.data(), pawcString, length);
		return sResult;
	}

	[[nodiscard]]
	std::wstring convert_chars_to_wide(const char *const pacString) {
		const size_t length = get_string_length_safely(pacString) * 4;
		if (!length)
			return L"";
		PRINT_DEBUG("Converting UTF-8-string to wide character-string");
		std::wstring wsResult(length, L'\0');
		std::mbstowcs(wsResult.data(), pacString, length);
		return wsResult;
	}

	[[nodiscard]]
	std::string get_app_name() {
		PRINT_DEBUG("Querying application name of this executable");
		std::string sAppName;
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
