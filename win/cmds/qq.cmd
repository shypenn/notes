@echo off
if '%1'=='' echo û�д����ļ�·�� & exit /b
echo ��ʼ�����ļ�%~nx1...

set fn="%~d1%~p1%~n1_new%~x1"
break >%fn%
setlocal enableDelayedExpansion
set ls=
for /f "usebackq" %%a in (%1) do (
	set "ln=%%a"
	if "!ln:~0,1!"=="(" (
		set "ln=!ln:(=!"
		set "ln=!ln:)=!"
		echo !ls!	!ln! >>%fn%
	) else set "ls=%%a"

)

echo �������, ������%fn%
start notepad %fn%
pause



