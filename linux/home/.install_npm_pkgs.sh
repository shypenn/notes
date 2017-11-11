file=.cfg/npm_pkgs.txt
echo installing node packages in $file

echo read as a whole line
read -r -d '' line < $file
if [ -n "$line" ];then
	echo npm i -g $line
	sudo npm i -g $line
else
	echo empty
fi
