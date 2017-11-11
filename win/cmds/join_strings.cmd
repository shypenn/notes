echo off

set LANG=zh_cn
if not "%1"=="" set LANG=%1

set OUT=all_strings_%LANG%.json
	REM clear/create this file.
break>%OUT%	

set /a TOTAL=0
SETLOCAL enableDelayedExpansion
set NEWL=^


for /f "tokens=* delims=^\" %%a in ('dir /b /s string?.json') do (
	call:PARSE "%%a"
)
echo.TOTAL=%TOTAL%
ENDLOCAL
EXIT /b

:PARSE
SETLOCAL
	set /a _total=%TOTAL%
	set FN=%~1
	echo.[PATH] %FN%
	set _dir1=
	set _dir2=
	set _dir3=
	set _file=
	set _partName=
	for /f %%A in ("%FN:\=!NEWL!%") do (
		set _dir1=!_dir2!
		set _dir2=!_dir3!
		set _dir3=!_file!
		set _file=%%A
	)
	if "%_dir3%"=="%LANG%" (
		set /a _total=%_total%+1
		echo.FILE=%_file%
		if "%_dir2%"=="locale" (
			set _partName=%_dir1%
		) else (
			REM /res
			set _partName=%_dir2%
		)
		echo.[!_partName!]------------------------------------------>>%OUT%
		type %1 >>%OUT%
		echo.>>%OUT%
	)
ENDLOCAL & set TOTAL=%_total%
EXIT /b
