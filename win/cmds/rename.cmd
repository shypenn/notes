@echo off
REM same name with different extensions;
set invalid=
if "%1"=="" set invalid=1
if "%2"=="" set invalid=1
if defined invalid (
	echo rename SourceName TargetName
	exit /b
)

SETLOCAL enableDelayedExpansion
set SRC=%1
set TRG=%2
set NEWL=^



for /f %%a in ('dir /b /s %1.*') do (
	call:RENAME "%%a"
)
ENDLOCAL
EXIT /b

:RENAME
SETLOCAL
	set partName1=
	set partName2=
	set LastPart=
	set NewLastPart=
	set pth=%~1
	echo %1
	for /f %%A in ("%pth:\=!NEWL!%") do (
		set partName1=!partName2!
		set LastPart=%%A
		call:SWAP %%A
		if defined partName2 (
			set partName2=!partName2!\!NewLastPart!
		) else (
			set partName2=!NewLastPart!
		)
	)
	echo  !partName1!\!NewLastPart!
	rename "!partName1!\%LastPart%"  "!NewLastPart!"
ENDLOCAL
EXIT /b

:SWAP
SETLOCAL
	set name1=
	set name2=
	set lastName=
	set fn=%1
	for /f %%A in ("%fn:.=!NEWL!%") do (
		set name1=!name2!
		set lastName=%%A
		if defined name2 (
			if "%%A"=="!SRC!" (
				set name2=!name2!.!TRG!
			) else (
				set name2=!name2!.%%A
			)
		) else (
			if "%%A"=="!SRC!" (
				set name2=!TRG!
			) else (
				set name2=%%A
			)
		)
	)

if defined name1 (
	ENDLOCAL & set NewLastPart=%name1%.%lastName%
) else (
	ENDLOCAL & set NewLastPart=%lastName%
)
exit /b
