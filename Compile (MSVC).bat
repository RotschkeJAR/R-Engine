@echo off
SETLOCAL EnableDelayedExpansion

set SRC=src
set BIN=bin\MSVC

set CC=cl
set CFLAG=/nologo /EHsc /std:c++20 /W1 /D_WIN32_WINNT=0x0A00 /favor:blend
set LDFLAG=gdi32.lib user32.lib

set OUT_LIB=%BIN%\RE.lib

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

del /f *.obj %BIN%\*.obj
for %%f in (%SRC%\*.cpp) do (
	%CC% %CFLAG% /c /I"C:\VulkanSDK\Include" "%%f"
	if %ERRORLEVEL% NEQ 0 (
		del /f *.obj
		echo ERROR : Failed compiling engine source code
		pause
		exit 1
	)
)
move *.obj %BIN%
lib /NOLOGO /OUT:%OUT_LIB% %BIN%\*.obj
%CC% %CFLAG% /I%SRC% *.cpp /Fe:"Game (MSVC; Windows).exe" /link %OUT_LIB% %LDFLAG% /SUBSYSTEM:WINDOWS
if %ERRORLEVEL% NEQ 0 (
	echo ERROR : Failed generating Windows executable
	pause
	exit 1
)
%CC% %CFLAG% /I%SRC% *.cpp /Fe:"Game (MSVC; Console).exe" /link %OUT_LIB% %LDFLAG% /SUBSYSTEM:CONSOLE
if %ERRORLEVEL% NEQ 0 (
	echo ERROR : Failed generating Console executable
) else (
	echo SUCCESS
)
pause
