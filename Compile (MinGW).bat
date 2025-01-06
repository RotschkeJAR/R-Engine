@echo off

set SRC=src
set BIN=bin\MinGW

set CC=g++
set CFLAG=-std=c++17 -Wall -march=x86-64 -m64 -ffast-math -O2
set LDFLAG=-lRE -lopengl32 -lgdi32 -luser32 -I%SRC% -L%BIN%

%CC% %CFLAG% -c %SRC%\*.cpp
if %ERRORLEVEL% NEQ 0 (
	del /f *.o
	pause
	exit 1
)
del /f "%BIN%\*.o"
move *.o %BIN% >nul
ar rs %BIN%\libRE.a %BIN%\*.o
%CC% %CFLAG% -c *.cpp %LDFLAG%
if %ERRORLEVEL% NEQ 0 (
	del /f *.o
	pause
	exit 1
)
%CC% -o "Game (MinGW; Windows).exe" *.o -mwindows %LDFLAG%
if %ERRORLEVEL% NEQ 0 (
	del /f *.o
	pause
	exit 1
)
%CC% -o "Game (MinGW; Console).exe" *.o %LDFLAG%
del /f *.o
pause
