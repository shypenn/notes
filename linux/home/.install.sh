#!bash

# install none	none
# @param {('none'|'update')} [vimPlugin] - how to treat vim plugins, install by default; 'none': no installing; 'update': update them; 
# @param {bool} [npmPkg] - if install npm packages (install by default); true: install; false:no-install


[ "$BASH_SOURCE" = "$0" ] && alias quit=exit || alias quit=return

log(){
	echo ----$@-----
}

handleVimPluglins(){
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
}

handleNpmPkgs(){
log parsing npm packages 
if [ "$2" = false ];then
	log bypass installing node packages
else
	. .install_npm_pkgs.sh
fi
}


#####################################################
# step 1: copy files to user directory [
# if [ "$(dirname `realpath $BASH_SOURCE`)" != "$HOME" ]; then
# mac has no realpath
if [ "$(cd `dirname $BASH_SOURCE`; pwd -P)" != "$HOME" ]; then
	source ./.copy.sh
else
	log copy canceled
fi
# step 1 ]

pushd ~

# step 2: config os [
if [[ $(uname -s) == Linux* ]]; then
	source ./.install_os.sh
fi
# step 2 ]

# step 3: whether to install or update vim plugins [
handleVimPluglins $1
# step 3 ]

# step 4: whether to install npm packages [
handleNpmPkgs $2
# step 4 ]

popd
log DONE
quit 0
