#include "RE_Ext Header.hpp"

namespace RE {
	
	std::string convertToUTF8(const wchar_t* wstring) {
		const size_t strSize = wcslen(wstring) + 1;
		std::string u8Str("", strSize);
		wcstombs(&u8Str[0], wstring, strSize);
		return u8Str;
	}

	std::wstring convertToWide(const char* string) {
		const size_t strSize = strlen(string) + 1;
		std::wstring wideStr(L"", strSize);
		mbstowcs(&wideStr[0], string, strSize);
		return wideStr;
	}

	std::string appName;

	const char* getAppName() {
		if (!appName.empty())
			return appName.c_str();
#ifdef RE_OS_WINDOWS
# define PATH_SIZE 500
		wchar_t wBuffer[PATH_SIZE] = {0};
		if (!GetModuleFileNameW(nullptr, wBuffer, PATH_SIZE)) {
			RE_ERROR("Failed retrieving the file name of the application");
			return nullptr;
		}
		appName = convertToUTF8(wBuffer);
#elif defined RE_OS_LINUX
		std::ifstream fs("/proc/self/comm");
		if (!fs) {
			RE_ERROR("Failed opening \"/proc/self/comm\" file for retrieving file name of the application");
			return nullptr;
		}
		fs >> appName;
#else
		RE_ERROR("Unknown platform for retrieving file name of the application");
		return nullptr;
#endif
		return appName.c_str();
	}

}
