@echo off
REM This script upgrades the solution to VS2010 and fixes the MSB8012 error.

FOR /F "tokens=3" %%A IN ('REG QUERY "HKLM\SOFTWARE\Microsoft\PowerShell\1" /v Install ^| FIND "Install"') DO SET PowerShellInstalled=%%A

IF NOT "%PowerShellInstalled%"=="0x1" GOTO NOT_INSTALLED

call "%vs100comntools%vsvars32.bat"
powershell -ExecutionPolicy Unrestricted .\build\UpgradeSolution.ps1 2010

RMDIR /s /q Backup
DEL UpgradeLog.htm

GOTO DONE

:NOT_INSTALLED
ECHO "This script needs powershell"

:DONE
PAUSE
