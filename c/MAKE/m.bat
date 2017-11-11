::m.exe target_Directory b(uild)/d(ebug)/r(un)
@set _TARGET_DIR=%1
@if not "%_TARGET_DIR%"=="" if exist  "%_TARGET_DIR%" (goto READ_OPTION)
@(echo Invalid target directory!)&(goto ERROR)
::remove the last \ for get app name and make it consistent
@if "%_TARGET_DIR:-1%"=="\" set "_TARGET_DIR=%_TARGET_DIR:,-1%"

@:READ_OPTION
@set _OPTION=%2
@if "%_OPTION%"=="d" (goto INIT)
@if "%_OPTION%"=="b" (goto INIT)
@if "%_OPTION%"=="r" (goto INIT)
@(echo build^(b^) or debug^(d^)?) & (goto ERROR)

@:INIT
::read variable
@set errorlevel=
@cmd /c "exit /b 0"
@call vcVars.bat
@if %errorlevel% neq 0 (goto ERROR)

::get app name, it is the directory name
@call :GET_APP_NAME %_TARGET_DIR%
@goto END_INIT
@:GET_APP_NAME
@set _APP_NAME=%~n1
@set _BUILD_DIR=OUT

@exit /b
@:END_INIT

@if "%_OPTION%"=="d" (goto DEBUG)
@if "%_OPTION%"=="r" (goto RUN)

::build
@set _MAKE_DIR=%cd%
@pushd %_TARGET_DIR%
@if not exist "%_BUILD_DIR%" (md "%_BUILD_DIR%")
@set _OBJLIST=
@for %%i in (*.cpp) do (@set _OBJLIST=%_OBJLIST% %_BUILD_DIR%\%%~ni.obj)
@set _RESLIST=
@for %%i in (*.rc) do (@set _RESLIST=%_RESLIST% %_BUILD_DIR%\%%~ni.res)
call nmake /nologo /D /f %_MAKE_DIR%\makefile.mk "BUILDDIR=%_BUILD_DIR%" "APPNAME=%_APP_NAME%" "OBJLIST=%_OBJLIST%" "RESLIST=%_RESLIST%"
@popd
@goto OUT

::debug
@:DEBUG
@pushd %_TARGET_DIR%
start /b windbg -QY -srcpath %_TARGET_DIR% -y %_TARGET_DIR% %_TARGET_DIR%\%_BUILD_DIR%\%_APP_NAME%.exe
@popd
@goto OUT

::run
:RUN
start "" %_TARGET_DIR%\%_BUILD_DIR%\%_APP_NAME%.exe

@:OUT
@exit /b 0

@:ERROR
@exit /b 1
