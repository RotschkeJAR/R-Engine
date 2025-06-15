@echo off
SETLOCAL EnableDelayedExpansion
del /f *.o
del /f *.obj

REM Script for compiling with MSVC 2022 x64 native tools and LLVM-MinGW 20250430 (LLVM 20.1.4) from https://github.com/mstorsjo/llvm-mingw/releases

set LIB=lib
set ERROR=false


set BIN=bin\MinGW\lib

set CC=g++
set CFLAG=-std=c2x -pedantic-errors -Wall -march=x86-64 -m64 -ffast-math -O2 -D_WIN32_WINNT=0x0A00

REM MinGW-specific compilation
for %%f in (%SRC%\*.c) do (
	%CC% %CFLAG% -c "%%f"
	if !ERRORLEVEL! NEQ 0 (
		set ERROR=true
	)
)
if %ERROR%==true (
	del /f *.o
	echo ERROR : Failed compiling librarys' source code with MinGW
	pause
	exit /b 1
)
del /f %BIN%\*.o
move *.o %BIN%
echo .
echo .
echo .



set BIN=bin\MSVC\lib

set CC=cl
set CFLAG=/nologo /EHsc /TC /std:c17 /W1 /D_WIN32_WINNT=0x0A00 /favor:blend

REM MSVC-specific compilation
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
for %%f in (%SRC%\*.c) do (
	%CC% %CFLAG% /c "%%f"
	if !ERRORLEVEL! NEQ 0 (
		set ERROR=true
	)
)
if %ERROR%==true (
	del /f *.obj
	echo ERROR : Failed compiling librarys' source code with MSVC
	pause
	exit /b 1
)
del /f %BIN%\*.obj
move *.obj %BIN%


echo SUCCESS
pause
