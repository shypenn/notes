@echo off
:: install none	none
:: @param {('none'|'update')} [vimPlugin] - how to treat vim plugins, install by default; 'none': no installing; 'update': update them; 
:: @param {bool} [ifNpmPkg] - if install npm packages, install by default; true: install; false:no-install

setlocal enableDelayedExpansion

mklink test_priviledge aaaa
if errorlevel 1 (
	call:log Elevated priviledge is required
	goto :end
)
del test_priviledge

:: step 1: copy files to user directory [
call:log **** copy files to user directory **** 
if not "%cd%"=="%userprofile%" ( call copy.cmd )
:: step 1 ]

call:pushd %userprofile%

:: step 2: config os [
regedit /s solarized-dark.reg
:: step 2 ]

:: step 3: whether to install or update vim plugins [
call:log **** parsing vim plugins **** 
if '%1' == 'none' (
	rd /q /s .vim
	call:log bypass install vim plugins
	call:popd
	goto:parseNpmPkgs
)

:: update
if '%1' == 'update' (
	rd /q /s .vim
	call:log updating plugins
	call:pushd vimfiles
	call update_vim_plg.cmd
	call:popd
	goto:parseNpmPkgs
)

:: install
call:log installing vim plugins
call:log change .vim to vimfiles...
if exist vimfiles rd /q /s vimfiles
move .vim vimfiles
call:pushd vimfiles
 
mklink /d autoload "bundle\vim-pathogen\autoload"

mkdir bundle
call install_vim_plg.cmd
call:popd
:: step 2 ]


:: step 4: whether to install npm packages [
:parseNpmPkgs
call:log **** parsing npm packages ****
if '%2' == 'false' (
	call:log bypass install node packages
) else (
	call install_npm_pkgs.cmd
)
:: step 3 ]

call:log finished

:: **** sub functions*************************************************************

:end
	call:popd
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
	call:log [PUSHD] CWD: %cd%
exit /b

:popd
	popd
	call:log [POPD] CWD: %cd%
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
