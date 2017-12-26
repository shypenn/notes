#!bash
[ "$BASH_SOURCE" = "$0" ] && alias quit=exit || alias quit=return

parseSys(){
	log parsing: $1
	if [ -a $1 ]; then
		unset ctn win
		while IFS= read -r line; do
			if [ -n "$line" ] \
			&& [ -n "${line##*LINUX_START*}" ] \
			&& [ -n "${line##*LINUX_END*}" ] \
			;then 
				if [ -z "${line##*WIN_START*}" ];then
					win=1
				elif [ -z "${line##*WIN_END*}" ];then
					unset win
				elif [ ! -v win ];then 
					ctn="$ctn"$'\n'"$line"
				fi
			fi
		done <$1
		echo "$ctn" > $1
	else 
		log "ERRORRRRRRRRRR: $1 doesn't exist"
	fi
}
############################################################################################

log copying files to home
srcDir=$(dirname $BASH_SOURCE)
log cd to source folder: $srcDir
cd $srcDir
cp -rf . ~/ || quit

log cd to ~ 
cd ~

rm *.cmd *.reg .copy.sh

log  parsing os specific files in $file 
file=.cfg/xsys_files.txt
if [ -a $file ]; then
	while read -r line; do
		if [ -z "$line" ]; then continue; fi
		if [ -n "$line" ]; then
			parseSys $line
		else
			log ["$line"] doesn\'t exist;
		fi
	done < $file 
else 
	log "ERRORRRRRRRRRR: $file doesn't exist"
	quit
fi
