@echo off

del /f src\* && del /f shaders\*
git fetch --all
git reset --hard origin/main
pause
