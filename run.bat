@echo off
REM GYM Management System - Windows Launcher
echo.
echo ============================================
echo      GYM Management System
echo ============================================
echo.

REM Build and run using MSYS2 MinGW64
C:\msys64\msys2_shell.cmd -mingw64 -defterm -no-start -c "cd 'E:/GYM Management System' && make && ./bin/gym_system"

echo.
echo Application closed.
pause
