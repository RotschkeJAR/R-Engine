@echo off

set SRC=src
set BIN=bin\MSVC

set CC=cl
set CFLAG=/nologo /EHsc /std:c++17 /favor:blend
set LDFLAG=gdi32.lib user32.lib

set OUT_LIB=%BIN%\RE.lib

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

del /f *.obj %BIN%\*.obj
%CC% %CFLAG% /c %SRC%\*.cpp /I "C:\VulkanSDK\1.4.304.0\Include"
move *.obj %BIN%
lib /NOLOGO /OUT:%OUT_LIB% %BIN%\*.obj
%CC% %CFLAG% /Fe:"Game (MSVC; Windows).exe" /I %SRC% *.cpp /link %OUT_LIB% %LDFLAG% /SUBSYSTEM:WINDOWS
%CC% %CFLAG% /Fe:"Game (MSVC; Console).exe" /I %SRC% *.cpp /link %OUT_LIB% %LDFLAG% /SUBSYSTEM:CONSOLE
del /f *.obj

pause
