@echo off
REM This script fixes the MSB8012 error when a project is converted from to VS2010.
REM We can remove this file once configure.exe can create VS2010 project files.

FOR /F "tokens=3" %%A IN ('REG QUERY "HKLM\SOFTWARE\Microsoft\PowerShell\1" /v Install ^| FIND "Install"') DO SET PowerShellInstalled=%%A

IF NOT "%PowerShellInstalled%"=="0x1" GOTO NOT_INSTALLED

RMDIR /s /q Backup
DEL UpgradeLog.htm

:NO_BACKUP

powershell -ExecutionPolicy Unrestricted .\build\FixVS2010.ps1

GOTO DONE

:NOT_INSTALLED
ECHO "This script needs powershell"

:DONE
PAUSE
