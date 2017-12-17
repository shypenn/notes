@echo off
setlocal enableDelayedExpansion

call:log coping...
call:pushd "%~dp0"
xcopy /EY * %userprofile%

call:pushd %userprofile%

call:log delete used files
del *.sh .*.txt

call:log parsing os specific files in %file% 
set "file=.cfg\xsys_files.txt"
if exist %file% (
	for /f %%a in (%file%) do (
		call:log paring %%a
		call:parseSys %%a
	)
) else (
	call:log ERRORRRRRRRRRR: %file% doesn't exist
)


:: **** sub functions*************************************************************

:end
	popd
	popd
	pause
exit /b

:getTmpFile
	set "%1=tmp%random%"
exit /b

:log
	echo.
	echo %*
exit /b

:pushd
	pushd %*
	echo.
	echo CWD: %cd%
exit /b

:parseSys
	setlocal
	call:getTmpFile tmp

	set lines=
	set /a ix=0
	set _start=
	for /f tokens^=1-20^*^ delims^=^!^&^"^|^> %%a in (%1) do (
		set "ln=%%a%%b%%c%%d%%e%%f%%g%%h%%i%%j%%k%%l%%m%%n%%o%%p%%q%%r%%s%%t%%u"
		set /a ix=!ix!+1
		set valid=0

		echo(!ln! | find "WIN_START" >nul
		if errorlevel 1 (
			echo(!ln! | find "WIN_END" >nul
			if errorlevel 1 (
				echo(!ln! | find "LINUX_START" >nul
				if errorlevel 1 (
					echo(!ln! | find "LINUX_END" >nul
					if errorlevel 1 (
						if not defined _start (
							set valid=1
						)
					) else (
						set _start=
					)
				) else (
					set _start=1
				)
			)
		)
		if !valid!==0 set "lines=!lines!,!ix!"
	)

	set /a ix=0
	for /f "tokens=*" %%a in (%1) do (
		set /a ix=!ix!+1
		call:strLen !ix!
		set /a err=!errorlevel!+1
		for /f %%A in ("^!lines:~0,!err!^!") do ( set prefix=%%A)
		if "!prefix!" == ",!ix!" (
			for /f %%I in ("^!lines:~!err!^!") do ( set lines=%%I)
		) else (
			echo(%%a>> %tmp%
		)
	)
	move %tmp% %1
	endlocal
exit /b


:strLen
	if "%1"=="" exit 0

	set str=%~1
	set /a len=0

	:loop
		set /a len=len+1
		set str=%str:~1%
		if defined str goto:loop
exit/b %len%
