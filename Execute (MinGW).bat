@echo off
SETLOCAL EnableDelayedExpansion

call "Game (MinGW; Console).exe"

if %ERRORLEVEL% NEQ 0 (
	echo Crashed : %ERRORLEVEL%
)

pause
