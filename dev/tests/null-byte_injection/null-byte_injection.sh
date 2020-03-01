#!/bin/sh
#
# myscript.sh
#

gcc -Wall -pedantic -std=c99 -lconfini null-byte_injection.c -O3 -o /tmp/null-byte_injection && { \
/tmp/null-byte_injection
rm /tmp/null-byte_injection
}
