#!/bin/sh
#
# sprepare.sh
#
# Unattended mode
#
# **WARNING** This script generates a big file! Use `prepare.sh` to know how big.
#

_TMPFILE="tmp.ini"
CATSRC='../../../examples/ini_files/self_explaining.conf'
CATDEST='big_file.ini'
CCPROG='gcc'
CSRC='performance.c'
COUT='speedtest'

echo "$(echo; cat $CATSRC)" > "${CATDEST}"
for INDEX in {1..15}; do cat "${CATDEST}" "${CATDEST}" > "${_TMPFILE}" && mv "${_TMPFILE}" "${CATDEST}"; done
"${CCPROG}" -lconfini -pedantic -o  "${COUT}" "${CSRC}"

