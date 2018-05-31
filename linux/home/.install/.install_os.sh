log(){
	echo -e "\e[33m$@\e[0m"
}

echo add aliyun package source
if [ `type -t apt` ]; then
	alias pkg=apt
	. /etc/lsb-release
	log DISTRIB_CODENAME: ${DISTRIB_CODENAME} 
	txt=$(eval "echo \"`<.aliyun_ubuntu.txt`\"")
	txt="${txt//\//\\\/}"
	txt="${txt//$'\n'/\\\n}"
	sed -i "1s/^/${txt}\n/" /etc/apt/sources.list
else
	alias pkg=yum
	yum install epel-release
fi

log install packages:
sudo pkg install -y \
	gparted ibus-table-wubi ibus-pinyin gvim gdebi thunderbird \
	git editorconfig

#sudo pkg install libgnome-keyring-dev
#sudo make --directory=/usr/share/doc/git/contrib/credential/gnome-keyring


log config grub
cat .grub_cfg.txt >> /etc/default/grub
cat .grub_win_entry.txt >> /etc/grub.d/40_custom
cp -f /boot/efi/EFI/Microsoft/Boot/bootmgfw.efi /boot/efi/EFI/Microsoft/Boot/bkpbootmgfw.efi
cp -f /boot/efi/EFI/ubuntu/grubx64.efi /boot/efi/EFI/Microsoft/Boot/bootmgfw.efi
update-grub


log add input method in ibus manually
ibus-setup
