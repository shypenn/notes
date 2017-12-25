@echo off

call:log apply solarized cmd theme 
regedit /s solarized-dark.reg

call:log install chocolatey
"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command "iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))" && SET "PATH=%PATH%;%ALLUSERSPROFILE%\chocolatey\bin"

call:log install editorconfig
choco install editorconfig.core

:: **** sub functions*************************************************************

:log
	echo.
	echo %*
exit /b

