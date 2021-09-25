#!/bin/sh
#
# libconfini/dev/tests/playground/playground.sh
#

gcc -Wall -pedantic -std=c99 -lconfini playground.c -O2 -o /tmp/playground && { \
	/tmp/playground
	rm /tmp/playground
}
