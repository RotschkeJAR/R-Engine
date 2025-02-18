#include "RE_Ext Header.hpp"

namespace RE {
	
	std::string convertToUTF8(const wchar_t* pwcString) {
		const REuint uiStringSize = wcslen(pwcString) + 1;
		std::string strConverted("", uiStringSize);
		wcstombs(&strConverted[0], pwcString, uiStringSize);
		return strConverted;
	}

	std::wstring convertToWide(const char* pcString) {
		const REuint uiStringSize = strlen(pcString) + 1;
		std::wstring wstrConverted(L"", uiStringSize);
		mbstowcs(&wstrConverted[0], pcString, uiStringSize);
		return wstrConverted;
	}

	std::string getAppName() {
		std::string strAppName;
#ifdef RE_OS_WINDOWS
# define PATH_SIZE 500
		wchar_t wcBuffer[PATH_SIZE] = {0};
		if (GetModuleFileNameW(nullptr, wcBuffer, PATH_SIZE)) 
			strAppName = convertToUTF8(wcBuffer);
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
