@echo off
SETLOCAL EnableDelayedExpansion

call "Game (MSVC; Console).exe"

if %ERRORLEVEL% NEQ 0 (
	echo Crashed : %ERRORLEVEL%
)

pause