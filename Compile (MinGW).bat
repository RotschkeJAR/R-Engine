@echo off
SETLOCAL EnableDelayedExpansion

rem Script for compiling with LLVM-MinGW 20250430 (LLVM 20.1.4) from https://github.com/mstorsjo/llvm-mingw/releases

set SRC=src
set BIN=bin\MinGW

set CC=g++
set CFLAG=-std=c++20 -pedantic-errors -Wall -march=x86-64 -m64 -ffast-math -O2 -D_WIN32_WINNT=0x0A00
set LDFLAG=-lRE -lgdi32 -luser32 -I%SRC% -L%BIN% -static-libgcc -static-libstdc++

for %%f in (%SRC%\*.cpp) do (
	echo %%f
	%CC% %CFLAG% -c -I"C:\VulkanSDK\Include" "%%f"
	if %ERRORLEVEL% NEQ 0 (
		del /f *.o
		echo ERROR : Failed compiling engine source code
		pause
		exit 1
	)
)
del /f "%BIN%\*.o"
move *.o %BIN% >nul
for %%f in (%BIN%\*.o) do (
	ar rs %BIN%\libRE.a "%%f"
)
for %%f in (*.cpp) do (
	echo %%f
	%CC% %CFLAG% -c -I"src" "%%f"
	if %ERRORLEVEL% NEQ 0 (
		del /f *.o
		echo ERROR : Failed compiling game source code
		pause
		exit 1
	)
)
for %%f in (*.o) do (
	set OBJS=!OBJS! %%f
)
%CC% -o "Game (MinGW; Windows).exe" %OBJS% -mwindows %LDFLAG%
if %ERRORLEVEL% NEQ 0 (
	del /f *.o
	echo ERROR : Failed generating Windows executable
	pause
	exit 1
)
%CC% -o "Game (MinGW; Console).exe" %OBJS% %LDFLAG%
del /f *.o
if %ERRORLEVEL% NEQ 0 (
	echo ERROR : Failed generating Console executable
) else (
	echo SUCCESS
)
pause
