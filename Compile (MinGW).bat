@echo off

set SRC=src
set BIN=bin\MinGW

set CC=g++
set CFLAG=-std=c++20 -pedantic-errors -Wall -march=x86-64 -m64 -ffast-math -O2 -D_WIN32_WINNT=0x0A00
set LDFLAG=-lRE -lgdi32 -luser32 -I%SRC% -L%BIN% -static-libgcc -static-libstdc++

for %%x in (%SRC%\*.cpp) do (
	echo %%x
	%CC% %CFLAG% -c -I"C:\VulkanSDK\Include" "%%x"
	if %ERRORLEVEL% NEQ 0 (
		del /f *.o
		pause
		exit 1
	)
)
del /f "%BIN%\*.o"
move *.o %BIN% >nul
ar rs %BIN%\libRE.a %BIN%\*.o
for %%x in (*.cpp) do (
	echo %%x
	%CC% %CFLAG% -c -I"src" "%%x"
	if %ERRORLEVEL% NEQ 0 (
		del /f *.o
		pause
		exit 1
	)
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
