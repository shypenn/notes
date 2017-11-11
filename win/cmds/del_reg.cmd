@echo off
REM HKEY_CLASSES_ROOT\Software\Classes\Youku.3gp	Youku.mp3	...
set invalid=
if "%1"=="" set invalid=1
if "%2"=="" set invalid=1
if defined invalid (
	echo del_reg path key
	exit /b
)

SETLOCAL enableDelayedExpansion
set cmd=%~dp0%strlen.cmd
call %cmd% "%2"
set LEN=%errorlevel%
echo length: %len%
set TAR=%2
set NEWL=^



for /f %%a in ('reg query %1') do (
	echo %%a
	set Matched=
	call:MATCH "%%a"
	if defined Matched (
		echo matched
		reg delete %%a /f
		if !errorlevel!==0 (echo deleted) else (echo failed)
	)
)
ENDLOCAL
EXIT /b

:MATCH
SETLOCAL
	set LastPart=
	set _matched=
	set pth=%~1
	for /f %%A in ("%pth:\=!NEWL!%") do (
		set LastPart=%%A
	)
	if "!lastpart:~,%LEN%!"=="%TAR%" set _matched=1
ENDLOCAL & set Matched=%_matched%
EXIT /b

