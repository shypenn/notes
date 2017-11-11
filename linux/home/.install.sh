#!bash

# install none	none
# @param {('none'|'update')} [vimPlugin] - how to treat vim plugins, install by default; 'none': no installing; 'update': update them; 
# @param {bool} [npmPkg] - if install npm packages, install by default; true: install; false:no-install


[ "$BASH_SOURCE" = "$0" ] && alias quit=exit || alias quit=return

log(){
	echo ----$@-----
}


#####################################################
if [ "$(dirname `realpath $pth`)" != "$HOME" ]; then
	source ./.copy.sh
fi
#####################################################

source ./.install_os.sh


#####################################################
log  parsing vim plugins 
if [ "$1" = none ];then
	log bypass install vim plugins
	quit 0
fi

if [ "$1" = update ];then
	log updating vim plugins
	. update_vim_plg.sh
	quit 0
fi

log installing vim plugins
pushd .vim

log make link autoload -- bundle/vim-pathogen/autoload
ln -s "bundle/vim-pathogen/autoload" autoload 

log create folder \'bundle\'
mkdir bundle
log installing...
. install_vim_plg.sh

popd



#####################################################
log parsing node packages 
if [ "$2" = false ];then
	log bypass installing node packages
else
	. .install_npm_pkgs.sh
fi
#####################################################

log DONE
quit 0
