call:pushd bundle
for /f %%a in (..\vim_plg.txt) do (
	call:log installing %%a...
	git clone --depth 1 %%a
)
call:popd

call:log overriding certain vim plugin files.
call ./post_process_plg.cmd
:: instant-markdown needs curl(https://curl.haxx.se/download.html), already available if git is installed; (in v2.10 curl misfunction; in v2.14, curl.cmd contains false path;

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

:popd
	popd
	echo.
	echo CWD: %cd%
exit /b
