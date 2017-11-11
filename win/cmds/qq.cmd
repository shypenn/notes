@echo off
if '%1'=='' echo 没有传入文件路径 & exit /b
echo 开始处理文件%~nx1...

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

echo 处理完毕, 保存至%fn%
start notepad %fn%
pause



