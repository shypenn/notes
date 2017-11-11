pushd bundle
for /f %%a in (..\vim_plg.txt) do (
	echo installing %%a...
	git clone --depth 1 %%a
)
popd

:: instant-markdown needs curl(https://curl.haxx.se/download.html), already available if git is installed; (in v2.10 curl misfunction; in v2.14, curl.cmd contains false path;
