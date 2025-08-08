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
	bool are_string_contents_equal(const char *const pacString1, const char *const pacString2) {
		return pacString1 && pacString2 && std::strcmp(pacString1, pacString2) == 0;
	}

	[[nodiscard]]
	size_t get_line_count(const char *const pacString) {
		size_t lineCount = 1;
		const size_t stringLength = get_string_length_safely(pacString);
		for (size_t characterIndex = 0; characterIndex < stringLength; characterIndex++)
			if (pacString[characterIndex] == '\n')
				lineCount++;
		return lineCount;
	}

	[[nodiscard]]
	std::string get_line(const char *const pacString, const size_t lineIndex) {
		std::stringstream ss("");
		size_t currentLineIndex = 0;
		const size_t stringLength = get_string_length_safely(pacString);
		for (size_t currentCharIndex = 0; currentCharIndex < stringLength; currentCharIndex++)
			if (pacString[currentCharIndex] == '\n')
				currentLineIndex++;
			else if (currentLineIndex == lineIndex)
				ss << pacString[currentCharIndex];
		return ss.str();
	}
	
	[[nodiscard]]
	std::string convert_wide_chars_to_utf8(const wchar_t *const pawcString) {
		if (!pawcString)
			return std::string("");
		const size_t stringSize = std::wcslen(pawcString) + 1;
		std::string sConverted("", stringSize);
		std::wcstombs(&sConverted[0], pawcString, stringSize);
		return sConverted;
	}

	[[nodiscard]]
	std::wstring convert_chars_to_wide(const char *const pacString) {
		if (!pacString)
			return std::wstring(L"");
		const size_t stringSize = std::strlen(pacString) + 1;
		std::wstring wsConverted(L"", stringSize);
		std::mbstowcs(&wsConverted[0], pacString, stringSize);
		return wsConverted;
	}

	[[nodiscard]]
	std::string get_app_name() {
		std::string sAppName("");
#ifdef RE_OS_WINDOWS
# define PATH_SIZE 500
		wchar_t awcBuffer[PATH_SIZE] = {0};
		if (GetModuleFileNameW(nullptr, awcBuffer, PATH_SIZE))
			sAppName = convert_wide_chars_to_utf8(awcBuffer);
		else
			RE_ERROR("Failed retrieving the file name of the application");
# undef PATH_SIZE
#elif defined RE_OS_LINUX
		std::ifstream fileStream("/proc/self/comm");
		if (fileStream.is_open())
			fileStream >> sAppName;
		else
			RE_ERROR("Failed opening \"/proc/self/comm\" file for retrieving file name of the application");
#else
		RE_ERROR("Unknown platform for retrieving file name of the application");
#endif
		return sAppName;
	}

}
