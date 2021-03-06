@echo off
@setlocal enableextensions
@cd /d "%~dp0"

set service_dir=C:\Services\Nv520ScreenFix
set service_name=Nv520ScreenFix

:admin
net session >nul 2>&1
if %errorLevel% NEQ 0 (
  echo Error: This script requires administrator privileges.
  echo Please, right-click "%~n0%~x0" and press "Run as administrator".
  pause
  exit 0
)

:uninstall
echo Uninstalling %service_name%
reg delete HKLM\Software\Microsoft\Windows\CurrentVersion\Run /f /v %service_name% 2>NUL
taskkill /f /im Nv520ScreenFix-x32.exe 2>NUL
taskkill /f /im Nv520ScreenFix-x64.exe 2>NUL
rmdir /s /q %service_dir% 2>NUL

:success
echo Complete!
pause
