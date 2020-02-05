#!/usr/bin/bash
#
# prepare.sh
#

_SCRIPTPATH="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
_TMPFILE="$(mktemp)"

cd "${_SCRIPTPATH}"
CATSRC='../../examples/ini_files/self_explaining.conf'
CATDEST='big_file.ini'
CCPROG='gcc'
CSRC='performance.c'
COUT='speedtest'

# Careful with this number, it's an exponent!
DOUBLINGS=15

read -p '**WARNING** This script will generate an INI file '$(($(stat --printf="%s" "${CATSRC}") * (1 << "${DOUBLINGS}")))' bytes large. Do you'$'\n wish to proceed? (y/N) ' -n1 _ANSW_
[[ "${_ANSW_}" == "${EOF}" ]] || echo
[[ "${_ANSW_,,}" == 'y' ]] || exit 0

echo "$(echo; cat $CATSRC)" > "${CATDEST}"
while ((DOUBLINGS > 0)); do
	cat "${CATDEST}" "${CATDEST}" > "${_TMPFILE}" && mv "${_TMPFILE}" "${CATDEST}"
	((DOUBLINGS--))
done

if "${CCPROG}" -lconfini -o  "${COUT}" "${CSRC}"; then
	echo "File \"${CATDEST}\" has been generated. Now launch the following command:"
	echo "    ${_SCRIPTPATH}/${COUT}"
else
	echo
	echo "File \"${CATDEST}\" has been generated, but an error occured while trying to"
	echo "compile \"${CSRC}\""
fi

# EOF

