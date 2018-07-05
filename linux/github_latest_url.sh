# github_latest_url name/repo
curl --silent "https://api.github.com/repos/$1/releases/latest" |
        grep '"browser_download_url":' |
                sed -E 's/.*"([^"]+)".*/\1/'
