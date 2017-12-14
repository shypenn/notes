@echo off
pushd bundle
for /d %%a in (*) do (
	pushd %%a
	echo updating %%a...
	git pull
	popd
)

call ./post_process_plg.cmd
pause

