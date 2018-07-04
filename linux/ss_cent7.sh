# ss_cent pwd

url=`. github_latest_url.sh shadowsocks/shadowsocks-libev`
file=`sed -E 's/.+\/([^\/]+\.tar\.gz)/\1/' <<< $url`
dir=`sed -E 's/(.+)\.tar\.gz/\1/' <<< $file`
conf=/etc/ss.conf
port=1234

tar -xzf $file
cd $dir
yum install epel-release -y
yum install gcc gettext autoconf libtool automake make pcre-devel asciidoc xmlto c-ares-devel libev-devel libsodium-devel mbedtls-devel -y
./configure & make & make install

yum install rng-tools
systemctl start rngd

cat > $conf << _CONF
{
    "server":"0.0.0.0",
    "server_port":$port,
    "password":"$1",
    "method":"chacha20-ietf-poly1305"
}
_CONF

cat >/etc/systemd/system/ss.service <<_SERV
[Unit]
Description=ssocks

[Service]
TimeoutStartSec=0
ExecStart=/usr/local/bin/ss-server -c $conf

[Install]
WantedBy=multi-user.target
_SERV

systemctl enable ss
systemctl start ss

# zone=`firewall-cmd --get-active-zones | sed '1q'`
# firewall-cmd --zone=$zone --add-port=$port/tcp --permanent
# firewall-cmd --reload
