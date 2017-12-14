echo cd bundle
pushd bundle

while read line; do
	if [ -z "$line" ];then continue;fi
	echo install "$line"
	git clone --depth 1 $line
done < "../vim_plg.txt"

popd

source ./post_process_plg.sh
