#disable  ctrl-s/q	:freeze/release
stty -ixon -ixoff

export EDITOR=vim
set -o vi
eval $(dircolors ~/.dircolors)
alias ls="ls --color=auto"

source .cfg/tty-solarized-dark.sh
export PS1='\
\[\e[1;34;3;4m\]\u\
\[\e[1;32;3;4m\]@\
\[\e[1;34;3;4m\]\l\
\[\e[1;32;3;4m\].\
\[\e[1;34;3;4m\]\t\
\[\e[1;32;3;4m\].\
\[\e[0;36;3;4m\]\w\
\[\e[0m\]} \
'


#start ssh-agent automatically
if [ -f "$HOME/.ssh" ]; then
	SSH_ENV="$HOME/.ssh/env"
	function start_sshagent {
		echo starting ssh-agent
		ssh-agent | sed 's/^echo/#echo/' > "${SSH_ENV}"
		echo started
		chmod u+x  "${SSH_ENV}"
		. "${SSH_ENV}" > /dev/null
		ssh-add
	}
	if [ -f "${SSH_ENV}" ] 
	then
		. "${SSH_ENV}" > /dev/null
		echo SSH_AGENT_PID: $SSH_AGENT_PID
		if [ -z $(ps -p $SSH_AGENT_PID -o pid=) ]
		then
			echo no ps
			start_sshagent
		fi
	else
		start_sshagent
	fi
fi

