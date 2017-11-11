@echo off
:: install none	none
:: @param {('none'|'update')} [vimPlugin] - how to treat vim plugins, install by default; 'none': no installing; 'update': update them; 
:: @param {bool} [ifNpmPkg] - if install npm packages, install by default; true: install; false:no-install

setlocal enableDelayedExpansion

mklink aaaa aaaa
if errorlevel 1 (
	call:log Elevated priviledge is required
	goto :end
)
del aaaa


call:log coping...
call:pushd "%~dp0"
xcopy /EY * %userprofile%
call:pushd %userprofile%
del install.cmd

set "file=.cfg\xsys_files.txt"
call:log parsing os specific files in %file% *****************************************************************
if exist %file% (
	for /f %%a in (%file%) do (
		call:log paring %%a
		call:parseSys %%a
	)
) else (
	call:log ERRORRRRRRRRRR: %file% doesn't exist
)


call:log parsing .idapro *****************************************************************
set idapro="%APPDATA%\Hex-Rays\IDA Pro"
call:log mkdir %idapro%
mkdir %idapro%
call:log copy .idapro/* to %idapro%
copy /Y .idapro\* %idapro%
del /Q .idapro

::call:log parsing .gitconfig: rename vim to gvim  *****************************************************************
::call:getTmpFile tmp
::for /f "tokens=*" %%a in (.gitconfig) do (
::	set "ln=%%a"
::	echo !ln:vim=gvim! >> %tmp% 
::)
::move %tmp% .gitconfig
:::: gvim downgrades to vim in tty;


::call:log parsing .npmrc *****************************************************************
:::: in global mode, adopt 'link'; 
::for /f %%a in ('npm config get prefix') do (set pfx=%%a)
::call:log deleting %pfx%\etc\npmrc
::del %pfx%\etc\npmrc
::call:log copying .npmrc to %pfx%
::for /f "tokens=*" %%a in (.npmrc) do (
::	set ln=%%a
::	if "!ln!" == "link=false" (
::		set ln=link=true
::	)
::	echo !ln! >> %pfx%\etc\npmrc
::)

call:log parsing vim plugins *****************************************************************
if '%1' == 'none' (
	rd /q /s .vim
	call:log bypass install vim plugins
	popd
	goto:end
)
if '%1' == 'update' (
	rd /q /s .vim
	call:log updating plugins
	pushd vimfiles
	call update_vim_plg.cmd
	popd
	goto:end
)

call:log installing vim plugins
call:log change .vim to vimfiles...
if exist vimfiles rd /q /s vimfiles
move .vim vimfiles
call:pushd vimfiles

call:log make link autoload -- bundle\vim-pathogen\autoload
mklink /d autoload "bundle\vim-pathogen\autoload"

mkdir bundle
call install_vim_plg.cmd
popd



call:log parsing node packages *****************************************************************
if '%2' == 'false' (
	call:log bypass install node packages
) else (
	call install_npm_pkgs.cmd
)



call:log finished
goto:end

:: **** sub functions*************************************************************

:end
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
