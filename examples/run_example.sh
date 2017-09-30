#!/usr/bin/bash
#
# Compile and run an example
#

if [[ ! $(which gcc 2>/dev/null) ]]; then
	echo 'You need to have gcc installed on your machine' 1>&2
	exit 1
fi

# constants
_TMP_FOLDER_="$(mktemp -d)"
_THIS_FOLDER_="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
_COMPILE_TO_="${_TMP_FOLDER_}/libconfini_example"
_EXAMPLES_=("${_THIS_FOLDER_}/other"/*.c "${_THIS_FOLDER_}/topics"/*.c)
_THIS_PATH_LENGTH_=$(expr ${#_THIS_FOLDER_} + 1)
_HIGHLIGHT_=0

if [[ $(which highlight 2>/dev/null) ]]; then
	_HIGHLIGHT_=1
fi

# variables
_DONE_=0

# other variables declared later:
# _GOOD_ANSWER_, _DONT_RUN_, _ITER_, _FILENAME_, _CHOICE_, _CHOSEN_LONG_, _CHOSEN_SHORT_, _EXIT_CODE_

while [[ "${_DONE_}" -eq 0 ]] ; do

	_GOOD_ANSWER_=0
	_DONT_RUN_=0
	_ITER_=0

	echo
	echo -e '\e[1;34m::\e[1;37m Available examples \e[1;34m::\e[0m'
	echo

	for _FILENAME_ in "${_EXAMPLES_[@]}"
	do
		_ITER_=$(expr ${_ITER_} + 1)
		echo -e "  [\\e[1;36m${_ITER_}\\e[0m] ${_FILENAME_:${_THIS_PATH_LENGTH_}}"
	done

	echo

	while [[ "${_GOOD_ANSWER_}" -eq 0 ]] ; do
		echo -en '\e[1;32m==>\e[1;37m Please choose an example to compile and run (leave empty to exit):\e[0m '
		read _CHOICE_
		if [[ -z "${_CHOICE_}" ]] || [[ "${_CHOICE_}" == '0' ]] ; then
			_GOOD_ANSWER_=1
			_DONT_RUN_=1
			_DONE_=1
		elif [[ ! "${_CHOICE_}" =~ ^[0-9]+$ ]] || [[ "${_CHOICE_}" -lt 0 ]] || [[ "${_CHOICE_}" -gt "${_ITER_}" ]] ; then
			echo -e '  \e[1;34m->\e[0m \e[1;31mInvalid choice\e[0m'
			echo
		else
			_GOOD_ANSWER_=1
			_DONT_RUN_=0
		fi
	done

	if [[ "${_DONT_RUN_}" -eq 0 ]] ; then
		_CHOSEN_LONG_="${_EXAMPLES_[$(expr ${_CHOICE_} - 1)]}"
		_CHOSEN_SHORT_=${_CHOSEN_LONG_:_THIS_PATH_LENGTH_}
		echo -e "  \\e[1;34m->\\e[0m Compiling \"${_CHOSEN_SHORT_}\" with gcc"
		echo -e "     (\x60gcc -lconfini -pedantic -std=c99 \"${_CHOSEN_SHORT_}\"\x60)..."
		if gcc -lconfini -pedantic -std=c99 "${_CHOSEN_LONG_}" -o "${_COMPILE_TO_}" ; then
			echo -e "  \\e[1;34m->\\e[0m File \"${_CHOSEN_SHORT_}\" has been successifully compiled. Run it..."
			echo
			echo -e "\\e[1;33m-----------------[\\e[1;36m${_CHOSEN_SHORT_}\\e[1;33m]-----------------\\e[0m"
			echo
			( cd "${_THIS_FOLDER_}" ; "${_COMPILE_TO_}" )
			_EXIT_CODE_="$?"
			echo
			echo -en '\e[1;33m------------------'
			for ((_ITER_=1; _ITER_<=${#_CHOSEN_SHORT_}; _ITER_++)); do
				echo -n '-';
			done
			echo -e '------------------\e[0m'
			echo
			echo -e "  \\e[1;34m->\\e[0m Program exited with status "$([[ "${_EXIT_CODE_}" -eq 0 ]] && echo '\e[1;32m' || echo '\e[1;31m')"${_EXIT_CODE_}\\e[0m."
			echo
			echo -en '\e[1;32m==>\e[1;37m Do you want to have a look at the source code? (y/n)\e[0m '
			read -n1 _CHOICE_
			[[ "${_CHOICE_}" == ${EOF} ]] || echo
			if [[ "${_CHOICE_}" == ${EOF} ]] || [[ "${_CHOICE_,,}" == "y" ]] ; then
				if [[ _HIGHLIGHT_ -eq 0 ]]; then
					cat "${_CHOSEN_LONG_}" | less
					echo -e '  \e[1;34m->\e[0m \e[1;31mNOTE:\e[0m For a better output of the C source code consider to install'
					echo -e '     \e[1;35mhighlight\e[0m on your system.'
				else
					highlight -O $([[ "$(tput colors)" -ge 256 ]] && echo 'xterm256' || echo 'ansi') "${_CHOSEN_LONG_}" | less -r
				fi

			fi
		else
			echo "==> Couldn't compile file \"${_CHOSEN_SHORT_}\"." 1>&2
		fi
	fi

done

rm -rf "${_TMP_FOLDER_}"

# EOF

