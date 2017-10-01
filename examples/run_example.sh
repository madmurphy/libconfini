#!/usr/bin/bash
#
# Compile and run an example
#

if [[ ! -t 0 ]] ; then
	echo 'Please, run this script in a terminal.' 1>&2
	exit 1
fi

if [[ ! $(which gcc 2>/dev/null) ]] ; then
	echo 'You need to have gcc installed on your machine.' 1>&2
	exit 1
fi

# constants
_TMP_FOLDER_="$(mktemp -d)"
_THIS_PATH_=$(cd $(dirname "${BASH_SOURCE[0]}") ; pwd)
_THIS_PATH_LENGTH_=$(expr "${#_THIS_PATH_}" + 1)
_COMPILE_TO_="${_TMP_FOLDER_}/libconfini_example"
_SRCS_=("${_THIS_PATH_}/other"/*.c
	"${_THIS_PATH_}/topics"/*.c)
_COLORS_=0
_HIGHLIGHT_=0

if which tput > /dev/null 2>&1 ; then 
	_COLORS_=$(tput -T "${TERM}" colors)
fi

if [[ "${_COLORS_}" -ge 8 ]] ; then
	_COL_YELLOW_='\e[0;33m'
	_COL_BRED_='\e[1;31m'
	_COL_BGREEN_='\e[1;32m'
	_COL_BYELLOW_='\e[1;33m'
	_COL_BBLUE_='\e[1;34m'
	_COL_BPURPLE_='\e[1;35m'
	_COL_BCYAN_='\e[1;36m'
	_COL_BWHITE_='\e[1;37m'
	_COL_DEFAULT_='\e[0m'
fi

if which highlight > /dev/null 2>&1 ; then
	_HIGHLIGHT_=1
fi

# variables
_DONE_=0

# other variables declared in this script:
# _GOOD_ANSWER_, _DONT_RUN_, _ITER_, _FILENAME_, _CHOICE_, _CHOSEN_LONG_, _CHOSEN_SHORT_, _EXIT_CODE_

while [[ "${_DONE_}" -eq 0 ]] ; do
	_GOOD_ANSWER_=0
	_DONT_RUN_=0
	_ITER_=0
	echo
	echo -e "${_COL_BBLUE_}::${_COL_BWHITE_} Available examples ${_COL_BBLUE_}::${_COL_DEFAULT_}"
	echo
	for _FILENAME_ in "${_SRCS_[@]}" ; do
		_ITER_=$(expr "${_ITER_}" + 1)
		echo -e "  [${_COL_BCYAN_}${_ITER_}${_COL_DEFAULT_}] ${_FILENAME_:${_THIS_PATH_LENGTH_}}"
	done
	echo
	while [[ "${_GOOD_ANSWER_}" -eq 0 ]] ; do
		echo -ne "${_COL_BGREEN_}==>${_COL_BWHITE_} Please choose an example to compile and run (leave empty to exit):${_COL_DEFAULT_} "
		read _CHOICE_
		if [[ -z "${_CHOICE_}" ]] || [[ "${_CHOICE_}" == '0' ]] ; then
			_GOOD_ANSWER_=1
			_DONT_RUN_=1
			_DONE_=1
		elif [[ ! "${_CHOICE_}" =~ ^[0-9]+$ ]] || [[ "${_CHOICE_}" -lt 0 ]] || [[ "${_CHOICE_}" -gt "${_ITER_}" ]] ; then
			echo -e "  ${_COL_BBLUE_}->${_COL_DEFAULT_} ${_COL_BRED_}Invalid choice${_COL_DEFAULT_}"
			echo
		else
			_GOOD_ANSWER_=1
			_DONT_RUN_=0
		fi
	done
	if [[ "${_DONT_RUN_}" -eq 0 ]] ; then
		_CHOSEN_LONG_="${_SRCS_[$(expr ${_CHOICE_} - 1)]}"
		_CHOSEN_SHORT_=${_CHOSEN_LONG_:_THIS_PATH_LENGTH_}
		echo -e "  ${_COL_BBLUE_}->${_COL_DEFAULT_} Compiling \"${_CHOSEN_SHORT_}\" with gcc"
		echo -e "     (\x60${_COL_YELLOW_}gcc -lconfini -pedantic -std=c99 \"${_CHOSEN_SHORT_}\"${_COL_DEFAULT_}\x60)..."
		if gcc -lconfini -pedantic -std=c99 "${_CHOSEN_LONG_}" -o "${_COMPILE_TO_}" ; then
			echo -e "  ${_COL_BBLUE_}->${_COL_DEFAULT_} File \"${_CHOSEN_SHORT_}\" has been successifully compiled. Run it..."
			echo
			echo -e "${_COL_BYELLOW_}-----------------[${_COL_BCYAN_}${_CHOSEN_SHORT_}${_COL_BYELLOW_}]-----------------${_COL_DEFAULT_}"
			echo
			( cd "${_THIS_PATH_}" ; "${_COMPILE_TO_}" )
			_EXIT_CODE_="$?"
			echo
			echo -ne "${_COL_BYELLOW_}------------------"
			for ((_ITER_=1; _ITER_<=${#_CHOSEN_SHORT_}; _ITER_++)); do
				echo -n '-';
			done
			echo -e "------------------${_COL_DEFAULT_}"
			echo
			echo -e "  ${_COL_BBLUE_}->${_COL_DEFAULT_} Program exited with status "$([[ "${_EXIT_CODE_}" -eq 0 ]] && echo "${_COL_BGREEN_}" || echo "${_COL_BRED_}")"${_EXIT_CODE_}${_COL_DEFAULT_}."
			echo
			echo -ne "${_COL_BGREEN_}==>${_COL_BWHITE_} Do you want to have a look at the source code? (Y/n)${_COL_DEFAULT_} "
			read -n1 _CHOICE_
			[[ "${_CHOICE_}" == "${EOF}" ]] || echo
			if [[ "${_CHOICE_}" == "${EOF}" ]] || [[ "${_CHOICE_,,}" == 'y' ]] ; then
				if [[ "${_HIGHLIGHT_}" -eq 0 ]] || [[ ! "${_COLORS_}" -ge 8 ]] ; then
					cat "${_CHOSEN_LONG_}" | less
					[[ "${_HIGHLIGHT_}" -eq 0 ]] &&\
					echo -e "  ${_COL_BBLUE_}->${_COL_DEFAULT_} ${_COL_BRED_}NOTE:${_COL_DEFAULT_} For a better output of the C source code consider to install\n     ${_COL_BPURPLE_}highlight${_COL_DEFAULT_} on your system."
				else
					highlight -O $([[ "${_COLORS_}" -ge 256 ]] && echo 'xterm256' || echo 'ansi') "${_CHOSEN_LONG_}" | less -r
				fi

			fi
		else
			echo -e "  ${_COL_BBLUE_}->${_COL_DEFAULT_} ${_COL_BRED_}Couldn't compile file \"${_CHOSEN_SHORT_}\".${_COL_DEFAULT_}"
		fi
	fi
done

rm -rf "${_TMP_FOLDER_}"

# EOF

