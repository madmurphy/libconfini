#!/bin/bash
#
# run-example.sh
#
# Compile and run an example on the fly
#
# Copyright 2017-2020, Stefano Gioffr\u00E9
#
# GNU General Public License, version 3 or any later version
#

if [[ ! -t 0 ]]; then
	echo 'Please, run this script in a terminal.' 1>&2
	exit 1
fi

if ! which gcc &> /dev/null; then
	echo 'You need to have gcc installed on your machine.' 1>&2
	exit 1
fi


# CONSTANTS

# Environment
_TMP_FOLDER_="$(mktemp -d)"
_THIS_PATH_="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
_THIS_PATH_LENGTH_="$(expr "${#_THIS_PATH_}" + 1)"
_COMPILED_PRG_="${_TMP_FOLDER_}/libconfini_example"
_SRCS_=()

_HIGHLIGHT_=0
if which highlight > /dev/null 2>&1; then
	_HIGHLIGHT_=1
fi

# Terminal output
_COLORS_=0
_DEL1_=$'\e[0K'
_UP1_=$'\e[1A'

if which tput &> /dev/null; then
	_COLORS_="$(tput -T "${TERM}" colors)"
fi

if [[ "${_COLORS_}" -ge 8 ]]; then
	_COL_YELLOW_=$'\e[0;33m'
	_COL_BLYELLOW_=$'\e[5;33m'
	_COL_BRED_=$'\e[1;31m'
	_COL_BGREEN_=$'\e[1;32m'
	_COL_BYELLOW_=$'\e[1;33m'
	_COL_BBLUE_=$'\e[1;34m'
	_COL_BPURPLE_=$'\e[1;35m'
	_COL_BCYAN_=$'\e[1;36m'
	_COL_BWHITE_=$'\e[1;37m'
	_COL_DEFAULT_=$'\e[0m'
fi


# VARIABLES

_SEENLIST_=()
_LASTSEEN_=0

# other variables declared in this script:
# _SEEN_, _ITER_, _FILENAME_, _CHOICE_, _LASTSEEN_,
# _CHOSEN_LONG_, _CHOSEN_SHORT_, _GCC_CMD_, _EXIT_CODE_


# SCRIPT BODY

while IFS= read -r -d $'\0' _ITER_; do
	_SRCS_+=("${_ITER_}")
	_SEENLIST_+=(0)
done < <(find "${_THIS_PATH_}" '(' -name '*.c' -o -name '*.cpp' ')' -print0 | sort -z)

while :; do
	_ITER_=0
	echo
	{
		echo "${_COL_BBLUE_}::${_COL_BWHITE_} Available examples ${_COL_BBLUE_}::${_COL_DEFAULT_}"
		echo
		for _FILENAME_ in "${_SRCS_[@]}"; do
			_SEEN_="${_SEENLIST_["${_ITER_}"]}"
			((_ITER_=_ITER_+1))
			echo "  [${_COL_BCYAN_}${_ITER_}${_COL_DEFAULT_}] $([[ "${_LASTSEEN_}" -eq "${_ITER_}" ]] && echo "${_COL_BLYELLOW_}" || [[ "${_SEEN_}" -eq 0 ]] || echo "${_COL_YELLOW_}")${_FILENAME_:${_THIS_PATH_LENGTH_}}$([[ "${_SEEN_}" -eq 0 ]] || echo "${_COL_DEFAULT_}")"
		done
	} | more
	echo
	_ITER_=0
	while [[ "${_ITER_}" -eq -0 ]]; do
		read -p "${_COL_BGREEN_}==>${_COL_BWHITE_} Please choose a number to compile and run (q to exit):${_COL_DEFAULT_} " _CHOICE_
		if [[ -z "${_CHOICE_}" ]]; then
			echo -ne "${_DEL1_}  ${_COL_BBLUE_}->${_COL_DEFAULT_} Please choose an example\\r${_UP1_}${_DEL1_}"
		elif [[ "${_CHOICE_}" =~ ^0+$ ]] || [[ "${_CHOICE_,,}" == 'q' ]]; then
			echo -ne "${_DEL1_}\\r"
			exit 0
		elif [[ "${_CHOICE_,,}" == 'g' ]]; then
			_ITER_=-1
			echo -ne "${_DEL1_}\\r"
		elif [[ ! "${_CHOICE_}" =~ ^[0-9]+$ ]] || [[ "${_CHOICE_}" -lt 0 ]] || [[ "${_CHOICE_}" -gt "${#_SRCS_[@]}" ]]; then
			echo -ne "${_DEL1_}  ${_COL_BBLUE_}->${_COL_DEFAULT_} ${_COL_BRED_}Invalid number${_COL_DEFAULT_}\\r${_UP1_}${_DEL1_}"
		else
			_ITER_=1
			echo -n "${_DEL1_}"
		fi
	done
	[[ "${_ITER_}" -ne -1 ]] || continue
	((_ITER_=_CHOICE_-1))
	_CHOSEN_LONG_="${_SRCS_["${_ITER_}"]}"
	_CHOSEN_SHORT_="${_CHOSEN_LONG_:_THIS_PATH_LENGTH_}"
	case "${_CHOSEN_SHORT_}" in *'.cpp') _GCC_CMD_=g++ ;; *'.c') _GCC_CMD_=gcc ;; esac
	_GCC_CMD_="${_GCC_CMD_} -Wall -pedantic -lconfini"
	if [[ "${_CHOSEN_SHORT_}" == *"glib_"* ]]; then
		# We need glib for these...
		_GCC_CMD_="${_GCC_CMD_} -lglib-2.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include"
	fi
	echo "  ${_COL_BBLUE_}->${_COL_DEFAULT_} Compiling \"${_CHOSEN_SHORT_}\" with gcc"
	echo "     (\`${_COL_YELLOW_}${_GCC_CMD_} '${_CHOSEN_SHORT_}'${_COL_DEFAULT_}\`)..."
	if ${_GCC_CMD_} -o "${_COMPILED_PRG_}" "${_CHOSEN_LONG_}"; then
		_LASTSEEN_="${_CHOICE_}"
		_SEENLIST_["${_ITER_}"]=1
		echo "  ${_COL_BBLUE_}->${_COL_DEFAULT_} File \"${_CHOSEN_SHORT_}\" has been successifully compiled."
		echo "  ${_COL_BBLUE_}->${_COL_DEFAULT_} Run it..."
		echo
		{
			echo "${_COL_BYELLOW_}-----------------[${_COL_BCYAN_}${_CHOSEN_SHORT_}${_COL_BYELLOW_}]-----------------${_COL_DEFAULT_}"
			echo
			(cd "$(dirname "${_CHOSEN_LONG_}")" && "${_COMPILED_PRG_}")
			_EXIT_CODE_="${?}"
			echo
			echo -n "${_COL_BYELLOW_}------------------"
			for ((_ITER_=1; _ITER_<=${#_CHOSEN_SHORT_}; _ITER_++)); do echo -n '-'; done
			echo "------------------${_COL_DEFAULT_}"
			echo
			echo "  ${_COL_BBLUE_}->${_COL_DEFAULT_} Program exited with status "$([[ "${_EXIT_CODE_}" -eq 0 ]] && echo -n "${_COL_BGREEN_}" || echo -n "${_COL_BRED_}")"${_EXIT_CODE_}${_COL_DEFAULT_}."
		} | less -XFKEr
		echo
		rm -f "${_COMPILED_PRG_}"
		read -n1 -s -p "${_COL_BGREEN_}==>${_COL_BWHITE_} Do you want to have a look at the source code? (y/N)${_COL_DEFAULT_} " _CHOICE_
		if [[ "${_CHOICE_,,}" == 'y' ]]; then
			echo 'y'
			if [[ "${_HIGHLIGHT_}" -eq 0 ]] || [[ ! "${_COLORS_}" -ge 8 ]]; then
				if [[ "${_COLORS_}" -ge 8 ]]; then
					echo "  ${_COL_BBLUE_}->${_COL_DEFAULT_} ${_COL_BCYAN_}NOTE:${_COL_DEFAULT_} For a better output of the C code consider installing ${_COL_BPURPLE_}highlight${_COL_DEFAULT_} on"
					echo '     your system.'
				fi
				cat "${_CHOSEN_LONG_}" | less -FK
			else
				highlight -O "$([[ "${_COLORS_}" -ge 256 ]] && echo -n 'xterm256' || echo -n 'ansi')" "${_CHOSEN_LONG_}" | less -r
			fi
		else
			echo 'n'
		fi
	else
		echo "  ${_COL_BBLUE_}->${_COL_DEFAULT_} ${_COL_BRED_}Couldn't compile file \"${_CHOSEN_SHORT_}\".${_COL_DEFAULT_}"
	fi
done

rm -rf "${_TMP_FOLDER_}"

# EOF

