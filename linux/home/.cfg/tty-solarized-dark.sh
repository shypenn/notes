# Solarized theme for tty, the dark version.
# Based on:
#   - Solarized (http://ethanschoonover.com/solarized)
#   - Xresources from http://github.com/altercation/solarized
# Generated with pty2tty.awk by Joep van Delft
# http://github.com/joepvd/tty-solarized
if [ "$TERM" = "linux" ]; then
	#foreground
    echo -en "\e]P0073642" # S_base02		black		30
    echo -en "\e]P1dc322f" # S_red			red			31
    echo -en "\e]P2859900" # S_green		green	 	32
    echo -en "\e]P3b58900" # S_yellow		brown	 	33
    echo -en "\e]P4268bd2" # S_blue			blue		34
    echo -en "\e]P5d33682" # S_magenta		magenta		35
    echo -en "\e]P62aa198" # S_cyan			cyan		36
    echo -en "\e]P7eee8d5" # S_base2		white 		37

	#background
    echo -en "\e]P8002b36" # S_base03					40
    echo -en "\e]P9cb4b16" # S_orange					 | 
    echo -en "\e]PA586e75" # S_base01					 |
    echo -en "\e]PB657b83" # S_base00					 |
    echo -en "\e]PC839496" # S_base0					 |
    echo -en "\e]PD6c71c4" # S_violet					 |
    echo -en "\e]PE93a1a1" # S_base1					 |
    echo -en "\e]PFfdf6e3" # S_base3					47
    clear # against bg artifacts
fi
