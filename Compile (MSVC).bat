@echo off
SETLOCAL EnableDelayedExpansion

REM Script for compiling with MSVC 2022 x64 native tools

set SRC=src
set BIN=bin\MSVC
set TEST=test

set CC=cl
set CFLAG=/nologo /EHsc /TP /std:c++20 /W1 /favor:blend /D_WIN32_WINNT=0x0A00
set LDFLAG=gdi32.lib user32.lib

set OUT_LIB=%BIN%\RE.lib

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

del /f *.obj
set ERROR=false
for %%f in (%SRC%\*.cpp) do (
	%CC% %CFLAG% /MP4 /c /I "C:\VulkanSDK\Include" /I lib "%%f" || set ERROR=true
)
if %ERROR%==true (
	echo ERROR : Failed compiling engine
	del /f *.obj
	pause
	exit /b 1
)
del /f %BIN%\*.obj
move *.obj %BIN%
del /f %OUT_LIB%
lib /NOLOGO /OUT:%OUT_LIB% %BIN%\*.obj %BIN%\lib\*.obj
if %ERRORLEVEL% NEQ 0 (
	echo ERROR : Failed creating static library
	del /f *.obj
	pause
	exit /b 1
)
for %%f in (%TEST%\*.cpp) do (
	%CC% %CFLAG% /c /I%SRC% "%%f" || set ERROR=true
)
if %ERROR%==true (
	echo ERROR : Failed compiling game
	del /f *.obj
	pause
	exit /b 1
)
link /NOLOGO *.obj %OUT_LIB% %LDFLAG% /OUT:"Game (MSVC; Windows).exe" /SUBSYSTEM:WINDOWS
if %ERRORLEVEL% NEQ 0 (
	echo ERROR : Failed generating Windows executable
	del /f *.obj
	pause
	exit /b 1
)
link /NOLOGO *.obj %OUT_LIB% %LDFLAG% /OUT:"Game (MSVC; Console).exe" /SUBSYSTEM:CONSOLE
if %ERRORLEVEL% NEQ 0 (
	echo ERROR : Failed generating Console executable
	del /f *.obj
	pause
	exit /b 1
)
echo SUCCESS
del /f *.obj
pause
