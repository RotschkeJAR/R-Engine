@echo off

set SRC=src
set BIN=bin\MSVC

set CC=cl
set CFLAG=/nologo /EHsc /std:c++20 /W1 /favor:blend
set LDFLAG=gdi32.lib user32.lib

set OUT_LIB=%BIN%\RE.lib

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

del /f *.obj %BIN%\*.obj
%CC% %CFLAG% /c %SRC%\*.cpp /I "C:\VulkanSDK\Include"
if %errorlevel% NEQ 0 (
	del /f *.obj
	pause
	exit 1
)
move *.obj %BIN%
lib /NOLOGO /OUT:%OUT_LIB% %BIN%\*.obj
%CC% %CFLAG% /I %SRC% *.cpp /Fe:"Game (MSVC; Windows).exe" /link %OUT_LIB% %LDFLAG% /SUBSYSTEM:WINDOWS
if %errorlevel% NEQ 0 (
	del /f *.obj
	pause
	exit 1
)
%CC% %CFLAG% /I %SRC% *.cpp /Fe:"Game (MSVC; Console).exe" /link %OUT_LIB% %LDFLAG% /SUBSYSTEM:CONSOLE
del /f *.obj

pause
