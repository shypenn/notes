@echo off
if "%1"=="" exit 0

set str=%~1
set /a len=0

:loop
	set /a len=len+1
	set str=%str:~1%
	if defined str goto:loop

exit /b %len%
