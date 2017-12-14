echo cd bundle
pushd bundle
for dir in `ls -d */`; do
	pushd $dir
	echo 'updating....'
	git pull
	popd
done
popd

source ./post_process_plg.sh
