@if defined VCVARS (goto OUT)

@set VCDIR=
@for /F "tokens=1,2*" %%i in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\SxS\VC7" /v "10.0"') do (
@	if "%%i"=="10.0" (
@		set "VCDIR=%%k"
	)
)
goto :eof
@if "%VCDIR%"=="" (echo No VCDIR)&(goto ERROR)

@set SDKDIR=
@for /F "tokens=1,2*" %%i in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Microsoft SDKs\Windows\v7.1" /v "InstallationFolder"') do (
@	if "%%i"=="InstallationFolder" (
@		set "SDKDIR=%%k"
	)
)
@if "%SDKDIR%"=="" (echo No SDKDIR)&(goto ERROR)

@set STR=

@set "STR=%VCDIR%LIB"
@if exist "%STR%" (
@	set "LIB=%STR%;%LIB%"
)
@set "STR=%SDKDIR%LIB"
@if exist "%STR%" (
@	set "LIB=%STR%;%LIB%"
)
@echo.
@echo LIB=%LIB%

@set "STR=%SDKDIR%INCLUDE"
@if exist "%STR%" (
@	set "INCLUDE=%STR%;%INCLUDE%"
)
@set "STR=%VCDIR%INCLUDE"
@if exist "%STR%" (
@	set "INCLUDE=%STR%;%INCLUDE%"
)
@echo.
@echo INCLUDE=%INCLUDE%

@set "STR=%VCDIR%BIN"
@if exist "%STR%" (
@	set "PATH=%STR%;%PATH%"
)
@set "STR=%VCDIR%..\common7\ide\"
@if exist "%STR%" (
@	set "PATH=%STR%;%PATH%"
)
@set "STR=%SDKDIR%BIN"
@if exist "%STR%" (
@	set "PATH=%STR%;%PATH%"
)
@set "STR=C:\Program Files\Debugging Tools for Windows (x86)"
@if exist "%STR%" (
@	set "PATH=%STR%;%PATH%"
) else (echo No debug directory.)
@echo.
@echo PATH=%PATH%

@set VCVARS=1
@:OUT
@exit /b 0

@:ERROR
@exit /b 1
