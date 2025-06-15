@echo off
SETLOCAL EnableDelayedExpansion

REM Script for compiling with MSVC 2022 x64 native tools

set SRC=src
set BIN=bin\MSVC

set CC=cl
set CFLAG=/nologo /EHsc /TP /std:c++20 /W1 /D_WIN32_WINNT=0x0A00 /favor:blend /MP3
set LDFLAG=gdi32.lib user32.lib

set OUT_LIB=%BIN%\RE.lib

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

del /f *.obj
set ERROR=false
for %%f in (%SRC%\*.cpp) do (
	%CC% %CFLAG% /c /I"C:\VulkanSDK\Include" "%%f"
	if !ERRORLEVEL! NEQ 0 (
		set ERROR=true
	)
)
if %ERROR%==true (
	del /f *.obj
	echo ERROR : Failed compiling engine source code
	pause
	exit /b 1
)
del /f %BIN%\*.obj
move *.obj %BIN%
lib /NOLOGO /OUT:%OUT_LIB% %BIN%\*.obj %BIN%\lib\*.obj
for %%f in (*.cpp) do (
	%CC% %CFLAG% /c /I%SRC% *.cpp "%%f"
	if !ERRORLEVEL! NEQ 0 (
		set ERROR=true
	)
)
if %ERROR%==true (
	del /f *.obj
	echo ERROR : Failed compiling game source code
	pause
	exit /b 1
)
link /NOLOGO *.obj %OUT_LIB% %LDFLAG% /OUT:"Game (MSVC; Windows).exe" /SUBSYSTEM:WINDOWS
if %ERRORLEVEL% NEQ 0 (
	del /f *.obj
	echo ERROR : Failed generating Windows executable
	pause
	exit 1
)
link /NOLOGO *.obj %OUT_LIB% %LDFLAG% /OUT:"Game (MSVC; Console).exe" /SUBSYSTEM:CONSOLE
if %ERRORLEVEL% NEQ 0 (
	echo ERROR : Failed generating Console executable
) else (
	echo SUCCESS
)
del /f *.obj
pause
