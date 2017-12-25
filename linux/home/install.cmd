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
if not "%~dp0"=="%userprofile%\" ( call copy.cmd )
:: step 1 ]

call:pushd %userprofile%

:: step 2: config os [
call install_os.cmd
:: step 2 ]

:: step 3: whether to install or update vim plugins [
call:parseVimPlugins
:: step 3 ]


:: step 4: whether to install npm packages [
:parseNpmPkgs
call:log **** parsing npm packages ****
if '%2' == 'false' (
	call:log bypass install node packages
) else (
	call install_npm_pkgs.cmd
)
:: step 4 ]

call:log finished

:: **** sub functions*************************************************************

:parseVimPlugins
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
exit /b

:end
	call:popd
	pause
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
