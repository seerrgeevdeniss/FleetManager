@echo off
chcp 65001 >nul
cd /d "%~dp0"

start "" /min "%~dp0build-win\bin\fleetmanager.exe"

timeout /t 3 /nobreak >nul

start "" msedge --app=http://localhost:8081

exit