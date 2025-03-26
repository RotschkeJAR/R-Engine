#include "RE_Internal Header.hpp"

namespace RE {

	bool is_string_empty(const char* pcString) {
		return std::strlen(pcString) == 0;
	}

	REuint get_line_count(const char* pcString) {
		if (is_string_empty(pcString))
			return 0;
		REuint u32CurrentLine = 1, u32CurrentCharacter = 0;
		while (true) {
			switch (pcString[u32CurrentCharacter]) {
				case '\0':
					return u32CurrentLine;
				case '\n':
					u32CurrentLine++;
				default:
					break;
			}
			u32CurrentCharacter++;
		}
	}

	std::string get_line(const char* pcString, REuint u32Line) {
		REuint u32CurrentLine = 0U, u32CurrentCharacter = 0U;
		std::stringstream resultStream("");
		bool bReachedEndOfString = false;
		while (true) {
			const bool bLineFound = u32CurrentLine == u32Line;
			while (true) {
				bool bBreakout = false;
				switch (pcString[u32CurrentCharacter]) {
					case '\0':
						bReachedEndOfString = true;
						// Don't break here!
					case '\n':
						bBreakout = true;
						break;
					default:
						if (bLineFound)
							resultStream << pcString[u32CurrentCharacter];
						break;
				}
				u32CurrentCharacter++;
				if (bBreakout)
					break;
			}
			if (bLineFound || bReachedEndOfString)
				break;
			u32CurrentLine++;
		}
		return std::string(resultStream.str());
	}
	
	std::string convert_wide_chars_to_utf8(const wchar_t* pwcString) {
		const REuint u32StringSize = std::wcslen(pwcString) + 1U;
		std::string strConverted("", u32StringSize);
		std::wcstombs(&strConverted[0], pwcString, u32StringSize);
		return strConverted;
	}

	std::wstring convert_chars_to_wide(const char* pcString) {
		const REuint u32StringSize = std::strlen(pcString) + 1U;
		std::wstring wstrConverted(L"", u32StringSize);
		std::mbstowcs(&wstrConverted[0], pcString, u32StringSize);
		return wstrConverted;
	}

	std::string get_app_name() {
		std::string strAppName("Unknown app name");
#ifdef RE_OS_WINDOWS
# define PATH_SIZE 500
		wchar_t wcBuffer[PATH_SIZE] = {0};
		if (GetModuleFileNameW(nullptr, wcBuffer, PATH_SIZE)) 
			strAppName = convert_wide_chars_to_utf8(wcBuffer);
		else
			RE_ERROR("Failed retrieving the file name of the application");
#elif defined RE_OS_LINUX
		std::ifstream fileStream("/proc/self/comm");
		if (fileStream.is_open())
			fileStream >> strAppName;
		else
			RE_ERROR("Failed opening \"/proc/self/comm\" file for retrieving file name of the application");
#else
		RE_ERROR("Unknown platform for retrieving file name of the application");
#endif
		return strAppName;
	}

}
