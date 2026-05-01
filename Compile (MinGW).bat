@echo off
SETLOCAL EnableDelayedExpansion

REM Script for compiling with LLVM-MinGW 20250430 (LLVM 20.1.4) from https://github.com/mstorsjo/llvm-mingw/releases

set SRC=src
set BIN=bin\MinGW
set TEST=test

set CC=g++
set CFLAG=-std=c++20 -pedantic-errors -Wall -march=x86-64 -m64 -ffast-math -O2 -D_WIN32_WINNT=0x0A00
set LDFLAG=-lRE -lgdi32 -luser32 -I%SRC% -L%BIN% -static-libgcc -static-libstdc++

set OUT_LIB=%BIN%\libRE.a

del /f *.o
set ERROR=false
for %%f in (%SRC%\*.cpp) do (
	echo %%f
	%CC% %CFLAG% -c -I "C:\VulkanSDK\Include" -I lib "%%f" || set ERROR=true
)
if %ERROR%==true (
	echo ERROR : Failed compiling engine
	del /f *.o
	pause
	exit /b 1
)
del /f "%BIN%\*.o"
move *.o %BIN% >nul
del /f %OUT_LIB%
for %%f in (%BIN%\*.o) do (
	ar rs %OUT_LIB% "%%f" || set ERROR=true
)
if %ERROR%==true (
	echo ERROR : Failed creating static library
	del /f *.o
	pause
	exit /b 1
)
for %%f in (%TEST%\*.cpp) do (
	echo %%f
	%CC% %CFLAG% -c -I"src" "%%f" || set ERROR=true
)
if %ERROR%==true (
	echo ERROR : Failed compiling game
	del /f *.o
	pause
	exit /b 1
)
for %%f in (*.o) do (
	set OBJS=!OBJS! %%f
)
%CC% -o "Game (MinGW; Windows).exe" %OBJS% -mwindows %LDFLAG%
if %ERRORLEVEL% NEQ 0 (
	echo ERROR : Failed generating Windows executable
	del /f *.o
	pause
	exit /b 1
)
%CC% -o "Game (MinGW; Console).exe" %OBJS% %LDFLAG%
if %ERRORLEVEL% NEQ 0 (
	echo ERROR : Failed generating Console executable
	del /f *.o
	pause
	exit /b 1
)
echo SUCCESS
del /f *.o
pause
