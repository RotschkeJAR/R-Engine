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

}
