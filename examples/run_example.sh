#!/usr/bin/bash
#
# run_example.sh
#

# constants
_TMP_FOLDER_="$(mktemp -d)"
_THIS_FOLDER_="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
_COMPILE_TO_="${_TMP_FOLDER_}/libconfini_example"
_EXAMPLES_=("${_THIS_FOLDER_}/other"/*.c "${_THIS_FOLDER_}/topics"/*.c)
_THIS_PATH_LENGTH_=$(expr ${#_THIS_FOLDER_} + 1)

# variables
_EXIT_=0
_ITER_=0
_GOOD_ANSWER_=0

echo 'Available examples:'
echo

for _FILENAME_ in "${_EXAMPLES_[@]}"
do
        _ITER_=$(expr ${_ITER_} + 1)
	echo "  [${_ITER_}] ${_FILENAME_:${_THIS_PATH_LENGTH_}}"
done

echo

while [[ "${_GOOD_ANSWER_}" -eq 0 ]] ; do
	read -p '==> Please choose an example to compile and run (0 to exit): ' _CHOICE_
	if [[ -z "${_CHOICE_}" ]] || [[ "${_CHOICE_}" == '0' ]] ; then
		_GOOD_ANSWER_=1
		_EXIT_=1
	elif [[ ! "${_CHOICE_}" =~ ^[0-9]+$ ]] || [[ "${_CHOICE_}" -lt 0 ]] || [[ "${_CHOICE_}" -gt "${_ITER_}" ]] ; then
		echo 'Invalid choice' 1>&2
	else
		_GOOD_ANSWER_=1
		_EXIT_=0
	fi
done

if [[ "${_EXIT_}" -eq 0 ]] ; then
	_CHOSEN_LONG_="${_EXAMPLES_[$(expr ${_CHOICE_} - 1)]}"
	_CHOSEN_SHORT_=${_CHOSEN_LONG_:_THIS_PATH_LENGTH_}
	echo "==> Compiling \"${_CHOSEN_SHORT_}\" with gcc"
	echo -e "    (\x60gcc -lconfini \"${_CHOSEN_SHORT_}\"\x60)..."
	if gcc -lconfini "${_CHOSEN_LONG_}" -o "${_COMPILE_TO_}" ; then
		echo "==> File \"${_CHOSEN_SHORT_}\" has been successifully compiled. Run it..."
		echo
		echo "-----------------[${_CHOSEN_SHORT_}]-----------------"
		echo
		( cd "${_THIS_FOLDER_}" ; "${_COMPILE_TO_}" )
		echo
		echo -n '------------------'
		for ((_ITER_=1; _ITER_<=${#_CHOSEN_SHORT_}; _ITER_++)); do
			echo -n '-';
		done
		echo '------------------'
		echo
		echo "==> Program exited with status $?."
		echo -n '==> Do you want to have a look at the source code? (y/n) '
		read -n1 _CHOICE_
		if [[ "${_CHOICE_}" == $EOF ]] || [[ "${_CHOICE_,,}" == "y" ]] ; then
			cat "${_CHOSEN_LONG_}" | less
		fi
		echo
	else
		echo "==> Couldn't compile file \"${_CHOSEN_SHORT_}\"." 1>&2
	fi
fi

rm -r "${_TMP_FOLDER_}"

# EOF

