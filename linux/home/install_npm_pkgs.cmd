set "file=.cfg\npm_pkgs.txt"
call:log installing node packages in !file!
for /f %%a in (!file!) do (
	call:log npm i -g %%a
	call npm i -g %%a
)
